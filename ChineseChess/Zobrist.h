#pragma once
#if 1
#include <Windows.h>
#endif

//#define DEBUG

#ifdef DEBUG
#include <iostream>
#include <iomanip>
using namespace std;
#endif // DEBUG


// RC4密码流生成器
class RC4Struct {
private:
	BYTE s[256];
	int x, y;
public:
	RC4Struct();			// 用空密钥初始化密码流生成器
	BYTE NextByte();		// 生成下一字节的流密码
	DWORD NextLong();		// 生成下四字节的流密码
};

class ZobristStruct {
private:
	DWORD dwKey, dwLock0, dwLock1;
public:
	friend class PositionStruct;
	ZobristStruct();		// 构造函数，相当于InitZero
	ZobristStruct(DWORD Key, DWORD Lock0, DWORD Lock1);		// 构造函数
	void InitZero();		// 用零填充Zobrist
	void InitRC4(RC4Struct &rc4);// 用密码流填充Zobrist

	ZobristStruct operator^(const ZobristStruct &zobr);// Zobrist结构的异或运算
	ZobristStruct operator^=(const ZobristStruct &zobr);// 相当于原代码中的Xor
#ifdef DEBUG
	friend ostream& operator<< (ostream &out, ZobristStruct &zobr);
#endif // DEBUG
};

class Zobrist {
private:
	ZobristStruct Player;
	ZobristStruct Table[14][256];
public:
	Zobrist();				// 相当于原代码中的InitZobrist
#ifdef DEBUG
	void display();			// 测试用
#endif // DEBUG
};
