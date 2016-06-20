// 2015-11-10
// basis_md5.h
// 
// MD5÷ß≥÷¿‡

#ifndef _BASIS_MD5_H_
#define _BASIS_MD5_H_

#include "basis_define.h"

namespace basis
{
////////////////////////////////////////////////////////////////////////////////
// class MD5
////////////////////////////////////////////////////////////////////////////////
class MD5 
{
public:
	MD5();
	void reset();
	string update(const string &str);
	string update(const void *input, uint32 length);
	string updatefile(const string& filepath);
		
private:
	string final();	
	void update_input(const uint8 *input, uint32 length);
	void transform(const uint8 block[64]);
	void encode(const uint32* input, uint8* output, uint32 length);
	void decode(const uint8 *input, uint32 *output, uint32 length);

private:
	uint32 m_state[4];	// state (ABCD) 
	uint32 m_count[2];	// number of bits, modulo 2^64 (low-order word first) 
	uint8 m_buffer[64];	// input buffer 
};

}
#endif