/**
* XiangQi Wizard Light - A Very Simple Chinese Chess Program
* Designed by Morning Yellow, Version: 0.3, Last Modified: Mar. 2008
* Copyright (C) 2004-2008 www.xqbase.com
*
* ����С��ʦ 0.3 ��Ŀ�꣺
* һ��������λ�ü�ֵ��ʵ�־������ۺ�����
* �����ó����߽�(Fail-Soft)��Alpha-Beta�����õ������壻
* �����õ��������ʵ��ʱ����ƣ�
* �ġ�ʵ����ʷ���������Ż�Alpha-Beta������Ч�ʣ�
* �塢ʵ��ɱ�岽�����ۣ�����Ч����ɱ�塣
*/

#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "resource.h"

// �汾��
const LPCSTR cszAbout = "����С��ʦ 0.3\n����ٿ�ȫ�� ������Ʒ\n\n"
"(C) 2004-2008 www.xqbase.com\n����Ʒ����GNUͨ�ù������Э��\n\n"
"��ӭ��¼ www.xqbase.com\n�������PC�� ������ʦ";

// ���ںͻ�ͼ����
const int WINDOW_STYLES = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
const int MASK_COLOR = RGB(0, 255, 0);
const int SQUARE_SIZE = 56;
const int BOARD_EDGE = 8;
const int BOARD_WIDTH = BOARD_EDGE + SQUARE_SIZE * 9 + BOARD_EDGE;
const int BOARD_HEIGHT = BOARD_EDGE + SQUARE_SIZE * 10 + BOARD_EDGE;

// ���̷�Χ
const int RANK_TOP = 3;
const int RANK_BOTTOM = 12;
const int FILE_LEFT = 3;
const int FILE_RIGHT = 11;

// ���ӱ��
const int PIECE_KING = 0;
const int PIECE_ADVISOR = 1;
const int PIECE_BISHOP = 2;
const int PIECE_KNIGHT = 3;
const int PIECE_ROOK = 4;
const int PIECE_CANNON = 5;
const int PIECE_PAWN = 6;

// ��������
const int MAX_GEN_MOVES = 128; // ���������߷���
const int LIMIT_DEPTH = 32;    // �����������
const int MATE_VALUE = 10000;  // ��߷�ֵ���������ķ�ֵ
const int WIN_VALUE = MATE_VALUE - 100; // ������ʤ���ķ�ֵ���ޣ�������ֵ��˵���Ѿ�������ɱ����
const int ADVANCED_VALUE = 3;  // ����Ȩ��ֵ

							   // �ж������Ƿ��������е�����
