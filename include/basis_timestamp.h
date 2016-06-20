#ifndef _BASIS_TIME_STAMP_H_
#define _BASIS_TIME_STAMP_H_

#include "basis_define.h"
#include "basis_timespan.h"

namespace basis
{
class BSTimeStamp
{
////////////////////////////////////////////////////////////////////////////////
// class BSTimeStamp
////////////////////////////////////////////////////////////////////////////////
public:
	BSTimeStamp();
	BSTimeStamp(time_t time_, int32 usec = 0);
	BSTimeStamp(const timeval& tv);
	BSTimeStamp(const tm& tm_);
	BSTimeStamp(const BSTimeStamp& timestamp);
	~BSTimeStamp();

	void set(const tm& tm_);
	int64 get();

public:
	bool operator == (const BSTimeStamp& timestamp) const;
	bool operator != (const BSTimeStamp& timestamp) const;
	bool operator >  (const BSTimeStamp& timestamp) const;
	bool operator <  (const BSTimeStamp& timestamp) const;
	bool operator >= (const BSTimeStamp& timestamp) const;
	bool operator <= (const BSTimeStamp& timestamp) const;

	BSTimeStamp& operator = (const BSTimeStamp& timestamp);
	BSTimeStamp& operator = (int64 timestamp);
	BSTimeStamp& operator +=(const BSTimeSpan& timespan);
	BSTimeStamp& operator -=(const BSTimeSpan& timespan);

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

#endif // _BASIS_TIME_STAMP_H
