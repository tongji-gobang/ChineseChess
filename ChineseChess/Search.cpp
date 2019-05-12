#include"Search.h"
#include"RESOURCE.H"
#include"ChessBoard.h"


S Search;

// 静态(Quiescence)搜索过程
int SearchQuiesc(int alpha, int beta) {
	int i, movenum;
	int value, best;
	int mvs[MAX_GEN_MOVES];

	// 1. 检查重复局面
	value = pos.IsRepetitive();
	if (value != 0)
		return pos.RepeatValue(value);	// 若重复则返回相应的重复分

	// 2. 限制搜索深度
	if (pos.RootDistance == LIMIT_DEPTH)
		return pos.Evaluate();

	// 3. 最佳值设为杀棋分
	best = -MATE_VALUE; // 这样可以知道，是否一个走法都没走过(杀棋)

	if (pos.LastCheck()) {
		// 4. 如果被将军，则生成全部走法
		movenum = pos.GenerateMoves(mvs);
		qsort(mvs, movenum, sizeof(int), CompareHistory);
	}
	else {

		// 5. 如果不被将军，先评价当前局面
		value = pos.Evaluate();
		if (value > best) {			// 找到最佳值
			best = value;
			if (value >= beta) {	// 找到beta走法
				return value;		// 评价好得足以截断而不需要试图吃子
			}
			if (value > alpha) {
				alpha = value;
			}
		}

		// 6. 如果局面评价没有截断，再考虑吃子走法
		movenum = pos.GenerateMoves(mvs, GEN_CAPTURE);
		qsort(mvs, movenum, sizeof(int), CompareMvvLva);	//按MVVLVA排序吃子着法
	}

	// 7. 逐一走这些走法，并进行递归
	for (i = 0; i < movenum; ++i) {
		int PieceCaptured;
		if (pos.MakeMove(mvs[i],PieceCaptured)) {
			value = -SearchQuiesc(-beta, -alpha);
			pos.UndoMakeMove(mvs[i],PieceCaptured);

			// 8. 进行Alpha-Beta大小判断和截断
			if (value > best) {		// 找到最佳值(但不能确定是Alpha、PV还是Beta走法)
				best = value;		// "best"就是目前要返回的最佳值，可能超出Alpha-Beta边界
				if (value >= beta) {// 找到一个Beta走法
					return value;
				}
				if (value > alpha) {	// 找到一个PV走法
					alpha = value;		// 更新alpha，缩小Alpha-Beta边界
				}
			}
		}
	}

	// 9. 若一个走法也没走，则说明被杀，返回杀棋分，否则返回best
	return best == -MATE_VALUE ? pos.RootDistance - MATE_VALUE : best;
}

int ProbeHash(int vl_Alpha, int vl_Beta, int Depth, int &mv) {
	bool  bMate; // 杀棋标志
				 //通过 [ dwKey % HASH_SIZE ] 得到具体走法
				 // 可以通过指针优化hsh
	HashItem hsh = Search.HashTable[pos.zobr.dwKey & HASH_SIZE_end];
	//
	if (hsh.Lock0 != pos.zobr.dwLock0 || hsh.Lock1 != pos.zobr.dwLock1) {		//如果检验码不同，则跳出
		mv = 0;
		return MATE_VALUE_neg;
	}

	mv = hsh.mv;
	bMate = FALSE;						//默认未搜索到杀棋

										//[win_value,Ban_value],则为存在杀棋但长将；
	if (hsh.vl > WIN_VALUE) {
		if (hsh.vl < BAN_VALUE)			//低于长将判负的分值则不写入置换表
			return MATE_VALUE_neg;			// 可能导致搜索的不稳定性，立刻退出，但最佳着法可能拿到
		hsh.vl -= pos.RootDistance;		//>Ban_value 杀棋且不长将
		bMate = TRUE;
	}
	else if (hsh.vl < -WIN_VALUE) {
		if (hsh.vl > -BAN_VALUE) {
			return MATE_VALUE_neg;     //负数类似
		}
		hsh.vl += pos.RootDistance;
		bMate = TRUE;
	}

	if (hsh.Depth >= Depth || bMate) {			//满足深度限制或者为杀棋（如果是杀棋，那么不需要满足深度条件）
		switch (hsh.Flag)
		{
			case HASH_BETA:		return (hsh.vl >= vl_Beta ? hsh.vl : MATE_VALUE_neg); break;
			case HASH_ALPHA:	return (hsh.vl <= vl_Alpha ? hsh.vl : MATE_VALUE_neg); break;
			default:			return hsh.vl; break;
		}
	}

	return MATE_VALUE_neg;
};



// 保存置换表项
void RecordHash(int Flag, int vl, int Depth, int mv) {
	// 可以通过指针优化hsh
	HashItem hsh = Search.HashTable[pos.zobr.dwKey & HASH_SIZE_end];
	if (hsh.Depth > Depth) {
		return;
	}

	if (vl > WIN_VALUE) {
		if (mv == 0 && vl <= BAN_VALUE) {		// 可能导致搜索的不稳定性，并且没有最佳着法，立刻退出
			return;
		}
		hsh.vl = vl + pos.RootDistance;			//>Ban_value 杀棋且不长将
	}
	else if (vl < -WIN_VALUE) {					// 同上
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

// 求MVV/LVA值
int MvvLva(int mv) {
	return (cucMvvLva[pos.Board[DstPos(mv)]] << 3) - cucMvvLva[pos.Board[SrcPos(mv)]];
}

// qsort按MVV/LVA值排序的比较函数
int CompareMvvLva(const void *p1, const void *p2) {
	return MvvLva(*(int *)p2) - MvvLva(*(int *)p1);
}

// qsort历史表排序的比较函数
int CompareHistory(const void *p1, const void *p2) {
	return Search.nHistoryTable[*(int *)p2] - Search.nHistoryTable[*(int *)p1];
}
