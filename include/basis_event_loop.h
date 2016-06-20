#ifndef _BASIS_EVENT_LOOP_H_
#define _BASIS_EVENT_LOOP_H_

#include "basis_define.h"
#include "basis_nocopy.h"
#include "basis_multiplexer.h"

namespace basis
{
	enum aeFdState
	{
		ae_none = 0,
		ae_readable = 1,
		ae_writeable = 2,
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
	virtual void onReadable(BSEventLoop *el, int fd) = 0;
};

class BSWriteableCallBack : public BSEventCallBack
{
public:
	virtual void onWriteable(BSEventLoop *el, int fd) = 0;
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
		BSFileEvent() : m_mask(ae_none), m_r_proc(NULL), m_w_proc(NULL) {}
		~BSFileEvent() {}

	private:
		int m_mask; 
		BSReadableCallBack* m_r_proc;
		BSWriteableCallBack* m_w_proc;
	};

	class BSFiredEvent
	{
	public:
		friend class BSEventLoop;
		friend class BSMultiplexer;

	public:
		BSFiredEvent() : m_fd(-1), m_mask(ae_none) {}
		~BSFiredEvent() {}

	private:
		int m_fd;
		int m_mask;
	};

	//////////////////////////////////////////////////////////////////////////
	// class BSEventLoop
	// 循环事件
	//////////////////////////////////////////////////////////////////////////
	class BSEventLoop : public BSNoCopy
	{
	public:
		friend class BSMultiplexer;

	public:
		BSEventLoop();
		~BSEventLoop();
		static BSEventLoop* CreateEventLoop(int setsize);
	
	public:
		void RunLoop();
		void SetStop();

		int	GetSetSize();
		bool ResizeSetSize(int setsize);

		bool	CreateFileEvent(int fd, int mask, BSEventCallBack *proc);
		void	DeleteFileEvent(int fd, int mask); 
		int	GetFileEvent(int fd);

		static int Wait(int fd, int mask, long long milliseconds);
		const char* GetApiName();

	public:
		int	ProcessEvents();

	private:
		int m_maxfd;
		int m_setsize;
		int m_stop;

		vector<BSFileEvent> m_events; // 更适用于linux; windows最好mod4,待优化?????
		vector<BSFiredEvent> m_fired;
		BSMultiplexer* m_multiplexer;
	};  
}
#endif