#ifndef _BASIS_MUTEX_H_
#define _BASIS_MUTEX_H_

#include "basis_define.h"

namespace basis
{

class MutexImpl;

// Recursion Mutex.
class BSMutex
{
public:
	BSMutex();
	~BSMutex();

	void lock();
	bool try_lock();
	bool try_lock(uint32 ms);
	void unlock();

private:
	MutexImpl* m_impl;
};

#ifdef __POSIX__

// BSFastMutex is not recursion. 
class BSFastMutex
{
public:
	BSFastMutex();
	~BSFastMutex();

	void lock();
	bool try_lock();
	bool try_lock(uint32 ms);
	void unlock();

private:
	MutexImpl* m_impl;
};

#endif // __POSIX__

} //namespace basis

#endif //_BASIS_MUTEX_H_

