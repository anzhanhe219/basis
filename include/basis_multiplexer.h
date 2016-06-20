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
		BSMultiplexer() : m_private_data(NULL) {}
		~BSMultiplexer() { Destroy(); }

		static BSMultiplexer* Create(int sz);

	public:
		bool AddEvent(BSEventLoop* el, int fd, int mask);
		bool DelEvent(BSEventLoop* el, int fd, int mask);
		int Poll(BSEventLoop *el,  struct timeval *tvp);

		bool Resize(int sz);
		const char* ApiName();

	private:
		void Destroy();

	private:
		void* m_private_data;
	};

}
#endif