static const char ccInBoard[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// �ж������Ƿ��ھŹ�������
static const char ccInFort[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// �жϲ����Ƿ�����ض��߷������飬1=˧(��)��2=��(ʿ)��3=��(��)
static const char ccLegalSpan[512] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0
};

// ���ݲ����ж����Ƿ����ȵ�����
static const char ccKnightPin[512] = {
	0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,-16,  0,-16,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0, 16,  0, 16,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0
};

// ˧(��)�Ĳ���
static const char ccKingDelta[4] = { -16, -1, 1, 16 };
// ��(ʿ)�Ĳ���
static const char ccAdvisorDelta[4] = { -17, -15, 15, 17 };
// ��Ĳ�������˧(��)�Ĳ�����Ϊ����
static const char ccKnightDelta[4][2] = { { -33, -31 },{ -18, 14 },{ -14, 18 },{ 31, 33 } };
// �������Ĳ���������(ʿ)�Ĳ�����Ϊ����
static const char ccKnightCheckDelta[4][2] = { { -33, -18 },{ -31, -14 },{ 14, 31 },{ 18, 33 } };

// ���̳�ʼ����
static const BYTE cucpcStartup[256] = {
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0, 20, 19, 18, 17, 16, 17, 18, 19, 20,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0, 21,  0,  0,  0,  0,  0, 21,  0,  0,  0,  0,  0,
	0,  0,  0, 22,  0, 22,  0, 22,  0, 22,  0, 22,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0, 14,  0, 14,  0, 14,  0, 14,  0, 14,  0,  0,  0,  0,
	0,  0,  0,  0, 13,  0,  0,  0,  0,  0, 13,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0, 12, 11, 10,  9,  8,  9, 10, 11, 12,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// ����λ�ü�ֵ��
static const BYTE cucvlPiecePos[7][256] = {
	{ // ˧(��)
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0, 11, 15, 11,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	},{ // ��(ʿ)
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0, 23,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	},{ // ��(��)
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0, 18,  0,  0,  0, 23,  0,  0,  0, 18,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	},{ // ��
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0, 90, 90, 90, 96, 90, 96, 90, 90, 90,  0,  0,  0,  0,
		0,  0,  0, 90, 96,103, 97, 94, 97,103, 96, 90,  0,  0,  0,  0,
		0,  0,  0, 92, 98, 99,103, 99,103, 99, 98, 92,  0,  0,  0,  0,
		0,  0,  0, 93,108,100,107,100,107,100,108, 93,  0,  0,  0,  0,
		0,  0,  0, 90,100, 99,103,104,103, 99,100, 90,  0,  0,  0,  0,
		0,  0,  0, 90, 98,101,102,103,102,101, 98, 90,  0,  0,  0,  0,
		0,  0,  0, 92, 94, 98, 95, 98, 95, 98, 94, 92,  0,  0,  0,  0,
		0,  0,  0, 93, 92, 94, 95, 92, 95, 94, 92, 93,  0,  0,  0,  0,
		0,  0,  0, 85, 90, 92, 93, 78, 93, 92, 90, 85,  0,  0,  0,  0,
		0,  0,  0, 88, 85, 90, 88, 90, 88, 90, 85, 88,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	},{ // ��
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,206,208,207,213,214,213,207,208,206,  0,  0,  0,  0,
		0,  0,  0,206,212,209,216,233,216,209,212,206,  0,  0,  0,  0,
		0,  0,  0,206,208,207,214,216,214,207,208,206,  0,  0,  0,  0,
		0,  0,  0,206,213,213,216,216,216,213,213,206,  0,  0,  0,  0,
		0,  0,  0,208,211,211,214,215,214,211,211,208,  0,  0,  0,  0,
		0,  0,  0,208,212,212,214,215,214,212,212,208,  0,  0,  0,  0,
		0,  0,  0,204,209,204,212,214,212,204,209,204,  0,  0,  0,  0,
		0,  0,  0,198,208,204,212,212,212,204,208,198,  0,  0,  0,  0,
		0,  0,  0,200,208,206,212,200,212,206,208,200,  0,  0,  0,  0,
		0,  0,  0,194,206,204,212,200,212,204,206,194,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	},{ // ��
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,100,100, 96, 91, 90, 91, 96,100,100,  0,  0,  0,  0,
		0,  0,  0, 98, 98, 96, 92, 89, 92, 96, 98, 98,  0,  0,  0,  0,
		0,  0,  0, 97, 97, 96, 91, 92, 91, 96, 97, 97,  0,  0,  0,  0,
		0,  0,  0, 96, 99, 99, 98,100, 98, 99, 99, 96,  0,  0,  0,  0,
		0,  0,  0, 96, 96, 96, 96,100, 96, 96, 96, 96,  0,  0,  0,  0,
		0,  0,  0, 95, 96, 99, 96,100, 96, 99, 96, 95,  0,  0,  0,  0,
		0,  0,  0, 96, 96, 96, 96, 96, 96, 96, 96, 96,  0,  0,  0,  0,
		0,  0,  0, 97, 96,100, 99,101, 99,100, 96, 97,  0,  0,  0,  0,
		0,  0,  0, 96, 97, 98, 98, 98, 98, 98, 97, 96,  0,  0,  0,  0,
		0,  0,  0, 96, 96, 97, 99, 99, 99, 97, 96, 96,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	},{ // ��(��)
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  9,  9,  9, 11, 13, 11,  9,  9,  9,  0,  0,  0,  0,
		0,  0,  0, 19, 24, 34, 42, 44, 42, 34, 24, 19,  0,  0,  0,  0,
		0,  0,  0, 19, 24, 32, 37, 37, 37, 32, 24, 19,  0,  0,  0,  0,
		0,  0,  0, 19, 23, 27, 29, 30, 29, 27, 23, 19,  0,  0,  0,  0,
		0,  0,  0, 14, 18, 20, 27, 29, 27, 20, 18, 14,  0,  0,  0,  0,
		0,  0,  0,  7,  0, 13,  0, 16,  0, 13,  0,  7,  0,  0,  0,  0,
		0,  0,  0,  7,  0,  7,  0, 15,  0,  7,  0,  7,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	}
};

// �ж������Ƿ���������
inline BOOL IN_BOARD(int sq) {
	return ccInBoard[sq] != 0;
}

// �ж������Ƿ��ھŹ���
inline BOOL IN_FORT(int sq) {
	return ccInFort[sq] != 0;
}

// ��ø��ӵĺ�����
inline int RANK_Y(int sq) {
	return sq >> 4;
}

// ��ø��ӵ�������
inline int FILE_X(int sq) {
	return sq & 15;
}

// ����������ͺ������ø���
inline int COORD_XY(int x, int y) {
	return x + (y << 4);
}

// ��ת����
inline int SQUARE_FLIP(int sq) {
	return 254 - sq;
}

// ������ˮƽ����
inline int FILE_FLIP(int x) {
	return 14 - x;
}

// �����괹ֱ����
inline int RANK_FLIP(int y) {
	return 15 - y;
}

// ����ˮƽ����
inline int MIRROR_SQUARE(int sq) {
	return COORD_XY(FILE_FLIP(FILE_X(sq)), RANK_Y(sq));
}

// ����ˮƽ����
inline int SQUARE_FORWARD(int sq, int sd) {
	return sq - 16 + (sd << 5);
}

// �߷��Ƿ����˧(��)�Ĳ���
inline BOOL KING_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 1;
}

// �߷��Ƿ������(ʿ)�Ĳ���
inline BOOL ADVISOR_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 2;
}

// �߷��Ƿ������(��)�Ĳ���
inline BOOL BISHOP_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 3;
}

// ��(��)�۵�λ��
inline int BISHOP_PIN(int sqSrc, int sqDst) {
	return (sqSrc + sqDst) >> 1;
}

// ���ȵ�λ��
inline int KNIGHT_PIN(int sqSrc, int sqDst) {
	return sqSrc + ccKnightPin[sqDst - sqSrc + 256];
}

// �Ƿ�δ����
inline BOOL HOME_HALF(int sq, int sd) {
	return (sq & 0x80) != (sd << 7);
}

// �Ƿ��ѹ���
inline BOOL AWAY_HALF(int sq, int sd) {
	return (sq & 0x80) == (sd << 7);
}

// �Ƿ��ںӵ�ͬһ��
inline BOOL SAME_HALF(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0x80) == 0;
}

// �Ƿ���ͬһ��
inline BOOL SAME_RANK(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0xf0) == 0;
}

// �Ƿ���ͬһ��
inline BOOL SAME_FILE(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0x0f) == 0;
}

// ��ú�ڱ��(������8��������16)
inline int SIDE_TAG(int sd) {
	return 8 + (sd << 3);
}

// ��öԷ���ڱ��
inline int OPP_SIDE_TAG(int sd) {
	return 16 - (sd << 3);
}

// ����߷������
inline int SRC(int mv) {
	return mv & 255;
}

// ����߷����յ�
inline int DST(int mv) {
	return mv >> 8;
}

