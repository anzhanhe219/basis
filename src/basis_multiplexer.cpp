
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

		uint32 m_curr_index;
		vector<BSFiredEvent> m_fired_events;
		map<int, BSFileEvent*> m_file_events;

	public:
		int max_fd()
		{
			if (m_file_events.empty())
			{
				return 0;
			}
			return m_file_events.rbegin()->first;
		}

		BSFileEvent* get_file_event(int fd)
		{
			return NULL;
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
			data->m_fired_events.reserve(sz);
			data->m_curr_index = 0;			
			mlp->m_private_data = data;			
			return mlp;
		}
		delete data;
		return NULL;
	}

	bool BSMultiplexer::Resize(int sz)
	{
		if (sz > FD_SETSIZE) return false;
		return true;
	}

	bool BSMultiplexer::AddEvent(int fd, int mask, BSFileEvent* arg)
	{
		if (mask == ae_none) return false;
		if (arg == NULL) return false;
		BSWinSelectData* data = (BSWinSelectData*)m_private_data;
		if (data->m_file_events.size() >= FD_SETSIZE) return false;
		if (mask & ae_readable)	FD_SET(fd, &data->m_r_set);
		if (mask & ae_writeable) FD_SET(fd, &data->m_w_set);
		if (data->m_file_events.find(fd) == data->m_file_events.end())
		{
			data->m_file_events[fd] = arg;
		}
		return true;
	}

	bool BSMultiplexer::DelEvent(int fd, int mask, BSFileEvent* arg)
	{
		if (mask == ae_none) return false;
		if (arg == NULL) return false;
		BSWinSelectData* data = (BSWinSelectData*)m_private_data;
		if (mask & ae_readable)	FD_CLR(fd, &data->m_r_set);
		if (mask & ae_writeable) FD_CLR(fd, &data->m_w_set);
		if (arg->m_mask == ae_empty_fd)
		{
			map<int, BSFileEvent*>::iterator it;
			it = data->m_file_events.find(fd);
			if (it != data->m_file_events.end())
			{
				data->m_file_events.erase(it);
			}
		}
		return true;
	}

	int BSMultiplexer::Poll(BSEventLoop *el,  struct timeval *tvp)
	{
		BSWinSelectData* data = (BSWinSelectData*)m_private_data;
		data->m_curr_index = 0;
		data->m_fired_events.clear();
		memcpy(&data->m_arg_r_set,  &data->m_r_set, sizeof(data->m_r_set));
		memcpy(&data->m_arg_w_set,  &data->m_w_set, sizeof(data->m_w_set));
		int nret = ::select(data->max_fd()+1, &data->m_arg_r_set, &data->m_arg_w_set, NULL, tvp);
		if (nret > 0)
		{
			int count = 0;
			map<int, BSFileEvent*>::iterator it = data->m_file_events.begin();
			for (; it != data->m_file_events.end(); ++it)
			{
				int fd = it->first;
				BSFileEvent* fe = it->second;

				if (fe == NULL) continue;
				if (fe->m_mask == ae_none) continue;
				
				int mark = ae_none;
				if (FD_ISSET(fd, &data->m_arg_r_set))
					mark |= ae_readable;
				if (FD_ISSET(fd, &data->m_arg_w_set))
					mark |= ae_writeable;
				if (mark == ae_none) continue;
				
				BSFiredEvent fr;
				fr.m_fd = fd;
				fr.m_mask = mark;
				fr.m_file_event = fe;
				data->m_fired_events.push_back(fr);
				count++;
			}
			return count;
		}
		else if (nret < 0)
		{
			// ´íÎóÈÕÖ¾
			int err = GetLastError();
			printf("select error %d.\n", err);
			return -1;
		}
		return 0;		
	}

	BSFiredEvent* BSMultiplexer::FetchFiredEvents()
	{
		BSWinSelectData* data = (BSWinSelectData*)m_private_data;
		if (data->m_curr_index >= data->m_fired_events.size()) return NULL;		
		return &data->m_fired_events[data->m_curr_index++];
	}

	BSFiredEvent* BSMultiplexer::GetFiredEvents(uint32 index)
	{
		BSWinSelectData* data = (BSWinSelectData*)m_private_data;
		if (index >= data->m_fired_events.size()) return NULL;		
		return &data->m_fired_events[index];
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

#else // use select

#endif
#endif

#endif // __POSIX__

}