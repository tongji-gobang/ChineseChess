#pragma once
// �߷�����ṹ
#include"ChessData.h"
#include"ChessBoard.h"
#include"Search.h"
const int PHASE_HASH = 0;
const int PHASE_KILLER_1 = 1;
const int PHASE_KILLER_2 = 2;
const int PHASE_GEN_MOVES = 3;
const int PHASE_REST = 4;

struct SortStruct {
	int mvHash, mvKiller1, mvKiller2; // �û����߷�������ɱ���߷�
	int nPhase, nIndex, nGenMoves;    // ��ǰ�׶Σ���ǰ���õڼ����߷����ܹ��м����߷�
	int mvs[MAX_GEN_MOVES];           // ���е��߷�

	void Init(int mvHash_);// ��ʼ�����趨�û����߷�������ɱ���߷�
	int Next(void); // �õ���һ���߷�

};


struct Sort
{
	int 
};




