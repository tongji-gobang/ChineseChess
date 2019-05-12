#pragma once
// �߷�����ṹ
#include"ChessData.h"
#include"ChessBoard.h"
#include"Search.h"


//�˺���������һ��״̬��ʹ�������߷���ʱ���ܹ��������������߷�
//״̬����״̬ת������
//ȡ�û����߷�-->ȡ��һ��ɱ���߷�-->ȡ�ڶ���ɱ���߷�-->���ɸþ����������߷���������ʷ������-->��һȡ�������߷�

const int PHASE_HASH = 0;			//�û����߷�״̬
const int PHASE_KILLER_1 = 1;		//ɱ���߷�һ״̬
const int PHASE_KILLER_2 = 2;		//ɱ���߷���״̬
const int PHASE_GEN_MOVES = 3;		//���ɾ����������߷�������
const int PHASE_REST = 4;			//��һȡ�������߷�

struct SortStruct {
	int mvHash, mvKiller1, mvKiller2; // �û����߷�������ɱ���߷�
	int nPhase, nIndex, nGenMoves;    // ��ǰ�׶Σ���ǰ���õڼ����߷����ܹ��м����߷�
	int mvs[MAX_GEN_MOVES];           // ���е��߷�

	void Init(int mvHash_);// ��ʼ�����趨�û����߷�������ɱ���߷�
	int Next(void); // �õ���һ���߷�

};






