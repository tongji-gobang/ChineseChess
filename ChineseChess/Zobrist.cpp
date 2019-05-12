#include "Zobrist.h"

// �ÿ���Կ��ʼ��������������
RC4Struct::RC4Struct()
{
	BYTE temp;
	int i, j;
	x = 0;
	y = 0;
	j = 0;
	for (i = 0; i < 256; ++i)
		s[i] = i;
	for (i = 0; i < 256; ++i) {
		j = (j + s[i]) & 255;
		temp = s[i];
		s[i] = s[j];
		s[j] = temp;
	}
}

//������һ�ֽڵ�������
BYTE RC4Struct::NextByte()
{
	BYTE temp;
	x = (x + 1) & 255;
	y = (y + s[x]) & 255;
	temp = s[x];
	s[x] = s[y];
	s[y] = temp;
	return s[(s[x] + s[y]) & 255];
}

DWORD RC4Struct::NextLong()
{
	BYTE t0, t1, t2, t3;
	t0 = NextByte();
	t1 = NextByte();
	t2 = NextByte();
	t3 = NextByte();
	return t0 + (t1 << 8) + (t2 << 16) + (t3 << 24);
}

// ���캯�����൱��InitZero
ZobristStruct::ZobristStruct()
{
	dwKey = 0;
	dwLock0 = 0;
	dwLock1 = 0;
}

ZobristStruct::ZobristStruct(DWORD Key, DWORD Lock0, DWORD Lock1)
{
	dwKey = Key;
	dwLock0 = Lock0;
	dwLock1 = Lock1;
}

// �������Zobrist
void ZobristStruct::InitZero()
{
	dwKey = 0;
	dwLock0 = 0;
	dwLock1 = 0;
}

// �����������Zobrist
void ZobristStruct::InitRC4(RC4Struct &rc4)
{
	dwKey = rc4.NextLong();
	dwLock0 = rc4.NextLong();
	dwLock1 = rc4.NextLong();
}

// Zobrist�ṹ���������
ZobristStruct ZobristStruct::operator^(const ZobristStruct &zobr)
{
	ZobristStruct t(dwKey ^ zobr.dwKey,
		dwLock0 ^ zobr.dwLock0,
		dwLock1 ^ zobr.dwLock1);
	return t;
}

// �൱��ԭ�����е�Xor
ZobristStruct ZobristStruct::operator^=(const ZobristStruct &zobr)
{
	dwKey ^= zobr.dwKey;
	dwLock0 ^= zobr.dwLock0;
	dwLock1 ^= zobr.dwLock1;
	return *this;
}

// �൱��ԭ�����е�InitZobrist
Zobrist::Zobrist()
{
	int i, j;
	RC4Struct rc4;

	Player.InitRC4(rc4);
	for (i = 0; i < 14; i++) {
		for (j = 0; j < 256; j++) {
			Table[i][j].InitRC4(rc4);
		}
	}
}

#ifdef DEBUG
ostream& operator<< (ostream &out, ZobristStruct &zobr)
{
	out << setw(8) << setfill('0') << zobr.dwKey;
	out << setw(8) << setfill('0') << zobr.dwLock0;
	out << setw(8) << setfill('0') << zobr.dwLock1;
	return out;
}

void Zobrist::display()
{
	RC4Struct rc4;
	cout << "player: " << hex << Player << endl;
	for (int i = 0; i < 14; i++) {
		for (int j = 0; j < 256; j++) {
			cout << hex << Table[i][j] << endl;
		}
	}
}
#endif // DEBUG
