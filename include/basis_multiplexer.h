#ifndef _BASIS_MULTIPLEXER_H_
#define _BASIS_MULTIPLEXER_H_

#include "basis_define.h"

namespace basis
{
//////////////////////////////////////////////////////////////////////////
// class BSMultiplexer
// ¶àÂ·¸´ÓÃÆ÷
//////////////////////////////////////////////////////////////////////////
class BSEventLoop;
class BSMultiplexer
{
public:
	BSMultiplexer();
	~BSMultiplexer();

	static BSMultiplexer* Create(int sz);	
	bool Resize(int sz);
	int AddEvent(BSEventLoop* el, int fd, int mask);
	int DelEvent(BSEventLoop* el, int fd, int mask);
	int Poll(BSEventLoop *el,  struct timeval *tvp);
	const char* ApiName();

private:
	void* m_private_data;
};

}
using namespace basis;
#endif