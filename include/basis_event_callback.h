#ifndef _BASIS_EVENT_CALLBACK_H_
#define _BASIS_EVENT_CALLBACK_H_

#include "basis_define.h"

namespace basis
{
	enum FdState
	{
		fs_none = 0x00,
		fs_readable = 0x01,
		fs_writeable = 0x02,
	};

	enum FdLogic
	{
		fl_empty = 0x0000,
		fl_accept = 0x0001,
		fl_read = 0x0002,
		fl_write = 0x0004,
		fl_connet = 0x0008,
	};

	//////////////////////////////////////////////////////////////////////////
	// class BSFdPartner
	// 描述符配套
	//////////////////////////////////////////////////////////////////////////
	class BSFdPartner 
	{
	public:
		friend class BSEventLoop;
		friend class BSMultiplexer;

	public:
		BSFdPartner() : m_logic_mask(fl_empty), m_arg(NULL) {}
		virtual ~BSFdPartner() {}

		void SetArg(void* arg) { m_arg = arg; }
		void* GetArg() { return m_arg; }

	private:
		int m_logic_mask; // FdLogicType
		void* m_arg;
	};

	class BSFiredFd
	{
	public:
		friend class BSEventLoop;
		friend class BSMultiplexer;

	public:
		BSFiredFd() : m_fd(-1), m_state_mask(fs_none), m_partner(NULL) {}
		~BSFiredFd() {}

	private:
		int m_fd;
		int m_state_mask; // FdState
		BSFdPartner* m_partner;
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
	virtual void onReadable(BSEventLoop *el, int fd, BSFdPartner* arg) = 0;
};

class BSWriteableCallBack : public BSEventCallBack
{
public:
	virtual void onWriteable(BSEventLoop *el, int fd, BSFdPartner* arg) = 0;
};

class BSTimerCallBack
{
public:
	virtual int   onTimeOut(BSEventLoop *el, long long id) = 0;
	virtual void onFinalizer(BSEventLoop *el, long long id) = 0;
};

}
#endif