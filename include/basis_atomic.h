// 2015-11-10
// basis_atomic.h
// 
// ԭ�Ӳ�����

#ifndef _BASIS_ATOMIC_H
#define _BASIS_ATOMIC_H

#include "basis_define.h"

namespace basis
{
///////////////////////////////////////////////////////////////////////////////
// class XAtomic32
///////////////////////////////////////////////////////////////////////////////
class XAtomic32
{
public:
	XAtomic32(int32 i = 0);
	XAtomic32(const XAtomic32& from);
	~XAtomic32();

	int32 get_value() const;  // ���ص�ǰֵ
	int32 set_value(int32 i); // ����ԭֵ
	int32 test_zero_inc(); // �����������ֵ

	operator int32 ();
	XAtomic32& operator=(int32 i);
	XAtomic32& operator=(const XAtomic32& from);

	int32 operator+= (int32 i); // �����������ֵ
	int32 operator-= (int32 i); // ���ؼ������ֵ
	int32 operator++ (int); // ����ԭֵ
	int32 operator-- (int); // ����ԭֵ
	int32 operator++ (); // �����������ֵ
	int32 operator-- (); // ���ؼ������ֵ

private:
	volatile int32 m_counter;
};
} //namespace basis

using namespace basis;

#endif //_BASIS_ATOMIC_

