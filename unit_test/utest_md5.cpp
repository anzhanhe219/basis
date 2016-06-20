#include "gtest/gtest.h"
#include "basis_md5.h"

TEST(md5, update)
{
	EXPECT_EQ(basis::MD5().update("abcd"), "E2FC714C4727EE9395F324CD2E7F331F");
}