// ���������յ����߷�
inline int MOVE(int sqSrc, int sqDst) {
	return sqSrc + sqDst * 256;
}

// �߷�ˮƽ����
inline int MIRROR_MOVE(int mv) {
	return MOVE(MIRROR_SQUARE(SRC(mv)), MIRROR_SQUARE(DST(mv)));
}

// ����ṹ
struct PositionStruct {
	int sdPlayer;                   // �ֵ�˭�ߣ�0=�췽��1=�ڷ�
	BYTE ucpcSquares[256];          // �����ϵ�����
	int vlWhite, vlBlack;           // �졢��˫����������ֵ
	int nDistance;                  // ������ڵ�Ĳ���

	void Startup(void);             // ��ʼ������
	void ChangeSide(void) {         // �������ӷ�
		sdPlayer = 1 - sdPlayer;
	}
	void AddPiece(int sq, int pc) { // �������Ϸ�һö����
		ucpcSquares[sq] = pc;
		// �췽�ӷ֣��ڷ�(ע��"cucvlPiecePos"ȡֵҪ�ߵ�)����
		if (pc < 16) {
			vlWhite += cucvlPiecePos[pc - 8][sq];
		}
		else {
			vlBlack += cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
		}
	}
	void DelPiece(int sq, int pc) { // ������������һö����
		ucpcSquares[sq] = 0;
		// �췽���֣��ڷ�(ע��"cucvlPiecePos"ȡֵҪ�ߵ�)�ӷ�
		if (pc < 16) {
			vlWhite -= cucvlPiecePos[pc - 8][sq];
		}
		else {
			vlBlack -= cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
		}
	}
	int Evaluate(void) const {      // �������ۺ���
		return (sdPlayer == 0 ? vlWhite - vlBlack : vlBlack - vlWhite) + ADVANCED_VALUE;
	}
	int MovePiece(int mv);                      // ��һ���������
	void UndoMovePiece(int mv, int pcCaptured); // ������һ���������
	BOOL MakeMove(int mv, int &pcCaptured);     // ��һ����
	void UndoMakeMove(int mv, int pcCaptured) { // ������һ����
		nDistance--;
		ChangeSide();
		UndoMovePiece(mv, pcCaptured);
	}
	int GenerateMoves(int *mvs) const;          // ���������߷�
	BOOL LegalMove(int mv) const;               // �ж��߷��Ƿ����
	BOOL Checked(void) const;                   // �ж��Ƿ񱻽���
	BOOL IsMate(void);                          // �ж��Ƿ�ɱ
};

// ��ʼ������
void PositionStruct::Startup(void) {
	int sq, pc;
	sdPlayer = vlWhite = vlBlack = nDistance = 0;
	memset(ucpcSquares, 0, 256);
	for (sq = 0; sq < 256; sq++) {
		pc = cucpcStartup[sq];
		if (pc != 0) {
			AddPiece(sq, pc);
		}
	}
}

// ��һ���������
int PositionStruct::MovePiece(int mv) {
	int sqSrc, sqDst, pc, pcCaptured;
	sqSrc = SRC(mv);
	sqDst = DST(mv);
	pcCaptured = ucpcSquares[sqDst];
	if (pcCaptured != 0) {
		DelPiece(sqDst, pcCaptured);
	}
	pc = ucpcSquares[sqSrc];
	DelPiece(sqSrc, pc);
	AddPiece(sqDst, pc);
	return pcCaptured;
}

// ������һ���������
void PositionStruct::UndoMovePiece(int mv, int pcCaptured) {
	int sqSrc, sqDst, pc;
	sqSrc = SRC(mv);
	sqDst = DST(mv);
	pc = ucpcSquares[sqDst];
	DelPiece(sqDst, pc);
	AddPiece(sqSrc, pc);
	if (pcCaptured != 0) {
		AddPiece(sqDst, pcCaptured);
	}
}

// ��һ����
BOOL PositionStruct::MakeMove(int mv, int &pcCaptured) {
	pcCaptured = MovePiece(mv);
	if (Checked()) {
		UndoMovePiece(mv, pcCaptured);
		return FALSE;
	}
	ChangeSide();
	nDistance++;
	return TRUE;
}

