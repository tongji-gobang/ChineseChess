#pragma once
#include <Windows.h>


const int BOOK_SIZE = 16384;   // ���ֿ��С

// ���ֿ���ṹ
// �����롢�߷�������ֵ
struct BookItem {
	DWORD dwLock;
	WORD wmv, wvl;
};


void LoadBook();			//װ�ؿ��ֿ�
int SearchBook();			//�������ֿ