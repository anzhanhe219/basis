
#include "basis_multiplexer.h"
#include "basis_event_loop.h"
#include "basis_socket.h"

namespace basis
{
#ifdef __WINDOWS__
#ifdef __USE_IOCP__

#else // use select

	class BSWinSelectData
	{
	public:
		FD_SET m_r_set;
		FD_SET m_w_set;
		FD_SET m_arg_r_set;
		FD_SET m_arg_w_set;

	public:
		uint32 m_curr_index;
		vector<BSFiredFd> m_fired_fds;
		map<int, BSFdPartner*> m_partners;

	public:
		int max_fd()
		{
			if (m_partners.empty()) return 0;
			return m_partners.rbegin()->first;
		}
	};

	BSMultiplexer* BSMultiplexer::Create(int sz)
	{
		if (sz > FD_SETSIZE) return NULL;

		BSWinSelectData* data = new BSWinSelectData;
		if (data == NULL) return NULL;
		if (BSMultiplexer* mlp = new BSMultiplexer())
		{
			FD_ZERO(&data->m_r_set);
			FD_ZERO(&data->m_w_set);
			data->m_fired_fds.reserve(sz);
			data->m_curr_index = 0;			
			mlp->m_private_data = data;			
			return mlp;
		}
		delete data;
		return NULL;
	}

	bool BSMultiplexer::AddEvent(int fd, int mask, BSFdPartner* partner)
	{
		if (mask == fs_none) return false;
		if (partner == NULL) return false;
		BSWinSelectData* data = (BSWinSelectData*)m_private_data;
		if (data->m_partners.size() >= FD_SETSIZE) return false;
		if (mask & fs_readable)	FD_SET(fd, &data->m_r_set);
		if (mask & fs_writeable) FD_SET(fd, &data->m_w_set);

		map<int, BSFdPartner*>::iterator it = data->m_partners.find(fd);
		if (it == data->m_partners.end())
		{
			data->m_partners[fd] = partner;
		}
		else
		{
			ASSERT(it->second ==partner);
		}
		return true;
	}

	bool BSMultiplexer::DelEvent(int fd, int mask, BSFdPartner* partner)
	{
		if (mask == fs_none) return false;
		if (partner == NULL) return false;
		BSWinSelectData* data = (BSWinSelectData*)m_private_data;
		if (mask & fs_readable)	FD_CLR(fd, &data->m_r_set);
		if (mask & fs_writeable) FD_CLR(fd, &data->m_w_set);

		if (partner->m_logic_mask == fl_empty)
		{
			map<int, BSFdPartner*>::iterator it;
			it = data->m_partners.find(fd);
			if (it != data->m_partners.end())
			{
				data->m_partners.erase(it);
			}
		}
		return true;
	}

	int BSMultiplexer::Poll(BSEventLoop *el,  struct timeval *tvp)
	{
		BSWinSelectData* data = (BSWinSelectData*)m_private_data;
		memcpy(&data->m_arg_r_set,  &data->m_r_set, sizeof(data->m_r_set));
		memcpy(&data->m_arg_w_set,  &data->m_w_set, sizeof(data->m_w_set));
		int nret = ::select(data->max_fd()+1, &data->m_arg_r_set, &data->m_arg_w_set, NULL, tvp);
		if (nret > 0)
		{
			int count = 0;
			data->m_curr_index = 0;
			data->m_fired_fds.clear();			
			map<int, BSFdPartner*>::iterator it = data->m_partners.begin();
			for (; it != data->m_partners.end(); ++it)
			{
				int fd = it->first;
				BSFdPartner* fp = it->second;

				if (fp == NULL) continue;
				if (fp->m_logic_mask == fl_empty) continue;
				
				int mark = fs_none;
				if (FD_ISSET(fd, &data->m_arg_r_set))
					mark |= fs_readable;
				if (FD_ISSET(fd, &data->m_arg_w_set))
					mark |= fs_writeable;
				if (mark == fs_none) continue;
				
				BSFiredFd fr;
				fr.m_fd = fd;
				fr.m_partner = fp;
				fr.m_state_mask = mark;
				data->m_fired_fds.push_back(fr);
				count++;
			}
			return count;
		}
		else if (nret < 0)
		{
			// 错误日志
			int err = BSSocket::last_error();
			printf("select error %d.\n", err);
			return -1;
		}
		return 0;		
	}

	BSFiredFd* BSMultiplexer::FetchFiredFd()
	{
		BSWinSelectData* data = (BSWinSelectData*)m_private_data;
		if (data->m_curr_index >= data->m_fired_fds.size()) return NULL;		
		return &data->m_fired_fds[data->m_curr_index++];
	}

	BSFiredFd* BSMultiplexer::GetFiredFd(uint32 index)
	{
		BSWinSelectData* data = (BSWinSelectData*)m_private_data;
		if (index >= data->m_fired_fds.size()) return NULL;		
		return &data->m_fired_fds[index];
	}

	void BSMultiplexer::Destroy()
	{
		delete (BSWinSelectData*)m_private_data;
	}

	const char* BSMultiplexer::ApiName()
	{
		return "windows select";
	}

#endif // __USE_IOCP__
#endif // __WINDOWS__

#ifdef __POSIX__
#ifdef __USE_EPOLL__