// ���������߷�
int PositionStruct::GenerateMoves(int *mvs) const {
	int i, j, nGenMoves, nDelta, sqSrc, sqDst;
	int pcSelfSide, pcOppSide, pcSrc, pcDst;
	// ���������߷�����Ҫ�������¼������裺

	nGenMoves = 0;
	pcSelfSide = SIDE_TAG(sdPlayer);
	pcOppSide = OPP_SIDE_TAG(sdPlayer);
	for (sqSrc = 0; sqSrc < 256; sqSrc++) {

		// 1. �ҵ�һ���������ӣ����������жϣ�
		pcSrc = ucpcSquares[sqSrc];
		if ((pcSrc & pcSelfSide) == 0) {
			continue;
		}

		// 2. ��������ȷ���߷�
		switch (pcSrc - pcSelfSide) {
		case PIECE_KING:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccKingDelta[i];
				if (!IN_FORT(sqDst)) {
					continue;
				}
				pcDst = ucpcSquares[sqDst];
				if ((pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					nGenMoves++;
				}
			}
			break;
		case PIECE_ADVISOR:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccAdvisorDelta[i];
				if (!IN_FORT(sqDst)) {
					continue;
				}
				pcDst = ucpcSquares[sqDst];
				if ((pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					nGenMoves++;
				}
			}
			break;
		case PIECE_BISHOP:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccAdvisorDelta[i];
				if (!(IN_BOARD(sqDst) && HOME_HALF(sqDst, sdPlayer) && ucpcSquares[sqDst] == 0)) {
					continue;
				}
				sqDst += ccAdvisorDelta[i];
				pcDst = ucpcSquares[sqDst];
				if ((pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					nGenMoves++;
				}
			}
			break;
		case PIECE_KNIGHT:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccKingDelta[i];
				if (ucpcSquares[sqDst] != 0) {
					continue;
				}
				for (j = 0; j < 2; j++) {
					sqDst = sqSrc + ccKnightDelta[i][j];
					if (!IN_BOARD(sqDst)) {
						continue;
					}
					pcDst = ucpcSquares[sqDst];
					if ((pcDst & pcSelfSide) == 0) {
						mvs[nGenMoves] = MOVE(sqSrc, sqDst);
						nGenMoves++;
					}
				}
			}
			break;
		case PIECE_ROOK:
			for (i = 0; i < 4; i++) {
				nDelta = ccKingDelta[i];
				sqDst = sqSrc + nDelta;
				while (IN_BOARD(sqDst)) {
					pcDst = ucpcSquares[sqDst];
					if (pcDst == 0) {
						mvs[nGenMoves] = MOVE(sqSrc, sqDst);
						nGenMoves++;
					}
					else {
						if ((pcDst & pcOppSide) != 0) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
							nGenMoves++;
						}
						break;
					}
					sqDst += nDelta;
				}
			}
			break;
		case PIECE_CANNON:
			for (i = 0; i < 4; i++) {
				nDelta = ccKingDelta[i];
				sqDst = sqSrc + nDelta;
				while (IN_BOARD(sqDst)) {
					pcDst = ucpcSquares[sqDst];
					if (pcDst == 0) {
						mvs[nGenMoves] = MOVE(sqSrc, sqDst);
						nGenMoves++;
					}
					else {
						break;
					}
					sqDst += nDelta;
				}
				sqDst += nDelta;
				while (IN_BOARD(sqDst)) {
					pcDst = ucpcSquares[sqDst];
					if (pcDst != 0) {
						if ((pcDst & pcOppSide) != 0) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
							nGenMoves++;
						}
						break;
					}
					sqDst += nDelta;
				}
			}
			break;
		case PIECE_PAWN:
			sqDst = SQUARE_FORWARD(sqSrc, sdPlayer);
			if (IN_BOARD(sqDst)) {
				pcDst = ucpcSquares[sqDst];
				if ((pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					nGenMoves++;
				}
			}
			if (AWAY_HALF(sqSrc, sdPlayer)) {
				for (nDelta = -1; nDelta <= 1; nDelta += 2) {
					sqDst = sqSrc + nDelta;
					if (IN_BOARD(sqDst)) {
						pcDst = ucpcSquares[sqDst];
						if ((pcDst & pcSelfSide) == 0) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
							nGenMoves++;
						}
					}
				}
			}
			break;
		}
	}
	return nGenMoves;
}

// �ж��߷��Ƿ����
BOOL PositionStruct::LegalMove(int mv) const {
	int sqSrc, sqDst, sqPin;
	int pcSelfSide, pcSrc, pcDst, nDelta;
	// �ж��߷��Ƿ�Ϸ�����Ҫ�������µ��жϹ��̣�

	// 1. �ж���ʼ���Ƿ����Լ�������
	sqSrc = SRC(mv);
	pcSrc = ucpcSquares[sqSrc];
	pcSelfSide = SIDE_TAG(sdPlayer);
	if ((pcSrc & pcSelfSide) == 0) {
		return FALSE;
	}

	// 2. �ж�Ŀ����Ƿ����Լ�������
	sqDst = DST(mv);
	pcDst = ucpcSquares[sqDst];
	if ((pcDst & pcSelfSide) != 0) {
		return FALSE;
	}

	// 3. �������ӵ����ͼ���߷��Ƿ����
	switch (pcSrc - pcSelfSide) {
	case PIECE_KING:
		return IN_FORT(sqDst) && KING_SPAN(sqSrc, sqDst);
	case PIECE_ADVISOR:
		return IN_FORT(sqDst) && ADVISOR_SPAN(sqSrc, sqDst);
	case PIECE_BISHOP:
		return SAME_HALF(sqSrc, sqDst) && BISHOP_SPAN(sqSrc, sqDst) &&
			ucpcSquares[BISHOP_PIN(sqSrc, sqDst)] == 0;
	case PIECE_KNIGHT:
		sqPin = KNIGHT_PIN(sqSrc, sqDst);
		return sqPin != sqSrc && ucpcSquares[sqPin] == 0;
	case PIECE_ROOK:
	case PIECE_CANNON:
		if (SAME_RANK(sqSrc, sqDst)) {
			nDelta = (sqDst < sqSrc ? -1 : 1);
		}
		else if (SAME_FILE(sqSrc, sqDst)) {
			nDelta = (sqDst < sqSrc ? -16 : 16);
		}
		else {
			return FALSE;
		}
		sqPin = sqSrc + nDelta;
		while (sqPin != sqDst && ucpcSquares[sqPin] == 0) {
			sqPin += nDelta;
		}
		if (sqPin == sqDst) {
			return pcDst == 0 || pcSrc - pcSelfSide == PIECE_ROOK;
		}
		else if (pcDst != 0 && pcSrc - pcSelfSide == PIECE_CANNON) {
			sqPin += nDelta;
			while (sqPin != sqDst && ucpcSquares[sqPin] == 0) {
				sqPin += nDelta;
			}
			return sqPin == sqDst;
		}
		else {
			return FALSE;
		}
	case PIECE_PAWN:
		if (AWAY_HALF(sqDst, sdPlayer) && (sqDst == sqSrc - 1 || sqDst == sqSrc + 1)) {
			return TRUE;
		}
		return sqDst == SQUARE_FORWARD(sqSrc, sdPlayer);
	default:
		return FALSE;
	}
}

