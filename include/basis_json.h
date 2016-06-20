#ifndef _BASIS_JSON_H_
#define _BASIS_JSON_H_

#include "basis_define.h"

#ifdef __WINDOWS__

namespace basis
{
//////////////////////////////////////////////////////////////////////////
// 要解析的json字符串
//////////////////////////////////////////////////////////////////////////
class BSJsonStr
{
public:
	BSJsonStr()
	{

	}
	BSJsonStr(const string& str)
		: mJsonStr(str)
		, mP(0)
	{
	}

	bool nextChar(int& c)
	{
		if (mP >= mJsonStr.size())
		{
			return false;
		}

		c =  (int)mJsonStr[mP];
		++mP;
		return true;
	}

	void setSource(const string& str)
	{
		if (!str.empty()) return;

		mJsonStr = str;
	}

	char currenChar()
	{
		if (mP >= mJsonStr.size())
		{
			return ' ';
		}

		return mJsonStr[mP];
	}

	uint32 remain()
	{
		return mJsonStr.size() > mP? mJsonStr.size() - mP: 0;
	}

	void movePos(int dis)
	{
		if ((dis > 0) && (mP + dis >= mJsonStr.size())) return;
		else if (dis < 0 && mP + dis < 0) return;

		mP = mP + dis;
	}

	bool eof()
	{
		return mP >= mJsonStr.size();
	}

	void clearPlace()
	{
		mP = 0;
	}

private:
	string mJsonStr; // json字符串
	uint32 mP; // 当前位置
};

//////////////////////////////////////////////////////////////////////////
// json 数值
//////////////////////////////////////////////////////////////////////////
class BSJsonValue	
{
public:
	typedef map<string, BSJsonValue> OBJECT;
	typedef vector<BSJsonValue> ARRARY;
		
	enum ValueType
	{
		JSON_NONE, /// null
		JSON_OBJECT, ///  object
		JSON_ARRARY, // arrary
		JSON_STRING, // string 
		JSON_INTEGER, // int
		JSON_REAL, // double 
		JSON_BOOL // bool 
	};

public:

	BSJsonValue(ValueType tpye = JSON_NONE);
	BSJsonValue(int64 nValue);
	BSJsonValue(double dValue);
	BSJsonValue(const string& sValues);
	BSJsonValue(bool bValue);

	BSJsonValue(const BSJsonValue& jsonValue);
	BSJsonValue& operator=(const BSJsonValue& jsonValue);
	bool operator==(const BSJsonValue& jsonValue) const;
	bool operator!=(const BSJsonValue& jsonValue) const;
	operator string() { return m_str_value; }

	void clear();

	ValueType type() { return mType;}

	int64& as_int();
	string& as_string();
	double& as_real();
	bool& as_bool();
	OBJECT& as_object();
	ARRARY& as_arr();

	int64 get_int();
	string get_string();
	double get_real();
	bool get_bool();
	OBJECT& get_object();
	ARRARY& get_arr();
	
	// 首次插入object值
	BSJsonValue* insertObjValue(const string& _key, BSJsonValue::ValueType _type);

	// array首次插入值
	BSJsonValue* insertArrValue(BSJsonValue::ValueType _type);

	// 放入object值
	bool putObjValue(const string& _key, BSJsonValue& _value);

	// array放入值
	bool putArrValue(BSJsonValue& _value);

private:
	ValueType mType;
	string m_str_value;
	double m_doub_value;
	int64 m_int_value;
	bool m_bool_value;
	OBJECT m_obj_value;
	ARRARY m_arr_value;
};

//////////////////////////////////////////////////////////////////////////
// buff内存
//////////////////////////////////////////////////////////////////////////
class BSBuffer
{
	typedef char* Buffer;
	enum DefaultMem
	{
		BUFFER_SIZE = 4096 // 4k
	};

public:
	BSBuffer(uint32 capacity = 0);
	void pushBackChar(char ch);

private:
	uint32 m_capacity; // 最大容量
	uint32 m_used; // 已经使用内存
	Buffer m_data;
};

//////////////////////////////////////////////////////////////////////////
// json解析器 (可以编码utf8)
//////////////////////////////////////////////////////////////////////////
class BSJsonParser
{
	enum Classes
	{
		C_SPACE,  /* space */
		C_WHITE,  /* other whitespace */
		C_LCURB,  /* {  */
		C_RCURB,  /* } */
		C_LSQRB,  /* [ */
		C_RSQRB,  /* ] */
		C_COLON,  /* : */
		C_COMMA,  /* , */
		C_QUOTE,  /* " */
		C_BACKS,  /* \ */
		C_SLASH,  /* / */
		C_PLUS,   /* + */
		C_MINUS,  /* - */
		C_POINT,  /* . */
		C_ZERO ,  /* 0 */
		C_DIGIT,  /* 123456789 */
		C_LOW_A,  /* a */
		C_LOW_B,  /* b */
		C_LOW_C,  /* c */
		C_LOW_D,  /* d */
		C_LOW_E,  /* e */
		C_LOW_F,  /* f */
		C_LOW_L,  /* l */
		C_LOW_N,  /* n */
		C_LOW_R,  /* r */
		C_LOW_S,  /* s */
		C_LOW_T,  /* t */
		C_LOW_U,  /* u */
		C_ABCDF,  /* ABCDF */
		C_E,      /* E */
		C_ETC,    /* everything else */
		C_STAR,   /* * */   
		NR_CLASSES
	};

