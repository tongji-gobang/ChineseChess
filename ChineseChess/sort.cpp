#include"sort.h"

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
