
#pragma once
//#inlcude "BOOKitem"
#include <Windows.h>
const int MAX_GEN_MOVES = 128; // 最大的生成走法数
const int MAX_MOVES = 256;     // 最大的历史走法数
const int LIMIT_DEPTH = 64;    // 最大的搜索深度
const int MATE_VALUE = 10000;  // 最高分值，即将死的分值
const int BAN_VALUE = MATE_VALUE - 100; // 长将判负的分值，低于该值将不写入置换表
const int WIN_VALUE = MATE_VALUE - 200; // 搜索出胜负的分值界限，超出此值就说明已经搜索出杀棋了
const int DRAW_VALUE = 20;     // 和棋时返回的分数(取负值)
const int ADVANCED_VALUE = 3;  // 先行权分值
const int RANDOM_MASK = 7;     // 随机性分值
const int NULL_MARGIN = 400;   // 空步裁剪的子力边界
const int NULL_DEPTH = 2;      // 空步裁剪的裁剪深度
const int HASH_SIZE = 1 << 20; // 置换表大小
const int HASH_ALPHA = 1;      // ALPHA节点的置换表项
const int HASH_BETA = 2;       // BETA节点的置换表项
const int HASH_PV = 3;         // PV节点的置换表项

							   // 增加的
const int HASH_SIZE_end = HASH_SIZE - 1;		// 20位的1
#define HistoryTable_SIZE 65536
const int MATE_VALUE_neg = -MATE_VALUE;

// 置换表项结构
/*
以Zobrist键值为指标: ( 键值%散列表项数 )  --> 局面。 由于很多局面都有可能跟散列表中同一项作用，因此散列项需要包含校验值(dwlock0,dwlock1)
深度、标志、
评分、移动
校验值
*/
struct HashItem {
	BYTE Depth, Flag;
	short vl;
	WORD mv;
	DWORD Lock0, Lock1;
};
int ProbeHash(int vl_Alpha, int vl_Beta, int Depth, int &mv); // 读取置换表项
void RecordHash(int Flag, int vl, int Depth, int mv);   //保存置换表项


// 与搜索有关的全局变量
struct S{
	int mvResult;             // 电脑走的棋
	int nHistoryTable[HistoryTable_SIZE]; // 历史表
	int mvKillers[LIMIT_DEPTH][2]; // 杀手走法表
	HashItem HashTable[HASH_SIZE]; // 置换表
								   //int BookSize;                 // 开局库大小
								   //BookItem BookTable[BOOK_SIZE]; // 开局库 (暂时不实现开局库相关)

};
extern S Search;
// 其他常数


							   
 // "qsort"按历史表排序的比较函数
int CompareHistory(const void *lpmv1, const void *lpmv2);

// 超出边界(Fail-Soft)的Alpha-Beta搜索过程
int SearchFull(int vlalpha, int vlbeta, int nDepth, bool NoNull = FALSE);

// 迭代加深搜索过程
void SearchMain(clock_t time_limit);

// "GenerateMoves"参数，是否只产生吃子着法
const bool GEN_CAPTURE = true;

// "SearchFull"的参数，空着裁剪开关
const bool NO_NULL = true;

// MVV/LVA每种子力的价值
static BYTE cucMvvLva[24] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	5, 1, 1, 3, 4, 3, 2, 0,
	5, 1, 1, 3, 4, 3, 2, 0
};

// 求MVV/LVA值
int MvvLva(int mv);

// "qsort"按MVV/LVA值排序的比较函数
int CompareMvvLva(const void *p1, const void *p2);

// "qsort"按历史表排序的比较函数
static int CompareHistory(const void *p1, const void *p2);

// 静态搜索
int SearchQuiesc(int alpha, int beta);


