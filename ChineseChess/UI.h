#pragma once
// ��ͼ�ν����йص�ȫ�ֱ���
#include"RESOURCE.H"
static struct {
	HINSTANCE hInst;                              // Ӧ�ó�����ʵ��
	HWND hWnd;                                    // �����ھ��
	HDC hdc, hdcTmp;                              // �豸�����ֻ��"ClickSquare"��������Ч
	HBITMAP bmpBoard, bmpSelected, bmpPieces[24]; // ��ԴͼƬ���
	int sqSelected, mvLast;                       // ѡ�еĸ��ӣ���һ����
	BOOL bFlipped;                                // �Ƿ�ת����
};