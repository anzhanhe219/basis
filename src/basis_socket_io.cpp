#include "basis_socket_io.h"
#include "basis_event_loop.h"

namespace basis {

	int BSSocketIO::connect_sync(BSSockAddr& addr, int ms)
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
		if (is_would_block(sock.last_error()))
		{
			int mark = BSEventLoop::Wait(sock, fs_readable|fs_writeable, ms);
			if (mark <= 0)
			{
				return -1;
			}
			if ((mark & fs_readable) && BSSocket::sock_error(sock))
			{
				return -1;
			}
			if (mark & fs_writeable)
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
		if (is_would_block(sock.last_error()))
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
			socklen_t length = sizeof(addr);
			int new_fd = ::accept(sock, &addr, &length);
			if (new_fd < 0)
			{
				int err = BSSocket().last_error();
				if (err == EINTR) continue;
				if (is_would_block(new_fd)) break;
			}
			new_socks.push_back(new_fd);
		}
		return true;
	}

	int BSSocketIO::write(int sock, const char* buffer, uint32 bf_sz)
	{
		return ::send(sock, buffer, bf_sz, 0);
	}

	int BSSocketIO::read(int sock, char* buff, uint32 max_bf_sz)
	{
		return ::recv(sock, buff, max_bf_sz, 0);
	}

	bool BSSocketIO::is_would_block(int error)
	{
#ifdef __WINDOWS__
		if (error == WSAEWOULDBLOCK) return true;
		if (error == WSAEINPROGRESS) return true; //?????
		return false;		
#endif // __WINDOWS__

#ifdef __POSIX__
		if (error == EAGAIN) return true;
		if (error == EWOULDBLOCK) return true;
		if (error == EINPROGRESS) return true; //?????
		return false;
#endif // __POSIX__

		UNEXPECT();
		return false;
	}

}
