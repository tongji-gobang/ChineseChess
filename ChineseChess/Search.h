
#pragma once
//#inlcude "BOOKitem"
#include <Windows.h>
#include <time.h>
const int MAX_GEN_MOVES = 128; // ���������߷���
const int MAX_MOVES = 256;     // ������ʷ�߷���
const int LIMIT_DEPTH = 64;    // �����������
const int MATE_VALUE = 10000;  // ��߷�ֵ���������ķ�ֵ
const int BAN_VALUE = MATE_VALUE - 100; // �����и��ķ�ֵ�����ڸ�ֵ����д���û���
const int WIN_VALUE = MATE_VALUE - 200; // ������ʤ���ķ�ֵ���ޣ�������ֵ��˵���Ѿ�������ɱ����
const int DRAW_VALUE = 20;     // ����ʱ���صķ���(ȡ��ֵ)
const int ADVANCED_VALUE = 3;  // ����Ȩ��ֵ
const int RANDOM_MASK = 7;     // ����Է�ֵ
const int NULL_MARGIN = 400;   // �ղ��ü��������߽�
const int NULL_DEPTH = 2;      // �ղ��ü��Ĳü����
const int HASH_SIZE = 1 << 20; // �û����С
const int HASH_ALPHA = 1;      // ALPHA�ڵ���û�����
const int HASH_BETA = 2;       // BETA�ڵ���û�����
const int HASH_PV = 3;         // PV�ڵ���û�����

							   // ���ӵ�
const int HASH_SIZE_end = HASH_SIZE - 1;		// 20λ��1
#define HistoryTable_SIZE 65536
const int MATE_VALUE_neg = -MATE_VALUE;

// �û�����ṹ
/*
��Zobrist��ֵΪָ��: ( ��ֵ%ɢ�б����� )  --> ���档 ���ںܶ���涼�п��ܸ�ɢ�б���ͬһ�����ã����ɢ������Ҫ����У��ֵ(dwlock0,dwlock1)
��ȡ���־��
���֡��ƶ�
У��ֵ
*/
struct HashItem {
	BYTE Depth, Flag;
	short vl;
	WORD mv;
	DWORD Lock0, Lock1;
};
int ProbeHash(int vl_Alpha, int vl_Beta, int Depth, int &mv); // ��ȡ�û�����
void RecordHash(int Flag, int vl, int Depth, int mv);   //�����û�����


// �������йص�ȫ�ֱ���
struct S{
	int mvResult;             // �����ߵ���
	int nHistoryTable[HistoryTable_SIZE]; // ��ʷ��
	int mvKillers[LIMIT_DEPTH][2]; // ɱ���߷���
	HashItem HashTable[HASH_SIZE]; // �û���
								   //int BookSize;                 // ���ֿ��С
								   //BookItem BookTable[BOOK_SIZE]; // ���ֿ� (��ʱ��ʵ�ֿ��ֿ����)

};
extern S Search;
// ��������


							   
 // "qsort"����ʷ������ıȽϺ���
int CompareHistory(const void *lpmv1, const void *lpmv2);

// �����߽�(Fail-Soft)��Alpha-Beta��������
int SearchFull(int vlalpha, int vlbeta, int nDepth, bool NoNull = FALSE);

// ����������������
void SearchMain(clock_t time_limit);

// "GenerateMoves"�������Ƿ�ֻ���������ŷ�
const bool GEN_CAPTURE = true;

// "SearchFull"�Ĳ��������Ųü�����
const bool NO_NULL = true;

// MVV/LVAÿ�������ļ�ֵ
static BYTE cucMvvLva[24] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	5, 1, 1, 3, 4, 3, 2, 0,
	5, 1, 1, 3, 4, 3, 2, 0
};

// ��MVV/LVAֵ
int MvvLva(int mv);

// "qsort"��MVV/LVAֵ����ıȽϺ���
int CompareMvvLva(const void *p1, const void *p2);

// "qsort"����ʷ������ıȽϺ���
static int CompareHistory(const void *p1, const void *p2);

// ��̬����
int SearchQuiesc(int alpha, int beta);


