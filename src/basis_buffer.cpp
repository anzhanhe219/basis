#include "basis_buffer.h"

namespace basis
{
//////////////////////////////////////////////////////////////////////////
// �Զ��建����
// class BSBuffer
//////////////////////////////////////////////////////////////////////////
BSBuffer::BSBuffer()
	: m_size(INIT_MEM)
	, m_use_size(0)
{
	m_begin_mem = (char*)malloc(INIT_MEM);	
	m_end_mem = m_begin_mem + m_size;
	m_begin_data = m_begin_mem;
	m_end_data = m_begin_data;
}


bool BSBuffer::fillData(void* data, uint32 _size)
{
	char* pData = (char*)data;

	// �ڴ治�㣬�����µ��ڴ�
	if ((m_size - m_use_size) < _size)
	{
		while ((m_size - m_use_size) < _size)
		{
			m_size *= 2; // ��ԭ���ڴ��������չһ��
			if (m_size > MAX_MEM)
			{
				return false;
			}
		}

		// ����ԭ�������ݵ��µ��ڴ���
		char* tmp = (char*)malloc(m_size);
		
		// �����ڴ�
		if (m_end_data > m_begin_data)
		{
			memcpy(tmp, m_begin_data, m_use_size);
		}
		// �����ڴ�
		else
		{
			uint32 after_size = m_end_mem - m_begin_data;
			uint32 before_size = m_end_data - m_begin_mem;
			memcpy(tmp, m_begin_data, after_size);
			memcpy(tmp + after_size, m_begin_mem, before_size);

			delete m_begin_mem;
			m_begin_mem = tmp;
			m_end_mem = m_begin_mem + m_size;
			m_begin_data = m_begin_mem;
			m_end_data = m_begin_data + m_use_size;
		}
	}
	
	// �����ڴ�
	if (m_end_data > m_begin_data)
	{
		// ����ڴ��ܹ���������
		if ((uint32)(m_end_mem - m_end_data) >= _size)
		{
			memcpy(m_end_data, pData, _size);
			m_end_data += _size;
		}
		else
		{
			uint32 mem_size = m_end_mem - m_end_data;
			memcpy(m_end_data, pData, mem_size);
			uint32 mem_size1 = _size - mem_size;
			memcpy(m_begin_mem, pData + mem_size, mem_size1);
			m_end_data = m_begin_mem + mem_size1;
			ASSERT(m_end_data < m_begin_data);
		}
	}
	// �����ڴ�ṹ
	else if (m_end_data < m_begin_data)
	{
		uint32 free_size = m_begin_data - m_end_data;
		if (_size > free_size)
		{
			return false;
		}
		memcpy(m_end_data, pData, _size);
		m_end_data += _size;
	}
	else
	{
		return false;
	}

	m_use_size += _size;
	
	return true;
}

bool BSBuffer::takeData(void* data, uint32 _size)
{
	// ����û�ж�̬�ͷ��ڴ棬���ⷴ�����ٺ��ͷ��ڴ�
	char* pData = (char*)data;

	if (_size > m_size || _size > m_use_size)
	{
		return false;
	}

	// �����ڴ�
	if (m_end_data > m_begin_data)
	{
		if (m_use_size < _size)
		{
			return false;
		}
		memcpy(pData, m_begin_data, _size);
		m_begin_data += _size;
	}
	// �����ڴ�
	else
	{
		// ����ڴ���
		if ((uint32)(m_end_mem - m_begin_data) >= _size)
		{
			memcpy(pData, m_begin_data, _size);
			m_begin_data += _size;
		}
		else
		{
			uint32 mem_size = m_end_mem - m_begin_data;
			// ǰ�����ڴ治��
			if ((uint32)(m_end_data - m_begin_mem) < _size - mem_size)
			{
				return false;
			}

			memcpy(pData, m_begin_data, mem_size);
			memcpy(pData + mem_size, m_begin_mem, _size - mem_size);
			m_begin_data = m_begin_mem + (_size - mem_size);
		}
	}

	m_use_size -= _size;

	return true;
}

BSBuffer::~BSBuffer()
{
	delete m_begin_mem;
	m_begin_mem = NULL;
	m_end_mem = NULL;
	m_begin_data = NULL;
	m_end_data = NULL;
}


}