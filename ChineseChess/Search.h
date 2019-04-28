
// �������йص�ȫ�ֱ���
struct S{
	int mvResult;             // �����ߵ���
	int nHistoryTable[65536]; // ��ʷ��
};
extern S Search;
// ��������
const int MAX_GEN_MOVES = 128; // ���������߷���
const int LIMIT_DEPTH = 32;    // �����������
const int MATE_VALUE = 10000;  // ��߷�ֵ���������ķ�ֵ
const int WIN_VALUE = MATE_VALUE - 100; // ������ʤ���ķ�ֵ���ޣ�������ֵ��˵���Ѿ�������ɱ����
const int ADVANCED_VALUE = 3;  // ����Ȩ��ֵ

							   
							   // "qsort"����ʷ������ıȽϺ���
static int CompareHistory(const void *lpmv1, const void *lpmv2);

// �����߽�(Fail-Soft)��Alpha-Beta��������
static int SearchFull(int vlAlpha, int vlBeta, int nDepth);

// ����������������
void SearchMain(void);


#pragma once
