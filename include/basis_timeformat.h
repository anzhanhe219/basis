#ifndef _BASIS_TIME_FORMAT_H_
#define _BASIS_TIME_FORMAT_H_

#include "basis_timestamp.h"

namespace basis
{
class BSTimeFormat
{
public:
	enum Style
	{
		YMDHMS_0,		/* 2010-09-29 09:12:08 */
		YMD_0,			/* 2010-09-29 */
		HMS_0,			/* 09:12:08 */
	};
	BSTimeFormat();
	BSTimeFormat(time_t timestamp);
	BSTimeFormat(const BSTimeStamp& timestamp);

public:
	// support two time format
	// format 1: 2008-03-12 15:01:12
	// format 2: 20080312150112
	static bool do_parse(BSTimeStamp& timestamp,const string& strTime);
	static string format_time(BSTimeStamp& timestamp, int32 style);
	static string format_time(const time_t timestamp, int32 style = YMDHMS_0);

private:
	BSTimeStamp m_timestamp;
};

}

#endif