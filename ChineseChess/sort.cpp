#include"sort.h"

/*
void SortStruct::Init(int mvHash_) { // 初始化，设定置换表走法和两个杀手走法
	mvHash = mvHash_;
	mvKiller1 = Search.mvKillers[pos.nDistance][0];
	mvKiller2 = Search.mvKillers[pos.nDistance][1];
	nPhase = PHASE_HASH;
}

// 得到下一个走法
int SortStruct::Next(void) {
	int mv;
	switch (nPhase) {
		// "nPhase"表示着法启发的若干阶段，依次为：

		// 0. 置换表着法启发，完成后立即进入下一阶段；
	case PHASE_HASH:
		nPhase = PHASE_KILLER_1;
		if (mvHash != 0) {
			return mvHash;
		}
		// 技巧：这里没有"break"，表示"switch"的上一个"case"执行完后紧接着做下一个"case"，下同

		// 1. 杀手着法启发(第一个杀手着法)，完成后立即进入下一阶段；
	case PHASE_KILLER_1:
		nPhase = PHASE_KILLER_2;
		if (mvKiller1 != mvHash && mvKiller1 != 0 && pos.LegalMove(mvKiller1)) {
			return mvKiller1;
		}

		// 2. 杀手着法启发(第二个杀手着法)，完成后立即进入下一阶段；
	case PHASE_KILLER_2:
		nPhase = PHASE_GEN_MOVES;
		if (mvKiller2 != mvHash && mvKiller2 != 0 && pos.LegalMove(mvKiller2)) {
			return mvKiller2;
		}

		// 3. 生成所有着法，完成后立即进入下一阶段；
	case PHASE_GEN_MOVES:
		nPhase = PHASE_REST;
		nGenMoves = pos.GenerateMoves(mvs);
		qsort(mvs, nGenMoves, sizeof(int), CompareHistory);
		nIndex = 0;

		// 4. 对剩余着法做历史表启发；
	case PHASE_REST:
		while (nIndex < nGenMoves) {
			mv = mvs[nIndex];
			nIndex++;
			if (mv != mvHash && mv != mvKiller1 && mv != mvKiller2) {
				return mv;
			}
		}

		// 5. 没有着法了，返回零。
	default:
		return 0;
	}
}
*/


//历史表比较函数
int compare(const void* a, const void*b) {
	return Search.nHistoryTable[*(int *)b] - Search.nHistoryTable[*(int *)a];
}


void SortStruct::Init(int mvHash_) {
	this->mvHash = mvHash_;		//初始化哈希走法
	this->mvKiller1 = Search.mvKillers[pos.RootDistance][0]; //从杀手表获取兄弟节点的杀手走法
	this->mvKiller2 = Search.mvKillers[pos.RootDistance][1];
	this->nPhase = PHASE_HASH;				//将最开始状态设置为获取哈希走法
}

//获取下一个走法
int SortStruct::Next() {
	int mv;

	//若状态为取散列表走法
	if (nPhase == PHASE_HASH) {
		nPhase = PHASE_KILLER_1;	//将状态改为杀手走法1
		if (mvHash)					//如果散列表走法可行 则返回该走法
			return mvHash;
	}

	//若状态为取杀手走法1
	if (nPhase == PHASE_KILLER_1) {
		nPhase = PHASE_KILLER_2;										//将状态改为杀手走法2

		if (mvKiller1&&mvKiller1!=mvHash&& pos.LegalMove(mvKiller1))	//判断杀手1走法是否为0且与散列表走法不同
																		//且为合法走法
			return mvKiller1;											//返回杀手1走法
	}


	//若状态为取杀手走法1
	if (nPhase == PHASE_KILLER_2) {
		nPhase = PHASE_GEN_MOVES;										//将状态改为生成所有走法状态

		if (mvKiller2&&mvKiller2 != mvHash&& pos.LegalMove(mvKiller2))	//判断杀手2走法是否为0且与散列表走法不同
																		//且为合法走法
			return mvKiller2;											//返回杀手2走法
	}


	//若状态为生成所有走法
	if (nPhase == PHASE_GEN_MOVES) {
		nPhase = PHASE_REST;											//将状态更新为逐一获取走法状态

		this->nGenMoves = pos.GenerateMoves(mvs);						//生成此局面所有走法
		qsort(mvs, nGenMoves, sizeof(int), compare);					//对这些走法进行历史表排序
		this->nIndex = 0;												//将采用走法的下标置0
	}


	//若状态为逐一获取走法状态
	if (nPhase == PHASE_REST) {
		while(nIndex < nGenMoves) {										//若采用走法下标小于生成的总走法数
			mv = mvs[nIndex];
			nIndex++; 											//将采用走法的下标加一
			if(mv != mvHash && mv!=mvKiller1 && mv !=mvKiller2)//若此走法不是散列表走法或杀手走法
				return mv;										//返回此走法
			
		}
	}

	return 0;
}