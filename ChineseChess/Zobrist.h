#pragma once
#if 1
#include <Windows.h>
#endif

#ifdef DEBUG
#include <iostream>
#include <iomanip>
using namespace std;
#endif // DEBUG


// RC4密码流生成器
class RC4Struct {
public:
    BYTE s[256]{};
	int x, y;
public:
	RC4Struct();			// 用空密钥初始化密码流生成器
	BYTE GenByte();			// 生成下一字节的流密码
	DWORD Gen4Byte();		// 生成下四字节的流密码
};

class ZobristStruct {
public:
	DWORD key0, key1, key2;
public:
	friend class PositionStruct;
	ZobristStruct();		// 构造函数，相当于InitZero
	ZobristStruct(DWORD Key, DWORD Lock0, DWORD Lock1);		// 构造函数
	void InitZero();		// 用零填充Zobrist
	void InitRC4(RC4Struct &rc4);// 用密码流填充Zobrist

	ZobristStruct operator^(const ZobristStruct &zobr);// Zobrist结构的异或运算
	ZobristStruct operator^=(const ZobristStruct &zobr);
#ifdef DEBUG
	friend ostream& operator<< (ostream &out, ZobristStruct &zobr);
#endif // DEBUG
};

class Zobrist {
public:
	ZobristStruct Player;
	ZobristStruct Table[14][256];
public:
	Zobrist();				//生成随机数表
#ifdef DEBUG
	void display();			// 测试用
#endif // DEBUG
};

//随机数表
extern Zobrist Zrand;