#ifndef _BASIS_BUFFER_H_
#define _BASIS_BUFFER_H_

#include "basis_define.h"

namespace basis
{
//////////////////////////////////////////////////////////////////////////
// �Զ��建���� (���ⲿ����������������)
// class BSBuffer
//////////////////////////////////////////////////////////////////////////
class BSBuffer
{
	enum { INIT_MEM = 10, MAX_MEM = 1024 * 1024 };

public:
	BSBuffer();
	~BSBuffer();
	
	// �ڴ��С
	uint32 capatiy() { return m_size; }
	// ʹ���ڴ�
	uint32 use_size() { return m_use_size; }

	void reserve(uint32 _size);

	// �������
	bool fill_data(void* pData, uint32 _size);
	// ��ȡ����
	bool take_data(void* pData, uint32 _size);

	friend BSBuffer operator+(BSBuffer& buffer, BSBuffer& buffer1);

	/*void print_data()
	{
	printf("buff data : %s\n", string(m_begin_mem).c_str());
	}*/

private:
	uint32 m_size; // �ڴ��С
	uint32 m_use_size; // δʹ���ڴ�
	char* m_begin_mem; // �ڴ���ʼָ�� (���ڴ�����һ��Բ�νṹ��ѭ��ʹ��)
	char* m_end_mem; // �ڴ����ָ��
	char* m_begin_data; // ������ʼλ�ã���ǰ�����ƶ�����ĩβ���ٴ�ͷ��ʼ��
	char* m_end_data; // ���ݽ���λ�� ��������һ��λ�ã�
};

}
using namespace basis;
#endif