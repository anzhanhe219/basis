#include "basis_rw_lock.h"
#include "basis_atomic.h"
#include "basis_mutex.h"
#include "basis_event.h"

namespace basis
{

#ifdef __WINDOWS__
class BSRWLock::SysRWLock
{
	enum {MAX_READ = 1000};
	
public:
	SysRWLock(int _type)
		: m_type(_type)
		, m_readCount(0)
		, m_writeCount(0)
		, m_writeWait(false)
	{
		
	}
	
	~SysRWLock()
	{

	}

	void readLock()
	{
		// �����е��̲߳��������������
		SmartLocker<BSMutex> locker(m_allLock);
		// ��ʱû�������ݹ�����
		// ���߳�����д  ���� ����д�ȴ�
		// �ȴ� ������
		while (m_writeCount || m_writeWait) { sleep(5); }

		// ��Դ���У��������ڽ��ж����в���
		if (0 == m_readCount)
		{
			m_signal.wait();
		}

		++m_readCount;
	}

	void writeLock()
	{
		SmartLocker<BSMutex> locker(m_allLock);
		while (true)
		{
			// û�ж���д����
			if (0 == m_readCount && 0 == m_writeCount)
			{
				++m_writeCount;
				m_signal.wait();
				break;
			}
		}
	}

	bool tryReadLock(uint32 _sec = 0)
	{
		SmartLocker<BSMutex> locker(m_allLock);
		// ��д��������д�ȴ�ʱ�����Խ��ж��ȴ�
		bool isRead = false;
		if (m_writeCount || !m_writeWait)
		{
			isRead = m_signal.wait(_sec);
		}

		// ������Խ��ж�����
		if (isRead)
		{
			++m_readCount;
		}

		return isRead;
	}

	bool tryWriteLock(uint32 _sec = 0)
	{
		SmartLocker<BSMutex> locker(m_allLock);
		// �ж�����д����ʱ������д�ȴ�
		bool tmpRW = false;
		
		if (m_readCount || m_writeWait)
		{
			tmpRW = m_signal.wait(_sec);
		}

		if (tmpRW && (m_writeCount && m_readCount))
		{
			++m_writeCount;
			return true;
		}

		return false;
	}

	void unlockRead()
	{
		SmartLocker<BSMutex> locker(m_allLock);
		VERIFY(0 == m_readCount);

		m_signal.setEvent();
	}

	void unlockWrite()
	{
		SmartLocker<BSMutex> locker(m_allLock);
		VERIFY(0 == m_writeCount);

		m_signal.setEvent();
	}
		
private:
	int m_type; // ��д����
	BSMutex m_allLock; // �������ݵ���
	uint32 m_readCount; // ������
	uint32 m_writeCount; // д����
	BSEvent m_signal; //  ���ź�
	bool m_writeWait; // ����д�ȴ�
};

#endif

#ifdef __POSIX__
class BSRWLock::SysRWLock
{
public:
	SysRWLock(int _type)
	{
		pthread_rwlockattr_t rwAttr;
		pthread_rwlockattr_init(&rwAttr);
		if (READ_PRO == _type)
		{
			VERIFY(!pthread_rwlockattr_setkind_np(&rwAttr, PTHREAD_RWLOCK_PREFER_READER_NP));
		}
		else if (WRITE_PRO == _type)
		{  // ������PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP�� ����д������
			// PTHREAD_RWLOCK_PREFER_WRITER_NP �����ܴﵽд�����ȵ�Ч��
			VERIFY(!pthread_rwlockattr_setkind_np(&rwAttr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP));
		}
		else
		{
			VERIFY(false)
		}
		
		VERIFY(!pthread_rwlock_init(&m_rwl, &rwAttr));
		VERIFY(!pthread_rwlockattr_destroy(&rwAttr));
	}

	~SysRWLock()
	{
		VERIFY(!pthread_rwlock_destroy(&m_rwLock));
	}

	void readLock()
	{
		VERIFY(pthread_rwlock_rdlock(&m_rwLock));
	}

	void writeLock()
	{
		VERIFY(pthread_rwlock_wrlock(&m_rwLock));
	}

	bool tryReadLock()
	{
		return 0 == pthread_rwlock_tryrdlock(&m_rwLock);
	}

	bool tryWriteLock()
	{
		return 0 == pthread_rwlock_trywrlock(&m_rwLock);
	}

private:
	pthread_rwlock_t m_rwLock;
};
#endif

BSRWLock::BSRWLock(int type)
	: m_lock(new SysRWLock(WRITE_PRO))
{

}

}