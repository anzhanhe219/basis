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
	BSNoCopy(const BSNoCopy& _copy);
	BSNoCopy& operator=(const BSNoCopy& _copy);
};

}
using namespace basis;
#endif