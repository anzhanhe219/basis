#ifndef _BASIS_EVENT_CALLBACK_H_
#define _BASIS_EVENT_CALLBACK_H_

#include "basis_define.h"

namespace basis
{
	enum FdState
	{
		ae_none = 0x00,
		ae_readable = 0x01,
		ae_writeable = 0x02,
	};

	enum FdLogicType
	{
		ae_empty_fd = 0x0000,
		ae_accept_fd = 0x0001,
		ae_read_fd = 0x0002,
		ae_write_fd = 0x0004,
		ae_connet_fd = 0x0008,
	};

//////////////////////////////////////////////////////////////////////////
// class BSEventCallBack
// 事件回调
//////////////////////////////////////////////////////////////////////////
class BSEventLoop;
class BSEventCallBack {};

class BSReadableCallBack : public BSEventCallBack
{
public:
	virtual void onReadable(BSEventLoop *el, int fd, void* arg) = 0;
};

class BSWriteableCallBack : public BSEventCallBack
{
public:
	virtual void onWriteable(BSEventLoop *el, int fd, void* arg) = 0;
};

class BSTimerCallBack
{
public:
	virtual int   onTimeOut(BSEventLoop *el, long long id) = 0;
	virtual void onFinalizer(BSEventLoop *el, long long id) = 0;
};

//////////////////////////////////////////////////////////////////////////
// class BSEventDefine
// 事件类型定义
//////////////////////////////////////////////////////////////////////////
	class BSFileEvent 
	{
	public:
		friend class BSEventLoop;
		friend class BSMultiplexer;

	public:
		BSFileEvent() : m_mask(ae_none), m_arg(NULL){}
		~BSFileEvent() {}

	private:
		int m_mask; 
		void* m_arg;
	};

	class BSFiredEvent
	{
	public:
		friend class BSEventLoop;
		friend class BSMultiplexer;

	public:
		BSFiredEvent() : m_fd(-1), m_mask(ae_none), m_file_event(NULL) {}
		~BSFiredEvent() {}

	private:
		int m_fd;
		int m_mask;
		BSFileEvent* m_file_event;
	};
}
#endif