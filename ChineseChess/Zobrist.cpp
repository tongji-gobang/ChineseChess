#include "Zobrist.h"

Zobrist Zrand;

// 用空密钥初始化密码流生成器
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

//生成下一字节的流密码
BYTE RC4Struct::GenByte()
{
	BYTE temp;
	x = (x + 1) & 255;
	y = (y + s[x]) & 255;
	temp = s[x];
	s[x] = s[y];
	s[y] = temp;
	return s[(s[x] + s[y]) & 255];
}

DWORD RC4Struct::Gen4Byte()
{
	BYTE t0, t1, t2, t3;
	t0 = GenByte();
	t1 = GenByte();
	t2 = GenByte();
	t3 = GenByte();
	return t0 + (t1 << 8) + (t2 << 16) + (t3 << 24);
}

// 构造函数，相当于InitZero
ZobristStruct::ZobristStruct()
{
	key0 = 0;
	key1 = 0;
	key2 = 0;
}

ZobristStruct::ZobristStruct(DWORD Key, DWORD Lock0, DWORD Lock1)
{
	key0 = Key;
	key1 = Lock0;
	key2 = Lock1;
}

// 用零填充Zobrist
void ZobristStruct::InitZero()
{
	key0 = 0;
	key1 = 0;
	key2 = 0;
}

// 用密码流填充Zobrist
void ZobristStruct::InitRC4(RC4Struct &rc4)
{
	key0 = rc4.Gen4Byte();
	key1 = rc4.Gen4Byte();
	key2 = rc4.Gen4Byte();
}

// Zobrist结构的异或运算
ZobristStruct ZobristStruct::operator^(const ZobristStruct &zobr)
{
	ZobristStruct t(key0 ^ zobr.key0,
		key1 ^ zobr.key1,
		key2 ^ zobr.key2);
	return t;
}

ZobristStruct ZobristStruct::operator^=(const ZobristStruct &zobr)
{
	key0 ^= zobr.key0;
	key1 ^= zobr.key1;
	key2 ^= zobr.key2;
	return *this;
}

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
	out << setw(8) << setfill('0') << zobr.key0;
	out << setw(8) << setfill('0') << zobr.key1;
	out << setw(8) << setfill('0') << zobr.key2;
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
