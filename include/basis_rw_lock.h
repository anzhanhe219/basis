#ifndef _BASIS_RW_LOCK_H_
#define _BASIS_RW_LOCK_H_

#include "basis_define.h"
#include "basis_nocopy.h"

// ¶ÁÐ´Ëø£¨·ÇµÝ¹é£©
namespace basis
{

//////////////////////////////////////////////////////////////////////////
class BSRWLock : public BSNoCopy 
{
	enum LOCK_PRO
	{
		READ_PRO,
		WRITE_PRO
	};
public:
	BSRWLock(int type = WRITE_PRO);
	~BSRWLock();
	void readLock();
	void writeLock();
	void unlockRead();
	void unlockWrite();
	bool tryReadLock();
	bool tryWriteLock();

private:
	class SysRWLock;
	SysRWLock* m_lock;
	int m_type;
};
}

#endif