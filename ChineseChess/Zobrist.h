#pragma once
#if 1
#include <Windows.h>
#endif

#ifdef DEBUG
#include <iostream>
#include <iomanip>
using namespace std;
#endif // DEBUG


// RC4������������
class RC4Struct {
public:
    BYTE s[256]{};
	int x, y;
public:
	RC4Struct();			// �ÿ���Կ��ʼ��������������
	BYTE GenByte();			// ������һ�ֽڵ�������
	DWORD Gen4Byte();		// ���������ֽڵ�������
};

class ZobristStruct {
public:
	DWORD key0, key1, key2;
public:
	friend class PositionStruct;
	ZobristStruct();		// ���캯�����൱��InitZero
	ZobristStruct(DWORD Key, DWORD Lock0, DWORD Lock1);		// ���캯��
	void InitZero();		// �������Zobrist
	void InitRC4(RC4Struct &rc4);// �����������Zobrist

	ZobristStruct operator^(const ZobristStruct &zobr);// Zobrist�ṹ���������
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
	Zobrist();				//�����������
#ifdef DEBUG
	void display();			// ������
#endif // DEBUG
};

//�������
extern Zobrist Zrand;