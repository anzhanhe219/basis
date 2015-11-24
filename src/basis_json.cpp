#include "basis_json.h"

namespace basis
{
//////////////////////////////////////////////////////////////////////////
// json 数值
//////////////////////////////////////////////////////////////////////////
BSJsonValue::BSJsonValue(ValueType type)
	: mType(type)
	, m_doub_value(0.0)
	, m_int_value(0)
	, m_bool_value(false)
	, m_str_value("")
{

}

BSJsonValue::BSJsonValue(int64 nValue)
	: mType(JSON_INTEGER)
	, m_doub_value(0.0)
	, m_int_value(nValue)
	, m_bool_value(false)
	, m_str_value("")
{

}

BSJsonValue::BSJsonValue(double dValue)
	: mType(JSON_REAL)
	, m_doub_value(dValue)
	, m_int_value(0)
	, m_bool_value(false)
	, m_str_value("")
{

}

BSJsonValue::BSJsonValue(const string& sValue)
	: mType(JSON_STRING)
	, m_str_value(sValue)
	, m_doub_value(0.0)
	, m_int_value(0)
	, m_bool_value(false)
{

}

BSJsonValue::BSJsonValue(bool bValue)
	: mType(JSON_BOOL)
	, m_doub_value(0.0)
	, m_int_value(0)
	, m_bool_value(bValue)
	, m_str_value("")
{

}

BSJsonValue::BSJsonValue(const BSJsonValue& jsonValue)
{
	*this = jsonValue;
}

BSJsonValue& BSJsonValue::operator=(const BSJsonValue& jsonValue)
{
	if (&jsonValue != this)
	{
		mType = jsonValue.mType;
		m_str_value = jsonValue.m_str_value;
		m_doub_value = jsonValue.m_doub_value;
		m_int_value = jsonValue.m_int_value;
		m_bool_value = jsonValue.m_bool_value;
		m_obj_value = jsonValue.m_obj_value;
		m_arr_value = jsonValue.m_arr_value;
	}

	return *this;
}

bool BSJsonValue::operator==(const BSJsonValue& jsonValue) const
{
	if (mType != jsonValue.mType) return false;
	
	switch (mType)
	{
	case JSON_NONE:
		return true;
	case JSON_STRING:
		return m_str_value == jsonValue.m_str_value;
	case JSON_REAL:
		return m_doub_value == jsonValue.m_doub_value;
	case JSON_INTEGER:
		return m_int_value == jsonValue.m_int_value;
	case JSON_BOOL:
		return m_bool_value == jsonValue.m_bool_value;
	case JSON_OBJECT:
		if (m_obj_value.size() != jsonValue.m_obj_value.size())
			return false;
		for (OBJECT::const_iterator mIt = m_obj_value.begin(), otherIt = jsonValue.m_obj_value.begin();
			mIt != m_obj_value.end(); ++mIt, ++otherIt)
		{
			if (mIt->first != otherIt->first || mIt->second != otherIt->second)
				return false;
		}
		return true;
	case JSON_ARRARY:
		if (m_arr_value.size() != jsonValue.m_arr_value.size()) return false;
		for (uint32 i = 0; i < m_arr_value.size(); ++i)
		{
			if (m_arr_value[i] != jsonValue.m_arr_value[i]) return false;
		}
		return true;
	default:
		return false;
 	}

	return false;
}

bool BSJsonValue::operator!= (const BSJsonValue& jsonValue) const
{
	return !(this->operator==(jsonValue));
}

void BSJsonValue::clear()
{
	mType = JSON_NONE;
	m_str_value.clear();
	m_doub_value = 0.0;
	m_int_value = 0;
	m_bool_value = false;
	m_obj_value.clear();
	m_arr_value.clear();

}

int64& BSJsonValue::as_int()
{
	clear();
	mType = JSON_INTEGER;
	return m_int_value;
}

string& BSJsonValue::as_string()
{
	clear();
	mType = JSON_STRING;
	return m_str_value;
}

double& BSJsonValue::as_real()
{
	clear();
	mType = JSON_REAL;
	return m_doub_value;
}

bool& BSJsonValue::as_bool()
{
	clear();
	mType = JSON_BOOL;
	return m_bool_value;
}

BSJsonValue::OBJECT& BSJsonValue::as_object()
{
	clear();
	mType = JSON_OBJECT;
	return m_obj_value;
}

BSJsonValue::ARRARY& BSJsonValue::as_arr()
{
	clear();
	mType = JSON_ARRARY;
	return m_arr_value;
}

int64 BSJsonValue::get_int()
{
	assert(mType == JSON_INTEGER);
	return m_int_value;
}

string BSJsonValue::get_string()
{
	assert(mType = JSON_STRING);
	return m_str_value;
}

double BSJsonValue::get_real()
{
	assert(mType == JSON_REAL);
	return m_doub_value;
}

bool BSJsonValue::get_bool()
{
	assert(mType == JSON_BOOL);
	return m_bool_value;
}

BSJsonValue::OBJECT& BSJsonValue::get_object()
{
	assert(mType == JSON_OBJECT);
	return m_obj_value;
}

BSJsonValue::ARRARY& BSJsonValue::get_arr()
{
	assert(mType == JSON_ARRARY);
	return m_arr_value;
}

BSJsonValue* BSJsonValue::insertObjValue(const string& _key, BSJsonValue::ValueType _type)
{
	if (_key.empty()) return NULL;

	m_obj_value[_key] = BSJsonValue(_type);

	return &m_obj_value[_key];
}

BSJsonValue* BSJsonValue::insertArrValue(BSJsonValue::ValueType _type)
{
	m_arr_value.push_back(BSJsonValue(_type));

	return &m_arr_value.back();
}

bool BSJsonValue::putObjValue(const string& _key, BSJsonValue& _value)
{
	if (_key.empty()) return false;

	OBJECT::iterator it = m_obj_value.find(_key);
	if (it != m_obj_value.end())
	{
		it->second = _value;
	}

	m_obj_value[_key] = _value;

	return true;
}

bool BSJsonValue::putArrValue(BSJsonValue& _value)
{
	if (BSJsonValue::JSON_NONE == _value.type()) return false;

	m_arr_value.push_back(_value);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// buff 内存
//////////////////////////////////////////////////////////////////////////
BSBuffer::BSBuffer(uint32 _capacity)
{
	if (_capacity)
	{
		m_capacity = BUFFER_SIZE;
	}

	m_used = 0;
	m_data = new char(m_capacity);
}

void BSBuffer::pushBackChar(char ch)
{
	if (m_used >= m_capacity)
	{
		Buffer tmp = m_data;
		m_data = new char(2*m_capacity + 1);
		memcpy(m_data, tmp, m_capacity);
		delete[] tmp;
	}

	memcpy(m_data + m_used, (void *)&ch, 1);

	++m_used;
}

//////////////////////////////////////////////////////////////////////////
// json解析器
//////////////////////////////////////////////////////////////////////////

static const unsigned char UTF8_LEAD_BITS[4] = { 0x00, 0xC0, 0xE0, 0xF0 };


const int BSJsonParser::asciiClass[] = {
	xx,      xx,      xx,      xx,      xx,      xx,      xx,      xx,
	xx,      C_WHITE, C_WHITE, xx,      xx,      C_WHITE, xx,      xx,
	xx,      xx,      xx,      xx,      xx,      xx,      xx,      xx,
	xx,      xx,      xx,      xx,      xx,      xx,      xx,      xx,

	C_SPACE, C_ETC,   C_QUOTE, C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,
	C_ETC,   C_ETC,   C_STAR,  C_PLUS,  C_COMMA, C_MINUS, C_POINT, C_SLASH,
	C_ZERO,  C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT,
	C_DIGIT, C_DIGIT, C_COLON, C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,

	C_ETC,   C_ABCDF, C_ABCDF, C_ABCDF, C_ABCDF, C_E,     C_ABCDF, C_ETC,
	C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,
	C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_ETC,
	C_ETC,   C_ETC,   C_ETC,   C_LSQRB, C_BACKS, C_RSQRB, C_ETC,   C_ETC,

	C_ETC,   C_LOW_A, C_LOW_B, C_LOW_C, C_LOW_D, C_LOW_E, C_LOW_F, C_ETC,
	C_ETC,   C_ETC,   C_ETC,   C_ETC,   C_LOW_L, C_ETC,   C_LOW_N, C_ETC,
	C_ETC,   C_ETC,   C_LOW_R, C_LOW_S, C_LOW_T, C_LOW_U, C_ETC,   C_ETC,
	C_ETC,   C_ETC,   C_ETC,   C_LCURB, C_ETC,   C_RCURB, C_ETC,   C_ETC
};

const int BSJsonParser::stateTransitionTable[NR_STATES][NR_CLASSES] = {
/*
                 white                                      1-9                                   ABCDF  etc
             space |  {  }  [  ]  :  ,  "  \  /  +  -  .  0  |  a  b  c  d  e  f  l  n  r  s  t  u  |  E  |  * */
/*start  GO*/ {GO,GO,-6,xx,-5,xx,xx,xx,xx,xx,CB,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*ok     OK*/ {OK,OK,xx,-8,xx,-7,xx,-3,xx,xx,CB,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*object OB*/ {OB,OB,xx,-9,xx,xx,xx,xx,SB,xx,CB,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*key    KE*/ {KE,KE,xx,xx,xx,xx,xx,xx,SB,xx,CB,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*colon  CO*/ {CO,CO,xx,xx,xx,xx,-2,xx,xx,xx,CB,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*value  VA*/ {VA,VA,-6,xx,-5,xx,xx,xx,SB,xx,CB,xx,MX,xx,ZX,IX,xx,xx,xx,xx,xx,FA,xx,NU,xx,xx,TR,xx,xx,xx,xx,xx},
/*array  AR*/ {AR,AR,-6,xx,-5,-7,xx,xx,SB,xx,CB,xx,MX,xx,ZX,IX,xx,xx,xx,xx,xx,FA,xx,NU,xx,xx,TR,xx,xx,xx,xx,xx},
/*string ST*/ {ST,xx,ST,ST,ST,ST,ST,ST,-4,EX,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST,ST},
/*escape EC*/ {xx,xx,xx,xx,xx,xx,xx,xx,ST,ST,ST,xx,xx,xx,xx,xx,xx,ST,xx,xx,xx,ST,xx,ST,ST,xx,ST,U1,xx,xx,xx,xx},
/*u1     U1*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,U2,U2,U2,U2,U2,U2,U2,U2,xx,xx,xx,xx,xx,xx,U2,U2,xx,xx},
/*u2     U2*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,U3,U3,U3,U3,U3,U3,U3,U3,xx,xx,xx,xx,xx,xx,U3,U3,xx,xx},
/*u3     U3*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,U4,U4,U4,U4,U4,U4,U4,U4,xx,xx,xx,xx,xx,xx,U4,U4,xx,xx},
/*u4     U4*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,UC,UC,UC,UC,UC,UC,UC,UC,xx,xx,xx,xx,xx,xx,UC,UC,xx,xx},
/*minus  MI*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,ZE,IT,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*zero   ZE*/ {OK,OK,xx,-8,xx,-7,xx,-3,xx,xx,CB,xx,xx,DF,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*int    IT*/ {OK,OK,xx,-8,xx,-7,xx,-3,xx,xx,CB,xx,xx,DF,IT,IT,xx,xx,xx,xx,DE,xx,xx,xx,xx,xx,xx,xx,xx,DE,xx,xx},
/*frac   FR*/ {OK,OK,xx,-8,xx,-7,xx,-3,xx,xx,CB,xx,xx,xx,FR,FR,xx,xx,xx,xx,E1,xx,xx,xx,xx,xx,xx,xx,xx,E1,xx,xx},
/*e      E1*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,E2,E2,xx,E3,E3,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*ex     E2*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,E3,E3,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*exp    E3*/ {OK,OK,xx,-8,xx,-7,xx,-3,xx,xx,xx,xx,xx,xx,E3,E3,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*tr     T1*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,T2,xx,xx,xx,xx,xx,xx,xx},
/*tru    T2*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,T3,xx,xx,xx,xx},
/*1      T3*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,CB,xx,xx,xx,xx,xx,xx,xx,xx,xx,OK,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*fa     F1*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,F2,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*fal    F2*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,F3,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*fals   F3*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,F4,xx,xx,xx,xx,xx,xx},
/*0      F4*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,CB,xx,xx,xx,xx,xx,xx,xx,xx,xx,OK,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*nu     N1*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,N2,xx,xx,xx,xx},
/*nul    N2*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,N3,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*null   N3*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,CB,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,OK,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*/      C1*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,C2},
/*/*     C2*/ {C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C3},
/**      C3*/ {C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,CE,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C2,C3},
/*_.     FX*/ {OK,OK,xx,-8,xx,-7,xx,-3,xx,xx,xx,xx,xx,xx,FR,FR,xx,xx,xx,xx,E1,xx,xx,xx,xx,xx,xx,xx,xx,E1,xx,xx},
/*\      D1*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,D2,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx},
/*\      D2*/ {xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,U1,xx,xx,xx,xx},
};

int BSJsonParser::xx = -1;

int BSJsonParser::topMode() 
{ 
	if (mModeStack.empty())
		return -1;
	return mModeStack.back(); 
}

bool BSJsonParser::popMode(Modes _mode) 
{
	if (!mModeStack.empty())
	{
		if (_mode == mModeStack.back())
		{
			mModeStack.pop_back();
			return true;
		}
	}

	return false;
}

BSJsonParser::BSJsonParser()
	: mState(GO)
	, mType(JSON_T_NONE)
	, mResult(NULL)
	, mEscape(false)
{
	mBuff.reserve(4096); // 预留4k的空间，防止重复开辟空间
	mModeStack.push_back(MODE_DONE);
}

bool BSJsonParser::parse(string& str_json, BSJsonValue& _result)
{
	mJsonStr = str_json;

	int ch;
	while (mJsonStr.nextChar(ch))
	{
		if (!parseChar(_result, ch))
		{
			break;
		}
	}

	if (MODE_DONE != mModeStack.back())
	{
		return false;
	}

	_result = *mResult;

	return true;
}

int BSJsonParser::utf8Check(char byte)
{
	unsigned char u = (unsigned char) byte;

	if(u < 0x80)
		return 1;

	if (0x80 <= u && u <= 0xBF) 
	{
		// 不符合多字节的编码规则
		return 0;
	}
	else if(u == 0xC0 || u == 0xC1) 
	{
		// 最小的两个字节都不能符合规则 0xC2
		return 0;
	}
	else if(0xC2 <= u && u <= 0xDF) 
	{
		// 2-byte sequence
		return 2;
	}
	else if(0xE0 <= u && u <= 0xEF) 
	{
		// 3-byte sequence
		return 3;
	}
	else if(0xF0 <= u && u <= 0xF4) 
	{
		// 4-byte sequence
		return 4;
	}
	else 
	{ 
		// u >= 0xF5
		// Restricted (start of 4-, 5- or 6-byte sequence) or invalid UTF-8
		return 0;
	}
}

bool BSJsonParser::utf8IsLegal(unsigned char* pos, uint32 length)
{
	if (0 == pos || 0 == length) return false;

	unsigned char a;
	// 现在的pos指向高位，将指针指向低位
	const unsigned char* srcptr = pos + length;
	switch (length)
	{
	default:
		return false;
		// Everything else falls through when true.
	case 4: 
		// 最后字节， 小于最小值， 大于最大值
		if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
	case 3: 
		// 第三个字节， 大于最小值， 小于最大值
		if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
	case 2:
		// 第二个字节，小于最大值
		if ((a = (*--srcptr)) > 0xBF) return false;
		// 分情况讨论最高的规则
		switch (*pos) 
		{
		case 0xE0:
			if (a < 0xA0) return false; 
			break;
		case 0xED:
			if (a > 0x9F) return false; 
			break;
		case 0xF0:
			if (a < 0x90) return false; 
			break;
		case 0xF4:
			if (a > 0x8F) return false; 
			break;
		default:
			if (a < 0x80) return false;
		}
	case 1:
		if (*pos >= 0x80 && *pos < 0xC2) return false;
	}
	return *pos <= 0xF4;
}

void BSJsonParser::startObject()
{
	BSJsonValue* pInsertVa = NULL;
	if (mValueStack.empty())
	{
		mResult = new BSJsonValue(BSJsonValue::JSON_OBJECT);
		mValueStack.push_back(mResult);
		return;
	}
	else 
	{
		BSJsonValue* pTopValue = mValueStack.back();
		if (BSJsonValue::JSON_OBJECT == pTopValue->type())
		{
			pInsertVa = pTopValue->insertObjValue(mKey, BSJsonValue::JSON_OBJECT); 
			if (NULL == pInsertVa) return;
		}
		else if (BSJsonValue::JSON_ARRARY == pTopValue->type())
		{
			pInsertVa = pTopValue->insertArrValue(BSJsonValue::JSON_OBJECT);
			if (NULL == pInsertVa) return;
		}
	}

	mValueStack.push_back(pInsertVa);
}

void BSJsonParser::endObjectOrArr()
{
	if (!mValueStack.empty())
	{
		mValueStack.pop_back();
	}
}

void BSJsonParser::startArray()
{
	BSJsonValue* pInsertVa = NULL;
	if (mValueStack.empty())
	{
		mResult = new BSJsonValue(BSJsonValue::JSON_ARRARY);
		mValueStack.push_back(mResult);
		return;
	}
	else
	{
		BSJsonValue* pTopValue = mValueStack.back();
		if (BSJsonValue::JSON_OBJECT == pTopValue->type())
		{
			// 返回新插入值的地址
			pInsertVa = pTopValue->insertObjValue(mKey, BSJsonValue::JSON_ARRARY);
			if (NULL == pInsertVa) return;
		}
		else if(BSJsonValue::JSON_ARRARY == pTopValue->type())
		{
			pInsertVa = pTopValue->insertArrValue(BSJsonValue::JSON_ARRARY);
			if (NULL == pInsertVa) return;
		}
	}

	mValueStack.push_back(pInsertVa);
}

bool BSJsonParser::parseBuff()
{
	if (JSON_T_NONE == mType) return true;

	BSJsonValue _value;
	do 
	{
		if (JSON_T_STRING == mType)
		{
			string str(mBuff.begin(), mBuff.end()); 
			_value.as_string() = str;
			break;
		}
		else if(JSON_T_INTEGER == mType)
		{
			int64 nVa = strtoul(mBuff.c_str(), NULL, 10);
			_value.as_int() = nVa;
			break;
		}
		else if (JSON_T_TRUE == mType)
		{
			_value.as_bool() = true;
			break;
		}
		else if (JSON_T_FALSE == mType)
		{
			_value.as_bool() = false;
			break;
		}
		else if (JSON_T_FLOAT == mType)
		{
			_value.as_real() = strtod(mBuff.c_str(), NULL);
			break;
		}


	} while (false);

	mBuff.clear();

	// 获取key值，返回
	if (!mModeStack.empty() && Modes::MODE_KEY ==  mModeStack.back())
	{
		if (BSJsonValue::JSON_STRING != _value.type())
		{
			return false;
		}

		mKey = _value;
		return true;
	}

	if (!mValueStack.empty())
	{
		BSJsonValue* pTopValue = mValueStack.back();
		if (BSJsonValue::JSON_OBJECT == pTopValue->type())
		{
			pTopValue->putObjValue(mKey, _value);
			mKey.clear();
		}
		else if (BSJsonValue::JSON_ARRARY == pTopValue->type())
		{
			pTopValue->putArrValue(_value);
		}
	}

	return true;
}

bool BSJsonParser::parseChar(BSJsonValue& _result, int nextChar)
{
	int nextClass, nextState;
	unsigned char ch = static_cast<unsigned char>(nextChar);

	// Determine the character's class.
	if (ch == 0) return false;
	if (ch >= 0x80)
	{
		// 暂时注释 这里为处理utf的过程
		nextClass = C_ETC; 
		int count = utf8Check(nextChar);
		if (!count)
		{
		throw "Bad character.";
		}

		char buffer[4];
		buffer[0] = nextChar;
		for(int i = 1; i < count; ++i)
		{
		int c = 0;
		if (!mJsonStr.nextChar(c)) return false;
		buffer[i] = c;
		}

		/*if (!utf8IsLegal((unsigned char*) buffer, count))
		{
		return false;
		}*/

		// 将编码放到缓冲区中
		for(int i = 0; i < count; ++i)
		{
			mBuff.push_back(buffer[i]);
		}

		return true;
	}
	else
	{
		nextClass = asciiClass[nextChar];
		if (nextClass < C_SPACE || nextClass >= NR_CLASSES) return false; 
		
		if (nextClass == C_SPACE || nextClass == C_WHITE) return true;
	}

	addCharToBuff(ch);

	nextState = stateTransitionTable[mState][nextClass];
	if (nextState >= 0)
	{
		mState = nextState;
	}
	else
	{
		switch (nextState)
		{
		// Unicode 编码 
		case UC:
			{
			mBuff.pop_back();
			uint32 _code;
			if(!getChUnicode(_code)) return false;
			string str = unicodeToUtf8(_code);
			mBuff.append(str.begin(), str.end()); ///////////???????????
			mState = ST;
			}
			break;
		// {
		case -6:
			mBuff.pop_back();
			startObject();
			putMode(MODE_KEY); // 将要出现数据状态入栈
			mType = JSON_T_NONE;
			mState = OB;
		break;
		// 字符串开始
		case SB:
			mBuff.pop_back();
			mType = JSON_T_STRING;
			mState = ST;
			break;
		// Null值
		case NU:
			mType = JSON_T_NULL;
			mState = N1;
			break;
		// " 结束为止的引号
		case -4:
			mBuff.pop_back();
			switch (topMode())
			{
			case MODE_KEY:
				{
				// 获取key值，将key值，放入结果中
				parseBuff();
				// 定义现在数据类型
				mType = JSON_T_NONE;
				// 将要出现的情况
				mState = CO; 
				break;
				}
			case MODE_OBJECT:
				{
				parseBuff();
				mState = OK;
				mType = JSON_T_NONE; 
				break;
				}
			case MODE_ARRAY:
				{
				parseBuff();
				mState = OK;
				mType = JSON_T_NONE;
				break;
				}
			default:
				return false;
			}
			break;
		// :
		case -2:
			mBuff.pop_back();
			if (!popMode(MODE_KEY)) return false;
			putMode(MODE_OBJECT);
			mState = VA;
			break;
		// bool值 false
		case FA:
			mType = JSON_T_FALSE;
			mState = F1;
			break;
		// bool值 true
		case TR:
			mType = JSON_T_TRUE;
			mState = T1;
			break;
		// 整数
		case IX:
			mType = JSON_T_INTEGER;
			mState = IT;
			break;
		// 整数 0
		case ZX:
			mType = JSON_T_INTEGER;
			mState = ZE;
			break;
		// 带 *e* 浮点型   
		case DE:
			mType = JSON_T_FLOAT;
			mState = E1;
			break;
		// 带 *.* 浮点型
		case DF:
			mType = JSON_T_FLOAT;
			mState = FR;
			break;
		// }
		case -8:
			{
			mBuff.pop_back();
			parseBuff(); // 必须放到状态出栈的前面，因为要用到状态栈中的状态
			endObjectOrArr();
			if (!popMode(MODE_OBJECT)) return false;
			mType = JSON_T_NONE;
			mState = OK;
			}
			break;
		// [ 待写
		case -5:
			{
				mBuff.pop_back();
				startArray();
				putMode(MODE_ARRAY);
				mState = AR;
				mType = JSON_T_NONE;
			}
			break;
		// ] 待写
		case -7:
			{
				mBuff.pop_back();
				parseBuff();
				endObjectOrArr();
				if (!popMode(MODE_ARRAY)) return false;
				mState = OK;
				mType = JSON_T_NONE;
			}
			break;
		// ,
		case -3:
			{
				mBuff.pop_back();
				switch (topMode())
				{
				case MODE_OBJECT:
					{
						parseBuff();
						// 在object里中的子object
						if (!popMode(MODE_OBJECT)) return false;
						putMode(MODE_KEY);
						mType = JSON_T_NONE; //值已清空，状态为none
						mState = KE;
					}
					break;
				case  MODE_ARRAY:
					{
						parseBuff();
						// 这里不能将数组状态出栈
						mState = VA;
						mType = JSON_T_NONE;
					}
					break;
				default:
					return false;
				}
				break;
			}	
			default:
				return false;
				break;
		}

	}

	return true;
}

void BSJsonParser::addEscapeChar(int ch)
{
	mEscape = false;
	// 移出转义字符
	mBuff.pop_back();

	switch (ch)
	{
	case 'b': // 回退
		mBuff.push_back('\b');
	case 'f': // 换页
		mBuff.push_back('\f');
	case 'n': // 换行
		mBuff.push_back('\n');
	case 'r': // 回车
		mBuff.push_back('\r');
	case 't':
		mBuff.push_back('\t');
	case '"':
		mBuff.push_back('"');
		break;
	case '\\':
		mBuff.push_back('\\');
		break;
	case '/':
		mBuff.push_back('/');
		break;
	case  'u':
		mBuff.push_back('\\');
		mBuff.push_back('u');
		break;
	default:
		break;
	}
}

void BSJsonParser::addCharToBuff(int ch)
{
	// 如果是转义字符
	if (mEscape)
	{
		addEscapeChar(ch);
	}

	mBuff.push_back(ch);
}

// 获取字符的十六进制
bool BSJsonParser::getChHex( char ch,uint8& hex)
{
	hex = 0;

	if (mJsonStr.eof()) return false;

	if (ch >= '0' && ch <= '9')
	{
		hex = ch - '0';
	}
	else if (ch >= 'a' && ch <= 'f')
	{
		hex = ch - 'a' + 10;
	}
	else if (ch >= 'A' && ch <= 'F')
	{
		hex = ch - 'A' + 10;
	}

	return true;
}

// 获取字符的unicode编码
bool BSJsonParser::getChUnicode(uint32& unicode)
{
	unicode = 0;
	if (mJsonStr.remain() < 6) return false;

	if (mJsonStr.currenChar() != '\\') return false;
	mJsonStr.movePos(1);

	if (mJsonStr.currenChar() != 'u') return false;
	mJsonStr.movePos(1);

	// 现在unicode的标准是按照4位
	uint8 _hex = 0;
	for (uint32 i = 0; i < 4; ++i)
	{
		if (!getChHex(mJsonStr.currenChar(), _hex)) return false;
		mJsonStr.movePos(1);

		unicode <<= 4;
		unicode += _hex;
	}

	// 16进制代理区
	if (unicode >= 0xD800 && unicode <= 0xDBFF)
	{
		uint32 highSur = unicode;
		uint32 lowSur = 0;

		if (mJsonStr.remain() < 6) return false;

		if (mJsonStr.currenChar() != '\\') return false;
		mJsonStr.movePos(1);

		if (mJsonStr.currenChar() != 'u') return false;
		mJsonStr.movePos(1);

		uint8 _hex = 0;
		for (uint32 i = 0; i < 4; ++i)
		{
			if (!getChHex(mJsonStr.currenChar(), _hex)) return false;
			mJsonStr.movePos(1);

			lowSur <<= 4;
			lowSur += _hex;
		}

		unicode = 0x10000 + ((highSur & 0x3FF) << 10) + lowSur & 0x3FF;
	}

	return true;
}

string BSJsonParser::unicodeToUtf8(uint32 unicode)
{
	string result;

	// based on description from http://en.wikipedia.org/wiki/UTF-8

	if (unicode <= 0x7f)
	{
		result.resize(1);
		result[0] = static_cast<char>(unicode);
	}
	else if (unicode <= 0x7FF)
	{
		result.resize(2);
		result[1] = static_cast<char>(0x80 | (0x3f & unicode));
		result[0] = static_cast<char>(0xC0 | (0x1f & (unicode >> 6)));
	}
	else if (unicode <= 0xFFFF)
	{
		result.resize(3);
		result[2] = static_cast<char>(0x80 | (0x3f & unicode));
		result[1] = static_cast<char>(0x80 | (0x3f & (unicode >> 6)));
		result[0] = static_cast<char>(0xE0 | (0xf & (unicode >> 12)));
	}
	else if (unicode <= 0x10FFFF) //unicode规定最大单汉字编码是0x10ffff
	{
		result.resize(4);
		result[3] = static_cast<char>(0x80 | (0x3f & unicode));
		result[2] = static_cast<char>(0x80 | (0x3f & (unicode >> 6)));
		result[1] = static_cast<char>(0x80 | (0x3f & (unicode >> 12)));
		result[0] = static_cast<char>(0xF0 | (0x7 & (unicode >> 18)));
	}

	return result;
}

}