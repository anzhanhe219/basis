#ifndef _BASIS_OBSERVER_EXT_H_
#define _BASIS_OBSERVER_EXT_H_

#include "basis_observer.h"

namespace basis {


template <class C, class N>
class BSObserverExt: public BSObserver
{
public:
	typedef void (C::*Callback)(N*);

	BSObserverExt(C* object, Callback method): 
		m_object(object), 
		m_callbk(method)
	{
	}
	
	BSObserverExt(const BSObserverExt& observer):
		BSObserver(observer),
		m_object(observer.m_object), 
		m_callbk(observer.m_callbk)
	{
	}
	
	virtual ~BSObserverExt()
	{
	}
	
	BSObserverExt& operator = (const BSObserverExt& observer)
	{
		if (&observer != this)
		{
			m_object = observer.m_object;
			m_callbk  = observer.m_callbk;
		}
		return *this;
	}
	
	void notify(BSNotification* pnotification) const
	{
		if (m_object)
		{
			N* pcastnf = dynamic_cast<N*>(pnotification);
			if (pcastnf)
			{
				(m_object->*m_callbk)(pcastnf);
			}
		}
	}
	
	bool equals(const BSObserver* observer) const
	{
		const BSObserverExt* pobs = dynamic_cast<const BSObserverExt*>(observer);
		return pobs && pobs->m_object == m_object && pobs->m_callbk == m_callbk;
	}

	bool accepts(BSNotification* pnotification) const
	{
		return dynamic_cast<N*>(pnotification) != NULL;
	}
	
	BSObserver* clone() const
	{
		return new BSObserverExt(*this);
	}
	
	void disable()
	{
		m_object = NULL;
	}
	
private:
	BSObserverExt();

	C*       m_object;
	Callback m_callbk;
	// lock ?????
};

} // namespace basis


#endif // _BASIS_OBSERVER_EXT_H_
