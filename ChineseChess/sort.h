#pragma once
// 走法排序结构
#include"ChessData.h"
#include"ChessBoard.h"
#include"Search.h"
const int PHASE_HASH = 0;
const int PHASE_KILLER_1 = 1;
const int PHASE_KILLER_2 = 2;
const int PHASE_GEN_MOVES = 3;
const int PHASE_REST = 4;

struct SortStruct {
	int mvHash, mvKiller1, mvKiller2; // 置换表走法和两个杀手走法
	int nPhase, nIndex, nGenMoves;    // 当前阶段，当前采用第几个走法，总共有几个走法
	int mvs[MAX_GEN_MOVES];           // 所有的走法

	void Init(int mvHash_);// 初始化，设定置换表走法和两个杀手走法
	int Next(void); // 得到下一个走法

};


struct Sort
{
	int 
};