// �ж��Ƿ񱻽���
BOOL PositionStruct::Checked() const {
	int i, j, sqSrc, sqDst;
	int pcSelfSide, pcOppSide, pcDst, nDelta;
	pcSelfSide = SIDE_TAG(sdPlayer);
	pcOppSide = OPP_SIDE_TAG(sdPlayer);
	// �ҵ������ϵ�˧(��)�����������жϣ�

	for (sqSrc = 0; sqSrc < 256; sqSrc++) {
		if (ucpcSquares[sqSrc] != pcSelfSide + PIECE_KING) {
			continue;
		}

		// 1. �ж��Ƿ񱻶Է��ı�(��)����
		if (ucpcSquares[SQUARE_FORWARD(sqSrc, sdPlayer)] == pcOppSide + PIECE_PAWN) {
			return TRUE;
		}
		for (nDelta = -1; nDelta <= 1; nDelta += 2) {
			if (ucpcSquares[sqSrc + nDelta] == pcOppSide + PIECE_PAWN) {
				return TRUE;
			}
		}

		// 2. �ж��Ƿ񱻶Է�������(����(ʿ)�Ĳ�����������)
		for (i = 0; i < 4; i++) {
			if (ucpcSquares[sqSrc + ccAdvisorDelta[i]] != 0) {
				continue;
			}
			for (j = 0; j < 2; j++) {
				pcDst = ucpcSquares[sqSrc + ccKnightCheckDelta[i][j]];
				if (pcDst == pcOppSide + PIECE_KNIGHT) {
					return TRUE;
				}
			}
		}

		// 3. �ж��Ƿ񱻶Է��ĳ����ڽ���(������˧����)
		for (i = 0; i < 4; i++) {
			nDelta = ccKingDelta[i];
			sqDst = sqSrc + nDelta;
			while (IN_BOARD(sqDst)) {
				pcDst = ucpcSquares[sqDst];
				if (pcDst != 0) {
					if (pcDst == pcOppSide + PIECE_ROOK || pcDst == pcOppSide + PIECE_KING) {
						return TRUE;
					}
					break;
				}
				sqDst += nDelta;
			}
			sqDst += nDelta;
			while (IN_BOARD(sqDst)) {
				int pcDst = ucpcSquares[sqDst];
				if (pcDst != 0) {
					if (pcDst == pcOppSide + PIECE_CANNON) {
						return TRUE;
					}
					break;
				}
				sqDst += nDelta;
			}
		}
		return FALSE;
	}
	return FALSE;
}

// �ж��Ƿ�ɱ
BOOL PositionStruct::IsMate(void) {
	int i, nGenMoveNum, pcCaptured;
	int mvs[MAX_GEN_MOVES];

	nGenMoveNum = GenerateMoves(mvs);
	for (i = 0; i < nGenMoveNum; i++) {
		pcCaptured = MovePiece(mvs[i]);
		if (!Checked()) {
			UndoMovePiece(mvs[i], pcCaptured);
			return FALSE;
		}
		else {
			UndoMovePiece(mvs[i], pcCaptured);
		}
	}
	return TRUE;
}

static PositionStruct pos; // ����ʵ��

						   // ��ͼ�ν����йص�ȫ�ֱ���
static struct {
	HINSTANCE hInst;                              // Ӧ�ó�����ʵ��
	HWND hWnd;                                    // �����ھ��
	HDC hdc, hdcTmp;                              // �豸�����ֻ��"ClickSquare"��������Ч
	HBITMAP bmpBoard, bmpSelected, bmpPieces[24]; // ��ԴͼƬ���
	int sqSelected, mvLast;                       // ѡ�еĸ��ӣ���һ����
	BOOL bFlipped;                                // �Ƿ�ת����
} Xqwl;

// �������йص�ȫ�ֱ���
static struct {
	int mvResult;             // �����ߵ���
	int nHistoryTable[65536]; // ��ʷ��
} Search;

// "qsort"����ʷ������ıȽϺ���
static int CompareHistory(const void *lpmv1, const void *lpmv2) {
	return Search.nHistoryTable[*(int *)lpmv2] - Search.nHistoryTable[*(int *)lpmv1];
}

// �����߽�(Fail-Soft)��Alpha-Beta��������
static int SearchFull(int vlAlpha, int vlBeta, int nDepth) {
	int i, nGenMoves, pcCaptured;
	int vl, vlBest, mvBest;
	int mvs[MAX_GEN_MOVES];
	// һ��Alpha-Beta��ȫ������Ϊ���¼����׶�

	// 1. ����ˮƽ�ߣ��򷵻ؾ�������ֵ
	if (nDepth == 0) {
		return pos.Evaluate();
	}

	// 2. ��ʼ�����ֵ������߷�
	vlBest = -MATE_VALUE; // ��������֪�����Ƿ�һ���߷���û�߹�(ɱ��)
	mvBest = 0;           // ��������֪�����Ƿ���������Beta�߷���PV�߷����Ա㱣�浽��ʷ��

						  // 3. ����ȫ���߷�����������ʷ������
	nGenMoves = pos.GenerateMoves(mvs);
	qsort(mvs, nGenMoves, sizeof(int), CompareHistory);

	// 4. ��һ����Щ�߷��������еݹ�
	for (i = 0; i < nGenMoves; i++) {
		if (pos.MakeMove(mvs[i], pcCaptured)) {
			vl = -SearchFull(-vlBeta, -vlAlpha, nDepth - 1);
			pos.UndoMakeMove(mvs[i], pcCaptured);

			// 5. ����Alpha-Beta��С�жϺͽض�
			if (vl > vlBest) {    // �ҵ����ֵ(������ȷ����Alpha��PV����Beta�߷�)
				vlBest = vl;        // "vlBest"����ĿǰҪ���ص����ֵ�����ܳ���Alpha-Beta�߽�
				if (vl >= vlBeta) { // �ҵ�һ��Beta�߷�
					mvBest = mvs[i];  // Beta�߷�Ҫ���浽��ʷ��
					break;            // Beta�ض�
				}
				if (vl > vlAlpha) { // �ҵ�һ��PV�߷�
					mvBest = mvs[i];  // PV�߷�Ҫ���浽��ʷ��
					vlAlpha = vl;     // ��СAlpha-Beta�߽�
				}
			}
		}
	}

	// 5. �����߷����������ˣ�������߷�(������Alpha�߷�)���浽��ʷ���������ֵ
	if (vlBest == -MATE_VALUE) {
		// �����ɱ�壬�͸���ɱ�岽����������
		return pos.nDistance - MATE_VALUE;
	}
	if (mvBest != 0) {
		// �������Alpha�߷����ͽ�����߷����浽��ʷ��
		Search.nHistoryTable[mvBest] += nDepth * nDepth;
		if (pos.nDistance == 0) {
			// �������ڵ�ʱ��������һ������߷�(��Ϊȫ�����������ᳬ���߽�)��������߷���������
			Search.mvResult = mvBest;
		}
	}
	return vlBest;
}

