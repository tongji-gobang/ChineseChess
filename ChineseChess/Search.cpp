#include"Search.h"
#include"RESOURCE.H"
#include"ChessBoard.h"


S Search;
/*
// "qsort"按历史表排序的比较函数
static int CompareHistory(const void *lpmv1, const void *lpmv2) {
	return Search.nHistoryTable[*(int *)lpmv2] - Search.nHistoryTable[*(int *)lpmv1];
}

// 超出边界(Fail-Soft)的Alpha-Beta搜索过程
static int SearchFull(int vlAlpha, int vlBeta, int nDepth) {
	int i, nGenMoves, pcCaptured;
	int vl, vlBest, mvBest;
	int mvs[MAX_GEN_MOVES];
	// 一个Alpha-Beta完全搜索分为以下几个阶段

	// 1. 到达水平线，则返回局面评价值
	if (nDepth == 0) {
		return pos.Evaluate();
	}

	// 2. 初始化最佳值和最佳走法
	vlBest = -MATE_VALUE; // 这样可以知道，是否一个走法都没走过(杀棋)
	mvBest = 0;           // 这样可以知道，是否搜索到了Beta走法或PV走法，以便保存到历史表

	// 3. 生成全部走法，并根据历史表排序
	nGenMoves = pos.GenerateMoves(mvs);
	qsort(mvs, nGenMoves, sizeof(int), CompareHistory);

	// 4. 逐一走这些走法，并进行递归
	for (i = 0; i < nGenMoves; i++) {
		if (pos.MakeMove(mvs[i], pcCaptured)) {
			vl = -SearchFull(-vlBeta, -vlAlpha, nDepth - 1);
			pos.UndoMakeMove(mvs[i], pcCaptured);

			// 5. 进行Alpha-Beta大小判断和截断
			if (vl > vlBest) {    // 找到最佳值(但不能确定是Alpha、PV还是Beta走法)
				vlBest = vl;        // "vlBest"就是目前要返回的最佳值，可能超出Alpha-Beta边界
				if (vl >= vlBeta) { // 找到一个Beta走法
					mvBest = mvs[i];  // Beta走法要保存到历史表
					break;            // Beta截断
				}
				if (vl > vlAlpha) { // 找到一个PV走法
					mvBest = mvs[i];  // PV走法要保存到历史表
					vlAlpha = vl;     // 缩小Alpha-Beta边界
				}
			}
		}
	}

	// 5. 所有走法都搜索完了，把最佳走法(不能是Alpha走法)保存到历史表，返回最佳值 
	if (vlBest == -MATE_VALUE) {
		// 如果是杀棋，就根据杀棋步数给出评价
		return pos.nDistance - MATE_VALUE;
	}
	if (mvBest != 0) {
		// 如果不是Alpha走法，就将最佳走法保存到历史表
		Search.nHistoryTable[mvBest] += nDepth * nDepth;
		if (pos.nDistance == 0) {
			// 搜索根节点时，总是有一个最佳走法(因为全窗口搜索不会超出边界)，将这个走法保存下来
			Search.mvResult = mvBest;
		}
	}
	return vlBest;
}

// 迭代加深搜索过程
void SearchMain(void) {
	int i, t, vl;

	// 初始化
	memset(Search.nHistoryTable, 0, 65536 * sizeof(int)); // 清空历史表
	t = clock();       // 初始化定时器
	pos.nDistance = 0; // 初始步数

					   // 迭代加深过程
	for (i = 1; i <= LIMIT_DEPTH; i++) {
		vl = SearchFull(-MATE_VALUE, MATE_VALUE, i);
		// 搜索到杀棋，就终止搜索
		if (vl > WIN_VALUE || vl < -WIN_VALUE) {
			break;
		}
		// 超过一秒，就终止搜索
		if (clock() - t > CLOCKS_PER_SEC) {
			break;
		}
	}
}
*/





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
		hsh.vl -= pos.nDistance;		//>Ban_value 杀棋且不长将
		bMate = TRUE;
	}
	else if (hsh.vl < -WIN_VALUE) {
		if (hsh.vl > -BAN_VALUE) {
			return MATE_VALUE_neg;     //负数类似
		}
		hsh.vl += pos.nDistance;
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
		hsh.vl = vl + pos.nDistance;			//>Ban_value 杀棋且不长将
	}
	else if (vl < -WIN_VALUE) {					// 同上
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

