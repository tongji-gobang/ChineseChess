
// 与搜索有关的全局变量
struct S{
	int mvResult;             // 电脑走的棋
	int nHistoryTable[65536]; // 历史表
};
extern S Search;
// 其他常数
const int MAX_GEN_MOVES = 128; // 最大的生成走法数
const int LIMIT_DEPTH = 32;    // 最大的搜索深度
const int MATE_VALUE = 10000;  // 最高分值，即将死的分值
const int WIN_VALUE = MATE_VALUE - 100; // 搜索出胜负的分值界限，超出此值就说明已经搜索出杀棋了
const int ADVANCED_VALUE = 3;  // 先行权分值

							   
							   // "qsort"按历史表排序的比较函数
static int CompareHistory(const void *lpmv1, const void *lpmv2);

// 超出边界(Fail-Soft)的Alpha-Beta搜索过程
static int SearchFull(int vlAlpha, int vlBeta, int nDepth);

// 迭代加深搜索过程
void SearchMain(void);


#pragma once
