#ifndef _BASIS_STR_TOOL_H_
#define _BASIS_STR_TOOL_H_

#include "basis_define.h"
#include <limits>

namespace basis
{
////////////////////////////////////////////////
// 字符串的相关操作
// class BSStrTool
////////////////////////////////////////////////
class BSStrTool
{
public:
	// 去除字符串头部指定字符
	static void wipeHead(string& str, const char* ch = " \t\r\n");
	// 去除字符串尾部指定字符
	static void wipeTail(string& str, const char* ch = " \t\r\n");
	// 去除字符串匹配字符
	static void wipeMatch(string& str, const char* ch = " \t\r\n");

	// 字符串转换大小写
	static void toUpper(string& str);
	static void toLower(string& str);

	// 替换指定字符集中的字符、字符串
	static void replaceChar(string& str, const char* exceptedChar, char benchChar);
	static void replaceString(string& str, const char* oldStr, const char* newChar);

	// 字符串拼接(最后一个字符串不连接分隔字符串)
	static string splicing(const vector<string>& strList, const string& sep);
	static void splicing(string& desStr, const vector<string>& strList, const string& sep);

	// 将字符串按照指定字符分割
	static uint32 split(const string& str, vector<string>& items, const char* sep);

	// 格式化输出string
	static string sprintf(const char* format, ...);

	// 将字符串转换成无符号整型
	static bool toUint32(const string& str, uint32& uValue, uint32 radix = 10);
	static uint32 toUint32Def(const string& str, uint32 defalut = 0, uint32 radix = 10);

	// 将字符串转换成有符号整型
	static bool toInt32(const string& str, int32& nValue, uint32 radix = 10);
	static uint32 toInt32Def(const string& str, int32 defalut = 0, uint32 radix = 10);
	
	// 将字符串转换成double型
	static bool toDoulbe(const string& str, double& dValue);
	static double toDoulbeDef(const string& str, double defalut = 0.0);

	// 转换成字符串
	template<typename T>
	static string toStr(T _value)
	{	
		ostringstream ostr;
		// 统一设置成double 10进制显示精度
		ostr.precision(numeric_limits<double>::digits10);
		ostr<<_value;
		return ostr.str();
	}

	// uint8类型 特化
	static string toStr(uint8 _value)
	{
		ostringstream ostr;
		ostr.precision(numeric_limits<double>::digits10);
		ostr<<(uint32)_value;
		return ostr.str();
	}
};

}
#endif