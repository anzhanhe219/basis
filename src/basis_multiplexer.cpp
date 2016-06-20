
#include "basis_multiplexer.h"
#include "basis_event_loop.h"

namespace basis
{
#ifdef __WINDOWS__
#ifdef __USE_IOCP__

#else // use select
#include <winsock.h>
#define MAX_FDSET 10240 // windows下暂定select支持最大值 

	class BSWinSelectData
	{
	public:
		FD_SET m_r_set;
		FD_SET m_w_set;
		FD_SET m_arg_r_set;
		FD_SET m_arg_w_set;
	};

	BSMultiplexer* BSMultiplexer::Create(int sz)
	{
		if (sz > MAX_FDSET) return NULL;

		BSWinSelectData* data = new BSWinSelectData;
		if (data == NULL) return NULL;
		if (BSMultiplexer* mlp = new BSMultiplexer())
		{
			FD_ZERO(&data->m_r_set);
			FD_ZERO(&data->m_w_set);
			mlp->m_private_data = data;
			return mlp;
		}
		delete data;
		return NULL;
	}

	bool BSMultiplexer::Resize(int sz)
	{
		if (sz > MAX_FDSET) return false;
		return true;
	}

	bool BSMultiplexer::AddEvent(BSEventLoop* el, int fd, int mask)
	{
		BSWinSelectData* data = (BSWinSelectData*)m_private_data;
		if (mask & ae_readable)	FD_SET(fd, &data->m_r_set);
		if (mask & ae_writeable) FD_SET(fd, &data->m_w_set);
		return true;
	}

	bool BSMultiplexer::DelEvent(BSEventLoop* el, int fd, int mask)
	{
		BSWinSelectData* data = (BSWinSelectData*)m_private_data;
		if (mask & ae_readable)	FD_CLR(fd, &data->m_r_set);
		if (mask & ae_writeable) FD_CLR(fd, &data->m_w_set);
		return true;
	}

	int BSMultiplexer::Poll(BSEventLoop *el,  struct timeval *tvp)
	{
		BSWinSelectData* data = (BSWinSelectData*)m_private_data;
		memcpy(&data->m_arg_r_set,  &data->m_r_set, sizeof(data->m_r_set));
		memcpy(&data->m_arg_w_set,  &data->m_w_set, sizeof(data->m_w_set));
		int nret = ::select(el->m_maxfd+1, &data->m_arg_r_set, &data->m_arg_w_set, NULL, tvp);
		if (nret > 0)
		{
			int count = 0;
			for (int i = 0; i <= el->m_maxfd; ++i)
			{
				BSFileEvent* fe = &el->m_events[i];
				if (fe->m_mask == ae_none) continue;
				
				int mark = ae_none;
				if (FD_ISSET(i, &data->m_arg_r_set) && (fe->m_mask & ae_readable))
					mark |= ae_readable;
				if (FD_ISSET(i, &data->m_arg_w_set) && (fe->m_mask & ae_writeable))
					mark |= ae_writeable;
				if (mark == ae_none) continue;
				
				BSFiredEvent* fr = &el->m_fired[count++];				
				fr->m_fd = i;
				fr->m_mask = mark;
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