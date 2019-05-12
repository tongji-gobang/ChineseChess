#include"Search.h"
#include"RESOURCE.H"
#include"ChessBoard.h"


S Search;

// ��̬(Quiescence)��������
int SearchQuiesc(int alpha, int beta) {
	int i, movenum;
	int value, best;
	int mvs[MAX_GEN_MOVES];

	// 1. ����ظ�����
	value = pos.IsRepetitive();
	if (value != 0)
		return pos.RepeatValue(value);	// ���ظ��򷵻���Ӧ���ظ���

	// 2. �����������
	if (pos.RootDistance == LIMIT_DEPTH)
		return pos.Evaluate();

	// 3. ���ֵ��Ϊɱ���
	best = -MATE_VALUE; // ��������֪�����Ƿ�һ���߷���û�߹�(ɱ��)

	if (pos.LastCheck()) {
		// 4. �����������������ȫ���߷�
		movenum = pos.GenerateMoves(mvs);
		qsort(mvs, movenum, sizeof(int), CompareHistory);
	}
	else {

		// 5. ������������������۵�ǰ����
		value = pos.Evaluate();
		if (value > best) {			// �ҵ����ֵ
			best = value;
			if (value >= beta) {	// �ҵ�beta�߷�
				return value;		// ���ۺõ����Խض϶�����Ҫ��ͼ����
			}
			if (value > alpha) {
				alpha = value;
			}
		}

		// 6. �����������û�нضϣ��ٿ��ǳ����߷�
		movenum = pos.GenerateMoves(mvs, GEN_CAPTURE);
		qsort(mvs, movenum, sizeof(int), CompareMvvLva);	//��MVVLVA��������ŷ�
	}

	// 7. ��һ����Щ�߷��������еݹ�
	for (i = 0; i < movenum; ++i) {
		int PieceCaptured;
		if (pos.MakeMove(mvs[i])) {
			value = -SearchQuiesc(-beta, -alpha);
			pos.UndoMakeMove();

			// 8. ����Alpha-Beta��С�жϺͽض�
			if (value > best) {		// �ҵ����ֵ(������ȷ����Alpha��PV����Beta�߷�)
				best = value;		// "best"����ĿǰҪ���ص����ֵ�����ܳ���Alpha-Beta�߽�
				if (value >= beta) {// �ҵ�һ��Beta�߷�
					return value;
				}
				if (value > alpha) {	// �ҵ�һ��PV�߷�
					alpha = value;		// ����alpha����СAlpha-Beta�߽�
				}
			}
		}
	}

	// 9. ��һ���߷�Ҳû�ߣ���˵����ɱ������ɱ��֣����򷵻�best
	return best == -MATE_VALUE ? pos.RootDistance - MATE_VALUE : best;
}

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
		hsh.vl -= pos.RootDistance;		//>Ban_value ɱ���Ҳ�����
		bMate = TRUE;
	}
	else if (hsh.vl < -WIN_VALUE) {
		if (hsh.vl > -BAN_VALUE) {
			return MATE_VALUE_neg;     //��������
		}
		hsh.vl += pos.RootDistance;
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
		hsh.vl = vl + pos.RootDistance;			//>Ban_value ɱ���Ҳ�����
	}
	else if (vl < -WIN_VALUE) {					// ͬ��
		if (mv == 0 && vl >= -BAN_VALUE) {
			return;
		}
		hsh.vl = vl - pos.RootDistance;
	}
	else {
		hsh.vl = vl;
	}

	Search.HashTable[pos.zobr.dwKey & HASH_SIZE_end] = { Depth,Flag,hsh.vl,mv,pos.zobr.dwLock0,pos.zobr.dwLock1 };
};

// ��MVV/LVAֵ
int MvvLva(int mv) {
	return (cucMvvLva[pos.Board[DstPos(mv)]] << 3) - cucMvvLva[pos.Board[SrcPos(mv)]];
}

// qsort��MVV/LVAֵ����ıȽϺ���
int CompareMvvLva(const void *p1, const void *p2) {
	return MvvLva(*(int *)p2) - MvvLva(*(int *)p1);
}

// qsort��ʷ������ıȽϺ���
int CompareHistory(const void *p1, const void *p2) {
	return Search.nHistoryTable[*(int *)p2] - Search.nHistoryTable[*(int *)p1];
}
