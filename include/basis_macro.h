#ifndef _BASIS_MACRO_H_
#define _BASIS_MACRO_H_

#include "basis_define.h"

namespace basis
{
///////////////////////////////////////////////////////////////////////////////
#undef ASSERT
#undef VERIFY
#ifdef __DEBUG__
#ifdef __WINDOWS__
#define ASSERT(x) \
	do { \
	if (!(x)) { \
	fprintf(stderr, "Assertion ("#x") failed at %s(..) in %s:%d, LastError:%u\r\n", __FUNCTION__, __FILE__, __LINE__, ::GetLastError()); \
	fflush(stderr); \
	abort(); \
	} \
	} while (0)

#define	ASSERTMSG(x, msg)\
	do { \
		if (!(x)) { \
			fprintf(stderr, "Assertion ("#x") failed msg(%s) at %s(..) in %s:%d, LastError:%u\r\n", msg, __FUNCTION__, __FILE__, __LINE__, ::GetLastError()); \
			fflush(stderr); \
			abort(); \
		} \
	} while (0)

#define VERIFY(x) \
	do { \
	if (!(x)) { \
	fprintf(stderr, "Verification ("#x") failed at %s(..) in %s:%d, LastError:%u\r\n", __FUNCTION__, __FILE__, __LINE__, ::GetLastError()); \
	fflush(stderr); \
	abort(); \
	} \
	} while (0)

#define VERIFYMSG(x, msg) \
	do { \
	if (!(x)) { \
	fprintf(stderr, "Verification ("#x") failed msg(%s) at %s(..) in %s:%d, LastError:%u\r\n", msg, __FUNCTION__, __FILE__, __LINE__, ::GetLastError()); \
	fflush(stderr); \
	abort(); \
	} \
	} while (0)

#define CHECKPTR(ptr) \
	do{ \
	if (ptr == NULL) { \
	fprintf(stderr, "CheckPtr ("#ptr") is NULL at %s(..) in %s:%d, LastError:%u\r\n", __FUNCTION__, __FILE__, __LINE__, ::GetLastError()); \
	fflush(stderr); \
	abort(); \
	} \
	} while(0)

#define  UNEXPECT() \
	do{ \
	fprintf(stderr, "The branch is unexpected at %s(..) in %s:%d, LastError:%u\r\n", __FUNCTION__, __FILE__, __LINE__, ::GetLastError()); \
	fflush(stderr); \
	abort(); \
	} while (0);

#else//__WINDOWS__
#define ASSERT(x) \
	do { \
	if (!(x)) { \
	fprintf(stderr, "Assertion ("#x") failed at %s(..) in %s:%d, errno:%d\r\n", __FUNCTION__, __FILE__, __LINE__, errno); \
	fflush(stderr); \
	abort(); \
	} \
	} while (0)

#define	ASSERTMSG(x, msg)\
	do { \
	if (!(x)) { \
	fprintf(stderr, "Assertion ("#x") failed msg(%s) at %s(..) in %s:%d, LastError:%u\r\n", msg, __FUNCTION__, __FILE__, __LINE__, ::GetLastError()); \
	fflush(stderr); \
	abort(); \
	} \
	} while (0)

#define VERIFY(x) \
	do { \
	if (!(x)) { \
	fprintf(stderr, "Verification ("#x") failed at %s(..) in %s:%d, errno:%d\r\n", __FUNCTION__, __FILE__, __LINE__, errno); \
	fflush(stderr); \
	abort(); \
	} \
	} while (0)

#define VERIFYMSG(x, msg) \
	do { \
	if (!(x)) { \
	fprintf(stderr, "Verification ("#x") failed msg(%s) at %s(..) in %s:%d, LastError:%u\r\n", msg, __FUNCTION__, __FILE__, __LINE__, ::GetLastError()); \
	fflush(stderr); \
	abort(); \
	} \
	} while (0)

#define CHECKPTR(ptr) \
	do { \
	if (ptr == NULL) {\
	fprintf(stderr, "Assertion ("#ptr") is NULL at %s(..) in %s:%d, errno:%d\r\n", __FUNCTION__, __FILE__, __LINE__, errno); \
	fflush(stderr); \
	abort(); \
	} \
	} while (0)

#define  UNEXPECT() \
	do{ \
	fprintf(stderr, "The branch is unexpected at %s(..) in %s:%d, LastError:%u\r\n", __FUNCTION__, __FILE__, __LINE__, ::GetLastError()); \
	fflush(stderr); \
	abort(); \
	} while (0);

#endif//__WINDOWS__
#else//__DEBUG__
#define ASSERT(x)
#define ASSERTMSG(x,msg)
#define VERIFY(x) (x)
#define VERIFYMSG(x, msg) (x)
#define CHECKPTR(ptr)
#define UNEXPECT()
#endif//__DEBUG__

}; // namespace basis

using namespace basis;
#endif //_BASIS_MACRO_H_