// ����������������
static void SearchMain(void) {
	int i, t, vl;

	// ��ʼ��
	memset(Search.nHistoryTable, 0, 65536 * sizeof(int)); // �����ʷ��
	t = clock();       // ��ʼ����ʱ��
	pos.nDistance = 0; // ��ʼ����

					   // �����������
	for (i = 1; i <= LIMIT_DEPTH; i++) {
		vl = SearchFull(-MATE_VALUE, MATE_VALUE, i);
		// ������ɱ�壬����ֹ����
		if (vl > WIN_VALUE || vl < -WIN_VALUE) {
			break;
		}
		// ����һ�룬����ֹ����
		if (clock() - t > CLOCKS_PER_SEC) {
			break;
		}
	}
}

// TransparentBlt �������������������ԭ������ Windows 98 ����Դй©������
static void TransparentBlt2(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
	HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, UINT crTransparent) {
	HDC hImageDC, hMaskDC;
	HBITMAP hOldImageBMP, hImageBMP, hOldMaskBMP, hMaskBMP;

	hImageBMP = CreateCompatibleBitmap(hdcDest, nWidthDest, nHeightDest);
	hMaskBMP = CreateBitmap(nWidthDest, nHeightDest, 1, 1, NULL);
	hImageDC = CreateCompatibleDC(hdcDest);
	hMaskDC = CreateCompatibleDC(hdcDest);
	hOldImageBMP = (HBITMAP)SelectObject(hImageDC, hImageBMP);
	hOldMaskBMP = (HBITMAP)SelectObject(hMaskDC, hMaskBMP);

	if (nWidthDest == nWidthSrc && nHeightDest == nHeightSrc) {
		BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest,
			hdcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY);
	}
	else {
		StretchBlt(hImageDC, 0, 0, nWidthDest, nHeightDest,
			hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, SRCCOPY);
	}
	SetBkColor(hImageDC, crTransparent);
	BitBlt(hMaskDC, 0, 0, nWidthDest, nHeightDest, hImageDC, 0, 0, SRCCOPY);
	SetBkColor(hImageDC, RGB(0, 0, 0));
	SetTextColor(hImageDC, RGB(255, 255, 255));
	BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest, hMaskDC, 0, 0, SRCAND);
	SetBkColor(hdcDest, RGB(255, 255, 255));
	SetTextColor(hdcDest, RGB(0, 0, 0));
	BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
		hMaskDC, 0, 0, SRCAND);
	BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
		hImageDC, 0, 0, SRCPAINT);

	SelectObject(hImageDC, hOldImageBMP);
	DeleteDC(hImageDC);
	SelectObject(hMaskDC, hOldMaskBMP);
	DeleteDC(hMaskDC);
	DeleteObject(hImageBMP);
	DeleteObject(hMaskBMP);
}

// ����͸��ͼƬ
inline void DrawTransBmp(HDC hdc, HDC hdcTmp, int xx, int yy, HBITMAP bmp) {
	SelectObject(hdcTmp, bmp);
	TransparentBlt2(hdc, xx, yy, SQUARE_SIZE, SQUARE_SIZE, hdcTmp, 0, 0, SQUARE_SIZE, SQUARE_SIZE, MASK_COLOR);
}

// ��������
static void DrawBoard(HDC hdc) {
	int x, y, xx, yy, sq, pc;
	HDC hdcTmp;

	// ������
	hdcTmp = CreateCompatibleDC(hdc);
	SelectObject(hdcTmp, Xqwl.bmpBoard);
	BitBlt(hdc, 0, 0, BOARD_WIDTH, BOARD_HEIGHT, hdcTmp, 0, 0, SRCCOPY);
	// ������
	for (x = FILE_LEFT; x <= FILE_RIGHT; x++) {
		for (y = RANK_TOP; y <= RANK_BOTTOM; y++) {
			if (Xqwl.bFlipped) {
				xx = BOARD_EDGE + (FILE_FLIP(x) - FILE_LEFT) * SQUARE_SIZE;
				yy = BOARD_EDGE + (RANK_FLIP(y) - RANK_TOP) * SQUARE_SIZE;
			}
			else {
				xx = BOARD_EDGE + (x - FILE_LEFT) * SQUARE_SIZE;
				yy = BOARD_EDGE + (y - RANK_TOP) * SQUARE_SIZE;
			}
			sq = COORD_XY(x, y);
			pc = pos.ucpcSquares[sq];
			if (pc != 0) {
				DrawTransBmp(hdc, hdcTmp, xx, yy, Xqwl.bmpPieces[pc]);
			}
			if (sq == Xqwl.sqSelected || sq == SRC(Xqwl.mvLast) || sq == DST(Xqwl.mvLast)) {
				DrawTransBmp(hdc, hdcTmp, xx, yy, Xqwl.bmpSelected);
			}
		}
	}
	DeleteDC(hdcTmp);
}

