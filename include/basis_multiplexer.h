#ifndef _BASIS_MULTIPLEXER_H_
#define _BASIS_MULTIPLEXER_H_

#include "basis_define.h"
#include "basis_event_callback.h"

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
		bool AddEvent(int fd, int mask, BSFdPartner* partner);
		bool DelEvent(int fd, int mask, BSFdPartner* partner);
		int Poll(BSEventLoop *el,  struct timeval *tvp);

		BSFiredFd* FetchFiredFd();
		BSFiredFd* GetFiredFd(uint32 index);

		const char* ApiName();

	private:
		void Destroy();

	private:
		void* m_private_data;
	};

}
#endif