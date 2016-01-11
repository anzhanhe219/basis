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

void BSBuffer::reserve(uint32 _size)
{
	if (_size > m_size)
	{
		if (_size > MAX_MEM) return;

		char* _buff = (char*)malloc(_size);
		if (NULL == _buff) return;

		if (m_use_size)
		{
			// ���ͽṹ����
			if (m_end_data > m_begin_data)
			{
				memmove(_buff, m_begin_data, m_use_size);
			}
			// ���νṹ���� (�����ͽṹת���ɳ��ͽṹ)
			else
			{
				uint32 after_size =  m_end_mem - m_begin_data;
				uint32 before_size = m_end_data - m_begin_mem;
				memmove(_buff, m_begin_data, after_size);
				memmove(_buff + after_size, m_begin_mem, before_size);
			}
		}

		delete m_begin_mem;
		m_begin_mem = _buff;
		m_end_mem = m_begin_mem + _size;
		m_begin_data = m_begin_mem;
		m_end_data = m_begin_data + m_use_size;
		m_size = _size;
	}
}

bool BSBuffer::fill_data(void* data, uint32 _size)
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
		
		// �����ڴ汣��
		if (m_end_data > m_begin_data)
		{
			memmove(tmp, m_begin_data, m_use_size);
		}
		// �����ڴ汣��
		else
		{
			uint32 after_size = m_end_mem - m_begin_data;
			uint32 before_size = m_end_data - m_begin_mem;
			memmove(tmp, m_begin_data, after_size);
			memmove(tmp + after_size, m_begin_mem, before_size);

			delete m_begin_mem;
			m_begin_mem = tmp;
			m_end_mem = m_begin_mem + m_size;
			m_begin_data = m_begin_mem;
			m_end_data = m_begin_data + m_use_size;
		}
	}

	// �����ڴ�ṹ
	if (m_end_data >= m_begin_data)
	{
		// ���ʹ洢
		if ((uint32)(m_end_mem - m_end_data) >= _size)
		{
			memmove(m_end_data, pData, _size);
			m_end_data += _size;
		} 
		// ���ʹ洢
		else
		{
			uint32 mem_size = m_end_mem - m_end_data;
			memmove(m_end_data, pData, mem_size);
			uint32 mem_size1 = _size - mem_size;
			memmove(m_begin_mem, pData + mem_size, mem_size1);
			m_end_data = m_begin_mem + mem_size1;
			//ASSERT(m_end_data < m_begin_data);
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
		memmove(m_end_data, pData, _size);
		m_end_data += _size;
	}

	m_use_size += _size;

	return true;
}

bool BSBuffer::take_data(void* data, uint32 _size)
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
		memmove(pData, m_begin_data, _size);
		m_begin_data += _size;
	}
	// �����ڴ�
	else
	{
		// ����ڴ���
		if ((uint32)(m_end_mem - m_begin_data) >= _size)
		{
			memmove(pData, m_begin_data, _size);
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

			memmove(pData, m_begin_data, mem_size);
			memmove(pData + mem_size, m_begin_mem, _size - mem_size);
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

BSBuffer operator+(BSBuffer& buffer, BSBuffer& buffer1)
{
	BSBuffer tmpBuffer;
	void* data = malloc(buffer.use_size());
	buffer.take_data(data, buffer.use_size());

	void* data1 = malloc(buffer1.use_size());
	buffer1.take_data(data1, buffer1.use_size());

	tmpBuffer.fill_data(data, buffer.use_size());
	tmpBuffer.fill_data(data1, buffer1.use_size());

	return tmpBuffer;
}

}