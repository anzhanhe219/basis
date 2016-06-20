#include "basis_sockaddr.h"
#include "basis_str_tool.h"
#ifdef _WINDOWS_
#include <WS2tcpip.h>
#endif // _WINDOWS_

namespace basis
{

basis::BSSockAddr BSSockAddr::make_sock_addr( const string& addr )
{
	string host = addr;
	string port;
	size_t pos = addr.find(':');
	while (pos != string::npos)
	{
		host = addr.substr(0, addr.find(':'));
		port = addr.substr(addr.find(':') + 1);

		uint32 nport = 0;
		if (!BSStrTool::toUint32(port, nport)) break;
		if (nport > (uint16)0xffff) break;
		return make_sock_addr(host, nport);
	}
	return ErrorAddr;
}

basis::BSSockAddr BSSockAddr::make_sock_addr( uint32 ipaddr, uint16 port )
{
	BSIpAddr ipt = BSIpAddr::make_ipaddr_by_num(ipaddr);
	if (ipt.is_error()) return ErrorAddr;
	return make_sock_addr(ipt, port);
}

basis::BSSockAddr BSSockAddr::make_sock_addr( const string& ipaddr, uint16 port )
{
	BSIpAddr ipt = BSIpAddr::make_ipaddr_by_ip(ipaddr);
	if (ipt.is_error())
	{
		ipt = BSIpAddr::make_ipaddr_by_host(ipaddr);
	}
	if (ipt.is_error())
	{
		return ErrorAddr;
	}
	return make_sock_addr(ipt, port);
}

basis::BSSockAddr BSSockAddr::make_sock_addr( const sockaddr_in& inaddr )
{
	return BSSockAddr(inaddr);
}

basis::BSSockAddr BSSockAddr::make_sock_addr( const sockaddr& addr )
{
	return BSSockAddr(addr);
}

basis::BSSockAddr BSSockAddr::make_sock_addr( const BSIpAddr& ipaddr, uint16 port )
{
	return BSSockAddr(ipaddr, port);
}

BSSockAddr::BSSockAddr()
{
	reset();
	m_inaddr.sin_addr.s_addr = INADDR_NONE;
}

BSSockAddr::BSSockAddr( uint16 port )
{
	reset();
	set_port(port);
}

BSSockAddr::BSSockAddr( const BSIpAddr& ip, uint16 port )
{
	reset();
	set_port(port);
	ip.to_sockaddr_in(m_inaddr);
}

BSSockAddr::BSSockAddr( const sockaddr_in& inaddr )
{
	reset();
	m_inaddr.sin_port = inaddr.sin_port;
	m_inaddr.sin_addr = inaddr.sin_addr;
}

BSSockAddr::BSSockAddr( const sockaddr& addr )
{
	reset();
	const sockaddr_in& inaddr = (const sockaddr_in&)(addr); 
	m_inaddr.sin_port = inaddr.sin_port;
	m_inaddr.sin_addr = inaddr.sin_addr;
}

BSSockAddr::BSSockAddr( const BSSockAddr& r )
	: m_inaddr(r.m_inaddr)
{

}

BSSockAddr::~BSSockAddr()
{

}

BSSockAddr& BSSockAddr::operator=( const BSSockAddr& r )
{
	if (&r != this)
	{
		m_inaddr = r.m_inaddr;
	}
	return *this;
}

bool BSSockAddr::operator==( const BSSockAddr& r )
{
	return m_inaddr.sin_family == r.m_inaddr.sin_family
		&& m_inaddr.sin_port == r.m_inaddr.sin_port
		&& m_inaddr.sin_addr.s_addr == r.m_inaddr.sin_addr.s_addr;
}

BSSockAddr::operator const sockaddr_in*() const
{
	return &m_inaddr;
}

BSSockAddr::operator const sockaddr*() const
{
	return (const sockaddr*)&m_inaddr;
}

BSSockAddr::operator sockaddr_in() const
{
	return m_inaddr;
}

BSSockAddr::operator sockaddr() const
{
	return *((sockaddr*)(&m_inaddr));
}

uint16 BSSockAddr::get_port() const
{
	return ntohs(m_inaddr.sin_port);
}

std::string BSSockAddr::get_ipaddr_string() const
{
	return BSIpAddr::make_ipaddr_by_sockaddr(m_inaddr).to_str();
}

uint32 BSSockAddr::get_ipaddr_num() const
{
	return BSIpAddr::make_ipaddr_by_sockaddr(m_inaddr).to_num();
}

std::string BSSockAddr::get_addr_desc() const
{
	char addr[24] = {0};
	sprintf(addr, "%s:%u", get_ipaddr_string().c_str(), get_port());
	return addr;
}

void BSSockAddr::reset()
{
	memset(&m_inaddr, 0, sizeof(m_inaddr));
	m_inaddr.sin_family = AF_INET;
}

void BSSockAddr::set_port( uint16 port )
{
	m_inaddr.sin_port = htons(port);
}

bool BSSockAddr::is_any() const
{
	return *(BSSockAddr*)this == AnyAddr;
}

bool BSSockAddr::is_error() const
{
	return *(BSSockAddr*)this == ErrorAddr;
}

int32 BSSockAddr::length()
{
	return sizeof(sockaddr_in);
}

int32 BSSockAddr::length() const
{
	return sizeof(sockaddr_in);
}

}//namespace basis
