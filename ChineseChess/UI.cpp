#include"UI.h"
#include"RESOURCE.H"
#include"ChessData.h"
#include"ChessBoard.h"
#include"Search.h"


UI Xqwl;

// TransparentBlt 的替代函数，用来修正原函数在 Windows 98 下资源泄漏的问题
void TransparentBlt2(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
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

// 绘制透明图片
void DrawTransBmp(HDC hdc, HDC hdcTmp, int xx, int yy, HBITMAP bmp) {
	SelectObject(hdcTmp, bmp);
	TransparentBlt2(hdc, xx, yy, SQUARE_SIZE, SQUARE_SIZE, hdcTmp, 0, 0, SQUARE_SIZE, SQUARE_SIZE, MASK_COLOR);
}

// 绘制棋盘
void DrawBoard(HDC hdc) {
	int x, y, xx, yy, sq, pc;
	HDC hdcTmp;

	// 画棋盘
	hdcTmp = CreateCompatibleDC(hdc);
	SelectObject(hdcTmp, Xqwl.bmpBoard);
	BitBlt(hdc, 0, 0, BOARD_WIDTH, BOARD_HEIGHT, hdcTmp, 0, 0, SRCCOPY);
	// 画棋子
	for (x = FILE_LEFT; x <= FILE_RIGHT; x++) {
		for (y = RANK_TOP; y <= RANK_BOTTOM; y++) {
			if (Xqwl.bFlipped) {
				xx = BOARD_EDGE + (MirrorCol(x) - FILE_LEFT) * SQUARE_SIZE;
				yy = BOARD_EDGE + (MirrorRow(y) - RANK_TOP) * SQUARE_SIZE;
			}
			else {
				xx = BOARD_EDGE + (x - FILE_LEFT) * SQUARE_SIZE;
				yy = BOARD_EDGE + (y - RANK_TOP) * SQUARE_SIZE;
			}
			sq = PositionIndex(x, y);
			pc = pos.Board[sq];
			if (pc != 0) {
				DrawTransBmp(hdc, hdcTmp, xx, yy, Xqwl.bmpPieces[pc]);
			}
			if (sq == Xqwl.sqSelected || sq == SrcPos(Xqwl.mvLast) || sq == DstPos(Xqwl.mvLast)) {
				DrawTransBmp(hdc, hdcTmp, xx, yy, Xqwl.bmpSelected);
			}
		}
	}
	DeleteDC(hdcTmp);
}

// 播放资源声音
void PlayResWav(int nResId) {
	PlaySound(MAKEINTRESOURCE(nResId), Xqwl.hInst, SND_ASYNC | SND_NOWAIT | SND_RESOURCE);
}

// 弹出不带声音的提示框
void MessageBoxMute(LPCSTR lpszText) {
	MSGBOXPARAMS mbp;
	mbp.cbSize = sizeof(MSGBOXPARAMS);
	mbp.hwndOwner = Xqwl.hWnd;
	mbp.hInstance = NULL;
	mbp.lpszText = lpszText;
	mbp.lpszCaption = "象棋AI";
	mbp.dwStyle = MB_USERICON;
    mbp.lpszIcon = MAKEINTRESOURCE(IDI_INFORMATION);
	mbp.dwContextHelpId = 0;
	mbp.lpfnMsgBoxCallback = NULL;
	mbp.dwLanguageId = 0;
	if (MessageBoxIndirect(&mbp) == 0) {
		// 系统图标在 Windows 98 下会失败，所以要使用应用程序图标
		mbp.hInstance = Xqwl.hInst;
		mbp.lpszIcon = MAKEINTRESOURCE(IDI_APPICON);
		MessageBoxIndirect(&mbp);
	}
}



// 绘制格子
void DrawSquare(int sq, BOOL bSelected = FALSE) {
	int sqFlipped, xx, yy, pc;

	sqFlipped = Xqwl.bFlipped ? CorrespondPos(sq) : sq;
	xx = BOARD_EDGE + (Column(sqFlipped) - FILE_LEFT) * SQUARE_SIZE;
	yy = BOARD_EDGE + (Row(sqFlipped) - RANK_TOP) * SQUARE_SIZE;
	SelectObject(Xqwl.hdcTmp, Xqwl.bmpBoard);
	BitBlt(Xqwl.hdc, xx, yy, SQUARE_SIZE, SQUARE_SIZE, Xqwl.hdcTmp, xx, yy, SRCCOPY);
	pc = pos.Board[sq];
	if (pc != 0) {
		DrawTransBmp(Xqwl.hdc, Xqwl.hdcTmp, xx, yy, Xqwl.bmpPieces[pc]);
	}
	if (bSelected) {
		DrawTransBmp(Xqwl.hdc, Xqwl.hdcTmp, xx, yy, Xqwl.bmpSelected);
	}
}

// 电脑回应一步棋
static void ResponseMove(void) {
    int vlRep;
    // 电脑走一步棋
    SetCursor((HCURSOR)LoadImage(NULL, IDC_WAIT, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
	TopSearch(1000);
    SetCursor((HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
    pos.MakeMove(Search.mvResult);
    // 清除上一步棋的选择标记
    DrawSquare(SrcPos(Xqwl.mvLast));
    DrawSquare(DstPos(Xqwl.mvLast));
    // 把电脑走的棋标记出来
    Xqwl.mvLast = Search.mvResult;
    DrawSquare(SrcPos(Xqwl.mvLast), DRAW_SELECTED);
    DrawSquare(DstPos(Xqwl.mvLast), DRAW_SELECTED);
    // 检查重复局面
    vlRep = pos.IsRepetitive(3);
    if (pos.IsMating()) {
        // 如果分出胜负，那么播放胜负的声音，并且弹出不带声音的提示框
        PlayResWav(IDR_LOSS);
        MessageBoxMute("请再接再厉！");
        Xqwl.bGameOver = TRUE;
    }
    else if (vlRep > 0) {
        vlRep = pos.RepeatValue(vlRep);
        // 注意："vlRep"是对玩家来说的分值
        PlayResWav(vlRep < -WIN_VALUE ? IDR_LOSS : vlRep > WIN_VALUE ? IDR_WIN : IDR_DRAW);
        MessageBoxMute(vlRep < -WIN_VALUE ? "长打作负，请不要气馁！" :
            vlRep > WIN_VALUE ? "电脑长打作负，祝贺你取得胜利！" : "双方不变作和，辛苦了！");
        Xqwl.bGameOver = TRUE;
    }
    else if (pos.MoveNum > 100) {
        PlayResWav(IDR_DRAW);
        MessageBoxMute("超过自然限着作和，辛苦了！");
        Xqwl.bGameOver = TRUE;
    }
    else {
        // 如果没有分出胜负，那么播放将军、吃子或一般走子的声音
        PlayResWav(pos.LastCheck() ? IDR_CHECK2 : pos.Capture() ? IDR_CAPTURE2 : IDR_MOVE2);
        if (pos.Capture()) {
            pos.InitAllMoves();
        }
    }
}

// 点击格子事件处理
static void ClickSquare(int sq) {
    int pc, mv, vlRep;
    Xqwl.hdc = GetDC(Xqwl.hWnd);
    Xqwl.hdcTmp = CreateCompatibleDC(Xqwl.hdc);
    sq = Xqwl.bFlipped ? CorrespondPos(sq) : sq;
    pc = pos.Board[sq];

    if ((pc & PieceFlag(pos.player)) != 0) {
        // 如果点击自己的子，那么直接选中该子
        if (Xqwl.sqSelected != 0) {
            DrawSquare(Xqwl.sqSelected);
        }
        Xqwl.sqSelected = sq;
        DrawSquare(sq, DRAW_SELECTED);
        if (Xqwl.mvLast != 0) {
            DrawSquare(SrcPos(Xqwl.mvLast));
            DrawSquare(DstPos(Xqwl.mvLast));
        }
        PlayResWav(IDR_CLICK); // 播放点击的声音

    }
    else if (Xqwl.sqSelected != 0 && !Xqwl.bGameOver) {
        // 如果点击的不是自己的子，但有子选中了(一定是自己的子)，那么走这个子
        mv = Move(Xqwl.sqSelected, sq);
        if (pos.LegalMove(mv)) {
            if (pos.MakeMove(mv)) {
                Xqwl.mvLast = mv;
                DrawSquare(Xqwl.sqSelected, DRAW_SELECTED);
                DrawSquare(sq, DRAW_SELECTED);
                Xqwl.sqSelected = 0;
                // 检查重复局面
                vlRep = pos.IsRepetitive(3);
                if (pos.IsMating()) {
                    // 如果分出胜负，那么播放胜负的声音，并且弹出不带声音的提示框
                    PlayResWav(IDR_WIN);
                    MessageBoxMute("祝贺你取得胜利！");
                    Xqwl.bGameOver = TRUE;
                }
                else if (vlRep > 0) {
                    vlRep = pos.RepeatValue(vlRep);
                    // 注意："vlRep"是对电脑来说的分值
                    PlayResWav(vlRep > WIN_VALUE ? IDR_LOSS : vlRep < -WIN_VALUE ? IDR_WIN : IDR_DRAW);
                    MessageBoxMute(vlRep > WIN_VALUE ? "长打作负，请不要气馁！" :
                        vlRep < -WIN_VALUE ? "电脑长打作负，祝贺你取得胜利！" : "双方不变作和，辛苦了！");
                    Xqwl.bGameOver = TRUE;
                }
                else if (pos.MoveNum > 100) {
                    PlayResWav(IDR_DRAW);
                    MessageBoxMute("超过自然限着作和，辛苦了！");
                    Xqwl.bGameOver = TRUE;
                }
                else {
                    // 如果没有分出胜负，那么播放将军、吃子或一般走子的声音
                    PlayResWav(pos.LastCheck() ? IDR_CHECK : pos.Capture() ? IDR_CAPTURE : IDR_MOVE);
                    if (pos.Capture()) {
                        pos.InitAllMoves();
                    }
                    ResponseMove(); // 轮到电脑走棋
                }
            }
            else {
                PlayResWav(IDR_ILLEGAL); // 播放被将军的声音
            }
        }
        // 如果根本就不符合走法(例如马不走日字)，那么程序不予理会
    }
    DeleteDC(Xqwl.hdcTmp);
    ReleaseDC(Xqwl.hWnd, Xqwl.hdc);
}

// 初始化棋局
void Startup(void) {
	pos.Startup();
	Xqwl.sqSelected = Xqwl.mvLast = 0;
}

// 窗体事件捕捉过程
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int x, y;
	HDC hdc;
	RECT rect;
	PAINTSTRUCT ps;
	MSGBOXPARAMS mbp;

	switch (uMsg) {
		// 新建窗口
	case WM_CREATE:
		// 调整窗口位置和尺寸
		GetWindowRect(hWnd, &rect);
		x = rect.left;
		y = rect.top;
		rect.right = rect.left + BOARD_WIDTH;
		rect.bottom = rect.top + BOARD_HEIGHT;
		AdjustWindowRect(&rect, WINDOW_STYLES, TRUE);
		MoveWindow(hWnd, x, y, rect.right - rect.left, rect.bottom - rect.top, TRUE);
		break;
		// 退出
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		// 菜单命令
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
			// 弹出对话框
			MessageBeep(MB_ICONINFORMATION);
			mbp.cbSize = sizeof(MSGBOXPARAMS);
			mbp.hwndOwner = hWnd;
			mbp.hInstance = Xqwl.hInst;
			mbp.lpszText = cszAbout;
			mbp.lpszCaption = "关于";
			mbp.dwStyle = MB_USERICON;
			mbp.lpszIcon = MAKEINTRESOURCE(IDI_APPICON);
			mbp.dwContextHelpId = 0;
			mbp.lpfnMsgBoxCallback = NULL;
			mbp.dwLanguageId = 0;
			MessageBoxIndirect(&mbp);
			break;
		}
		break;
		// 绘图
	case WM_PAINT:
		hdc = BeginPaint(Xqwl.hWnd, &ps);
		DrawBoard(hdc);
		EndPaint(Xqwl.hWnd, &ps);
		break;
		// 鼠标点击
	case WM_LBUTTONDOWN:
		x = FILE_LEFT + (LOWORD(lParam) - BOARD_EDGE) / SQUARE_SIZE;
		y = RANK_TOP + (HIWORD(lParam) - BOARD_EDGE) / SQUARE_SIZE;
		if (x >= FILE_LEFT && x <= FILE_RIGHT && y >= RANK_TOP && y <= RANK_BOTTOM) {
			ClickSquare(PositionIndex(x, y));
		}
		break;
		// 其他事件
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return FALSE;
}

// 装入资源图片
HBITMAP LoadResBmp(int nResId) {
	return (HBITMAP)LoadImage(Xqwl.hInst, MAKEINTRESOURCE(nResId), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
}
