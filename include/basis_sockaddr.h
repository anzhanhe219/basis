#ifndef __SOCKET_ADDR_H__
#define __SOCKET_ADDR_H__

#include "basis_define.h"
#include "basis_ipaddr.h"

namespace basis
{

////////////////////////////////////////////////////////////////////////////////
// class BSSockAddr
////////////////////////////////////////////////////////////////////////////////
class BSSockAddr
{
public:
	static BSSockAddr make_sock_addr(const string& addr); // "192.168.0.1:6669" or "www.baidu.com:80"
	static BSSockAddr make_sock_addr(uint32 ipaddr, uint16 port);
	static BSSockAddr make_sock_addr(const BSIpAddr& ipaddr, uint16 port);
	static BSSockAddr make_sock_addr(const string& ipaddr, uint16 port); // host:"www.baidu.com" or "192.168.0.1"
	static BSSockAddr make_sock_addr(const sockaddr_in& inaddr);
	static BSSockAddr make_sock_addr(const sockaddr& addr);

	const static BSSockAddr AnyAddr;
	const static BSSockAddr ErrorAddr;

public:
	BSSockAddr();
	~BSSockAddr();

	BSSockAddr(const BSSockAddr& r);
	BSSockAddr& operator=(const BSSockAddr& r);
	bool operator==(const BSSockAddr& r);

	operator const sockaddr_in* () const;
	operator const sockaddr* () const;
	operator sockaddr_in() const;
	operator sockaddr() const;
	int32 length();
	int32 length() const;

public:
	uint16 get_port() const;
	string get_ipaddr_string() const;
	uint32 get_ipaddr_num() const;	
	string get_addr_desc() const;

	bool is_any() const;
	bool is_error() const;

private:
	BSSockAddr(uint16 port);
	BSSockAddr(const BSIpAddr& ip, uint16 port);
	BSSockAddr(const sockaddr_in& inaddr);
	BSSockAddr(const sockaddr& addr);

private:
	void reset();
	void set_port(uint16 port);

private:
	sockaddr_in  m_inaddr;
};

}//namespace basis

#endif//_XCORE_SOCK_ADDR_H_
