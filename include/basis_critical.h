#ifndef _BASIS_CRITICAL_H_
#define _BASIS_CRITICAL_H_

#include "basis_define.h"

namespace basis
{

class BSCritical
{
public:
	BSCritical();

	~BSCritical();
	
public:	
	void lock();

	bool try_lock();

	void unlock();

private:
	class CriticalImpl;
	CriticalImpl* m_impl;
};

} //namespace basis

#endif //_BASIS_ATOMIC_

