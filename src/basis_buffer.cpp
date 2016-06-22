#include "basis_buffer.h"

namespace basis
{
//////////////////////////////////////////////////////////////////////////
// 自定义缓冲区
// class BSBuffer
//////////////////////////////////////////////////////////////////////////
BSBuffer::BSBuffer()
	: m_size(0)
	, m_use_size(0)
	, m_begin_mem(NULL)
	, m_end_mem(NULL)
	, m_begin_data(NULL)
	, m_end_data(NULL)
{
}

bool BSBuffer::reserve(uint32 _size)
{
	if (_size > MAX_MEM) return false;
	if (_size <= m_size) return true;
	
	char* _buff = (char*)malloc(_size);
	if (NULL == _buff) return false;
	if (m_use_size)
	{
		// 长型结构保存
		if (m_end_data > m_begin_data)
		{
			memcpy(_buff, m_begin_data, m_use_size);
		}
		// 环形结构保存 (将环型结构转换成长型结构)
		else
		{
			uint32 after_size =  m_end_mem - m_begin_data;
			uint32 before_size = m_end_data - m_begin_mem;
			memcpy(_buff, m_begin_data, after_size);
			memcpy(_buff + after_size, m_begin_mem, before_size);
		}
	}

	free(m_begin_mem);
	m_begin_mem = _buff;
	m_end_mem = m_begin_mem + _size;
	m_begin_data = m_begin_mem;
	m_end_data = m_begin_data + m_use_size;
	m_size = _size;

	return true;
}

bool BSBuffer::put_data(const void* data, uint32 _size)
{
	if (NULL == data) return false;

	const char* pData = (const char*)data;
	while (m_size < m_use_size + _size)
	{
		if (m_size == 0)  m_size = 256; // 初始化
		else if(m_size >= MAX_MEM) return false;
		else 	{
			m_size = m_size * 2;
		}
		if (m_size > MAX_MEM) m_size = MAX_MEM;
	}
	if (!reserve(m_size)) return true;

	// 拷贝数据
	if (m_end_data >= m_begin_data)
	{
		if (m_end_mem  >= _size + m_end_data)// 长型存储
		{
			memcpy(m_end_data, pData, _size);
			m_end_data += _size;
		} 		
		else// 环型存储
		{
			uint32 mem_size = m_end_mem - m_end_data;
			memcpy(m_end_data, pData, mem_size);
			uint32 mem_size1 = _size - mem_size;
			memcpy(m_begin_mem, pData + mem_size, mem_size1);
			m_end_data = m_begin_mem + mem_size1;
		}
	}
	else
	{
		memcpy(m_end_data, pData, _size);
		m_end_data += _size;
	}

	m_use_size += _size;
	return true;
}

uint32 BSBuffer::take_data(void* data, uint32 _size)
{
	if (NULL == data) return 0;
	if (m_use_size == 0) return 0;

	uint32 take_size = _size;
	if (m_use_size < take_size)
	{
		take_size = m_use_size;
	}

	char* pData = (char*)data;
	if (m_end_data > m_begin_data)// 长型内存
	{
		memcpy(pData, m_begin_data, take_size);
		m_begin_data += take_size;
	}	
	else// 环形内存
	{
		if (m_end_mem  >= m_begin_data + take_size)
		{
			memcpy(pData, m_begin_data, take_size);
			m_begin_data += take_size;
		}
		else
		{
			uint32 mem_size = m_end_mem - m_begin_data;
			memcpy(pData, m_begin_data, mem_size);
			memcpy(pData + mem_size, m_begin_mem, take_size - mem_size);
			m_begin_data = m_begin_mem + (take_size - mem_size);
		}
	}

	m_use_size -= take_size;
	return take_size;
}

BSBuffer::~BSBuffer()
{
	free(m_begin_mem);
	m_begin_mem = NULL;
	m_end_mem = NULL;
	m_begin_data = NULL;
	m_end_data = NULL;
}

}