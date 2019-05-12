#include"Search.h"
#include"RESOURCE.H"
#include"ChessBoard.h"


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
		int PieceCaptured;
		if (pos.MakeMove(mvs[i],PieceCaptured)) {
			value = -SearchQuiesc(-beta, -alpha);
			pos.UndoMakeMove(mvs[i],PieceCaptured);

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



//  Alpha-Beta����
int SearchFull(int vlalpha, int vlbeta, int depth, bool NoNull = FALSE) {
	int vl, vl_best;
	int mv, mv_best, mv_hash;
	int new_depth;
	int hash_type;
	SortStruct Sort;

	// ����ˮƽ�ߣ����þ�̬����
	if (depth <= 0) {
		return SearchQuiesc(vlalpha, vlbeta);
	}

	// ����ظ�����,��ֹ����,Ҳ���ǲ�Ҫ����
	vl = pos.IsRepetitive(); //  ? nRecurĬ�ϲ���Ϊ1, �²�͡���һ�β�ͬ������
	if (vl != 0) {
		return pos.RepeatValue(vl);
	}

	// ���Ｋ����Ⱦͷ��ؾ�������
	if (pos.RootDistance == LIMIT_DEPTH) {
		return pos.Evaluate();
	}

	// �����û���
	vl = ProbeHash(vlalpha, vlbeta, depth, mv_hash);  // ! mv_hash ���ô���
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

	// �ղ��ü�
	if (!NoNull && !pos.LastCheck() && pos.NullOkay()) {
		pos.MoveNull();
		// ? ��������д�� alpha = vlbeta - 1; beta = vlbeta, -1�����ú�ԭ����ʲô
		vl = -SearchFull(-vlbeta, 1 - vlbeta, depth - NULL_DEPTH - 1, NO_NULL);
		pos.UndoMoveNull();
		if (vl >= vlbeta) {
			return vl;
		}
	}

	// ��ʼ���߷�����ṹ�����ֵ������߷�
	hash_type = HASH_ALPHA;
	vl_best = -MATE_VALUE; // ��������֪�����Ƿ�һ���߷���û�߹�(ɱ��)
	mv_best = 0;           // ��������֪�����Ƿ���������Beta�߷���PV�߷����Ա㱣�浽��ʷ��
	Sort.Init(mv_hash);
	/*
	void Init(int mvHash_) { // ��ʼ�����趨�û����߷�������ɱ���߷�
	mvHash = mvHash_;
	mvKiller1 = Search.mvKillers[pos.nDistance][0]; // ! �ɼ�ɱ���߷�������������������ɵģ���ʵ����ֻinit��mvHash
	mvKiller2 = Search.mvKillers[pos.nDistance][1];
	nPhase = PHASE_HASH;
	}
	*/

	// �ݹ�
	while ((mv = Sort.Next()) != 0) {
		if (pos.MakeMove(mv)) {
			// ��������  // ! ����֮����߷�����������Ϸ
			new_depth = pos.LastCheck() ? depth : depth - 1;

			// PVS����  // ! ����
			if (vl_best == -MATE_VALUE) {  // ! ����vlBest�ĳ�ʼֵ������������if���ǵ�һ��SearchFull
				vl = -SearchFull(-vlbeta, -vlalpha, new_depth); // ! ��һ����ʵ����
			}
			else {
				/*
				��AlphaBeta()���������������ã�����ҵ���һ������ô����ͱ��ˡ������ó���Ĵ���(Alpha, Beta)��
				������(Alpha, Alpha + 1)����������������ǰ���ǣ��������뷵��С�ڻ����Alpha��ֵ��
				���ȷʵ��������ô�Ѵ��ڵ����沿��ȥ���ͻᵼ�¸���Ľضϡ���Ȼ�����ǰ���Ǵ�ģ�
				����ֵ��Alpha + 1����ߣ���ô���������ÿ�Ĵ���������
				*/
				vl = -SearchFull(-vlalpha - 1, -vlalpha, new_depth);
				if (vl > vlalpha && vl < vlbeta) { // ! �ҵ�һ�������PV���
												   // ! ��Ҫ�����������˼��裬�����������һ�����ʱ�ҵ�һ��PV�ŷ�����ô��͵õ�PV���
					vl = -SearchFull(-vlbeta, -vlalpha, new_depth);
				}
			}
			pos.UndoMakeMove();

			// Alpha-Beta��֦
			if (vl > vl_best) {             // �ҵ����ֵ(������ȷ����Alpha��PV����Beta�߷�)
				vl_best = vl;                 // "vlBest"����ĿǰҪ���ص����ֵ�����ܳ���Alpha-Beta�߽�
				if (vl >= vlbeta) {         // �ҵ�һ��Beta�߷�
					hash_type = HASH_BETA;
					mv_best = mv;             // Beta�߷�Ҫ���浽��ʷ��
					break;                    // Beta�ض�
				}
				// ! ��������ifֻ���ȥһ��
				if (vl > vlalpha) {         // �ҵ�һ��PV�߷�
					hash_type = HASH_PV;
					mv_best = mv;              // PV�߷�Ҫ���浽��ʷ��
					vlalpha = vl;              // ��СAlpha-Beta�߽�
				}
			}
		}
	}

	// 5. �����߷����������ˣ�������߷�(������Alpha�߷�)���浽��ʷ���������ֵ
	if (vl_best == -MATE_VALUE) {
		// �����ɱ�壬�͸���ɱ�岽����������  // ?  Ϊʲô���������۷�ʽ
		return pos.RootDistance - MATE_VALUE;
	}
	// ��¼���û���
	RecordHash(hash_type, vl_best, depth, mv_best); // ? �ú�����˵ ���mv=0�ͻ�return��'Ϊ���ȶ�'
	if (mv_best != 0) {
		// �������Alpha�߷����ͽ�����߷����浽��ʷ��
		// ! Alpha�߷�: ���н�㶼��AlphaС������ǳ���/Alpha��û���¹�, ���ʱmvBest�ͻ��ǳ�ʼ��ֵ0
		// ! ��������alpha�߷�
		SetBestMove(mv_best, depth);
	}
	return vl_best;
}



// �����Ķ������
static void SearchMain(clock_t time_limit) {
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
		//vl = SearchRoot(i);
		// ! SearchFull(-MATE_VALUE, MATE_VALUE,  i,  NoNull = FALSE);
		// ������ɱ�壬����ֹ����
		if (vl > WIN_VALUE || vl < -WIN_VALUE) {
			break; // ! ɱ�嶼�� nDistance - 	VALUE_MATE
		}
		// ����һ�룬����ֹ����
		if (clock() - t > time_limit - 1000) { // ! ����һ���ʱ�� �Է���һ �����㹻��ʱ������bestmove�ַ���
			break;
		}
	}
}
