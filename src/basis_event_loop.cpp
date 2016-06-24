#include "basis_event_loop.h"


namespace basis
{
	//////////////////////////////////////////////////////////////////////////
	// class BSEventLoop
	// 循环事件
	//////////////////////////////////////////////////////////////////////////
	BSEventLoop::BSEventLoop()
		: m_maxfd(-1)
		, m_setsize(0)
		, m_stop (0)
		, m_multiplexer(NULL)
	{
	}

	BSEventLoop::~BSEventLoop()
	{
		delete m_multiplexer;
		m_multiplexer = NULL;
	}

	basis::BSEventLoop* BSEventLoop::CreateEventLoop(int setsize)
	{		
		if (BSEventLoop* el = new BSEventLoop())
		{
			el->m_setsize = setsize;
			el->m_events.resize(setsize);
			el->m_fired.resize(setsize);
			el->m_multiplexer = BSMultiplexer::Create(setsize);
			if (el->m_multiplexer) return el;
			delete el;
		}
		return NULL;
	}

	void BSEventLoop::RunLoop()
	{
		while (!m_stop)
		{
			ProcessEvents();
		}
	}

	void BSEventLoop::SetStop()
	{
		m_stop = 1;
	}

	int BSEventLoop::GetSetSize()
	{
		return m_setsize;
	}

	bool BSEventLoop::ResizeSetSize(int setsize)
	{
		if (setsize == m_setsize) return true;
		if (setsize <= m_maxfd) return false;
		if (!m_multiplexer->Resize(setsize)) return false;
		
		m_setsize = setsize;
		m_events.resize(setsize);
		m_fired.resize(setsize);
		return true;
	}

	bool BSEventLoop::CreateFileEvent(int fd, int mask, BSEventCallBack *proc)
	{
		if (proc == NULL) return false;
		if (fd >= m_setsize) return false;

		BSFileEvent* fe = GetElement(fd);
		if (fe == NULL)
		{
			return false;
		}

		if (!m_multiplexer->AddEvent(this, fd, mask))
		{
			return false;
		}

		fe->m_mask |= mask;
		if (mask & ae_readable)
		{
			fe->m_r_proc = (BSReadableCallBack*)proc;
		}
		if (mask & ae_writeable)
		{
			fe->m_w_proc =  (BSWriteableCallBack*)proc;
		}
		
		if (fd > m_maxfd) m_maxfd = fd;
		return true;
	}

	void BSEventLoop::DeleteFileEvent(int fd, int mask)
	{
		if (fd > m_maxfd) return;

		BSFileEvent *fe = GetElement(fd);
		if (fe == NULL) return;		
		if (fe->m_mask == ae_none)  return;

		fe->m_mask &= ~mask;
		if (fd == m_maxfd && fe->m_mask == ae_none)
		{
			int t_max = m_maxfd-1;
			for ( ; t_max >= 0 ; --t_max)
				if ((fe = GetElement(t_max)) && (fe->m_mask != ae_none)) break;
			m_maxfd = t_max;
		}

		m_multiplexer->DelEvent(this, fd, mask);
	}

	int BSEventLoop::GetFileEvent(int fd)
	{
		if (BSFileEvent *fe = GetElement(fd))
		{
			return fe->m_mask;
		}
		return ae_none;
	}

	int BSEventLoop::Wait(int fd, int mask, long long milliseconds)
	{
		FD_SET r_set, w_set;
		FD_ZERO(&r_set);
		FD_ZERO(&w_set);
		if (mask & ae_readable)
		{
			FD_SET(fd, &r_set);
		}
		if (mask & ae_writeable)
		{
			FD_SET(fd, &w_set);
		}

		struct  timeval *pval = NULL, val;
		if (milliseconds >= 0)
		{
			val.tv_sec = (int32)(milliseconds / 1000);
			val.tv_usec = (int32)(milliseconds % 1000);
			pval = &val;
		}
		int result = ::select(fd+1, &r_set, &w_set, NULL, pval);
		if (result > 0)
		{
			result = 0;
			if (FD_ISSET(fd, &r_set))
			{
				result |= ae_readable;
			}
			if (FD_ISSET(fd, &w_set))
			{
				result |= ae_writeable;
			}
		}
		else if (result < 0)
		{
			result = -1;
		}
		return result;
	}

	const char* BSEventLoop::GetApiName()
	{
		return m_multiplexer->ApiName();
	}

	int BSEventLoop::ProcessEvents()
	{
		if (m_maxfd == -1) return 0;
		int count = m_multiplexer->Poll(this, NULL);

		for (int i = 0; i < count; ++i)
		{
			BSFiredEvent* fr = &m_fired[i];
			BSFileEvent* fe = GetElement(fr->m_fd);
			CHECKPTR(fe);
			if (fe == NULL) continue;

			// 先处理读事件 再处理写事件  同时有读写并且回调函数一样的只执行一个
			bool p_flag = false;
			if (fe->m_mask & fr->m_mask & ae_readable)
			{
				fe->m_r_proc->onReadable(this, fr->m_fd);
			}
			if (fe->m_mask & fr->m_mask & ae_writeable)
			{
				if (!p_flag || (void*)fe->m_r_proc != (void*)fe->m_w_proc)
				{
					fe->m_w_proc->onWriteable(this, fr->m_fd);
				}
			}
		}
		return count;		
	}

	BSFileEvent* BSEventLoop::GetElement(int fd)
	{
#ifdef __WINDOWS__
		int mod = fd % 4;
		if (mod != 0) return NULL;
		if (mod >= (int)m_events.size()) return NULL;
		return &m_events[mod];
#endif //__WINDOWS__

#ifdef __POSIX__
		if (fd >= (int)m_events.size()) return NULL;
		return &m_events[fd];
#endif // __POSIX__

		return NULL;
	}

}
