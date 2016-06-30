#ifndef _BASIS_MULTIPLEXER_H_
#define _BASIS_MULTIPLEXER_H_

#include "basis_define.h"
#include "basis_event_callback.h"

namespace basis
{
	//////////////////////////////////////////////////////////////////////////
	// class BSMultiplexer
	// ��·������
	//////////////////////////////////////////////////////////////////////////
	class BSEventLoop;
	class BSMultiplexer
	{
	public:
		BSMultiplexer() : m_private_data(NULL) {}
		~BSMultiplexer() { Destroy(); }

		static BSMultiplexer* Create(int sz);

	public:
		bool AddEvent(int fd, int mask, BSFileEvent* arg);
		bool DelEvent(int fd, int mask, BSFileEvent* arg);
		int Poll(BSEventLoop *el,  struct timeval *tvp);

		BSFiredEvent* FetchFiredEvents();
		BSFiredEvent* GetFiredEvents(uint32 index);

		bool Resize(int sz);
		const char* ApiName();

	private:
		void Destroy();

	private:
		void* m_private_data;
	};

}
#endif