#include "basis_socket.h"


namespace basis
{

BSSocket::~BSSocket()
{
	close();
}

bool BSSocket::open(BSSocketType tp)
{
	if (m_sock != INVALID_SOCKET)
	{
		BSSocket().swap(*this);
	}

	m_sock = ::socket(AF_INET, tp, 0);
	return isopen();
}

bool BSSocket::close()
{
	#ifdef _WINDOWS_
	if (isopen() && ::closesocket(m_sock) == -1)
	{
		return false;
	}
	#else // _WINDOWS_
	if (isopen() && ::close(m_sock) == -1)
	{
		return false;
	}
	#endif
	m_sock = INVALID_SOCKET;
	return true;
}

bool BSSocket::isopen()
{
	return m_sock != INVALID_SOCKET;
}

bool BSSocket::attach( SOCKET sock )
{
	if (sock == INVALID_SOCKET) return false;
	BSSocket().swap(*this);
	m_sock = sock;
	return true;
}

SOCKET BSSocket::detach()
{
	SOCKET sock = m_sock;
	m_sock = INVALID_SOCKET;
	return sock;
}

basis::BSSockAddr BSSocket::local_addr() const
{
	do 
	{
		if (INVALID_SOCKET == m_sock) break;

		struct sockaddr saddr = {};
		socklen_t namelen = (socklen_t)sizeof(sockaddr);
		if (0 != getsockname(m_sock, &saddr, &namelen)) break;
		
		return BSSockAddr::make_sock_addr(saddr);
	} while (false);

	return BSSockAddr::ErrorAddr;
}

basis::BSSockAddr BSSocket::remote_addr() const
{
	do 
	{
		if (INVALID_SOCKET == m_sock) break;

		struct sockaddr saddr = {};
		socklen_t namelen = (socklen_t)sizeof(sockaddr);
		if (0 != getpeername(m_sock, &saddr, &namelen)) break;

		return BSSockAddr::make_sock_addr(saddr);
	} while (false);

	return BSSockAddr::ErrorAddr;
}

bool BSSocket::set_nonblock( bool bl )
{
	if (m_sock == INVALID_SOCKET) return false;
	
	#ifdef __WINDOWS__
	unsigned long op = (unsigned long)bl;
	int ret = ioctlsocket(m_sock, FIONBIO, (unsigned long*)&op);
	#endif//__WINDOWS__
	
	#ifdef __POSIX__
	int ret = fcntl(m_sock, F_GETFL, 0);
	if (ret == -1) return false;
	if (bl)
		ret = fcntl(m_sock, F_SETFL, ret | O_NONBLOCK);
	else
		ret = fcntl(m_sock, F_SETFL, ret & (~O_NONBLOCK));
	#endif//__POSIX__
	
	return (0 == ret);
}

bool BSSocket::set_send_bufsize( uint32 sz )
{
	if (m_sock == INVALID_SOCKET) return false;
	int ret = setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, (const char*)&sz, sizeof(sz));
	return (0 == ret);
}

bool BSSocket::set_recv_bufsize( uint32 sz )
{
	if (m_sock == INVALID_SOCKET) return false;

	socklen_t len = sizeof(sz);
	int ret = getsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char*)&sz, &len);
	return (0 == ret);
}

bool BSSocket::get_send_bufsize( uint32& sz ) const
{
	if (m_sock == INVALID_SOCKET) return false;

	socklen_t len = sizeof(sz);
	int ret = getsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, (char*)&sz, &len);
	return (0 == ret);
}

bool BSSocket::get_recv_bufsize( uint32& sz ) const
{
	if (m_sock == INVALID_SOCKET) return false;

	int ret = setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (const char*)&sz, sizeof(int));
	return (0 == ret);
}

int BSSocket::last_error()
{
	#ifdef __WINDOWS__
	return WSAGetLastError();
	#endif // __WINDOWS__

	#ifdef __POSIX__
	return errno;
	#endif // __POSIX__

	UNEXPECT();
	return 0;
}

const char* BSSocket::error_msg( int err )
{
	return NULL;
}

void BSSocket::swap( BSSocket& sock )
{
	::swap(sock.m_sock, m_sock);
}

int BSSocket::sock_error(int fd)
{
	int tmp_err = 0;
	int len = sizeof(tmp_err);
	int ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, (char*)&tmp_err, (socklen_t *)&len);
	ASSERT(ret == 0);
	return tmp_err;
}

BSSocket::operator SOCKET()
{
	return m_sock;
}

BSSocket::operator SOCKET() const
{
	return m_sock;
}

} // namespace basis
