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

	int BSEventLoop::Wait(int fd, int mask, long long milliseconds)
	{
		FD_SET r_set, w_set;
		FD_ZERO(&r_set);
		FD_ZERO(&w_set);
		if (mask & fs_readable)
		{
			FD_SET(fd, &r_set);
		}
		if (mask & fs_writeable)
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
				result |= fs_readable;
			}
			if (FD_ISSET(fd, &w_set))
			{
				result |= fs_writeable;
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
			BSFiredFd* fr = m_multiplexer->GetFiredFd((uint32)i);
			CHECKPTR(fr);
			BSFdPartner* fp = fr->m_partner;
			CHECKPTR(fp);

			// accept处理
			if ((fp->m_logic_mask & fl_accept) && (fr->m_state_mask & fs_readable))
			{
				if (m_accept_proc == NULL) UNEXPECT();
				m_accept_proc->onReadable(this, fr->m_fd, fp);
				continue;
			}

			// connected处理
			if ((fp->m_logic_mask & fl_connet) && (fr->m_state_mask & fs_writeable))
			{
				if (m_connected_proc == NULL) UNEXPECT();
				m_connected_proc->onWriteable(this, fr->m_fd, fp);
				continue;
			}

			bool proccess_flag = false;
			// readable处理
			if ((fp->m_logic_mask & fl_read) && (fr->m_state_mask & fs_readable))
			{
				if (m_read_proc == NULL) UNEXPECT();
				m_read_proc->onReadable(this, fr->m_fd, fp);
				proccess_flag = true;
			}
			// writeable处理
			if ((fp->m_logic_mask & fl_write) && (fr->m_state_mask & fs_writeable))
			{
				if (m_write_proc == NULL) UNEXPECT();
				m_write_proc->onWriteable(this, fr->m_fd, fp);
				proccess_flag = true;
			}
			if (proccess_flag) continue;
			UNEXPECT();
		}
		return count;		
	}

	bool BSEventLoop::RegisterAcceptEvent(int fd, BSFdPartner* partner)
	{
		if (partner == NULL) return false;
		if (m_accept_proc == NULL) return false;
		if (partner->m_logic_mask != fl_empty) return false;
		if (!m_multiplexer->AddEvent(fd, fs_readable, partner)) return false;
		partner->m_logic_mask |= fl_accept;
		return true;
	}

	bool BSEventLoop::RegisterConnectedEvent(int fd, BSFdPartner* partner)
	{
		if (partner == NULL) return false;
		if (m_connected_proc == NULL) return false;
		if (partner->m_logic_mask != fl_empty) return false; 
		if (!m_multiplexer->AddEvent(fd, fs_writeable, partner)) return false;
		partner->m_logic_mask |= fl_connet;
		return true;
	}

	bool BSEventLoop::RegisterReadEvent(int fd, BSFdPartner* partner)
	{
		if (partner == NULL)  return false;
		if (m_read_proc == NULL) return false;
		if (partner->m_logic_mask & (fl_connet|fl_accept)) return false; 
		if (!m_multiplexer->AddEvent(fd, fs_readable, partner)) return false;
		partner->m_logic_mask |= fl_read;
		return true;
	}

	bool BSEventLoop::RegisterWriteEvent(int fd, BSFdPartner* partner)
	{
		if (partner == NULL)  return false;
		if (m_read_proc == NULL) return false;
		if (partner->m_logic_mask & (fl_connet|fl_accept)) return false; 
		if (!m_multiplexer->AddEvent(fd, fs_writeable, partner)) return false;
		partner->m_logic_mask |= fl_write;
		return true;
	}

	void BSEventLoop::UnregisterAcceptEvent(int fd, BSFdPartner* partner)
	{
		if (partner == NULL) return;
		ASSERT(partner->m_logic_mask == fl_accept);
		partner->m_logic_mask = fl_empty;
		m_multiplexer->DelEvent(fd, fs_readable, partner);
		return;
	}

	void BSEventLoop::UnregisterConnectedEvent(int fd, BSFdPartner* partner)
	{
		if (partner == NULL) return;
		ASSERT(partner->m_logic_mask == fl_connet);
		partner->m_logic_mask = fl_empty;
		m_multiplexer->DelEvent(fd, fs_writeable, partner);
		return;
	}

	void BSEventLoop::UnregisterReadEvent(int fd, BSFdPartner* partner)
	{
		if (partner == NULL) return;
		ASSERT(partner->m_logic_mask & fl_read);
		partner->m_logic_mask &= ~fl_read;
		m_multiplexer->DelEvent(fd, fs_readable, partner);
		return;
	}

	void BSEventLoop::UnregisterWriteEvent(int fd, BSFdPartner* partner)
	{
		if (partner == NULL) return;
		ASSERT(partner->m_logic_mask & fl_write);
		partner->m_logic_mask &= ~fl_write;
		m_multiplexer->DelEvent(fd, fs_writeable, partner);
		return;
	}

}
