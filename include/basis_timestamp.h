#ifndef _BASIS_TIME_STAMP_H_
#define _BASIS_TIME_STAMP_H_

#include "basis_define.h"

namespace basis
{
class BSTimeStamp
{
////////////////////////////////////////////////////////////////////////////////
// class BSTimeStamp
////////////////////////////////////////////////////////////////////////////////
public:
	BSTimeStamp();
	BSTimeStamp(time_t time_, int64 usec = 0);
	BSTimeStamp(const timeval& tv);
	BSTimeStamp(const tm& tm_);
	BSTimeStamp(const BSTimeStamp& timestamp);
	~BSTimeStamp();

public:
	bool operator == (const BSTimeStamp& timestamp) const;
	bool operator != (const BSTimeStamp& timestamp) const;
	bool operator >  (const BSTimeStamp& timestamp) const;
	bool operator <  (const BSTimeStamp& timestamp) const;
	bool operator >= (const BSTimeStamp& timestamp) const;
	bool operator <= (const BSTimeStamp& timestamp) const;

private:
	int64 m_usec;
};

inline bool BSTimeStamp::operator==( const BSTimeStamp& timestamp ) const
{
	return (m_usec == timestamp.m_usec);
}

inline bool BSTimeStamp::operator!=( const BSTimeStamp& timestamp ) const
{
	return( m_usec != timestamp.m_usec);
}

inline bool BSTimeStamp::operator <( const BSTimeStamp& timestamp ) const
{
	return(m_usec < timestamp.m_usec);
}

inline bool BSTimeStamp::operator >( const BSTimeStamp& timestamp ) const
{
	return(m_usec > timestamp.m_usec);
}

inline bool BSTimeStamp::operator >=( const BSTimeStamp& timestamp ) const
{
	return(m_usec >= timestamp.m_usec);
}

inline bool BSTimeStamp::operator <=( const BSTimeStamp& timestamp ) const
{
	return (m_usec <= timestamp.m_usec);
}

}
using namespace basis;

#endif // _BASIS_TIME_STAMP_H
