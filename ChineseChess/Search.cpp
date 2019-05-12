
#define DEBUG
//#define CPP3 1

//#define CPP4 2

//#define CPP5  3
#define CPP6 4


#include"Search.h"
#include"RESOURCE.H"
#include"ChessBoard.h"
#include"sort.h"

S Search;

// ��������move
void SetBestMove(int mv, int nDepth) {
	int *lpmvKillers;
	Search.nHistoryTable[mv] += nDepth * nDepth;  // д����ʷ��
	lpmvKillers = Search.mvKillers[pos.RootDistance];  // д��ɱ�ֱ� // ! Search.mvKillers[MAX][2]
	if (lpmvKillers[0] != mv) {
		lpmvKillers[1] = lpmvKillers[0];
		lpmvKillers[0] = mv;
	}
}

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
		//int PieceCaptured;
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

	Search.HashTable[pos.zobr.dwKey & HASH_SIZE_end] = { (BYTE)Depth,(BYTE)Flag,hsh.vl,(WORD)mv,pos.zobr.dwLock0,pos.zobr.dwLock1 };
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

#ifdef CPP3
int SearchFull(int vlAlpha, int vlBeta, int nDepth, bool NoNull) {
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

	// 4. ��һ����Щ�߷��������еݹ�  //��������ȱ���
	for (i = 0; i < nGenMoves; i++) {
		if (pos.MakeMove(mvs[i])) {
			vl = -SearchFull(-vlBeta, -vlAlpha, nDepth - 1);
			pos.UndoMakeMove();

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
		return pos.RootDistance - MATE_VALUE;
	}
	if (mvBest != 0) {
		// �������Alpha�߷����ͽ�����߷����浽��ʷ��
		Search.nHistoryTable[mvBest] += nDepth * nDepth;
		if (pos.RootDistance == 0) {
			// �������ڵ�ʱ��������һ������߷�(��Ϊȫ�����������ᳬ���߽�)��������߷���������
			Search.mvResult = mvBest;
		}
	}
	return vlBest;
}

#elif CPP4
int SearchFull(int vlAlpha, int vlBeta, int nDepth, bool bNoNull) {
	int i, nGenMoves;
	int vl, vlBest, mvBest;
	int mvs[MAX_GEN_MOVES];
	// һ��Alpha-Beta��ȫ������Ϊ���¼����׶�

	if (pos.RootDistance > 0) {

		// 1. ����ˮƽ�ߣ�����þ�̬����(ע�⣺���ڿղ��ü�����ȿ���С����)  // ! ԭ�򣺿ղ��ü�������n.distence--
		if (nDepth <= 0) {
			return SearchQuiesc(vlAlpha, vlBeta);
		}

		// 1-1. ����ظ�����(ע�⣺��Ҫ�ڸ��ڵ��飬�����û���߷��ˣ�// ! ���Ϸ�if��nDistance > 0��)
		vl = pos.IsRepetitive();
		if (vl != 0) {
			return pos.RepeatValue(vl);
		}

		// 1-2. ���Ｋ����Ⱦͷ��ؾ�������
		if (pos.RootDistance == LIMIT_DEPTH) {
			return pos.Evaluate();
		}
		//  �к��������Ƿǳ�������(���˲о�����)��ͨ������ֵ����ߣ���һ�����þ������Щ��
		//  ���п��ܵ��ŷ���ʹ�����ø��㣬
		//  �����ľ����Ϊ���޵��š�(Zugzwang)(�����˼Ϊǿ������)��ͨ��ֻ�����ڲо��С�
		//  ��ˣ�����������һ��ϣ���߳��߽�Ľ��(��Alpha-Beta�����ķ���ֵ������Beta)��
		//  ����������������������Ȩ���ŷ����������š�(Null-Move)������ʹ��ͨ��������õġ�
		//  �����Ȩ�ŷ��߳��߽磬��ô������õ��ŷ�Ҳ���ܸ߳��߽磬��Ϳ���ֱ�ӷ���Beta����Ҫ��ȥ�����ˡ�
		//  Ҫ���������ø��죬��Ȩ�ŷ����������ͨ���ȳ����ŷ�ǳ��
		//  �����С�ģ�����������ı����������Ҳ����ʹ���������е�һЩ��Ҫ����·��
		//  ��Ҫ���������ÿ���(��Ϊ��������������˻������ֻ�������ۺ���)������ҪС�ģ�ֻ���ڲ�������޵��ŵ������ʹ�á�

		// 1-3. ���Կղ��ü�(���ڵ��Betaֵ��"MATE_VALUE"�����Բ����ܷ����ղ��ü�)
		if (!bNoNull && !pos.LastCheck() && pos.NullOkay()) {
			pos.MoveNull();
			vl = -SearchFull(-vlBeta, 1 - vlBeta, nDepth - NULL_DEPTH - 1, NO_NULL);
			pos.UndoMoveNull();
			if (vl >= vlBeta) {
				return vl;
			}
		}
	}

	// 2. ��ʼ�����ֵ������߷�
	vlBest = -MATE_VALUE; // ��������֪�����Ƿ�һ���߷���û�߹�(ɱ��)
	mvBest = 0;           // ��������֪�����Ƿ���������Beta�߷���PV�߷����Ա㱣�浽��ʷ��

						  // 3. ����ȫ���߷�����������ʷ������
	nGenMoves = pos.GenerateMoves(mvs);
	qsort(mvs, nGenMoves, sizeof(int), CompareHistory);

	// 4. ��һ����Щ�߷��������еݹ�
	for (i = 0; i < nGenMoves; i++) {
		if (pos.MakeMove(mvs[i])) {
			// �������� pos.InCheck() ? nDepth : nDepth - 1   // ? why������������������������
			vl = -SearchFull(-vlBeta, -vlAlpha, pos.LastCheck() ? nDepth : nDepth - 1);
			pos.UndoMakeMove();

			// 5. ����Alpha-Beta��С�жϺͽض�
			if (vl > vlBest) {    // �ҵ����ֵ(������ȷ����Alpha��PV����Beta�߷�)

								  // ! ����vlbest��ʼֵΪ-MATE_VALUE����һ����ab����������£������ܳ���Alpha-Beta�߽硱

				vlBest = vl;        // "vlBest"����ĿǰҪ���ص����ֵ��
				if (vl >= vlBeta) { // �ҵ�һ��Beta�߷�
					mvBest = mvs[i];  // Beta�߷�Ҫ���浽��ʷ��  ���涯��������
					break;            // Beta�ض�
				}
				if (vl > vlAlpha) { // �ҵ�һ��PV�߷�
					mvBest = mvs[i];  // PV�߷�Ҫ���浽��ʷ��   ���涯��������
					vlAlpha = vl;     // ��СAlpha-Beta�߽�
				}
			}
		}
	}

	// 5. �����߷����������ˣ�������߷�(������Alpha�߷�)���浽��ʷ���������ֵ   //�ǲ��Ǿ���PV�߷���
	if (vlBest == -MATE_VALUE) {
		// �����ɱ�壬�͸���ɱ�岽����������
		return pos.RootDistance - MATE_VALUE;
	}
	if (mvBest != 0) {
		// �������Alpha�߷����ͽ�����߷����浽��ʷ��
		Search.nHistoryTable[mvBest] += nDepth * nDepth;
		if (pos.RootDistance == 0) {
			// �������ڵ�ʱ��������һ������߷�(��Ϊȫ�����������ᳬ���߽�������͸�����)��������߷��������� why
			Search.mvResult = mvBest;
		}
	}
	return vlBest;
}
#elif CPP5
int SearchFull(int vlAlpha, int vlBeta, int nDepth, bool bNoNull) {
	int nHashFlag, vl, vlBest;
	int mv, mvBest, mvHash;
	SortStruct Sort;
	// һ��Alpha-Beta��ȫ������Ϊ���¼����׶�

	if (pos.RootDistance > 0) {
		// 1. ����ˮƽ�ߣ�����þ�̬����(ע�⣺���ڿղ��ü�����ȿ���С����)
		if (nDepth <= 0) {
			return SearchQuiesc(vlAlpha, vlBeta);
		}

		// 1-1. ����ظ�����(ע�⣺��Ҫ�ڸ��ڵ��飬�����û���߷���)
		vl = pos.IsRepetitive();
		if (vl != 0) {
			return pos.RepeatValue(vl);
		}

		// 1-2. ���Ｋ����Ⱦͷ��ؾ�������
		if (pos.RootDistance == LIMIT_DEPTH) {
			return pos.Evaluate();
		}

		// 1-3. �����û���ü������õ��û����߷�
		vl = ProbeHash(vlAlpha, vlBeta, nDepth, mvHash);
		if (vl > -MATE_VALUE) {
			return vl;
		}

		// 1-4. ���Կղ��ü�(���ڵ��Betaֵ��"MATE_VALUE"�����Բ����ܷ����ղ��ü�)
		if (!bNoNull && !pos.LastCheck() && pos.NullOkay()) {
			pos.MoveNull();
			vl = -SearchFull(-vlBeta, 1 - vlBeta, nDepth - NULL_DEPTH - 1, NO_NULL);
			pos.UndoMoveNull();
			if (vl >= vlBeta) {
				return vl;
			}
		}
	}
	else {
		mvHash = 0;
	}

	// 2. ��ʼ�����ֵ������߷�
	nHashFlag = HASH_ALPHA;
	vlBest = -MATE_VALUE; // ��������֪�����Ƿ�һ���߷���û�߹�(ɱ��)
	mvBest = 0;           // ��������֪�����Ƿ���������Beta�߷���PV�߷����Ա㱣�浽��ʷ��

						  // 3. ��ʼ���߷�����ṹ
	Sort.Init(mvHash);

	// 4. ��һ����Щ�߷��������еݹ�
	while ((mv = Sort.Next()) != 0) {
		if (pos.MakeMove(mv)) {
			// ��������
			vl = -SearchFull(-vlBeta, -vlAlpha, pos.LastCheck() ? nDepth : nDepth - 1);
			pos.UndoMakeMove();

			// 5. ����Alpha-Beta��С�жϺͽض�
			if (vl > vlBest) {    // �ҵ����ֵ(������ȷ����Alpha��PV����Beta�߷�)
				vlBest = vl;        // "vlBest"����ĿǰҪ���ص����ֵ�����ܳ���Alpha-Beta�߽�
				if (vl >= vlBeta) { // �ҵ�һ��Beta�߷�
					nHashFlag = HASH_BETA;  // ! ���趨�� ��ʲô���͵��û���
					mvBest = mv;      // Beta�߷�Ҫ���浽��ʷ��
					break;            // Beta�ض�
				}
				if (vl > vlAlpha) { // �ҵ�һ��PV�߷�
					nHashFlag = HASH_PV;    // ! ���趨�� ��ʲô���͵��û��� �����¼
					mvBest = mv;      // PV�߷�Ҫ���浽��ʷ��
					vlAlpha = vl;     // ��СAlpha-Beta�߽�
				}
			}
		}
	}

	// 5. �����߷����������ˣ�������߷�(������Alpha�߷�)���浽��ʷ���������ֵ
	if (vlBest == -MATE_VALUE) {
		// �����ɱ�壬�͸���ɱ�岽����������
		return pos.RootDistance - MATE_VALUE;
	}
	// ��¼���û���
	RecordHash(nHashFlag, vlBest, nDepth, mvBest);
	if (mvBest != 0) {
		// �������Alpha�߷����ͽ�����߷����浽��ʷ��
		SetBestMove(mvBest, nDepth);
		if (pos.RootDistance == 0) {
			// �������ڵ�ʱ��������һ������߷�(��Ϊȫ�����������ᳬ���߽�)��������߷���������
			Search.mvResult = mvBest;
		}
	}
	return vlBest;
}
#else CPP6
//Alpha - Beta����
//int SearchFull(int vlalpha, int vlbeta, int depth, bool NoNull) {
//	int vl, vl_best;
//	int mv, mv_best, mv_hash;
//	int new_depth;
//	int hash_type;
//	SortStruct Sort;
//
//	// ����ˮƽ�ߣ����þ�̬����
//	if (depth <= 0) {
//		//return SearchQuiesc(vlalpha, vlbeta);
//		return pos.Evaluate();
//	}
//
//	// ����ظ�����,��ֹ����,Ҳ���ǲ�Ҫ����
//	vl = pos.IsRepetitive(); //  ? nRecurĬ�ϲ���Ϊ1, �²�͡���һ�β�ͬ������
//	if (vl != 0) {
//		return pos.RepeatValue(vl);
//	}
//
//	// ���Ｋ����Ⱦͷ��ؾ�������
//	if (pos.RootDistance == LIMIT_DEPTH) {
//		return pos.Evaluate();
//	}
//
//	// �����û���
//	vl = ProbeHash(vlalpha, vlbeta, depth, mv_hash);  // ! mv_hash ���ô���
//	if (vl > -MATE_VALUE) {
//		return vl; // ? ������һ��ɱ��
//	}
//
//	  //�к��������Ƿǳ�������(���˲о�����)��ͨ������ֵ����ߣ���һ�����þ������Щ��
//	  //���п��ܵ��ŷ���ʹ�����ø��㣬�����ľ����Ϊ���޵��š�(Zugzwang)(�����˼Ϊǿ������)��ͨ��ֻ�����ڲо��С�
//	  //��ˣ�����������һ��ϣ���߳��߽�Ľ��(��Alpha-Beta�����ķ���ֵ������Beta)��
//	  //����������������������Ȩ���ŷ����������š�(Null-Move)������ʹ��ͨ��������õġ�
//	  //! �����Ȩ�ŷ��߳��߽磬��ô������õ��ŷ�Ҳ���ܸ߳��߽磬��Ϳ���ֱ�ӷ���Beta����Ҫ��ȥ�����ˡ� // ! ԭ�򣺿����õ�һ��betaʹ�ô��ڱ�խ��
//	  //Ҫ���������ø��죬��Ȩ�ŷ����������ͨ���ȳ����ŷ�ǳ��
//	  //�����С�ģ�����������ı����������Ҳ����ʹ���������е�һЩ��Ҫ����·��
//	  //��Ҫ���������ÿ���(��Ϊ��������������˻������ֻ�������ۺ���)������ҪС�ģ�ֻ���ڲ�������޵��ŵ������ʹ�á�
//
//	 //�ղ��ü�
//	if (!NoNull && !pos.LastCheck() && pos.NullOkay()) {
//		pos.MoveNull();
//		// ? ��������д�� alpha = vlbeta - 1; beta = vlbeta, -1�����ú�ԭ����ʲô
//		vl = -SearchFull(-vlbeta, 1 - vlbeta, depth - NULL_DEPTH - 1, NO_NULL);
//		pos.UndoMoveNull();
//		if (vl >= vlbeta) {
//			return vl;
//		}
//	}
//
//	// ��ʼ���߷�����ṹ�����ֵ������߷�
//	hash_type = HASH_ALPHA;
//	vl_best = -MATE_VALUE; // ��������֪�����Ƿ�һ���߷���û�߹�(ɱ��)
//	mv_best = 0;           // ��������֪�����Ƿ���������Beta�߷���PV�߷����Ա㱣�浽��ʷ��
//	Sort.Init(mv_hash);
//	/*
//	void Init(int mvHash_) { // ��ʼ�����趨�û����߷�������ɱ���߷�
//	mvHash = mvHash_;
//	mvKiller1 = Search.mvKillers[pos.nDistance][0]; // ! �ɼ�ɱ���߷�������������������ɵģ���ʵ����ֻinit��mvHash
//	mvKiller2 = Search.mvKillers[pos.nDistance][1];
//	nPhase = PHASE_HASH;
//	}
//	*/
//
//	// �ݹ�
//	while ((mv = Sort.Next()) != 0) {
//		if (pos.MakeMove(mv)) {
//			// ��������  // ! ����֮����߷�����������Ϸ
//			new_depth = pos.LastCheck() ? depth : depth - 1;
//
//			// PVS����  // ! ����
//			if (vl_best == -MATE_VALUE) {  // ! ����vlBest�ĳ�ʼֵ������������if���ǵ�һ��SearchFull
//				vl = -SearchFull(-vlbeta, -vlalpha, new_depth); // ! ��һ����ʵ����
//			}
//			else {
//				/*
//				��AlphaBeta()���������������ã�����ҵ���һ������ô����ͱ��ˡ������ó���Ĵ���(Alpha, Beta)��
//				������(Alpha, Alpha + 1)����������������ǰ���ǣ��������뷵��С�ڻ����Alpha��ֵ��
//				���ȷʵ��������ô�Ѵ��ڵ����沿��ȥ���ͻᵼ�¸���Ľضϡ���Ȼ�����ǰ���Ǵ�ģ�
//				����ֵ��Alpha + 1����ߣ���ô���������ÿ�Ĵ���������
//				*/
//				vl = -SearchFull(-vlalpha - 1, -vlalpha, new_depth);
//				if (vl > vlalpha && vl < vlbeta) { // ! �ҵ�һ�������PV���
//												   // ! ��Ҫ�����������˼��裬�����������һ�����ʱ�ҵ�һ��PV�ŷ�����ô��͵õ�PV���
//					vl = -SearchFull(-vlbeta, -vlalpha, new_depth);
//				}
//			}
//			pos.UndoMakeMove();
//
//			// Alpha-Beta��֦
//			if (vl > vl_best) {             // �ҵ����ֵ(������ȷ����Alpha��PV����Beta�߷�)
//				vl_best = vl;                 // "vlBest"����ĿǰҪ���ص����ֵ�����ܳ���Alpha-Beta�߽�
//				if (vl >= vlbeta) {         // �ҵ�һ��Beta�߷�
//					hash_type = HASH_BETA;
//					mv_best = mv;             // Beta�߷�Ҫ���浽��ʷ��
//					break;                    // Beta�ض�
//				}
//				// ! ��������ifֻ���ȥһ��
//				if (vl > vlalpha) {         // �ҵ�һ��PV�߷�
//					hash_type = HASH_PV;
//					mv_best = mv;              // PV�߷�Ҫ���浽��ʷ��
//					vlalpha = vl;              // ��СAlpha-Beta�߽�
//				}
//			}
//		}
//	}
//
//
//	// 5. �����߷����������ˣ�������߷�(������Alpha�߷�)���浽��ʷ���������ֵ
//	if (vl_best == -MATE_VALUE) {
//		// �����ɱ�壬�͸���ɱ�岽����������  // ?  Ϊʲô���������۷�ʽ
//		return pos.RootDistance - MATE_VALUE;
//	}
//	// ��¼���û���
//	RecordHash(hash_type, vl_best, depth, mv_best); // ? �ú�����˵ ���mv=0�ͻ�return��'Ϊ���ȶ�'
//	if (mv_best != 0) {
//		// �������Alpha�߷����ͽ�����߷����浽��ʷ��
//		// ! Alpha�߷�: ���н�㶼��AlphaС������ǳ���/Alpha��û���¹�, ���ʱmvBest�ͻ��ǳ�ʼ��ֵ0
//		// ! ��������alpha�߷�
//		SetBestMove(mv_best, depth);
//	}
//	return vl_best;
//}


int SearchFull(int vlAlpha, int vlBeta, int nDepth, bool bNoNull) {
	int nHashFlag, vl, vlBest;
	int mv, mvBest, mvHash, nNewDepth;
	SortStruct Sort;
	// һ��Alpha-Beta��ȫ������Ϊ���¼����׶�

	// 1. ����ˮƽ�ߣ�����þ�̬����(ע�⣺���ڿղ��ü�����ȿ���С����)
	if (nDepth <= 0) {
		return SearchQuiesc(vlAlpha, vlBeta);
	}

	// 1-1. ����ظ�����(ע�⣺��Ҫ�ڸ��ڵ��飬�����û���߷���) // ! Ŀ���Ƿ�ֹ����
	vl = pos.IsRepetitive(); //  ? nRecurĬ�ϲ���Ϊ1, �²�͡���һ�β�ͬ������
	if (vl != 0) {
		return pos.RepeatValue(vl);
	}

	// 1-2. ���Ｋ����Ⱦͷ��ؾ�������
	if (pos.RootDistance == LIMIT_DEPTH) {
		return pos.Evaluate();
	}

	// 1-3. �����û���ü������õ��û����߷�
	vl = ProbeHash(vlAlpha, vlBeta, nDepth, mvHash);  // ! mvHash ���ô���
	if (vl > -MATE_VALUE) {
		return vl; // ? ������һ��ɱ��
	}

	//  �к��������Ƿǳ�������(���˲о�����)��ͨ������ֵ����ߣ���һ�����þ������Щ��
	//  ���п��ܵ��ŷ���ʹ�����ø��㣬�����ľ����Ϊ���޵��š�(Zugzwang)(�����˼Ϊǿ������)��ͨ��ֻ�����ڲо��С�
	//  ��ˣ�����������һ��ϣ���߳��߽�Ľ��(��Alpha-Beta�����ķ���ֵ������Beta)��
	//  ����������������������Ȩ���ŷ����������š�(Null-Move)������ʹ��ͨ��������õġ�
	//  ! �����Ȩ�ŷ��߳��߽磬��ô������õ��ŷ�Ҳ���ܸ߳��߽磬��Ϳ���ֱ�ӷ���Beta����Ҫ��ȥ�����ˡ� // ! ԭ�򣺿����õ�һ��betaʹ�ô��ڱ�խ��
	//  Ҫ���������ø��죬��Ȩ�ŷ����������ͨ���ȳ����ŷ�ǳ��
	//  �����С�ģ�����������ı����������Ҳ����ʹ���������е�һЩ��Ҫ����·��
	//  ��Ҫ���������ÿ���(��Ϊ��������������˻������ֻ�������ۺ���)������ҪС�ģ�ֻ���ڲ�������޵��ŵ������ʹ�á�

	// 1-4. ���Կղ��ü�(���ڵ��Betaֵ��"MATE_VALUE"�����Բ����ܷ����ղ��ü�)
	if (!bNoNull && !pos.LastCheck() && pos.NullOkay()) {
		pos.MoveNull();
		// ? ��������д�� alpha = vlBeta - 1; beta = vlBeta, -1�����ú�ԭ����ʲô
		vl = -SearchFull(-vlBeta, 1 - vlBeta, nDepth - NULL_DEPTH - 1, NO_NULL);
		pos.UndoMoveNull();
		if (vl >= vlBeta) {
			return vl;
		}
	}

	// 2. ��ʼ�����ֵ������߷�
	nHashFlag = HASH_ALPHA;
	vlBest = -MATE_VALUE; // ��������֪�����Ƿ�һ���߷���û�߹�(ɱ��)
	mvBest = 0;           // ��������֪�����Ƿ���������Beta�߷���PV�߷����Ա㱣�浽��ʷ��

						  // 3. ��ʼ���߷�����ṹ
	Sort.Init(mvHash);
	//"
	//	void Init(int mvHash_) { // ��ʼ�����趨�û����߷�������ɱ���߷�
	//	mvHash = mvHash_;
	//	mvKiller1 = Search.mvKillers[pos.nDistance][0]; // ! �ɼ�ɱ���߷�������������������ɵģ���ʵ����ֻinit��mvHash
	//	mvKiller2 = Search.mvKillers[pos.nDistance][1];
	//	nPhase = PHASE_HASH;
	//}
	//"
		// 4. ��һ����Щ�߷��������еݹ�
		while ((mv = Sort.Next()) != 0) {
			if (pos.MakeMove(mv)) {
				// ��������  // ?
				nNewDepth = pos.LastCheck() ? nDepth : nDepth - 1;

				// PVS  // ! ����
				if (vlBest == -MATE_VALUE) {  // ! ����vlBest�ĳ�ʼֵ������������if���ǵ�һ��SearchFull
					vl = -SearchFull(-vlBeta, -vlAlpha, nNewDepth); // ! ��һ����ʵ����
				}
				else {
					/*"
						��AlphaBeta()���������������ã�����ҵ���һ������ô����ͱ��ˡ������ó���Ĵ���(Alpha, Beta)��
						������(Alpha, Alpha + 1)����������������ǰ���ǣ��������뷵��С�ڻ����Alpha��ֵ��
						���ȷʵ��������ô�Ѵ��ڵ����沿��ȥ���ͻᵼ�¸���Ľضϡ���Ȼ�����ǰ���Ǵ�ģ�
						����ֵ��Alpha + 1����ߣ���ô���������ÿ�Ĵ���������
						"*/
						vl = -SearchFull(-vlAlpha - 1, -vlAlpha, nNewDepth);
					if (vl > vlAlpha && vl < vlBeta) { // ! �ҵ�һ�������PV���
													   // ! ��Ҫ�����������˼��裬�����������һ�����ʱ�ҵ�һ��PV�ŷ�����ô��͵õ�PV���
						vl = -SearchFull(-vlBeta, -vlAlpha, nNewDepth);
					}
				}
				pos.UndoMakeMove();

				// 5. ����Alpha-Beta��С�жϺͽض�
				if (vl > vlBest) {    // �ҵ����ֵ(������ȷ����Alpha��PV����Beta�߷�)
					vlBest = vl;        // "vlBest"����ĿǰҪ���ص����ֵ�����ܳ���Alpha-Beta�߽�
					if (vl >= vlBeta) { // �ҵ�һ��Beta�߷�
						nHashFlag = HASH_BETA;
						mvBest = mv;      // Beta�߷�Ҫ���浽��ʷ��
						break;            // Beta�ض�
					}
					// ! ��������ifֻ���ȥһ��
					if (vl > vlAlpha) { // �ҵ�һ��PV�߷�
						nHashFlag = HASH_PV;
						mvBest = mv;      // PV�߷�Ҫ���浽��ʷ��
						vlAlpha = vl;     // ��СAlpha-Beta�߽�
					}
				}
			}
		}

	// 5. �����߷����������ˣ�������߷�(������Alpha�߷�)���浽��ʷ���������ֵ
	if (vlBest == -MATE_VALUE) {
		// �����ɱ�壬�͸���ɱ�岽����������  // ?  Ϊʲô���������۷�ʽ
		return pos.RootDistance - MATE_VALUE;
	}
	// ��¼���û���
	RecordHash(nHashFlag, vlBest, nDepth, mvBest); // ? �ú�����˵ ���mv=0�ͻ�return��'Ϊ���ȶ�'
	if (mvBest != 0) {
		// �������Alpha�߷����ͽ�����߷����浽��ʷ��
		// ! Alpha�߷�: ���н�㶼��AlphaС������ǳ���/Alpha��û���¹�, ���ʱmvBest�ͻ��ǳ�ʼ��ֵ0
		// ! ��������alpha�߷�
		SetBestMove(mvBest, nDepth);
	}
	return vlBest;
}
#endif // CPP3



// ���ڵ��Alpha-Beta��������
static int SearchRoot(int nDepth) {
	int vl, vlBest, mv, nNewDepth;
	SortStruct Sort;

	vlBest = -MATE_VALUE;
	Sort.Init(Search.mvResult);
	while ((mv = Sort.Next()) != 0) {
		if (pos.MakeMove(mv)) {
			nNewDepth = pos.LastCheck() ? nDepth : nDepth - 1;
			if (vlBest == -MATE_VALUE) {
				vl = -SearchFull(-MATE_VALUE, MATE_VALUE, nNewDepth, NO_NULL);
			}
			else {
				vl = -SearchFull(-vlBest - 1, -vlBest, nNewDepth);
				if (vl > vlBest) {
					vl = -SearchFull(-MATE_VALUE, -vlBest, nNewDepth, NO_NULL);
				}
			}
			pos.UndoMakeMove();
			if (vl > vlBest) {
				vlBest = vl;
				Search.mvResult = mv;
				if (vlBest > -WIN_VALUE && vlBest < WIN_VALUE) {
					vlBest += (rand() & RANDOM_MASK) - (rand() & RANDOM_MASK);
				}
			}
		}
	}
	RecordHash(HASH_PV, vlBest, nDepth, Search.mvResult);
	SetBestMove(Search.mvResult, nDepth);
	return vlBest;
}

// �����Ķ������
void SearchMain(clock_t time_limit) {
	int i, t, vl, n_mvs;
	int mvs[MAX_GEN_MOVES];

	// ��ʼ��
	memset(Search.nHistoryTable, 0, 65536 * sizeof(int));       // �����ʷ��
	memset(Search.mvKillers, 0, LIMIT_DEPTH * 2 * sizeof(int)); // ���ɱ���߷���
	memset(Search.HashTable, 0, HASH_SIZE * sizeof(HashItem));  // ����û���
	t = clock();                                                // ��ʼ����ʱ��
	pos.RootDistance = 0; // ��ʼ����


						  // ����Ƿ�ֻ��Ψһ�߷�
	vl = 0;
	n_mvs = pos.GenerateMoves(mvs);
#ifdef DEBUG
	for (int i = 0; i < n_mvs; i++) {
		printf("%x\n", mvs[i]);
	}
#endif // DEBUG

	for (i = 0; i < n_mvs; i++) {
		if (pos.MakeMove(mvs[i])) {
			pos.UndoMakeMove();
			Search.mvResult = mvs[i];
			vl++;
		}
	}
	if (vl == 1) {
		return;
	}

	// �����������
	for (i = 1; i <= LIMIT_DEPTH; i++) {
		vl = SearchRoot(i);
		//vl = SearchFull(-MATE_VALUE, MATE_VALUE, i);
		// ������ɱ�壬����ֹ����
		if (vl > WIN_VALUE || vl < -WIN_VALUE) {
			break; // ! ɱ�嶼�� nDistance - 	VALUE_MATE
		}
		// ����һ�룬����ֹ����
		if (clock() - t > time_limit - 10) { // ! ����һ���ʱ�� �Է���һ �����㹻��ʱ������bestmove�ַ���
			break;
		}
	}
}
