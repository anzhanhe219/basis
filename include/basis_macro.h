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

#define VERIFY(x) \
	do { \
	if (!(x)) { \
	fprintf(stderr, "Verification ("#x") failed at %s(..) in %s:%d, LastError:%u\r\n", __FUNCTION__, __FILE__, __LINE__, ::GetLastError()); \
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

#else//__WINDOWS__
#define ASSERT(x) \
	do { \
	if (!(x)) { \
	fprintf(stderr, "Assertion ("#x") failed at %s(..) in %s:%d, errno:%d\r\n", __FUNCTION__, __FILE__, __LINE__, errno); \
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

#define CHECKPTR(ptr) \
	do { \
	if (ptr == NULL) {\
	fprintf(stderr, "Assertion ("#ptr") is NULL at %s(..) in %s:%d, errno:%d\r\n", __FUNCTION__, __FILE__, __LINE__, errno); \
	fflush(stderr); \
	abort(); \
	} \
	} while (0)

#endif//__WINDOWS__
#else//__DEBUG__
#define ASSERT(x)
#define VERIFY(x) (x)
#define CHECKPTR(ptr)
#endif//__DEBUG__

}; // namespace basis

using namespace basis;
#endif //_BASIS_MACRO_H_
