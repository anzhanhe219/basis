#include "basis_timespan.h"

namespace basis
{
////////////////////////////////////////////////////////////////////////////////
// class BSTimeSpan
////////////////////////////////////////////////////////////////////////////////
BSTimeSpan::BSTimeSpan()
	: m_span(0)
{
	//empty
}

BSTimeSpan::BSTimeSpan( const BSTimeSpan& span )
	: m_span(span.m_span)
{
	//empty
}

BSTimeSpan::BSTimeSpan( int64 usec )
{
	m_span = usec;
}

BSTimeSpan::BSTimeSpan( const timeval& tv )
{
	m_span = tv.tv_usec + tv.tv_sec * 1000000;
}

BSTimeSpan::BSTimeSpan( int64 sec, int64 usec )
{
	m_span = usec + sec * 1000000;
}

BSTimeSpan::~BSTimeSpan()
{
	//empty
}

int64 BSTimeSpan::to_sec() const
{
	int64 sec = m_span / 1000000;
	if (m_span % 1000000 >= 5000)
	{
		sec++;
	}
	return sec;
}

int64 BSTimeSpan::to_msec() const
{
	int64 msec = m_span / 1000;
	if (m_span % 1000 >= 500)
	{
		msec++;
	}
	return msec;
}

int64 BSTimeSpan::to_usec() const
{
	return m_span;
}

BSTimeSpan& BSTimeSpan::operator=( const BSTimeSpan& timespan )
{
	if (this != &timespan)
	{
		m_span = timespan.m_span;
	}
	return *this;
}

BSTimeSpan& BSTimeSpan::operator+=( const BSTimeSpan& timespan )
{
	m_span += timespan.m_span;
	return *this;
}

BSTimeSpan& BSTimeSpan::operator-=( const BSTimeSpan& timespan )
{
	m_span -= timespan.m_span;
	return *this;
}

BSTimeSpan BSTimeSpan::operator+( const BSTimeSpan& timespan ) const
{
	return BSTimeSpan(m_span + timespan.m_span);
}

BSTimeSpan BSTimeSpan::operator-( const BSTimeSpan& timespan ) const
{
	return BSTimeSpan(m_span -timespan.m_span);
}

}//namespace basis