#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	void readRegmark(char *regmark);  //����д��������

#ifdef __cplusplus
}
#endif
// ��ͼ�ν����йص�ȫ�ֱ���
#include"RESOURCE.H"

struct UI{
	HINSTANCE hInst;                              // Ӧ�ó�����ʵ��
	HWND hWnd;                                    // �����ھ��
	HDC hdc, hdcTmp;                              // �豸�����ֻ��"ClickSquare"��������Ч
	HBITMAP bmpBoard, bmpSelected, bmpPieces[24]; // ��ԴͼƬ���
	int sqSelected, mvLast;                       // ѡ�еĸ��ӣ���һ����
	BOOL bFlipped, bGameOver;                                // �Ƿ�ת����, ��Ϸ�Ƿ����
};
extern UI Xqwl;

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

// "DrawSquare"����
const BOOL DRAW_SELECTED = TRUE;

// TransparentBlt �������������������ԭ������ Windows 98 ����Դй©������
void TransparentBlt2(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
	HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, UINT crTransparent);

// ����͸��ͼƬ
void DrawTransBmp(HDC hdc, HDC hdcTmp, int xx, int yy, HBITMAP bmp);

// ��������
 void DrawBoard(HDC hdc);

// ������Դ����
void PlayResWav(int nResId);

// ����������������ʾ��
void MessageBoxMute(LPCSTR lpszText);

// ���Ƹ���
void DrawSquare(int sq, BOOL bSelected );

// ���Ի�Ӧһ����
void ResponseMove(void);

// ��������¼�����
void ClickSquare(int sq);

// ��ʼ�����
void Startup(void);

// �����¼���׽����
 LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// װ����ԴͼƬ
HBITMAP LoadResBmp(int nResId);