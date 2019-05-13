#pragma once
#include <Windows.h>


const int BOOK_SIZE = 16384;   // 开局库大小

// 开局库项结构
// 检验码、走法、评估值
struct BookItem {
	DWORD dwLock;
	WORD wmv, wvl;
};


void LoadBook();			//装载开局库
int SearchBook();			//搜索开局库