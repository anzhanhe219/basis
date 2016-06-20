#ifndef __IPADDR_H__
#define __IPADDR_H__

#include "basis_define.h"

namespace basis
{

////////////////////////////////////////////////////////////////////////////////
// class BSIpAddr
////////////////////////////////////////////////////////////////////////////////
class BSIpAddr
{
public:
	static BSIpAddr make_ipaddr_by_host(const string& host);
	static BSIpAddr make_ipaddr_by_ip(const string& ip);
	static BSIpAddr make_ipaddr_by_num(uint32 ipnum);
	static BSIpAddr make_ipaddr_by_sockaddr(const sockaddr_in& sock_addr);

	const static BSIpAddr ErrorIp;
	const static BSIpAddr AnyIp;

public:
	~BSIpAddr() {}

	BSIpAddr(const BSIpAddr& r);
	const BSIpAddr& operator=(const BSIpAddr& r);
	bool operator==(const BSIpAddr& r);

public:
	bool is_error() const;
	bool is_any() const;

	uint32 to_num() const;
	string to_str() const ;
	const char* to_str(string& ip) const;
	bool to_sockaddr_in(sockaddr_in& sock_addr) const;

protected:
	BSIpAddr(const in_addr& ipaddr) : m_ip_addr(ipaddr) {}
	BSIpAddr(uint32 ipnum);

private:
	in_addr m_ip_addr; //v4
};

} // namespace basis

#endif//__IPADDR_H__
