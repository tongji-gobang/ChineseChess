#include"UI.h"
#include"RESOURCE.H"
#include"ChessData.h"
#include"ChessBoard.h"
#include"Search.h"


UI Xqwl;

// TransparentBlt �������������������ԭ������ Windows 98 ����Դй©������
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

// ����͸��ͼƬ
void DrawTransBmp(HDC hdc, HDC hdcTmp, int xx, int yy, HBITMAP bmp) {
	SelectObject(hdcTmp, bmp);
	TransparentBlt2(hdc, xx, yy, SQUARE_SIZE, SQUARE_SIZE, hdcTmp, 0, 0, SQUARE_SIZE, SQUARE_SIZE, MASK_COLOR);
}

// ��������
void DrawBoard(HDC hdc) {
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

// ������Դ����
void PlayResWav(int nResId) {
	PlaySound(MAKEINTRESOURCE(nResId), Xqwl.hInst, SND_ASYNC | SND_NOWAIT | SND_RESOURCE);
}

// ����������������ʾ��
void MessageBoxMute(LPCSTR lpszText) {
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



// ���Ƹ���
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

// ���Ի�Ӧһ����
static void ResponseMove(void) {
    int vlRep;
    // ������һ����
    SetCursor((HCURSOR)LoadImage(NULL, IDC_WAIT, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
	TopSearch(1000);
    SetCursor((HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
    pos.MakeMove(Search.mvResult);
    // �����һ�����ѡ����
    DrawSquare(SrcPos(Xqwl.mvLast));
    DrawSquare(DstPos(Xqwl.mvLast));
    // �ѵ����ߵ����ǳ���
    Xqwl.mvLast = Search.mvResult;
    DrawSquare(SrcPos(Xqwl.mvLast), DRAW_SELECTED);
    DrawSquare(DstPos(Xqwl.mvLast), DRAW_SELECTED);
    // ����ظ�����
    vlRep = pos.IsRepetitive(3);
    if (pos.IsMating()) {
        // ����ֳ�ʤ������ô����ʤ�������������ҵ���������������ʾ��
        PlayResWav(IDR_LOSS);
        MessageBoxMute("���ٽ�������");
        Xqwl.bGameOver = TRUE;
    }
    else if (vlRep > 0) {
        vlRep = pos.RepeatValue(vlRep);
        // ע�⣺"vlRep"�Ƕ������˵�ķ�ֵ
        PlayResWav(vlRep < -WIN_VALUE ? IDR_LOSS : vlRep > WIN_VALUE ? IDR_WIN : IDR_DRAW);
        MessageBoxMute(vlRep < -WIN_VALUE ? "�����������벻Ҫ���٣�" :
            vlRep > WIN_VALUE ? "���Գ���������ף����ȡ��ʤ����" : "˫���������ͣ������ˣ�");
        Xqwl.bGameOver = TRUE;
    }
    else if (pos.MoveNum > 100) {
        PlayResWav(IDR_DRAW);
        MessageBoxMute("������Ȼ�������ͣ������ˣ�");
        Xqwl.bGameOver = TRUE;
    }
    else {
        // ���û�зֳ�ʤ������ô���Ž��������ӻ�һ�����ӵ�����
        PlayResWav(pos.LastCheck() ? IDR_CHECK2 : pos.Capture() ? IDR_CAPTURE2 : IDR_MOVE2);
        if (pos.Capture()) {
            pos.InitAllMoves();
        }
    }
}

// ��������¼�����
static void ClickSquare(int sq) {
    int pc, mv, vlRep;
    Xqwl.hdc = GetDC(Xqwl.hWnd);
    Xqwl.hdcTmp = CreateCompatibleDC(Xqwl.hdc);
    sq = Xqwl.bFlipped ? CorrespondPos(sq) : sq;
    pc = pos.Board[sq];

    if ((pc & PieceFlag(pos.player)) != 0) {
        // �������Լ����ӣ���ôֱ��ѡ�и���
        if (Xqwl.sqSelected != 0) {
            DrawSquare(Xqwl.sqSelected);
        }
        Xqwl.sqSelected = sq;
        DrawSquare(sq, DRAW_SELECTED);
        if (Xqwl.mvLast != 0) {
            DrawSquare(SrcPos(Xqwl.mvLast));
            DrawSquare(DstPos(Xqwl.mvLast));
        }
        PlayResWav(IDR_CLICK); // ���ŵ��������

    }
    else if (Xqwl.sqSelected != 0 && !Xqwl.bGameOver) {
        // �������Ĳ����Լ����ӣ�������ѡ����(һ�����Լ�����)����ô�������
        mv = Move(Xqwl.sqSelected, sq);
        if (pos.LegalMove(mv)) {
            if (pos.MakeMove(mv)) {
                Xqwl.mvLast = mv;
                DrawSquare(Xqwl.sqSelected, DRAW_SELECTED);
                DrawSquare(sq, DRAW_SELECTED);
                Xqwl.sqSelected = 0;
                // ����ظ�����
                vlRep = pos.IsRepetitive(3);
                if (pos.IsMating()) {
                    // ����ֳ�ʤ������ô����ʤ�������������ҵ���������������ʾ��
                    PlayResWav(IDR_WIN);
                    MessageBoxMute("ף����ȡ��ʤ����");
                    Xqwl.bGameOver = TRUE;
                }
                else if (vlRep > 0) {
                    vlRep = pos.RepeatValue(vlRep);
                    // ע�⣺"vlRep"�ǶԵ�����˵�ķ�ֵ
                    PlayResWav(vlRep > WIN_VALUE ? IDR_LOSS : vlRep < -WIN_VALUE ? IDR_WIN : IDR_DRAW);
                    MessageBoxMute(vlRep > WIN_VALUE ? "�����������벻Ҫ���٣�" :
                        vlRep < -WIN_VALUE ? "���Գ���������ף����ȡ��ʤ����" : "˫���������ͣ������ˣ�");
                    Xqwl.bGameOver = TRUE;
                }
                else if (pos.MoveNum > 100) {
                    PlayResWav(IDR_DRAW);
                    MessageBoxMute("������Ȼ�������ͣ������ˣ�");
                    Xqwl.bGameOver = TRUE;
                }
                else {
                    // ���û�зֳ�ʤ������ô���Ž��������ӻ�һ�����ӵ�����
                    PlayResWav(pos.LastCheck() ? IDR_CHECK : pos.Capture() ? IDR_CAPTURE : IDR_MOVE);
                    if (pos.Capture()) {
                        pos.InitAllMoves();
                    }
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
void Startup(void) {
	pos.Startup();
	Xqwl.sqSelected = Xqwl.mvLast = 0;
}

// �����¼���׽����
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
			ClickSquare(PositionIndex(x, y));
		}
		break;
		// �����¼�
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return FALSE;
}

// װ����ԴͼƬ
HBITMAP LoadResBmp(int nResId) {
	return (HBITMAP)LoadImage(Xqwl.hInst, MAKEINTRESOURCE(nResId), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
}