// ������Դ����
inline void PlayResWav(int nResId) {
	PlaySound(MAKEINTRESOURCE(nResId), Xqwl.hInst, SND_ASYNC | SND_NOWAIT | SND_RESOURCE);
}

// ����������������ʾ��
static void MessageBoxMute(LPCSTR lpszText) {
	MSGBOXPARAMS mbp;
	mbp.cbSize = sizeof(MSGBOXPARAMS);
	mbp.hwndOwner = Xqwl.hWnd;
	mbp.hInstance = NULL;
	mbp.lpszText = lpszText;
	mbp.lpszCaption = "����С��ʦ";
	mbp.dwStyle = MB_USERICON;
	mbp.lpszIcon = MAKEINTRESOURCE(IDI_INFORMATION);
	mbp.dwContextHelpId = 0;
	mbp.lpfnMsgBoxCallback = NULL;
	mbp.dwLanguageId = 0;
	if (MessageBoxIndirect(&mbp) == 0) {
		// ϵͳͼ���� Windows 98 �»�ʧ�ܣ�����Ҫʹ��Ӧ�ó���ͼ��
		mbp.hInstance = Xqwl.hInst;
		mbp.lpszIcon = MAKEINTRESOURCE(IDI_APPICON);
		MessageBoxIndirect(&mbp);
	}
}

// "DrawSquare"����
const BOOL DRAW_SELECTED = TRUE;

// ���Ƹ���
static void DrawSquare(int sq, BOOL bSelected = FALSE) {
	int sqFlipped, xx, yy, pc;

	sqFlipped = Xqwl.bFlipped ? SQUARE_FLIP(sq) : sq;
	xx = BOARD_EDGE + (FILE_X(sqFlipped) - FILE_LEFT) * SQUARE_SIZE;
	yy = BOARD_EDGE + (RANK_Y(sqFlipped) - RANK_TOP) * SQUARE_SIZE;
	SelectObject(Xqwl.hdcTmp, Xqwl.bmpBoard);
	BitBlt(Xqwl.hdc, xx, yy, SQUARE_SIZE, SQUARE_SIZE, Xqwl.hdcTmp, xx, yy, SRCCOPY);
	pc = pos.ucpcSquares[sq];
	if (pc != 0) {
		DrawTransBmp(Xqwl.hdc, Xqwl.hdcTmp, xx, yy, Xqwl.bmpPieces[pc]);
	}
	if (bSelected) {
		DrawTransBmp(Xqwl.hdc, Xqwl.hdcTmp, xx, yy, Xqwl.bmpSelected);
	}
}

