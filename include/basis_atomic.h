#ifndef _BASIS_ATOMIC_H_
#define _BASIS_ATOMIC_H_

#include "basis_define.h"

namespace basis
{

class BSAtomic32
{
public:
	BSAtomic32(int32 i = 0);
	BSAtomic32(const BSAtomic32& from);
	~BSAtomic32();

	int32 get_value() const;  // 返回当前值
	int32 set_value(int32 i); // 返回原值
	int32 test_zero_inc(); // 返回增量后的值

	operator int32 ();
	BSAtomic32& operator=(int32 i);
	BSAtomic32& operator=(const BSAtomic32& from);

	int32 operator+= (int32 i); // 返回增量后的值
	int32 operator-= (int32 i); // 返回减量后的值
	int32 operator++ (int); // 返回原值
	int32 operator-- (int); // 返回原值
	int32 operator++ (); // 返回增量后的值
	int32 operator-- (); // 返回减量后的值

private:
	volatile int32 m_counter;
};
} //namespace basis

#endif //_BASIS_ATOMIC_H_

