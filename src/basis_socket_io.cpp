#include "basis_socket_io.h"
#include "basis_event_loop.h"

namespace basis {

	int BSSocketIO::connect_syne(BSSockAddr& addr, int ms)
	{
		BSSocket sock;
		if (!sock.open(st_tcp)) return -1;
		if (!sock.set_nonblock(true)) return -1;

		// 立刻成功连接(本机有可能)
		if (::connect(sock, addr, sizeof(BSSockAddr)) == 0)
		{
			sock.set_nonblock(false);
			return sock.detach();
		}
		// 检查错误码
		int last_err = sock.last_error();
		if (last_err == EINPROGRESS || last_err == EWOULDBLOCK)
		{
			int mark = BSEventLoop::Wait(sock, ae_readable|ae_writeable, ms);
			if (mark <= 0)
			{
				return -1;
			}
			if ((mark & ae_readable) && BSSocket::sock_error(sock))
			{
				return -1;
			}
			if (mark & ae_writeable)
			{
				sock.set_nonblock(false);
				return sock.detach();
			}
		}
		return -1;
	}

	int BSSocketIO::connect_asyn(BSSockAddr& addr, bool& success)
	{
		BSSocket sock;
		if (!sock.open(st_tcp)) return -1;
		if (!sock.set_nonblock(true)) return -1;

		// 立刻成功连接(本机有可能)
		if (::connect(sock, addr, addr.length()) == 0)
		{
			sock.set_nonblock(false);
			return sock.detach();
		}
		// 检查错误码
		int last_err = sock.last_error();
		if (last_err == EINPROGRESS 
			|| last_err == EWOULDBLOCK
			|| last_err == WSAEWOULDBLOCK)
		{
			return sock.detach();
		}
		return -1;
	}

	bool BSSocketIO::listen(int sock, const BSSockAddr& sockaddr, int backlog)
	{
		if (::bind(sock, sockaddr, sockaddr.length()) < 0) return false;
		if (::listen(sock, backlog) < 0) return false;
		return true;
	}

	bool BSSocketIO::accept(int sock, vector<int>& new_socks)
	{
		while (1)
		{
			struct sockaddr addr;
			int32 length = sizeof(addr);
			int new_fd = ::accept(sock, &addr, &length);
			if (new_fd < 0)
			{
				int err = BSSocket().last_error();
				if (err == EINTR) continue;
				if (err == EWOULDBLOCK) break;	
				if (err == WSAEWOULDBLOCK) break;
			}
			new_socks.push_back(new_fd);
		}
		return true;
	}

	int BSSocketIO::write(int sock, void* buffer, uint32 bf_sz)
	{
		return ::send(sock, (const char*)buffer, bf_sz, 0);
	}

	int BSSocketIO::read(int sock, void* buff, uint32 max_bf_sz)
	{
		return ::recv(sock, (char*)buff, max_bf_sz, 0);
	}

}