	enum {
		max_epoll_size = 10240,
		max_epoll_ready = 1024;
	};

	class BSEpollData
	{
	public:
		BSEpollData() : m_epoll_fd(-1), m_curr_index(0) {}

		int m_epoll_fd;
		int m_curr_index;
		vector<int> m_maskes;
		vector<BSFiredFd> m_fired_fds;
		struct epoll_event m_polled_events[max_epoll_ready];
	};

	BSMultiplexer* BSMultiplexer::Create(int sz)
	{
		if (sz > max_epoll_size) return NULL;

		BSEpollData* data = new BSEpollData;
		if (data == NULL) return NULL;
		do 
		{
			if (data->m_epoll_fd = epoll_create(sz) < 0) break;
			if (BSMultiplexer* mlp = new BSMultiplexer())
			{
				data->m_fired_fds.reserve(512);
				data->m_partners.resize(sz, 0);
				mlp->m_private_data = data;			
				return mlp;
			}
		} while (false);

		delete data;
		return NULL;
	}

	bool BSMultiplexer::AddEvent(int fd, int mask, BSFdPartner* partner)
	{
		if (mask == fs_none) return false;
		if (partner == NULL) return false;
		BSEpollData* data = (BSEpollData*)m_private_data;	
		if (fd >= data->m_maskes.size()) return false;
		int* fd_mask = &data->m_maskes[fd];

		struct epoll_event evt;
		evt.data.fd = fd;
		evt.data.ptr = partner;
		evt.events = *fd_mask;
		if (mask & fs_readable)	evt.events |= EPOLLIN;
		if (mask & fs_writeable) evt.events |= EPOLLOUT;
		int ctl = (*fd_mask) ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;

		if (epoll_ctl(data->m_epoll_fd, ctl, fd, &evt) == -1)
		{
			return false;
		}

		*fd_mask = evt.events; // 记录已经注册事件
		return true;
	}

	bool BSMultiplexer::DelEvent(int fd, int mask, BSFdPartner* partner)
	{
		if (mask == fs_none) return false;
		if (partner == NULL) return false;
		BSEpollData* data = (BSEpollData*)m_private_data;	
		if (fd >= data->m_maskes.size()) return false;
		int* fd_mask = &data->m_maskes[fd];

		struct epoll_event evt;
		evt.data.fd = fd;
		evt.data.ptr = partner;
		evt.events = *fd_mask;
		if (mask & fs_readable)	evt.events &= ~EPOLLIN;
		if (mask & fs_writeable) evt.events &= ~EPOLLOUT;
		int ctl = (evt.events) ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;

		if (epoll_ctl(data->m_epoll_fd, ctl, fd, &evt) == -1)
		{
			return false;
		}

		*fd_mask = evt.events;
		return true;
	}

	int BSMultiplexer::Poll(BSEventLoop *el,  struct timeval *tvp)
	{
		int timeout = tvp ? tvp->tv_sec*1000 + tvp->tv_usec/1000 : -1;
		BSEpollData* data = (BSEpollData*)m_private_data;	
		data->m_curr_index = 0;
		data->m_fired_fds.clear();
		int nret=epoll_wait(data->m_epoll_fd, data->m_polled_events, max_epoll_ready, timeout); 
		if (nret > 0)
		{
			for (int i = 0; i < nret; ++i)
			{
				struct epoll_event* evt = &data->m_polled_events[i];

				int fd = evt->data.fd;
				BSFdPartner* fp = (BSFdPartner*)evt.data.ptr;
				if (fp == NULL) continue;
				if (fp->m_logic_mask == fl_empty) continue;

				int mark = fs_none;
				if (evt->events & EPOLLIN)
					mark |= fs_readable;
				if (evt->events & EPOLLOUT)
					mark |= fs_writeable;
				if (mark == fs_none) continue;

				BSFiredFd fr;
				fr.m_fd = fd;
				fr.m_partner = fp;
				fr.m_state_mask = mark;
				data->m_fired_fds.push_back(fr);
			}
			return data->m_fired_fds.size();
		}
		else if (nret < 0)
		{
			// 错误日志
			int err = BSSocket::last_error();
			printf("epoll error %d.\n", err);
			return -1;
		}
		return 0;		
	}

	BSFiredFd* BSMultiplexer::FetchFiredFd()
	{
		BSEpollData* data = (BSEpollData*)m_private_data;
		if (data->m_curr_index >= data->m_fired_fds.size()) return NULL;		
		return &data->m_fired_fds[data->m_curr_index++];
	}

	BSFiredFd* BSMultiplexer::GetFiredFd(uint32 index)
	{
		BSEpollData* data = (BSEpollData*)m_private_data;
		if (index >= data->m_fired_fds.size()) return NULL;		
		return &data->m_fired_fds[index];
	}

	void BSMultiplexer::Destroy()
	{
		delete (BSEpollData*)m_private_data;
	}

	const char* BSMultiplexer::ApiName()
	{
		return "linux epoll";
	}
#else
#ifdef __USE_POLL__


#else // use select

#endif
#endif

#endif // __POSIX__

}