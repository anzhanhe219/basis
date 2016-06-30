#ifndef _BASIS_EVENT_LOOP_H_
#define _BASIS_EVENT_LOOP_H_

#include "basis_define.h"
#include "basis_nocopy.h"
#include "basis_event_callback.h"
#include "basis_multiplexer.h"

namespace basis {

	//////////////////////////////////////////////////////////////////////////
	// class BSEventLoop
	// Ñ­»·ÊÂ¼þ
	//////////////////////////////////////////////////////////////////////////
	class BSEventLoop : public BSNoCopy
	{
	public:
		friend class BSMultiplexer;

	public:
		~BSEventLoop();
		static BSEventLoop* CreateEventLoop(int setsize);

	public:
		void SetAcceptCallBack(BSReadableCallBack* accept_cb) { m_accept_proc = accept_cb; }
		void SetReadableCallBack(BSReadableCallBack* read_cb) { m_read_proc = read_cb; }
		void SetWriteableCallBack(BSWriteableCallBack* write_cb) { m_write_proc = write_cb; }
		void SetConnctedCallBack(BSWriteableCallBack* connected_cb) { m_connected_proc = connected_cb; }

	public:
		bool RegisterAcceptEvent(int fd,  BSFdPartner* partner);
		bool RegisterConnectedEvent(int fd, BSFdPartner* partner);
		bool RegisterReadEvent(int fd, BSFdPartner* partner);
		bool RegisterWriteEvent(int fd, BSFdPartner* partner);

		void UnregisterAcceptEvent(int fd, BSFdPartner* partner);
		void UnregisterConnectedEvent(int fd, BSFdPartner* partner);
		void UnregisterReadEvent(int fd, BSFdPartner* partner);
		void UnregisterWriteEvent(int fd, BSFdPartner* partner);

	public:
		void RunLoop();
		void SetStop();

		static int Wait(int fd, int mask, long long milliseconds);
		const char* GetApiName();

	public:
		int	ProcessEvents();

	private:
		BSEventLoop();
		
		int m_stop;
		BSMultiplexer* m_multiplexer;
		BSReadableCallBack* m_accept_proc;
		BSReadableCallBack* m_read_proc;
		BSWriteableCallBack* m_write_proc;
		BSWriteableCallBack* m_connected_proc;
	};  
}
#endif