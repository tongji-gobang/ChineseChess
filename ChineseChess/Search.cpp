#include"Search.h"
#include"RESOURCE.H"
#include"ChessBoard.h"


S Search;
/*
// "qsort"����ʷ������ıȽϺ���
static int CompareHistory(const void *lpmv1, const void *lpmv2) {
	return Search.nHistoryTable[*(int *)lpmv2] - Search.nHistoryTable[*(int *)lpmv1];
}

// �����߽�(Fail-Soft)��Alpha-Beta��������
static int SearchFull(int vlAlpha, int vlBeta, int nDepth) {
	int i, nGenMoves, pcCaptured;
	int vl, vlBest, mvBest;
	int mvs[MAX_GEN_MOVES];
	// һ��Alpha-Beta��ȫ������Ϊ���¼����׶�

	// 1. ����ˮƽ�ߣ��򷵻ؾ�������ֵ
	if (nDepth == 0) {
		return pos.Evaluate();
	}

	// 2. ��ʼ�����ֵ������߷�
	vlBest = -MATE_VALUE; // ��������֪�����Ƿ�һ���߷���û�߹�(ɱ��)
	mvBest = 0;           // ��������֪�����Ƿ���������Beta�߷���PV�߷����Ա㱣�浽��ʷ��

	// 3. ����ȫ���߷�����������ʷ������
	nGenMoves = pos.GenerateMoves(mvs);
	qsort(mvs, nGenMoves, sizeof(int), CompareHistory);

	// 4. ��һ����Щ�߷��������еݹ�
	for (i = 0; i < nGenMoves; i++) {
		if (pos.MakeMove(mvs[i], pcCaptured)) {
			vl = -SearchFull(-vlBeta, -vlAlpha, nDepth - 1);
			pos.UndoMakeMove(mvs[i], pcCaptured);

			// 5. ����Alpha-Beta��С�жϺͽض�
			if (vl > vlBest) {    // �ҵ����ֵ(������ȷ����Alpha��PV����Beta�߷�)
				vlBest = vl;        // "vlBest"����ĿǰҪ���ص����ֵ�����ܳ���Alpha-Beta�߽�
				if (vl >= vlBeta) { // �ҵ�һ��Beta�߷�
					mvBest = mvs[i];  // Beta�߷�Ҫ���浽��ʷ��
					break;            // Beta�ض�
				}
				if (vl > vlAlpha) { // �ҵ�һ��PV�߷�
					mvBest = mvs[i];  // PV�߷�Ҫ���浽��ʷ��
					vlAlpha = vl;     // ��СAlpha-Beta�߽�
				}
			}
		}
	}

	// 5. �����߷����������ˣ�������߷�(������Alpha�߷�)���浽��ʷ���������ֵ 
	if (vlBest == -MATE_VALUE) {
		// �����ɱ�壬�͸���ɱ�岽����������
		return pos.nDistance - MATE_VALUE;
	}
	if (mvBest != 0) {
		// �������Alpha�߷����ͽ�����߷����浽��ʷ��
		Search.nHistoryTable[mvBest] += nDepth * nDepth;
		if (pos.nDistance == 0) {
			// �������ڵ�ʱ��������һ������߷�(��Ϊȫ�����������ᳬ���߽�)��������߷���������
			Search.mvResult = mvBest;
		}
	}
	return vlBest;
}

// ����������������
void SearchMain(void) {
	int i, t, vl;

	// ��ʼ��
	memset(Search.nHistoryTable, 0, 65536 * sizeof(int)); // �����ʷ��
	t = clock();       // ��ʼ����ʱ��
	pos.nDistance = 0; // ��ʼ����

					   // �����������
	for (i = 1; i <= LIMIT_DEPTH; i++) {
		vl = SearchFull(-MATE_VALUE, MATE_VALUE, i);
		// ������ɱ�壬����ֹ����
		if (vl > WIN_VALUE || vl < -WIN_VALUE) {
			break;
		}
		// ����һ�룬����ֹ����
		if (clock() - t > CLOCKS_PER_SEC) {
			break;
		}
	}
}
*/





int ProbeHash(int vl_Alpha, int vl_Beta, int Depth, int &mv) {
	bool  bMate; // ɱ���־
				 //ͨ�� [ dwKey % HASH_SIZE ] �õ������߷�
				 // ����ͨ��ָ���Ż�hsh
	HashItem hsh = Search.HashTable[pos.zobr.dwKey & HASH_SIZE_end];
	//
	if (hsh.Lock0 != pos.zobr.dwLock0 || hsh.Lock1 != pos.zobr.dwLock1) {		//��������벻ͬ��������
		mv = 0;
		return MATE_VALUE_neg;
	}

	mv = hsh.mv;
	bMate = FALSE;						//Ĭ��δ������ɱ��

										//[win_value,Ban_value],��Ϊ����ɱ�嵫������
	if (hsh.vl > WIN_VALUE) {
		if (hsh.vl < BAN_VALUE)			//���ڳ����и��ķ�ֵ��д���û���
			return MATE_VALUE_neg;			// ���ܵ��������Ĳ��ȶ��ԣ������˳���������ŷ������õ�
		hsh.vl -= pos.nDistance;		//>Ban_value ɱ���Ҳ�����
		bMate = TRUE;
	}
	else if (hsh.vl < -WIN_VALUE) {
		if (hsh.vl > -BAN_VALUE) {
			return MATE_VALUE_neg;     //��������
		}
		hsh.vl += pos.nDistance;
		bMate = TRUE;
	}

	if (hsh.Depth >= Depth || bMate) {			//����������ƻ���Ϊɱ�壨�����ɱ�壬��ô����Ҫ�������������
		switch (hsh.Flag)
		{
		case HASH_BETA:		return (hsh.vl >= vl_Beta ? hsh.vl : MATE_VALUE_neg); break;
		case HASH_ALPHA:	return (hsh.vl <= vl_Alpha ? hsh.vl : MATE_VALUE_neg); break;
		default:			return hsh.vl; break;
		}
	}

	return MATE_VALUE_neg;
};



// �����û�����
void RecordHash(int Flag, int vl, int Depth, int mv) {
	// ����ͨ��ָ���Ż�hsh
	HashItem hsh = Search.HashTable[pos.zobr.dwKey & HASH_SIZE_end];
	if (hsh.Depth > Depth) {
		return;
	}

	if (vl > WIN_VALUE) {
		if (mv == 0 && vl <= BAN_VALUE) {		// ���ܵ��������Ĳ��ȶ��ԣ�����û������ŷ��������˳�
			return;
		}
		hsh.vl = vl + pos.nDistance;			//>Ban_value ɱ���Ҳ�����
	}
	else if (vl < -WIN_VALUE) {					// ͬ��
		if (mv == 0 && vl >= -BAN_VALUE) {
			return;
		}
		hsh.vl = vl - pos.nDistance;
	}
	else {
		hsh.vl = vl;
	}

	Search.HashTable[pos.zobr.dwKey & HASH_SIZE_end] = { Depth,Flag,hsh.vl,mv,pos.zobr.dwLock0,pos.zobr.dwLock1 };
};

