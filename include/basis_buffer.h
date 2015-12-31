#ifndef _BASIS_BUFFER_H_
#define _BASIS_BUFFER_H_

#include "basis_define.h"

namespace basis
{
//////////////////////////////////////////////////////////////////////////
// �Զ��建����
// class BSBuffer
//////////////////////////////////////////////////////////////////////////
class BSBuffer
{
	enum { INIT_MEM = 1024, MAX_MEM = 1024 * 1024 };

public:
	BSBuffer();
	~BSBuffer();
	
	// �ڴ��С
	uint32 capatiy() { return m_size; }
	// ʹ���ڴ�
	uint32 useSize() { return m_use_size; }

	// �������
	bool fillData(void* pData, uint32 _size);
	// ��ȡ����
	bool takeData(void* pData, uint32 _size);

private:
	uint32 m_size; // �ڴ��С
	uint32 m_use_size; // δʹ���ڴ�
	char* m_begin_mem; // �ڴ���ʼָ�� (���ڴ�����һ��Բ�νṹ��ѭ��ʹ��)
	char* m_end_mem; // �ڴ����ָ��
	char* m_begin_data; // ������ʼλ�ã���ǰ�����ƶ�����ĩβ���ٴ�ͷ��ʼ��
	char* m_end_data; // ���ݽ���λ�� ��������һ��λ�ã�
};

BSBuffer operator+(BSBuffer& buffer, BSBuffer& buffer1)
{
	BSBuffer tmpBuffer;
	void* data = malloc(buffer.useSize());
	buffer.takeData(data, buffer.useSize());
	
	void* data1 = malloc(buffer1.useSize());
	buffer1.takeData(data1, buffer1.useSize());

	tmpBuffer.fillData(data, buffer.useSize());
	tmpBuffer.fillData(data1, buffer1.useSize());

	return tmpBuffer;
}

}
using namespace basis;
#endif