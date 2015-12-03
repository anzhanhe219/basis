#ifndef _BASIS_TIMESPAN_H_
#define _BASIS_TIMESPAN_H_

#include "basis_define.h"

namespace basis
{
////////////////////////////////////////////////////////////////////////////////
// class BSTimeSpan
////////////////////////////////////////////////////////////////////////////////
class BSTimeSpan
{
public:
	BSTimeSpan();
	BSTimeSpan(int64 usec);
	BSTimeSpan(int64 sec, int64 usec);
	BSTimeSpan(const BSTimeSpan& span);
	BSTimeSpan(const timeval& tv);
	~BSTimeSpan();

public:
	int64 to_sec() const;
	int64 to_msec() const;
	int64 to_usec() const;

public:
	bool operator == (const BSTimeSpan& timespan) const;
	bool operator != (const BSTimeSpan& timespan) const;
	bool operator >  (const BSTimeSpan& timespan) const;
	bool operator <  (const BSTimeSpan& timespan) const;
	bool operator >= (const BSTimeSpan& timespan) const;
	bool operator <= (const BSTimeSpan& timespan) const;

	BSTimeSpan operator + (const BSTimeSpan& timespan) const;
	BSTimeSpan operator - (const BSTimeSpan& timespan) const;
	BSTimeSpan& operator =  (const BSTimeSpan& timespan); 
	BSTimeSpan& operator += (const BSTimeSpan& timespan);
	BSTimeSpan& operator -= (const BSTimeSpan& timespan);

private:
	int64 m_span; 
};

inline bool BSTimeSpan::operator == ( const BSTimeSpan& timespan ) const
{
	return (m_span == timespan.m_span);
}

inline bool BSTimeSpan::operator != ( const BSTimeSpan& timespan ) const
{
	return (m_span != timespan.m_span);
}

inline bool BSTimeSpan::operator > ( const BSTimeSpan& timespan ) const
{
	return (m_span > timespan.m_span);
}

inline bool BSTimeSpan::operator < ( const BSTimeSpan& timespan ) const
{
	return (m_span < timespan.m_span);
}

inline bool BSTimeSpan::operator >= ( const BSTimeSpan& timespan ) const
{
	return (m_span >= timespan.m_span);
}

inline bool BSTimeSpan::operator <= ( const BSTimeSpan& timespan ) const
{
	return (m_span <= timespan.m_span);
}

}

using namespace basis;
#endif