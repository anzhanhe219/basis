#ifndef _BASIS_NO_COPY_
#define _BASIS_NO_COPY_

// ���ɸ��ƿ�����

namespace basis
{

class BSNoCopy
{
protected:
	BSNoCopy() {}
	~BSNoCopy() {}

private:
	BSNoCopy(const BSNoCopy&);
	BSNoCopy& operator=(const BSNoCopy&);
};

}; //namespace basis

#endif //_BASIS_NO_COPY_