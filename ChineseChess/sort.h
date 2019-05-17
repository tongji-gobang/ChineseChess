#pragma once
// 走法排序结构
#include"ChessData.h"
#include"ChessBoard.h"
#include"Search.h"


//此函数构造了一个状态机使得生成走法的时候能够最先生成最优走法
//状态机的状态转移如下
//取置换表走法-->取第一个杀手走法-->取第二个杀手走法-->生成该局面下所有走法并根据历史表排序-->逐一取排序后的走法

const int PHASE_HASH = 0;			//置换表走法状态
const int PHASE_KILLER_1 = 1;		//杀手走法一状态
const int PHASE_KILLER_2 = 2;		//杀手走法二状态
const int PHASE_GEN_MOVES = 3;		//生成局面下所有走法并排序
const int PHASE_REST = 4;			//逐一取排序后的走法

struct SortMoves {
	int hash_mv;// 置换表走法
	int killer_mv1, killer_mv2; //两个杀手走法
	int which_phrase, idx, n_mvs;    // 当前阶段，当前采用第几个走法，总共有几个走法
	int mvs[MAX_GEN_MOVES];           // 所有的走法

	void Init(int mvHash_);// 初始化，设定置换表走法和两个杀手走法
	int GetNextMv(void); // 得到下一个走法

};






