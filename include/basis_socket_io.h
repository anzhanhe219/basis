#ifndef __SOCKET_IO_H__
#define __SOCKET_IO_H__

#include "basis_define.h"
#include "basis_socket.h"

namespace basis
{
	class BSSocketIO
	{
	public:
		static int connect_sync(BSSockAddr& addr, int ms);
		static int connect_asyn(BSSockAddr& addr, bool& success);
		static bool listen(int sock, const BSSockAddr& sockaddr, int backlog);
		static bool accept(int sock, vector<int>& new_socks);
		static int write(int sock, void* buffer, uint32 bf_sz);
		static int read(int sock, void* buff, uint32 max_bf_sz);

		static bool is_would_block(int error);
	};

} // namespace basis

#endif//__SOCKET_IO_H__
