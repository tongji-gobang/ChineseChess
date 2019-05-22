

//#define CPP3 1

//#define CPP4 2

//#define CPP5  3
#define CPP6 4

#include"Search.h"
#include"RESOURCE.H"
#include"ChessBoard.h"
#include"sort.h"

S Search;

clock_t start_time;
clock_t time_limit;
clock_t this_time;
clock_t reserved_time = 20;

// 设置最优move
void SetBestMove(int mv, int depth) {
	int *p;
	Search.nHistoryTable[mv] += depth * depth;  // 写入历史表
	p = Search.mvKillers[pos.RootDistance];  // 写入杀手表 // ! Search.mvKillers[MAX][2]
	if (p[0] != mv) {
		p[1] = p[0];
		p[0] = mv;
	}
}

// 静态搜索：克服水平线效应
int QuiescSearch(int alpha, int beta, int depth = 0) {
	int i, movenum;
	int value, best;
	int mvs[MAX_GEN_MOVES];

    if (clock() - start_time + reserved_time >= time_limit)
        return pos.Evaluate();

	//检查重复局面
	value = pos.IsRepetitive();
	if (value != 0)
		return pos.RepeatValue(value);	//若重复则返回相应的重复分

	//限制搜索深度，避免搜索过深
	if (pos.RootDistance == LIMIT_DEPTH)
		return pos.Evaluate();

	//最佳值设为杀棋分，若最后best没有更改，则被杀
	best = -MATE_VALUE;

	if (pos.LastCheck()) {
		//如果被将军，则生成全部走法
		movenum = pos.GenerateMoves(mvs);
		qsort(mvs, movenum, sizeof(int), CompareHistory);
	}
	else {

		//如果不被将军，先评价当前局面
		value = pos.Evaluate();
		if (value > best) {			//找到最佳值
			best = value;
			if (value >= beta) {	//找到beta走法
				return value;		//评价好得足以截断而不需要试图吃子
			}
			if (value > alpha) {	//找到一个PV走法
				alpha = value;
			}
		}

		//如果局面评价没有截断，再考虑吃子走法
		movenum = pos.GenerateMoves(mvs, GEN_CAPTURE);		//生成所有吃子走法
		qsort(mvs, movenum, sizeof(int), CompareMvvLva);	//按MVVLVA排序吃子着法
	}

	//对每一种吃子走法进行递归
	for (i = 0; i < movenum; ++i) {
		//int PieceCaptured;
		if (pos.MakeMove(mvs[i])) {

			value = -QuiescSearch(-beta, -alpha, depth + 1);
			pos.UndoMakeMove();

			//进行Alpha-Beta大小判断和截断
			if (value > best) {			//找到最佳值
				best = value;
				if (value >= beta) {	//找到一个beta走法
					return value;
				}
				if (value > alpha) {	//找到一个PV走法
					alpha = value;		//更新alpha，缩小alpha-beta边界
				}
			}
		}
	}

	//若一个走法也没走，则说明被杀，返回杀棋分，否则返回best
	return best == -MATE_VALUE ? pos.RootDistance - MATE_VALUE : best;
}