	enum States
		/// State codes
	{
		GO,  /* start    */
		OK,  /* ok       */
		OB,  /* object   */
		KE,  /* key      */
		CO,  /* colon    */
		VA,  /* value    */
		AR,  /* array    */
		ST,  /* string   */
		EC,  /* escape   */
		U1,  /* u1       */
		U2,  /* u2       */
		U3,  /* u3       */
		U4,  /* u4       */
		MI,  /* minus    */
		ZE,  /* zero     */
		IT,  /* integer  */
		FR,  /* fraction */
		E1,  /* e        */
		E2,  /* ex       */
		E3,  /* exp      */
		T1,  /* tr       */
		T2,  /* tru      */
		T3,  /* true     */
		F1,  /* fa       */
		F2,  /* fal      */
		F3,  /* fals     */
		F4,  /* false    */
		N1,  /* nu       */
		N2,  /* nul      */
		N3,  /* null     */
		C1,  /* /        */
		C2,  /* / *     */
		C3,  /* *        */
		FX,  /* *.* *eE* */
		D1,  /* second UTF-16 character decoding started by \ */
		D2,  /* second UTF-16 character proceeded by u */
		NR_STATES
	};

	enum Modes // 数据状态
		/// Modes that can be pushed on the _pStack.
	{
		MODE_ARRAY = 1,
		MODE_DONE = 2,
		MODE_KEY = 3,
		//MODE_VALUE = 4,
		MODE_OBJECT = 4, // 代表两种： key -- value 键值对 和 value 
	};

	enum Actions
	{
		CB = -10, /* _comment begin */
		CE = -11, /* _comment end */
		FA = -12, /* 0 */
		TR = -13, /* 0 */
		NU = -14, /* null */
		DE = -15, /* double detected by exponent e E */
		DF = -16, /* double detected by fraction . */
		SB = -17, /* string begin */
		MX = -18, /* integer detected by minus */
		ZX = -19, /* integer detected by zero */
		IX = -20, /* integer detected by 1-9 */
		EX = -21, /* next char is _escaped */
		UC = -22  /* Unicode character read */
	};

	enum JSONType
	{
		JSON_T_NONE = 0,
		JSON_T_INTEGER,
		JSON_T_FLOAT,
		JSON_T_NULL,
		JSON_T_TRUE,
		JSON_T_FALSE,
		JSON_T_STRING,
		JSON_T_MAX
	};	

	int topMode();

	bool popMode(Modes _mode);

	void putMode(Modes _mode)
	{
		mModeStack.push_back(_mode); 
	}

public:
	BSJsonParser();

	int utf8Check(char byte);
	bool utf8IsLegal(unsigned char* pos, uint32 length);
		

	bool parse(string& str_json, BSJsonValue& _result);
	bool parseChar(BSJsonValue& _result, int code);
		
	void startObject();

	void endObjectOrArr();

	void startArray();

	bool parseBuff();
	void addEscapeChar(int ch);
	void addCharToBuff(int ch);
		
	// 获取字符的十六进制
	bool getChHex(char ch,uint8& hex);
		
	// 获取字符的unicode编码
	bool getChUnicode(uint32& unicode);
	// unicode转utf8
	string unicodeToUtf8(uint32 unicode);
		
private:
	static const int asciiClass[128];
	static const int stateTransitionTable[NR_STATES][NR_CLASSES];
	static int xx;

private:
	int mState;
	int mType; // 正在解析的数据类型
	string mKey; // 最近的key值
	string mModeStack; // 数据状态栈
	BSJsonValue* mResult;
	vector<BSJsonValue*> mValueStack; // 数值栈

	string mBuff; // 存放字符串的内存
	bool mEscape; // 是否是转义字符

	BSJsonStr mJsonStr; // json字符串
		
	//BSJsonValue* mPLastObOrVar; // 最近的object或者是数组（解决层层嵌套问题）
};

}

#endif // __WINDOWS__

#endif // !_BASIS_JSON_H_