#ifndef _BASIS_BUFFER_H_
#define _BASIS_BUFFER_H_

#include "basis_define.h"
#include "basis_nocopy.h"

namespace basis
{
//////////////////////////////////////////////////////////////////////////
// class BSBuffer
// ���̰߳�ȫ
//////////////////////////////////////////////////////////////////////////
class BSBuffer : public BSNoCopy
{
	enum { MAX_MEM = 1024 * 1024 };

public:
	BSBuffer();
	~BSBuffer();	

public:
	// �ڴ��С
	uint32 capatiy() const { return m_size; }
	// ʹ���ڴ�
	uint32 use_size() const { return m_use_size; }
	// �������
	bool put_data(const void* pData, uint32 _size);
	// ��ȡ����
	uint32 take_data(void* pData, uint32 _size);

private:
	bool reserve(uint32 _size);	

private:
	uint32 m_size; // �ڴ��С
	uint32 m_use_size; // δʹ���ڴ�
	char* m_begin_mem; // �ڴ���ʼָ�� (���ڴ�����һ��Բ�νṹ��ѭ��ʹ��)
	char* m_end_mem; // �ڴ����ָ��
	char* m_begin_data; // ������ʼλ�ã���ǰ�����ƶ�����ĩβ���ٴ�ͷ��ʼ��
	char* m_end_data; // ���ݽ���λ�� ��������һ��λ�ã�
};

}
#endif