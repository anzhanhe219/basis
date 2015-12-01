#include "basis_rw_lock.h"
#include "basis_event.h"
#include "basis_mutex.h"
#include "basis_critical.h"

namespace basis
{
#ifdef __WINDOWS__
	class BSRWLock::SysRWLock
	{	
	public:
		SysRWLock(int _type)
			: m_type(_type)
			, m_readCount(0)
			, m_writeCount(0)
			, m_rWait(0)
			, m_wWait(0)
			, m_signal(true) // ��Ϊ�����ź�
		{
		}

		~SysRWLock()
		{
			//ASSERT((!m_rWait && !m_wWait));
			//ASSERT(!total() && "the rw locker is currently locked");
			//ASSERT(!m_writeThread && "the rw locker is currently locked");
		}

		void readLock()
		{
			SmartLocker<BSCritical> locker(m_allLock);

			//ASSERT(xcore::thread_id() != m_writeThread && "have write lock, can't read lock");
			//ASSERT(!readCount(xcore::thread_id()) && "rw locker can't recurrence lock");

			// ������д״̬ ��������д�ȴ�
			if (/*m_isWrite*/m_writeCount || (WRITE_PRO == m_type && m_wWait))
			{
				++m_rWait;
				m_signal.reset(); // ����Ϊ���ź�״̬,Ϊ�˺����ȴ�
				locker.unlock();
				m_signal.wait(); // �ȴ���ȡ��Դ
				locker.lock();
				--m_rWait;
				locker.unlock();
			}

			locker.lock();
			//readInsert(xcore::thread_id());
			++m_readCount; 
			locker.unlock();

		}

		bool tryReadLock(uint32 _sec)
		{
			SmartLocker<BSCritical> locker(m_allLock);
			if (WRITE_PRO == m_type && m_wWait) return false;

			//ASSERT(xcore::thread_id() != m_writeThread && "have write lock, can't read lock");
			//ASSERT(!readCount(xcore::thread_id()) && "rw locker can't recurrence lock");

			// ���д״̬ ������ д�ȴ�
			if (m_writeCount || (WRITE_PRO == m_type && m_wWait))
			{
				locker.unlock(); // �ſ�m_isWrite״̬����ֹ����

				m_signal.wait(_sec);

				locker.lock();
				// ��Ȼ��д״̬
				if (m_writeCount) return false;
				else ++m_readCount;
			}

			return true;
		}

		void unlockRead()
		{
			SmartLocker<BSCritical> locker(m_allLock);

			if (m_readCount > 0)
			{
				--m_readCount;
				//ASSERT(readErase(xcore::thread_id()), true);
				if (!m_readCount) 
				{
					if (m_wWait || m_rWait)
					{
						m_signal.set();
					}
				}
			}
		}

		void writeLock()
		{	
			
			SmartLocker<BSCritical> locker(m_allLock);

			//ASSERT(xcore::thread_id() != m_writeThread && "have write lock, can't read lock");
			//ASSERT(!readCount(xcore::thread_id()) && "rw locker can't recurrence lock");

			while (m_readCount || m_writeCount)
			{
				++m_wWait; // ����д�ȴ�
				m_signal.reset(); // Ϊ�˷�ֹ�źŵĻ��ң���������Ϊ���ź�״̬
				locker.unlock();
				m_signal.wait(); // �ȴ��ź�
				locker.lock();
				--m_wWait;
				locker.unlock();
			}

			// ����״̬
			locker.lock();
			//setWriteThreadId(xcore::thread_id())
			++m_writeCount;
			locker.unlock();
		}

		bool tryWriteLock(uint32 _sec = 0)
		{
			SmartLocker<BSCritical> locker(m_allLock);

			//ASSERT(xcore::thread_id() != m_writeThread && "have write lock, can't read lock");
			//ASSERT(!readCount(xcore::thread_id()) && "rw locker can't recurrence lock");

			// ����״̬
			if (m_readCount || m_writeCount)
			{
				locker.unlock();
				m_signal.wait(_sec);
				locker.lock();
				if (m_readCount || m_writeCount)
				{
					return false;
				}
				else
				{
					++m_writeCount;
				}
			}

			return true;
		}

		void unlockWrite()
		{
			SmartLocker<BSCritical> locker(m_allLock);
			if (m_writeCount)
			{
				--m_writeCount;
				if (!m_writeCount)
				{
					// д�߳�����
					setWriteThreadId(0);
					if (m_wWait > 0 || m_rWait > 0)
					{
						m_signal.set();
					}
				}
			}		
		}

		void unlock()
		{
			unlockRead();
			unlockWrite();
		}

	private:
		int m_type; // ��д����
		BSCritical m_allLock; // �������ݵ���
		uint32 m_readCount; // ������
		uint32 m_writeCount; // д����
		uint32 m_rWait; // ���ȴ�����
		uint32 m_wWait; // д�ȴ�����
		BSEvent m_signal; //  ��д�ź�

#ifdef __DEBUG__
		uint32 m_writeThread; // д�߳�id
		set<uint32> m_readThreadList; // ���߳��б�
		BSCritical m_thread_lock;

		void setWriteThreadId(uint32 threadId)
		{
			m_writeThread = threadId;
		}

		uint32 readCount(uint32 id)
		{
			SmartLocker<BSCritical> lock_(m_thread_lock);
			return (uint32)m_readThreadList.count(id);
		}
		uint32 readTotal()
		{
			SmartLocker<BSCritical> lock_(m_thread_lock);
			return (uint32)m_readThreadList.size();
		}
		void readInsert(uint32 id)
		{
			SmartLocker<BSCritical> lock_(m_thread_lock);
			m_readThreadList.insert(id);
		}
		void readErase(uint32 id)
		{
			SmartLocker<BSCritical> lock_(m_thread_lock);
			m_readThreadList.erase(id);
		}

#endif
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

	BSRWLock::~BSRWLock()
	{
		if (NULL != m_lock)
		{
			delete m_lock;
		}
	}

	void BSRWLock::readLock()
	{
		if (NULL != m_lock)
		{
			m_lock->readLock();
		}
	}

	bool BSRWLock::tryReadLock(uint32 _sec)
	{
		if (NULL != m_lock)
		{
			return m_lock->tryReadLock(_sec);
		}

		return false;
	}

	bool BSRWLock::tryWriteLock(uint32 _sec)
	{
		if (NULL != m_lock)
		{
			return m_lock->tryReadLock(_sec);
		}

		return false;
	}

	void BSRWLock::writeLock()
	{
		if (NULL != m_lock)
		{
			m_lock->writeLock();
		}
	}

	void BSRWLock::unlockRead()
	{
		if (NULL != m_lock)
		{
			m_lock->unlockRead();
		}
	}

	void BSRWLock::unlockWrite()
	{
		if (NULL != m_lock)
		{
			m_lock->unlockWrite();
		}
	}

	void BSRWLock::unlock()
	{
		unlockRead();
		unlockWrite();
	}

}