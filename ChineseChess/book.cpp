
#include "book.h"
#include "Search.h"
#include "ChessBoard.h"
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#pragma comment(lib, "winmm")

HINSTANCE hInst; // 句柄全局
// 装入开局库
void LoadBook() {
	HRSRC hrsrc;
	
	// 修改句柄，原来是结构体，现在不需要了
	hrsrc = FindResource(hInst, "BOOK_DATA", RT_RCDATA);					//资源加载！！？无法确定非windows环境下是否可用
	Search.BookSize = SizeofResource(hInst, hrsrc) / sizeof(BookItem);
	if (Search.BookSize > BOOK_SIZE) {
		Search.BookSize = BOOK_SIZE;
	}
	memcpy(Search.BookTable, LockResource(LoadResource(hInst, hrsrc)),
		Search.BookSize * sizeof(BookItem));
}

int CompareBook(const void *lpbk1, const void *lpbk2) {
	DWORD dw1, dw2;
	dw1 = ((BookItem *)lpbk1)->dwLock;
	dw2 = ((BookItem *)lpbk2)->dwLock;
	return dw1 > dw2 ? 1 : dw1 < dw2 ? -1 : 0;
}

// 搜索开局库
int SearchBook() {
	int i, vl, nBookMoves, mv;
	int mvs[MAX_GEN_MOVES], vls[MAX_GEN_MOVES];
	bool isMirror;
	BookItem bkToSearch, *bk;
	CurrentBoard posMirror;
	// 搜索开局库的过程有以下几个步骤

	// 1. 如果没有开局库，则立即返回
	if (Search.BookSize == 0) {
		
		return 0;
	}
	// 2. 搜索当前局面
	isMirror = FALSE;				//从非镜像局面开始寻找
	bkToSearch.dwLock = pos.zobr.key2;
	bk = (BookItem *)bsearch(&bkToSearch, Search.BookTable, Search.BookSize, sizeof(BookItem), CompareBook);	//对开局库进行二分搜索
	// 3. 如果没有找到，那么搜索当前局面的镜像局面
	if (bk == NULL) {
		isMirror = TRUE;
		pos.Mirror(posMirror);
		bkToSearch.dwLock = posMirror.zobr.key2;
		bk = (BookItem *)bsearch(&bkToSearch, Search.BookTable, Search.BookSize, sizeof(BookItem), CompareBook);
	}
	// 4. 如果镜像局面也没找到，则立即返回
	if (bk == NULL) {
		return 0;
	}
	// 5. 如果找到，则向前查第一个开局库项
	while (bk >= Search.BookTable && bk->dwLock == bkToSearch.dwLock) {
		--bk;
	}
	++bk;
	// 6. 把走法和分值写入到"mvs"和"vls"数组中
	vl = nBookMoves = 0;
	while (bk < Search.BookTable + Search.BookSize && bk->dwLock == bkToSearch.dwLock) {
		mv = (isMirror ? MirrorMove(bk->wmv) : bk->wmv);
		if (pos.LegalMove(mv)) {
			mvs[nBookMoves] = mv;
			vls[nBookMoves] = bk->wvl;
			vl += vls[nBookMoves];
			nBookMoves++;
			if (nBookMoves == MAX_GEN_MOVES) {
				break; // 防止"BOOK.DAT"中含有异常数据
			}
		}
		++bk;
	}
	if (vl == 0) {
		return 0; // 防止"BOOK.DAT"中含有异常数据
	}
	// 7. 根据权重随机选择一个走法
	vl = rand() % vl;
	for (i = 0; i < nBookMoves; ++i) {
		vl -= vls[i];
		if (vl < 0) {			//确定随机数所落在区间对应的操作i
			break;
		}
	}
	return mvs[i];
}
