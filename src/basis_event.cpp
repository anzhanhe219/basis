#include "basis_event.h"
#include "basis_checked_delete.h"

namespace basis 
{

#ifdef __WINDOWS__

class BSEvent::BSEventImpl
{
public:
	~BSEventImpl();

private:
	friend class BSEvent;

	BSEventImpl( bool manual );

	bool setEvent();

	bool resetEvent();

	bool pulseEvent();

	bool wait( uint32 ms );

private:
	HANDLE m_handle;
};

BSEvent::BSEventImpl::BSEventImpl( bool manual )
{
	m_handle = CreateEvent(0, manual, false, 0);
	ASSERT(m_handle);
}

BSEvent::BSEventImpl::~BSEventImpl()
{
	VERIFY(CloseHandle(m_handle));
}

bool BSEvent::BSEventImpl::setEvent()
{
	return SetEvent(m_handle) ? true : false;
}

bool BSEvent::BSEventImpl::resetEvent()
{
	return ResetEvent(m_handle) ? true : false;
}

bool BSEvent::BSEventImpl::wait( uint32 ms )
{
	switch (WaitForSingleObject(m_handle, ms))
	{
	case WAIT_OBJECT_0:
		return true;
	case WAIT_TIMEOUT:
		return false;
	case WAIT_FAILED:
	default:
		ASSERT(false);
		break;
	}
	return false;
}

#endif//__WINDOWS__

#ifdef __POSIX__

class BSEvent::BSEventImpl
{
public:
	~BSEventImpl();

private:
	friend class BSEvent;

	BSEventImpl( bool manual );

	bool setEvent();

	bool resetEvent();

	bool pulseEvent();

	bool wait( uint32 ms );

private:
	bool m_manual;
	bool m_variable; 
	pthread_cond_t m_cond;
	pthread_mutex_t m_mutex;
};

BSEvent::BSEventImpl::BSEventImpl( bool manual )
	: m_manual(manual)
	, m_variable(false)
{
	// init mutex
	pthread_mutex_init(&m_mutex, NULL);
	// init cond
	pthread_cond_init(&m_cond, NULL);
}

BSEvent::BSEventImpl::~BSEventImpl()
{
	// destroy cond
	pthread_cond_destroy(&m_cond);
	// destroy mutex
	pthread_mutex_destroy(&m_mutex);
}

bool BSEvent::BSEventImpl::setEvent()
{
	pthread_mutex_lock(&m_mutex);
	m_variable = true;
	pthread_mutex_unlock(&m_mutex);
	if (!m_manual)
	{
		pthread_cond_signal(&m_cond);
	}
	else
	{
		pthread_cond_broadcast(&m_cond);
	}
	return true;
}

bool BSEvent::BSEventImpl::resetEvent()
{
	pthread_mutex_lock(&m_mutex);
	m_variable = false;
	pthread_mutex_unlock(&m_mutex);
	return true;
}

bool BSEvent::BSEventImpl::wait( uint32 ms )
{
	if (ms >= 0)
	{
		// set time
		struct timespec tsp;
		struct timeval  now;
		gettimeofday(&now, NULL);
		uint32 scn = ms / 1000;
		ms = now.tv_usec * 1000;
		tsp.tv_sec = now.tv_sec + scn + ms / 1000;
		tsp.tv_nsec = ms % 1000;

		pthread_mutex_lock(&m_mutex);
		while (!m_variable)
		{
			int result = pthread_cond_timedwait(&m_cond, &m_mutex, &tsp);
			if (result == ETIMEDOUT)
			{
				pthread_mutex_unlock(&m_mutex);
				return false;
			}
		}
		if (!m_manual)
		{
			m_variable = false;
		}
		pthread_mutex_unlock(&m_mutex);
		return true;
	}
	else
	{
		pthread_mutex_lock(&m_mutex);
		while (!m_variable)
		{
			pthread_cond_wait(&m_cond, &m_mutex);
		}
		if (!m_manual)
		{
			m_variable = false;
		}
		pthread_mutex_unlock(&m_mutex);
		return true;
	}	
}

#endif// __POSIX__

BSEvent::BSEvent( bool manual /*= false*/ )
	: m_impl(new(nothrow) BSEventImpl(manual))
{
	ASSERT(m_impl);
}

BSEvent::~BSEvent()
{
	checked_delete(m_impl);
}

bool BSEvent::set()
{
	if (m_impl == NULL) return false;
	return m_impl->setEvent();
}

bool BSEvent::reset()
{
	if (m_impl == NULL) return false;
	return m_impl->resetEvent();
}

bool BSEvent::wait( uint32 ms /*=-1*/ ) 
{
	if (m_impl == NULL) return false;
	return m_impl->wait(ms);
}

}//namespace basis