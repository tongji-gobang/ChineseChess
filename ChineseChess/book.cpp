
#include "book.h"
#include "Search.h"
#include "ChessBoard.h"
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#pragma comment(lib, "winmm")

HINSTANCE hInst; // ���ȫ��
// װ�뿪�ֿ�
void LoadBook() {
	HRSRC hrsrc;
	
	// �޸ľ����ԭ���ǽṹ�壬���ڲ���Ҫ��
	hrsrc = FindResource(hInst, "BOOK_DATA", RT_RCDATA);					//��Դ���أ������޷�ȷ����windows�������Ƿ����
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

// �������ֿ�
int SearchBook() {
	int i, vl, nBookMoves, mv;
	int mvs[MAX_GEN_MOVES], vls[MAX_GEN_MOVES];
	bool isMirror;
	BookItem bkToSearch, *bk;
	CurrentBoard posMirror;
	// �������ֿ�Ĺ��������¼�������

	// 1. ���û�п��ֿ⣬����������
	if (Search.BookSize == 0) {
		
		return 0;
	}
	// 2. ������ǰ����
	isMirror = FALSE;				//�ӷǾ�����濪ʼѰ��
	bkToSearch.dwLock = pos.zobr.key2;
	bk = (BookItem *)bsearch(&bkToSearch, Search.BookTable, Search.BookSize, sizeof(BookItem), CompareBook);	//�Կ��ֿ���ж�������
	// 3. ���û���ҵ�����ô������ǰ����ľ������
	if (bk == NULL) {
		isMirror = TRUE;
		pos.Mirror(posMirror);
		bkToSearch.dwLock = posMirror.zobr.key2;
		bk = (BookItem *)bsearch(&bkToSearch, Search.BookTable, Search.BookSize, sizeof(BookItem), CompareBook);
	}
	// 4. ����������Ҳû�ҵ�������������
	if (bk == NULL) {
		return 0;
	}
	// 5. ����ҵ�������ǰ���һ�����ֿ���
	while (bk >= Search.BookTable && bk->dwLock == bkToSearch.dwLock) {
		--bk;
	}
	++bk;
	// 6. ���߷��ͷ�ֵд�뵽"mvs"��"vls"������
	vl = nBookMoves = 0;
	while (bk < Search.BookTable + Search.BookSize && bk->dwLock == bkToSearch.dwLock) {
		mv = (isMirror ? MirrorMove(bk->wmv) : bk->wmv);
		if (pos.LegalMove(mv)) {
			mvs[nBookMoves] = mv;
			vls[nBookMoves] = bk->wvl;
			vl += vls[nBookMoves];
			nBookMoves++;
			if (nBookMoves == MAX_GEN_MOVES) {
				break; // ��ֹ"BOOK.DAT"�к����쳣����
			}
		}
		++bk;
	}
	if (vl == 0) {
		return 0; // ��ֹ"BOOK.DAT"�к����쳣����
	}
	// 7. ����Ȩ�����ѡ��һ���߷�
	vl = rand() % vl;
	for (i = 0; i < nBookMoves; ++i) {
		vl -= vls[i];
		if (vl < 0) {			//ȷ������������������Ӧ�Ĳ���i
			break;
		}
	}
	return mvs[i];
}
