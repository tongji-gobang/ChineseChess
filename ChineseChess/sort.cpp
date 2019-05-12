#include"sort.h"

/*
void SortStruct::Init(int mvHash_) { // ��ʼ�����趨�û����߷�������ɱ���߷�
	mvHash = mvHash_;
	mvKiller1 = Search.mvKillers[pos.nDistance][0];
	mvKiller2 = Search.mvKillers[pos.nDistance][1];
	nPhase = PHASE_HASH;
}

// �õ���һ���߷�
int SortStruct::Next(void) {
	int mv;
	switch (nPhase) {
		// "nPhase"��ʾ�ŷ����������ɽ׶Σ�����Ϊ��

		// 0. �û����ŷ���������ɺ�����������һ�׶Σ�
	case PHASE_HASH:
		nPhase = PHASE_KILLER_1;
		if (mvHash != 0) {
			return mvHash;
		}
		// ���ɣ�����û��"break"����ʾ"switch"����һ��"case"ִ��������������һ��"case"����ͬ

		// 1. ɱ���ŷ�����(��һ��ɱ���ŷ�)����ɺ�����������һ�׶Σ�
	case PHASE_KILLER_1:
		nPhase = PHASE_KILLER_2;
		if (mvKiller1 != mvHash && mvKiller1 != 0 && pos.LegalMove(mvKiller1)) {
			return mvKiller1;
		}

		// 2. ɱ���ŷ�����(�ڶ���ɱ���ŷ�)����ɺ�����������һ�׶Σ�
	case PHASE_KILLER_2:
		nPhase = PHASE_GEN_MOVES;
		if (mvKiller2 != mvHash && mvKiller2 != 0 && pos.LegalMove(mvKiller2)) {
			return mvKiller2;
		}

		// 3. ���������ŷ�����ɺ�����������һ�׶Σ�
	case PHASE_GEN_MOVES:
		nPhase = PHASE_REST;
		nGenMoves = pos.GenerateMoves(mvs);
		qsort(mvs, nGenMoves, sizeof(int), CompareHistory);
		nIndex = 0;

		// 4. ��ʣ���ŷ�����ʷ��������
	case PHASE_REST:
		while (nIndex < nGenMoves) {
			mv = mvs[nIndex];
			nIndex++;
			if (mv != mvHash && mv != mvKiller1 && mv != mvKiller2) {
				return mv;
			}
		}

		// 5. û���ŷ��ˣ������㡣
	default:
		return 0;
	}
}
*/


//��ʷ��ȽϺ���
int compare(const void* a, const void*b) {
	return *(int *)a - *(int *)b;
}


void SortStruct::Init(int mvHash_) {
	this->mvHash = mvHash_;		//��ʼ����ϣ�߷�
	this->mvKiller1 = Search.mvKillers[pos.nDistance][0]; //��ɱ�ֱ��ȡ�ֵܽڵ��ɱ���߷�
	this->mvKiller2 = Search.mvKillers[pos.nDistance][1];
	this->nPhase = PHASE_HASH;				//���ʼ״̬����Ϊ��ȡ��ϣ�߷�
}

//��ȡ��һ���߷�
int SortStruct::Next() {
	int mv;

	//��״̬Ϊȡɢ�б��߷�
	if (nPhase == PHASE_HASH) {
		nPhase = PHASE_KILLER_1;	//��״̬��Ϊɱ���߷�1
		if (mvHash)					//���ɢ�б��߷����� �򷵻ظ��߷�
			return mvHash;
	}

	//��״̬Ϊȡɱ���߷�1
	if (nPhase == PHASE_KILLER_1) {
		nPhase = PHASE_KILLER_2;										//��״̬��Ϊɱ���߷�2

		if (mvKiller1&&mvKiller1!=mvHash&& pos.LegalMove(mvKiller1))	//�ж�ɱ��1�߷��Ƿ�Ϊ0����ɢ�б��߷���ͬ
																		//��Ϊ�Ϸ��߷�
			return mvKiller1;											//����ɱ��1�߷�
	}


	//��״̬Ϊȡɱ���߷�1
	if (nPhase == PHASE_KILLER_2) {
		nPhase = PHASE_GEN_MOVES;										//��״̬��Ϊ���������߷�״̬

		if (mvKiller2&&mvKiller2 != mvHash&& pos.LegalMove(mvKiller2))	//�ж�ɱ��2�߷��Ƿ�Ϊ0����ɢ�б��߷���ͬ
																		//��Ϊ�Ϸ��߷�
			return mvKiller1;											//����ɱ��2�߷�
	}


	//��״̬Ϊ���������߷�
	if (nPhase == PHASE_GEN_MOVES) {
		nPhase = PHASE_REST;											//��״̬����Ϊ��һ��ȡ�߷�״̬

		this->nGenMoves = pos.GenerateMoves(mvs);						//���ɴ˾��������߷�
		qsort(mvs, nGenMoves, sizeof(int), compare);					//����Щ�߷�������ʷ������
		this->nIndex = 0;												//�������߷����±���0
	}


	//��״̬Ϊ��һ��ȡ�߷�״̬
	if (nPhase == PHASE_REST) {
		while(nIndex < nGenMoves) {										//�������߷��±�С�����ɵ����߷���
			if(mvs[nIndex] != mvHash&&mvs[nIndex]!=mvKiller1&&mvs[nIndex]!=mvKiller2)//�����߷�����ɢ�б��߷���ɱ���߷�
				return mvs[nIndex];										//���ش��߷�
			nIndex++;													//�������߷����±��һ
		}
	}

	return 0;
}