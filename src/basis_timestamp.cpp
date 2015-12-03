#include "basis_timestamp.h"
#include <time.h>

namespace basis
{
////////////////////////////////////////////////////////////////////////////////
// class BSTimeStamp
////////////////////////////////////////////////////////////////////////////////
BSTimeStamp::BSTimeStamp()
{
#ifdef __WINDOWS__
	struct tm _tm;
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	_tm.tm_year  = st.wYear - 1900;
	_tm.tm_mon   = st.wMonth - 1;
	_tm.tm_mday  = st.wDay;
	_tm.tm_hour  = st.wHour;
	_tm.tm_min   = st.wMinute;
	_tm.tm_sec   = st.wSecond;
	_tm.tm_isdst = -1;
	int64 sec = mktime(&_tm);
	m_usec = st.wMilliseconds * 1000 + sec * 1000000;
#endif//__WINDOWS__

#ifdef __POSIX__
	struct timeval tv_ = {};
	int ret = gettimeofday(&tv_, NULL);
	ASSERT(0 == ret);
	m_usec = tv_.tv_usec + tv_.tv_sec * 1000000;
#endif//__POSIX__
}


BSTimeStamp::BSTimeStamp( time_t time_, int64 usec /*= 0*/ )
{
	m_usec = (int64)(time_ * 1000000) + usec;
}

BSTimeStamp::BSTimeStamp( const BSTimeStamp& timestamp )
{
	m_usec = timestamp.m_usec;
	//memcpy(this, &timestamp, sizeof(timestamp));
}

BSTimeStamp::BSTimeStamp( const timeval& tv )
{
	m_usec = tv.tv_usec + tv.tv_sec * 1000000;
}

BSTimeStamp::BSTimeStamp( const tm& tm_ )
{
	int64 sec = mktime((struct tm*)&tm_);
	m_usec = sec * 1000000;
}

BSTimeStamp::~BSTimeStamp()
{
	//empty
}

}// namespace basis