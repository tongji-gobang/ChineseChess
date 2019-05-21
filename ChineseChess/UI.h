#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	void readRegmark(char *regmark);  //这里写函数声明

#ifdef __cplusplus
}
#endif
// 与图形界面有关的全局变量
#include"RESOURCE.H"

struct UI{
	HINSTANCE hInst;                              // 应用程序句柄实例
	HWND hWnd;                                    // 主窗口句柄
	HDC hdc, hdcTmp;                              // 设备句柄，只在"ClickSquare"过程中有效
	HBITMAP bmpBoard, bmpSelected, bmpPieces[24]; // 资源图片句柄
	int sqSelected, mvLast;                       // 选中的格子，上一步棋
	BOOL bFlipped, bGameOver;                                // 是否翻转棋盘, 游戏是否结束
};
extern UI Xqwl;
const LPCSTR cszAbout = "人工智能原理作业\n"
"组长：	刘鲲\n"
"组员：	沈琪\n"
"	张钇文\n"
"	高鹏\n"
"	杨君临\n";
// 窗口和绘图属性
const int WINDOW_STYLES = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
const int MASK_COLOR = RGB(0, 255, 0);
const int SQUARE_SIZE = 56;
const int BOARD_EDGE = 8;
const int BOARD_WIDTH = BOARD_EDGE + SQUARE_SIZE * 9 + BOARD_EDGE;
const int BOARD_HEIGHT = BOARD_EDGE + SQUARE_SIZE * 10 + BOARD_EDGE;

// "DrawSquare"参数
const BOOL DRAW_SELECTED = TRUE;

// TransparentBlt 的替代函数，用来修正原函数在 Windows 98 下资源泄漏的问题
void TransparentBlt2(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
	HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, UINT crTransparent);

// 绘制透明图片
void DrawTransBmp(HDC hdc, HDC hdcTmp, int xx, int yy, HBITMAP bmp);

// 绘制棋盘
 void DrawBoard(HDC hdc);

// 播放资源声音
void PlayResWav(int nResId);

// 弹出不带声音的提示框
void MessageBoxMute(LPCSTR lpszText);

// 绘制格子
void DrawSquare(int sq, BOOL bSelected );

// 电脑回应一步棋
void ResponseMove(void);

// 点击格子事件处理
void ClickSquare(int sq);

// 初始化棋局
void Startup(void);

// 窗体事件捕捉过程
 LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 装入资源图片
HBITMAP LoadResBmp(int nResId);