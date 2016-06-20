#ifndef _BASIS_SUBJECT_H_
#define _BASIS_SUBJECT_H_

#include "basis_define.h"
#include "basis_observer.h"

namespace basis {


class BSSubject
{
public:
	BSSubject();
	~BSSubject();

	void register_observer(const BSObserver& observer);
	void remove_observer(const BSObserver& observer);
	
	bool has_observers() const;
	uint32 count_observers() const;
	bool has_observer(const BSObserver& observer) const;
	
	void notify_observers(BSNotification* pnotification);

private:
	list<BSObserver*> m_observers;
	// lock ?????
};

} // namespace basis

#endif // _BASIS_SUBJECT_H_
