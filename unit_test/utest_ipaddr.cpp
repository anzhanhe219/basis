#include "basis_ipaddr.h"
#include "gtest/gtest.h"

TEST(Iptest, ttt)
{
	EXPECT_EQ(basis::BSIpAddr::make_ipaddr_by_ip("115.159.54.142").to_str(), "115.159.54.142");
}