#ifndef _BASIS_THREAD_H_
#define _BASIS_THREAD_H_

#include "basis_define.h"
#include "basis_runnable.h"

namespace basis
{

class BSThread : public BSRunnable
{
public:
	BSThread(const string& name);
	~BSThread();

public:
	bool start(BSRunnable* runnable = NULL);
	void stop();
	void join();
	bool wait_quit(uint32 ms);

public:
	bool started();
	uint32 thread_id();
	const string& name();

public:
	virtual void run(BSThread* pthread) {}

private:
	BSThread( const BSThread& );
	BSThread& operator=( const BSThread& );

private:
	class BSThreadImpl;
	BSThreadImpl* m_impl;
};

uint32 thread_id();

} //namespace basis

#endif //_BASIS_THREAD_H_

