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


// RC4������������
class RC4Struct {
private:
	BYTE s[256];
	int x, y;
public:
	RC4Struct();			// �ÿ���Կ��ʼ��������������
	BYTE NextByte();		// ������һ�ֽڵ�������
	DWORD NextLong();		// ���������ֽڵ�������
};

class ZobristStruct {
public:
	DWORD dwKey, dwLock0, dwLock1;
public:
	friend class PositionStruct;
	ZobristStruct();		// ���캯�����൱��InitZero
	ZobristStruct(DWORD Key, DWORD Lock0, DWORD Lock1);		// ���캯��
	void InitZero();		// �������Zobrist
	void InitRC4(RC4Struct &rc4);// �����������Zobrist

	ZobristStruct operator^(const ZobristStruct &zobr);// Zobrist�ṹ���������
	ZobristStruct operator^=(const ZobristStruct &zobr);// �൱��ԭ�����е�Xor
#ifdef DEBUG
	friend ostream& operator<< (ostream &out, ZobristStruct &zobr);
#endif // DEBUG
};

class Zobrist {
private:
	ZobristStruct Player;
	ZobristStruct Table[14][256];
public:
	Zobrist();				// �൱��ԭ�����е�InitZobrist
#ifdef DEBUG
	void display();			// ������
#endif // DEBUG
};
