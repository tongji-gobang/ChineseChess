#include"sort.h"


//历史表比较函数
struct compare {
    bool operator()(const int& a, const int& b) const
    {
        return Search.nHistoryTable[b] < Search.nHistoryTable[a];
    }
};

void SortMoves::Init(int hash_mv_) {
	this->hash_mv = hash_mv_;		//初始化哈希走法
	this->killer_mv1 = Search.mvKillers[pos.RootDistance][0]; //从杀手表获取兄弟节点的杀手走法
	this->killer_mv2 = Search.mvKillers[pos.RootDistance][1];
	this->which_phrase = PHASE_HASH;				//将最开始状态设置为获取哈希走法
}

//获取下一个走法
int SortMoves::GetNextMv() {
	int mv;

	//若状态为取散列表走法
	if (which_phrase == PHASE_HASH) {
		which_phrase = PHASE_KILLER_1;	//将状态改为杀手走法1
		if (hash_mv)					//如果散列表走法可行 则返回该走法
			return hash_mv;
	}

	// 杀手着法启发(Killer Heuristic)是基于这样一个思想，搜索某个结点时首先尝试着法a1，
	// 由a1的后续着法b1产生截断，回到原来的结点时再搜索a1的兄弟结点a2时，
	// 如果b1仍旧是a2的后续着法，那么b1很有可能也会产生截断。
	// 大多数程序会为每层分配2个杀手着法，并采用先进先出的方式管理
	// 1. 杀手着法启发(第一个杀手着法)，完成后立即进入下一阶段；

	//若状态为取杀手走法1
	if (which_phrase == PHASE_KILLER_1) {
		which_phrase = PHASE_KILLER_2;										//将状态改为杀手走法2

		if (killer_mv1&&killer_mv1 != hash_mv&& pos.LegalMove(killer_mv1))	//判断杀手1走法是否为0且与散列表走法不同
																		//且为合法走法
			return killer_mv1;											//返回杀手1走法
	}


	//若状态为取杀手走法1
	if (which_phrase == PHASE_KILLER_2) {
		which_phrase = PHASE_GEN_MOVES;										//将状态改为生成所有走法状态

		if (killer_mv2&&killer_mv2 != hash_mv&& pos.LegalMove(killer_mv2))	//判断杀手2走法是否为0且与散列表走法不同
																		//且为合法走法
			return killer_mv2;											//返回杀手2走法
	}

	// “历史表启发”(History Heuristic)是杀手着法启发的扩展，
	// 历史表记录的是整个搜索树中着法的好坏。历史表的思想是：
	// 搜索树中某个结点上的一个好的着法，对于其他结点可能也是好的。
	// 没有什么非常可靠的理由来支持这个思想，但根据历史表来排序着法，
	// 总比不排序要好得多，而且实践证明这是一种效果非常好的启发算法，几乎每个程序都用到。

	//若状态为生成所有走法
	if (which_phrase == PHASE_GEN_MOVES) {
		which_phrase = PHASE_REST;											//将状态更新为逐一获取走法状态

		this->n_mvs = pos.GenerateMoves(mvs);						//生成此局面所有走法
		std::sort(std::begin(mvs), std::begin(mvs) + n_mvs, compare());					//对这些走法进行历史表排序
		this->idx = 0;												//将采用走法的下标置0
	}


	//若状态为逐一获取走法状态
	if (which_phrase == PHASE_REST) {
		while(idx < n_mvs) {										//若采用走法下标小于生成的总走法数
			mv = mvs[idx];
			idx++; 											//将采用走法的下标加一
			if(mv != hash_mv && mv!= killer_mv1 && mv != killer_mv2)//若此走法不是散列表走法或杀手走法
				return mv;										//返回此走法
			
		}
	}

	return 0;
}