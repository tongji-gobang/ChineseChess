
// 与搜索有关的全局变量
static struct {
	int mvResult;             // 电脑走的棋
	int nHistoryTable[65536]; // 历史表
} Search;
#pragma once