// 查找置换表项
int ProbeHash(int vl_Alpha, int vl_Beta, int Depth, int &mv) {
	bool  bMate; // 杀棋标志
				 //通过 [ key0 % HASH_SIZE ] 得到具体走法
				 // 可以通过指针优化hsh
	HashItem hsh = Search.HashTable[pos.zobr.key0 & HASH_SIZE_end];
	//
	if (hsh.Lock0 != pos.zobr.key1 || hsh.Lock1 != pos.zobr.key2) {		//如果检验码不同，则跳出
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
	HashItem hsh = Search.HashTable[pos.zobr.key0 & HASH_SIZE_end];
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

	Search.HashTable[pos.zobr.key0 & HASH_SIZE_end] = { (BYTE)Depth,(BYTE)Flag,hsh.vl,(WORD)mv,pos.zobr.key1,pos.zobr.key2 };
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

// 完整的alphabeta搜索
int WholeSearch(int alpha, int beta, int depth, bool no_null_cut) {
	// 变量意义：

	// 结点分值相关
	// 要记录到置换表的结点种类，临时记录当前局面分值，最佳alpha值
	int hash_type, vl, best_value;

	// 走法相关
	// 临时记录走法，最佳走法，要记录至置换表的走法
	int mv, best_mv, hash_mv;

	// 将军延伸 / 空步裁剪的深度更新
	int update_depth;

	// 走法排序
	SortMoves Sort;

    if (clock() - start_time + reserved_time >= time_limit)
        return pos.Evaluate();

	// 到达水平线，静态搜索
	if (depth <= 0) {
		return QuiescSearch(alpha, beta, 0);
	}

	// 检查重复局面 目的是防止长将
	vl = pos.IsRepetitive(); //  nRecur默认参数为1, 和“上一次不同”即可，若为3则是检测长将，可见isCheck的调用
	if (vl != 0) {
		return pos.RepeatValue(vl);
	}

	// 虽然是迭代加深，用尽时间即可，但我们仍然设置了极限深度，到达就返回局面评价
	if (pos.RootDistance == LIMIT_DEPTH) {
		return pos.Evaluate();
	}

	// 查看置换表，看以前是否走过，节约时间
	// 置换表中存储了当时走那个走法的一些重要的局面信息
	vl = ProbeHash(alpha, beta, depth, hash_mv);  // 注意！hash_mv 引用传参
	if (vl > -MATE_VALUE) {
		return vl; // 返回了一步杀棋
	}

	//  空着搜索, 先搜索“弃权”着法 
	//  因为无论是谁，走一步总会使情况变好，那么不走的情况下还能截断（情况好），那么走了也大概率截断
	//  故如果弃权着法高出边界，那么真正最好的着法也可能高出边界，就可以直接返回Beta而不要再去搜索了
	if (!no_null_cut && !pos.LastCheck() && pos.CanUselessMove()) {
		pos.UselessMove();
		// 参数这样写？ alpha = beta - 1; beta = beta, -1的作用和原理
		// 是为了缩小搜索窗口 加大截断的概率，因为我们只想知道“有没有截断”，而不是截断“好不好”
		vl = -WholeSearch(-beta, 1 - beta, depth - NULL_DEPTH - 1, NO_NULL);
		pos.UndoUselessMove();
		if (vl >= beta) {
			return vl;
		}
	}

	// 初始化最佳值和最佳走法
	hash_type = HASH_ALPHA;	  // 保存到置换表的时候需要注明是哪种走法
	best_mv = 0;			  // 判断=0就可以知道是否搜索到了Beta走法或PV走法，以便保存到历史表
	best_value = -MATE_VALUE; // 判断=-MATE_VALUE这样可以知道，是否一个走法都没走过(杀棋)
	           
	// 初始化走法排序
	Sort.Init(hash_mv);
	
	// 走走法，递归
	while ((mv = Sort.GetNextMv()) != 0) {  // 每次吐出下一个走法
		if (pos.MakeMove(mv)) {
			// 将军延伸 认为将军的走法接下来都很有戏
			update_depth = pos.LastCheck() ? depth : depth - 1;

			// PVS 搜索 具体原理见报告4.3
			if (best_value == -MATE_VALUE) {  // ! 根据best_value的初始值，如果进入这个if即是第一次WholeSearch
				vl = -WholeSearch(-beta, -alpha, update_depth); // ! 第一次还是得老实搜索
			}
			else {
				// 不是用常规的窗口(Alpha, Beta)，而是用(Alpha, Alpha + 1)来搜索。
				// 这样做的前提是，搜索必须返回小于或等于Alpha的值，
				vl = -WholeSearch(-alpha - 1, -alpha, update_depth);
				if (vl > alpha && vl < beta) { // ! 找到一个结点是PV结点
					// ! 主要变例搜索作了假设，如果你在搜索一个结点时找到一个PV着法，那么你就得到PV结点
					vl = -WholeSearch(-beta, -alpha, update_depth);
				}
			}
			pos.UndoMakeMove();

			// Alpha-Beta截断
			if (vl > best_value) {    
				best_value = vl;        
				if (vl >= beta) { 
					hash_type = HASH_BETA;
					best_mv = mv;     
					break;           
				}
				// ! 上下两个if只会进去一个
				if (vl > alpha) {
					hash_type = HASH_PV;
					best_mv = mv;      
					alpha = vl;     
				}
			}
		}
	}

	// 如果是杀棋，就根据杀棋步数给出评价
	if (best_value == -MATE_VALUE) {
		return pos.RootDistance - MATE_VALUE;
	}

	// 把最佳走法保存，返回最佳值
	// 记录到置换表
	RecordHash(hash_type, best_value, depth, best_mv);
	if (best_mv != 0) {
		// 如果不是Alpha走法，就将最佳走法保存到历史表
		// ! Alpha走法: 所有结点都比Alpha小，情况非常差/Alpha都没更新过, 则此时best_mv就会是初始化值0
		// ! 不是则不是alpha走法
		SetBestMove(best_mv, depth);
	}
	return best_value;
}

// 首步Alpha-Beta搜索
// 为什么要区分第一次（根节点）和其它结点？因为很多优化和启发对第一次搜索无效并且有反作用
static int FirstSearch(int depth) {
	// 临时变量记录局面分值，最好的走法，临时变量记录当前走法，更新深度
	int vl, best_value, mv, update_depth;
	SortMoves Sort;

	best_value = -MATE_VALUE;	// 初始化值，可用于判断

    if (clock() - start_time + reserved_time >= time_limit)
        return best_value;

	// 初始化走法表
	Sort.Init(Search.mvResult);

	// 循环走法
	while ((mv = Sort.GetNextMv()) != 0) {
		if (pos.MakeMove(mv)) {
			update_depth = pos.LastCheck() ? depth : depth - 1;

			// PVS搜索，原理见报告或WholeSearch
			if (best_value == -MATE_VALUE) {
				vl = -WholeSearch(-MATE_VALUE, MATE_VALUE, update_depth, NO_NULL);
			}
			else {
				vl = -WholeSearch(-best_value - 1, -best_value, update_depth);
				if (vl > best_value) {
					vl = -WholeSearch(-MATE_VALUE, -best_value, update_depth, NO_NULL);
				}
			}
			pos.UndoMakeMove();
			if (vl > best_value) {
				best_value = vl;
				Search.mvResult = mv;
				if (best_value > -WIN_VALUE && best_value < WIN_VALUE) {
					// 增加随机性 以加强稳定性
					best_value += (rand() & RANDOM_MASK) - (rand() & RANDOM_MASK);
				}
			}
		}
	}

	// 做好记录
	RecordHash(HASH_PV, best_value, depth, Search.mvResult);
	SetBestMove(Search.mvResult, depth);
	return best_value;
}

// 搜索的顶层调用
void TopSearch(clock_t limit) {
	int i, vl, n_mvs;
	int mvs[MAX_GEN_MOVES];

    time_limit = limit;

	// 初始化
	start_time = clock(); // 初始化定时器
	memset(Search.nHistoryTable, 0, 65536 * sizeof(int)); // 清空历史表
	memset(Search.mvKillers, 0, LIMIT_DEPTH * 2 * sizeof(int)); // 清空杀手走法表
	memset(Search.HashTable, 0, HASH_SIZE * sizeof(HashItem)); // 清空置换表
	pos.RootDistance = 0; // 初始步数

	// 开局库
	Search.mvResult = SearchBook();
#ifdef DEBUG
	printf("Openbook: %x\n", Search.mvResult);
#endif // DEBUG

	if (Search.mvResult != 0) {
		pos.MakeMove(Search.mvResult);
		if (pos.IsRepetitive(3) == 0) {
			pos.UndoMakeMove();
			return;
		}
		pos.UndoMakeMove();
	}

	// 检查是否只有唯一走法
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

	clock_t t_init = clock() - start_time;
	clock_t t_sum = t_init;
	int decay_factor = 1;
	// 迭代加深过程
	for (i = 1; i <= LIMIT_DEPTH; i++) {
		clock_t this_start = clock();
		vl = FirstSearch(i);

		// 搜索到杀棋，就终止搜索
		if (vl > WIN_VALUE || vl < -WIN_VALUE) {
			break; // ! 杀棋都是 nDistance -    VALUE_MATE
		}
		
		//衰减判断
		if (i < 10) {
			decay_factor = 5;
		}
		else if (i >= 10 && i < 20) {
			decay_factor = 4;
		}
		else if (i >= 20 && i < 30) {
			decay_factor = 3;
		}
		else {
			decay_factor = 2;
		}
		this_time = clock() - this_start;
		t_sum += this_time;
		// 退出搜索判断
		// -10是为了以防万一
		if (time_limit - 10 - t_sum < this_time * decay_factor) {
#ifdef DEBUG
			printf("time remain: %d\n", time_limit - 10 - t_sum);
			printf("last round t: %d\n", this_time);
#endif // DEBUG
			break;
		}
#ifdef DEBUG
		printf("this time: %d\n", this_time);
#endif // DEBUG
	}
#ifdef DEBUG
	printf("total time usage: %d\n", clock() - start_time);
#endif // DEBUG
}