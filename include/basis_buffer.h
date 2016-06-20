#ifndef _BASIS_BUFFER_H_
#define _BASIS_BUFFER_H_

#include "basis_define.h"

namespace basis
{
//////////////////////////////////////////////////////////////////////////
// class BSBuffer
// 非线程安全
//////////////////////////////////////////////////////////////////////////
class BSBuffer
{
	enum { MAX_MEM = 1024 * 1024 };

public:
	BSBuffer();
	~BSBuffer();	
	BSBuffer(const BSBuffer& _buff);

	// 内存大小
	uint32 capatiy() const { return m_size; }
	// 使用内存
	uint32 use_size() const { return m_use_size; }

	void reserve(uint32 _size);

	// 填充数据
	bool fill_data(const void* pData, uint32 _size);
	// 提取数据
	uint32 take_data(void* pData, uint32 _size);

	BSBuffer& operator=(const BSBuffer& _buff);

private:
	uint32 m_size; // 内存大小
	uint32 m_use_size; // 未使用内存
	char* m_begin_mem; // 内存起始指针 (将内存理解成一个圆形结构，循环使用)
	char* m_end_mem; // 内存结束指针
	char* m_begin_data; // 数据起始位置（从前往后移动，到末尾，再从头开始）
	char* m_end_data; // 数据结束位置 （数据下一个位置）
};

}
#endif