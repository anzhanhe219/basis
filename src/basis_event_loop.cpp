#include "basis_event_loop.h"

namespace basis
{
	//////////////////////////////////////////////////////////////////////////
	// class BSEventLoop
	// 循环事件
	//////////////////////////////////////////////////////////////////////////
	BSEventLoop::BSEventLoop()
		: m_stop (0)
		, m_multiplexer(NULL)
		, m_accept_proc(NULL)
		, m_read_proc(NULL)
		, m_write_proc(NULL)
		, m_connected_proc(NULL)
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

	bool BSEventLoop::ResizeSetSize(int setsize)
	{
		return m_multiplexer->Resize(setsize);
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
		int count = m_multiplexer->Poll(this, NULL);
		for (int i = 0; i < count; ++i)
		{
			BSFiredEvent* fr = m_multiplexer->GetFiredEvents((uint32)i);
			CHECKPTR(fr);
			BSFileEvent* fe = fr->m_file_event;
			CHECKPTR(fe);

			// accept处理
			if ((fe->m_mask & ae_accept_fd) && (fr->m_mask & ae_readable))
			{
				if (m_accept_proc == NULL) UNEXPECT();
				m_accept_proc->onReadable(this, fr->m_fd, fe);
				continue;
			}

			// connected处理
			if ((fe->m_mask & ae_connet_fd) && (fr->m_mask & ae_writeable))
			{
				if (m_connected_proc == NULL) UNEXPECT();
				m_connected_proc->onWriteable(this, fr->m_fd, fe);
				continue;
			}

			bool proccess_flag = false;
			// readable处理
			if ((fe->m_mask & ae_read_fd) && (fr->m_mask & ae_readable))
			{
				if (m_read_proc == NULL) UNEXPECT();
				m_read_proc->onReadable(this, fr->m_fd, fe);
				proccess_flag = true;
			}
			// writeable处理
			if ((fe->m_mask & ae_write_fd) && (fr->m_mask & ae_writeable))
			{
				if (m_write_proc == NULL) UNEXPECT();
				m_write_proc->onWriteable(this, fr->m_fd, fe);
				proccess_flag = true;
			}
			if (proccess_flag) continue;
			UNEXPECT();
		}
		return count;		
	}

	bool BSEventLoop::RegisterAcceptEvent(int fd, BSFileEvent* arg)
	{
		if (arg == NULL) return false;
		if (m_accept_proc == NULL) return false;
		if (arg->m_mask != ae_empty_fd) return false;
		if (!m_multiplexer->AddEvent(fd, ae_readable, arg)) return false;
		arg->m_mask |= ae_accept_fd;
		return true;
	}

	bool BSEventLoop::RegisterConnectedEvent(int fd, BSFileEvent* arg)
	{
		if (arg == NULL) return false;
		if (m_connected_proc == NULL) return false;
		if (arg->m_mask != ae_empty_fd) return false; 
		if (!m_multiplexer->AddEvent(fd, ae_writeable, arg)) return false;
		arg->m_mask |= ae_connet_fd;
		return true;
	}

	bool BSEventLoop::RegisterReadEvent(int fd, BSFileEvent* arg)
	{
		if (arg == NULL)  return false;
		if (m_read_proc == NULL) return false;
		if (arg->m_mask & (ae_connet_fd|ae_accept_fd)) return false; 
		if (!m_multiplexer->AddEvent(fd, ae_readable, arg)) return false;
		arg->m_mask |= ae_read_fd;
		return true;
	}

	bool BSEventLoop::RegisterWriteEvent(int fd, BSFileEvent* arg)
	{
		if (arg == NULL)  return false;
		if (m_read_proc == NULL) return false;
		if (arg->m_mask & (ae_connet_fd|ae_accept_fd)) return false; 
		if (!m_multiplexer->AddEvent(fd, ae_writeable, arg)) return false;
		arg->m_mask |= ae_write_fd;
		return true;
	}

	void BSEventLoop::UnregisterAcceptEvent(int fd, BSFileEvent* arg)
	{
		if (arg == NULL) return;
		ASSERT(arg->m_mask == ae_accept_fd);
		arg->m_mask = ae_empty_fd;
		m_multiplexer->DelEvent(fd, ae_readable, arg);
		return;
	}

	void BSEventLoop::UnregisterConnectedEvent(int fd, BSFileEvent* arg)
	{
		if (arg == NULL) return;
		ASSERT(arg->m_mask == ae_connet_fd);
		arg->m_mask = ae_empty_fd;
		m_multiplexer->DelEvent(fd, ae_writeable, arg);
		return;
	}

	void BSEventLoop::UnregisterReadEvent(int fd, BSFileEvent* arg)
	{
		if (arg == NULL) return;
		ASSERT(arg->m_mask & ae_read_fd);
		arg->m_mask &= ~ae_read_fd;
		m_multiplexer->DelEvent(fd, ae_readable, arg);
		return;
	}

	void BSEventLoop::UnregisterWriteEvent(int fd, BSFileEvent* arg)
	{
		if (arg == NULL) return;
		ASSERT(arg->m_mask & ae_write_fd);
		arg->m_mask &= ~ae_write_fd;
		m_multiplexer->DelEvent(fd, ae_writeable, arg);
		return;
	}

}
