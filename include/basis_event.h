#ifndef _BASIS_EVENT_H_
#define _BASIS_EVENT_H_

#include "basis_define.h"

namespace basis
{

class BSEvent
{
public:
	BSEvent(bool manual = false);

	~BSEvent();

public:
	bool set();

	bool reset();

	bool wait(uint32 ms = -1);

private:
	BSEvent( const BSEvent& );
	BSEvent& operator=( const BSEvent& );

private:
	class BSEventImpl;
	BSEventImpl* m_impl;
};

} //namespace basis

#endif //_BASIS_EVENT_H_

