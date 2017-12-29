
#include "basis_multiplexer.h"
#include "basis_event_loop.h"

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
			int err = GetLastError();
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
		delete (BSMultiplexer*)m_private_data;
	}

	const char* BSMultiplexer::ApiName()
	{
		return "windows select";
	}

#endif // __USE_IOCP__
#endif // __WINDOWS__

#ifdef __POSIX__
#ifdef __USE_EPOLL__

#else
#ifdef __USE_POLL__
	class BSPOXSelectData
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

		BSPOXSelectData* data = new BSPOXSelectData;
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
		BSPOXSelectData* data = (BSPOXSelectData*)m_private_data;
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
			ASSERT(it->second == partner);
		}
		return true;
	}

	bool BSMultiplexer::DelEvent(int fd, int mask, BSFdPartner* partner)
	{
		if (mask == fs_none) return false;
		if (partner == NULL) return false;
		BSPOXSelectData* data = (BSPOXSelectData*)m_private_data;
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

	int BSMultiplexer::Poll(BSEventLoop *el, struct timeval *tvp)
	{
		BSPOXSelectData* data = (BSPOXSelectData*)m_private_data;
		memcpy(&data->m_arg_r_set, &data->m_r_set, sizeof(data->m_r_set));
		memcpy(&data->m_arg_w_set, &data->m_w_set, sizeof(data->m_w_set));
		int nret = ::select(data->max_fd() + 1, &data->m_arg_r_set, &data->m_arg_w_set, NULL, tvp);
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
			printf("select error %d.\n", errno);
			return -1;
		}
		return 0;
	}

	BSFiredFd* BSMultiplexer::FetchFiredFd()
	{
		BSPOXSelectData* data = (BSPOXSelectData*)m_private_data;
		if (data->m_curr_index >= data->m_fired_fds.size()) return NULL;
		return &data->m_fired_fds[data->m_curr_index++];
	}

	BSFiredFd* BSMultiplexer::GetFiredFd(uint32 index)
	{
		BSPOXSelectData* data = (BSPOXSelectData*)m_private_data;
		if (index >= data->m_fired_fds.size()) return NULL;
		return &data->m_fired_fds[index];
	}

	void BSMultiplexer::Destroy()
	{
		delete (BSMultiplexer*)m_private_data;
	}

	const char* BSMultiplexer::ApiName()
	{
		return "linux select";
	}

#else // use select

#endif
#endif

#endif // __POSIX__

}