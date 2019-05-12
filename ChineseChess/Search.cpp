
#define DEBUG

#include"Search.h"
#include"RESOURCE.H"
#include"ChessBoard.h"
#include"sort.h"

S Search;

// 设置最优move
void SetBestMove(int mv, int nDepth) {
	int *lpmvKillers;
	Search.nHistoryTable[mv] += nDepth * nDepth;  // 写入历史表
	lpmvKillers = Search.mvKillers[pos.RootDistance];  // 写入杀手表 // ! Search.mvKillers[MAX][2]
	if (lpmvKillers[0] != mv) {
		lpmvKillers[1] = lpmvKillers[0];
		lpmvKillers[0] = mv;
	}
}

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
		//int PieceCaptured;
		if (pos.MakeMove(mvs[i])) {
			value = -SearchQuiesc(-beta, -alpha);
			pos.UndoMakeMove();

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

	Search.HashTable[pos.zobr.dwKey & HASH_SIZE_end] = { (BYTE)Depth,(BYTE)Flag,hsh.vl,(WORD)mv,pos.zobr.dwLock0,pos.zobr.dwLock1 };
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


// debug 把原来完全的搜索注释掉了
//  Alpha-Beta搜索
//int SearchFull(int vlalpha, int vlbeta, int depth, bool NoNull) {
//	int vl, vl_best;
//	int mv, mv_best, mv_hash;
//	int new_depth;
//	int hash_type;
//	SortStruct Sort;
//
//	// 到达水平线，调用静态搜索
//	if (depth <= 0) {
//		//return SearchQuiesc(vlalpha, vlbeta);
//		return pos.Evaluate();
//	}
//
//	// 检查重复局面,防止长将,也就是不要做了
//	//vl = pos.IsRepetitive(); //  ? nRecur默认参数为1, 猜测和“上一次不同”即可
//	//if (vl != 0) {
//	//	return pos.RepeatValue(vl);
//	//}
//
//	// 到达极限深度就返回局面评价
//	//if (pos.RootDistance == LIMIT_DEPTH) {
//	//	return pos.Evaluate();
//	//}
//
//	// 搜索置换表
//	//vl = ProbeHash(vlalpha, vlbeta, depth, mv_hash);  // ! mv_hash 引用传参
//	//if (vl > -MATE_VALUE) {
//	//	return vl; // ? 返回了一步杀棋
//	//}
//
//	//  有害的着子是非常罕见的(除了残局以外)。通常如果轮到你走，你一定能让局面更好些。
//	//  所有可能的着法都使局面变得更糟，这样的局面称为“无等着”(Zugzwang)(德语，意思为强迫着子)，通常只发生在残局中。
//	//  因此，假设你搜索一个希望高出边界的结点(即Alpha-Beta搜索的返回值至少是Beta)，
//	//  空着搜索就是先搜索“弃权”着法【即“空着”(Null-Move)】，即使它通常不是最好的。
//	//  ! 如果弃权着法高出边界，那么真正最好的着法也可能高出边界，你就可以直接返回Beta而不要再去搜索了。 // ! 原因：可以拿到一个beta使得窗口变窄？
//	//  要把搜索做得更快，弃权着法搜索的深度通常比常规着法浅。
//	//  你必须小心，这种启发会改变搜索结果，也可能使你忽略棋局中的一些重要的线路。
//	//  不要连续两次用空着(因为这样你的搜索会退化，结果只返回评价函数)，而且要小心，只能在不会出现无等着的情况下使用。
//
//	// 空步裁剪
//	//if (!NoNull && !pos.LastCheck() && pos.NullOkay()) {
//	//	pos.MoveNull();
//	//	// ? 参数这样写？ alpha = vlbeta - 1; beta = vlbeta, -1的作用和原理是什么
//	//	vl = -SearchFull(-vlbeta, 1 - vlbeta, depth - NULL_DEPTH - 1, NO_NULL);
//	//	pos.UndoMoveNull();
//	//	if (vl >= vlbeta) {
//	//		return vl;
//	//	}
//	//}
//
//	// 初始化走法排序结构、最佳值和最佳走法
//	hash_type = HASH_ALPHA;
//	vl_best = -MATE_VALUE; // 这样可以知道，是否一个走法都没走过(杀棋)
//	mv_best = 0;           // 这样可以知道，是否搜索到了Beta走法或PV走法，以便保存到历史表
//	Sort.Init(mv_hash);
//	/*
//	void Init(int mvHash_) { // 初始化，设定置换表走法和两个杀手走法
//	mvHash = mvHash_;
//	mvKiller1 = Search.mvKillers[pos.nDistance][0]; // ! 可见杀手走法不是在这个函数中生成的，故实际上只init了mvHash
//	mvKiller2 = Search.mvKillers[pos.nDistance][1];
//	nPhase = PHASE_HASH;
//	}
//	*/
//
//	// 递归
//	//while ((mv = Sort.Next()) != 0) {
//	//	if (pos.MakeMove(mv)) {
//	//		// 将军延伸  // ! 将军之后的走法往往都很有戏
//	//		new_depth = pos.LastCheck() ? depth : depth - 1;
//
//	//		// PVS搜索  // ! 新增
//	//		if (vl_best == -MATE_VALUE) {  // ! 根据vlBest的初始值，如果进入这个if即是第一次SearchFull
//	//			vl = -SearchFull(-vlbeta, -vlalpha, new_depth); // ! 第一次老实搜索
//	//		}
//	//		else {
//	//			/*
//	//			“AlphaBeta()”函数就正常调用，如果找到了一个，那么情况就变了。不是用常规的窗口(Alpha, Beta)，
//	//			而是用(Alpha, Alpha + 1)来搜索。这样做的前提是，搜索必须返回小于或等于Alpha的值，
//	//			如果确实这样，那么把窗口的上面部分去掉就会导致更多的截断。当然，如果前提是错的，
//	//			返回值是Alpha + 1或更高，那么搜索必须用宽的窗口重做。
//	//			*/
//	//			vl = -SearchFull(-vlalpha - 1, -vlalpha, new_depth);
//	//			if (vl > vlalpha && vl < vlbeta) { // ! 找到一个结点是PV结点
//	//											   // ! 主要变例搜索作了假设，如果你在搜索一个结点时找到一个PV着法，那么你就得到PV结点
//	//				vl = -SearchFull(-vlbeta, -vlalpha, new_depth);
//	//			}
//	//		}
//	//		pos.UndoMakeMove();
//
//	//		// Alpha-Beta剪枝
//	//		if (vl > vl_best) {             // 找到最佳值(但不能确定是Alpha、PV还是Beta走法)
//	//			vl_best = vl;                 // "vlBest"就是目前要返回的最佳值，可能超出Alpha-Beta边界
//	//			if (vl >= vlbeta) {         // 找到一个Beta走法
//	//				hash_type = HASH_BETA;
//	//				mv_best = mv;             // Beta走法要保存到历史表
//	//				break;                    // Beta截断
//	//			}
//	//			// ! 上下两个if只会进去一个
//	//			if (vl > vlalpha) {         // 找到一个PV走法
//	//				hash_type = HASH_PV;
//	//				mv_best = mv;              // PV走法要保存到历史表
//	//				vlalpha = vl;              // 缩小Alpha-Beta边界
//	//			}
//	//		}
//	//	}
//	//}
//
//	// ---------------
//	int nGenMoves = pos.GenerateMoves(mvs);
//	qsort(mvs, nGenMoves, sizeof(int), CompareHistory);
//
//	for (i = 0; i < nGenMoves; i++) {
//		if (pos.MakeMove(mvs[i], pcCaptured)) {
//			vl = -SearchFull(-vlBeta, -vlAlpha, nDepth - 1);
//			pos.UndoMakeMove(mvs[i], pcCaptured);
//
//			// 5. 进行Alpha-Beta大小判断和截断
//			if (vl > vlBest) {    // 找到最佳值(但不能确定是Alpha、PV还是Beta走法)  
//
//
//				vlBest = vl;        // "vlBest"就是目前要返回的最佳值，可能超出Alpha-Beta边界
//				if (vl >= vlBeta) { // 找到一个Beta走法
//					mvBest = mvs[i];  // Beta走法要保存到历史表
//					break;            // Beta截断
//				}
//				if (vl > vlAlpha) { // 找到一个PV走法
//					mvBest = mvs[i];  // PV走法要保存到历史表
//					vlAlpha = vl;     // 缩小Alpha-Beta边界
//				}
//			}
//		}
//	}
//
//
//	// -----------------
//
//	// 5. 所有走法都搜索完了，把最佳走法(不能是Alpha走法)保存到历史表，返回最佳值
//	if (vl_best == -MATE_VALUE) {
//		// 如果是杀棋，就根据杀棋步数给出评价  // ?  为什么是这种评价方式
//		return pos.RootDistance - MATE_VALUE;
//	}
//	// 记录到置换表
//	RecordHash(hash_type, vl_best, depth, mv_best); // ? 该函数里说 如果mv=0就会return，'为了稳定'
//	if (mv_best != 0) {
//		// 如果不是Alpha走法，就将最佳走法保存到历史表
//		// ! Alpha走法: 所有结点都比Alpha小，情况非常差/Alpha都没更新过, 则此时mvBest就会是初始化值0
//		// ! 不是则不是alpha走法
//		SetBestMove(mv_best, depth);
//	}
//	return vl_best;
//}

int SearchFull(int vlAlpha, int vlBeta, int nDepth, bool NoNull) {
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

	// 4. 逐一走这些走法，并进行递归  //，深度优先遍历
	for (i = 0; i < nGenMoves; i++) {
		if (pos.MakeMove(mvs[i])) {
			vl = -SearchFull(-vlBeta, -vlAlpha, nDepth - 1);
			pos.UndoMakeMove();

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
		return pos.RootDistance - MATE_VALUE;
	}
	if (mvBest != 0) {
		// 如果不是Alpha走法，就将最佳走法保存到历史表
		Search.nHistoryTable[mvBest] += nDepth * nDepth;
		if (pos.RootDistance == 0) {
			// 搜索根节点时，总是有一个最佳走法(因为全窗口搜索不会超出边界)，将这个走法保存下来
			Search.mvResult = mvBest;
		}
	}
	return vlBest;
}


// 根节点的Alpha-Beta搜索过程
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
      } else {
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

// 搜索的顶层调用
void SearchMain(clock_t time_limit) {
	int i, t, vl, n_mvs;
	int mvs[MAX_GEN_MOVES];

	// 初始化
	memset(Search.nHistoryTable, 0, 65536 * sizeof(int));       // 清空历史表
	memset(Search.mvKillers, 0, LIMIT_DEPTH * 2 * sizeof(int)); // 清空杀手走法表
	memset(Search.HashTable, 0, HASH_SIZE * sizeof(HashItem));  // 清空置换表
	t = clock();                                                // 初始化定时器
	pos.RootDistance = 0; // 初始步数


						  // 检查是否只有唯一走法
	vl = 0;
	//n_mvs = pos.GenerateMoves(mvs);
#ifdef DEBUG
	//for (int i = 0; i < n_mvs; i++) {
	//	printf("%x\n", mvs[i]);
	//}
#endif // DEBUG

	/*for (i = 0; i < n_mvs; i++) {
		if (pos.MakeMove(mvs[i])) {
			pos.UndoMakeMove();
			Search.mvResult = mvs[i];
			vl++;
		}
	}
	if (vl == 1) {
		return;
	}*/

	// 迭代加深过程
	for (i = 1; i <= LIMIT_DEPTH; i++) {
		//vl = SearchRoot(i);
		vl = SearchFull(-MATE_VALUE, MATE_VALUE, i);
		// 搜索到杀棋，就终止搜索
		if (vl > WIN_VALUE || vl < -WIN_VALUE) {
			break; // ! 杀棋都是 nDistance - 	VALUE_MATE
		}
		// 超过一秒，就终止搜索
		if (clock() - t > time_limit - 10) { // ! 减掉一点点时间 以防万一 让有足够的时间生成bestmove字符串
			break;
		}
	}
}