// ���Ի�Ӧһ����
static void ResponseMove(void) {
	int pcCaptured;
	// ������һ����
	SetCursor((HCURSOR)LoadImage(NULL, IDC_WAIT, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
	SearchMain();
	SetCursor((HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
	pos.MakeMove(Search.mvResult, pcCaptured);
	// �����һ�����ѡ����
	DrawSquare(SRC(Xqwl.mvLast));
	DrawSquare(DST(Xqwl.mvLast));
	// �ѵ����ߵ����ǳ���
	Xqwl.mvLast = Search.mvResult;
	DrawSquare(SRC(Xqwl.mvLast), DRAW_SELECTED);
	DrawSquare(DST(Xqwl.mvLast), DRAW_SELECTED);
	if (pos.IsMate()) {
		// ����ֳ�ʤ������ô����ʤ�������������ҵ���������������ʾ��
		PlayResWav(IDR_LOSS);
		MessageBoxMute("���ٽ�������");
	}
	else {
		// ���û�зֳ�ʤ������ô���Ž��������ӻ�һ�����ӵ�����
		PlayResWav(pos.Checked() ? IDR_CHECK2 : pcCaptured != 0 ? IDR_CAPTURE2 : IDR_MOVE2);
	}
}

// ��������¼�����
static void ClickSquare(int sq) {
	int pc, mv;
	Xqwl.hdc = GetDC(Xqwl.hWnd);
	Xqwl.hdcTmp = CreateCompatibleDC(Xqwl.hdc);
	sq = Xqwl.bFlipped ? SQUARE_FLIP(sq) : sq;
	pc = pos.ucpcSquares[sq];

	if ((pc & SIDE_TAG(pos.sdPlayer)) != 0) {
		// �������Լ����ӣ���ôֱ��ѡ�и���
		if (Xqwl.sqSelected != 0) {
			DrawSquare(Xqwl.sqSelected);
		}
		Xqwl.sqSelected = sq;
		DrawSquare(sq, DRAW_SELECTED);
		if (Xqwl.mvLast != 0) {
			DrawSquare(SRC(Xqwl.mvLast));
			DrawSquare(DST(Xqwl.mvLast));
		}
		PlayResWav(IDR_CLICK); // ���ŵ��������

	}
	else if (Xqwl.sqSelected != 0) {
		// �������Ĳ����Լ����ӣ�������ѡ����(һ�����Լ�����)����ô�������
		mv = MOVE(Xqwl.sqSelected, sq);
		if (pos.LegalMove(mv)) {
			if (pos.MakeMove(mv, pc)) {
				Xqwl.mvLast = mv;
				DrawSquare(Xqwl.sqSelected, DRAW_SELECTED);
				DrawSquare(sq, DRAW_SELECTED);
				Xqwl.sqSelected = 0;
				if (pos.IsMate()) {
					// ����ֳ�ʤ������ô����ʤ�������������ҵ���������������ʾ��
					PlayResWav(IDR_WIN);
					MessageBoxMute("ף����ȡ��ʤ����");
				}
				else {
					// ���û�зֳ�ʤ������ô���Ž��������ӻ�һ�����ӵ�����
					PlayResWav(pos.Checked() ? IDR_CHECK : pc != 0 ? IDR_CAPTURE : IDR_MOVE);
					ResponseMove(); // �ֵ���������
				}
			}
			else {
				PlayResWav(IDR_ILLEGAL); // ���ű�����������
			}
		}
		// ��������Ͳ������߷�(������������)����ô���������
	}
	DeleteDC(Xqwl.hdcTmp);
	ReleaseDC(Xqwl.hWnd, Xqwl.hdc);
}

// ��ʼ�����
static void Startup(void) {
	pos.Startup();
	Xqwl.sqSelected = Xqwl.mvLast = 0;
}

// �����¼���׽����
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int x, y;
	HDC hdc;
	RECT rect;
	PAINTSTRUCT ps;
	MSGBOXPARAMS mbp;

	switch (uMsg) {
		// �½�����
	case WM_CREATE:
		// ��������λ�úͳߴ�
		GetWindowRect(hWnd, &rect);
		x = rect.left;
		y = rect.top;
		rect.right = rect.left + BOARD_WIDTH;
		rect.bottom = rect.top + BOARD_HEIGHT;
		AdjustWindowRect(&rect, WINDOW_STYLES, TRUE);
		MoveWindow(hWnd, x, y, rect.right - rect.left, rect.bottom - rect.top, TRUE);
		break;
		// �˳�
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		// �˵�����
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDM_FILE_RED:
		case IDM_FILE_BLACK:
			Xqwl.bFlipped = (LOWORD(wParam) == IDM_FILE_BLACK);
			Startup();
			hdc = GetDC(Xqwl.hWnd);
			DrawBoard(hdc);
			if (Xqwl.bFlipped) {
				Xqwl.hdc = hdc;
				Xqwl.hdcTmp = CreateCompatibleDC(Xqwl.hdc);
				ResponseMove();
				DeleteDC(Xqwl.hdcTmp);
			}
			ReleaseDC(Xqwl.hWnd, hdc);
			break;
		case IDM_FILE_EXIT:
			DestroyWindow(Xqwl.hWnd);
			break;
		case IDM_HELP_HOME:
			ShellExecute(NULL, NULL, "http://www.xqbase.com/", NULL, NULL, SW_SHOWNORMAL);
			break;
		case IDM_HELP_ABOUT:
			// ����������С��ʦͼ��ĶԻ���
			MessageBeep(MB_ICONINFORMATION);
			mbp.cbSize = sizeof(MSGBOXPARAMS);
			mbp.hwndOwner = hWnd;
			mbp.hInstance = Xqwl.hInst;
			mbp.lpszText = cszAbout;
			mbp.lpszCaption = "��������С��ʦ";
			mbp.dwStyle = MB_USERICON;
			mbp.lpszIcon = MAKEINTRESOURCE(IDI_APPICON);
			mbp.dwContextHelpId = 0;
			mbp.lpfnMsgBoxCallback = NULL;
			mbp.dwLanguageId = 0;
			MessageBoxIndirect(&mbp);
			break;
		}
		break;
		// ��ͼ
	case WM_PAINT:
		hdc = BeginPaint(Xqwl.hWnd, &ps);
		DrawBoard(hdc);
		EndPaint(Xqwl.hWnd, &ps);
		break;
		// �����
	case WM_LBUTTONDOWN:
		x = FILE_LEFT + (LOWORD(lParam) - BOARD_EDGE) / SQUARE_SIZE;
		y = RANK_TOP + (HIWORD(lParam) - BOARD_EDGE) / SQUARE_SIZE;
		if (x >= FILE_LEFT && x <= FILE_RIGHT && y >= RANK_TOP && y <= RANK_BOTTOM) {
			ClickSquare(COORD_XY(x, y));
		}
		break;
		// �����¼�
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return FALSE;
}

// װ����ԴͼƬ
inline HBITMAP LoadResBmp(int nResId) {
	return (HBITMAP)LoadImage(Xqwl.hInst, MAKEINTRESOURCE(nResId), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
}

// ��ڹ���
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int i;
	MSG msg;
	WNDCLASSEX wce;

	// ��ʼ��ȫ�ֱ���
	Xqwl.hInst = hInstance;
	Xqwl.bFlipped = FALSE;
	Startup();

	// װ��ͼƬ
	Xqwl.bmpBoard = LoadResBmp(IDB_BOARD);
	Xqwl.bmpSelected = LoadResBmp(IDB_SELECTED);
	for (i = PIECE_KING; i <= PIECE_PAWN; i++) {
		Xqwl.bmpPieces[SIDE_TAG(0) + i] = LoadResBmp(IDB_RK + i);
		Xqwl.bmpPieces[SIDE_TAG(1) + i] = LoadResBmp(IDB_BK + i);
	}

	// ���ô���
	wce.cbSize = sizeof(WNDCLASSEX);
	wce.style = 0;
	wce.lpfnWndProc = (WNDPROC)WndProc;
	wce.cbClsExtra = wce.cbWndExtra = 0;
	wce.hInstance = hInstance;
	wce.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, LR_SHARED);
	wce.hCursor = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wce.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wce.lpszMenuName = MAKEINTRESOURCE(IDM_MAINMENU);
	wce.lpszClassName = "XQWLIGHT";
	wce.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, LR_SHARED);
	RegisterClassEx(&wce);

	// �򿪴���
	Xqwl.hWnd = CreateWindow("XQWLIGHT", "����С��ʦ", WINDOW_STYLES,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if (Xqwl.hWnd == NULL) {
		return 0;
	}
	ShowWindow(Xqwl.hWnd, nCmdShow);
	UpdateWindow(Xqwl.hWnd);

	// ������Ϣ
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}