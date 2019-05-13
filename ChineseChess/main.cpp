#include"UI.h"
#include"ChessBoard.h"
#include"ChessData.h"
#include"Search.h"
#include"Ucci.h"
//#define DEBUG_UCCI





#ifdef DEBUG_UCCI
int main()
{
	CommandEnum IdleComm;
	CommandInfo comm;
	int mv_rst = 0;
	if (first_line() == CommandUcci) {
		printf("id name AI Intro 2019\n");
		fflush(stdout);
		printf("id copyright 2019\n");
		fflush(stdout);
		printf("id author 5\n");
		fflush(stdout);
		printf("id user Unknown User\n");
		fflush(stdout);
		printf("copyprotection ok\n");
		fflush(stdout);
		printf("ucciok\n");
		fflush(stdout);

		while (1) {
			IdleComm = idle_line(comm);
			switch (IdleComm) {
				case CommandIsReady:
					printf("readyok\n");
					fflush(stdout);
					break;
				case CommandPosition:
					//ThisSearch.Position.LoadFen(Command.Position.FenStr);
					//ThisSearch.Position.StartMove = ThisSearch.Position.MoveNum;
					//printf("enter process fen\n");
					process_fen(comm); // ��move��ȡ���� �浽Command��movelist��

									   //for (int i = 0; i < Command.nmv; i++) {
									   //	printf("%d ", Command.movelist[i]);
									   //	fflush(stdout);
									   //}
					break;
				case CommandGoTime:
					//ThisSearch.Search(Command.Search.DepthTime.Depth, 2/* Set by User */);
					SearchMain(1000);
					mv_rst = Search.mvResult;
					printf("[!!%x!!]", mv_rst);
					if (mv_rst != 0) {

						long rst_str = best_move2str(mv_rst);
						printf("bestmove %.4s\n", (char *)&rst_str);
						fflush(stdout);

					}
					else {
						printf("nobestmove\n");
						fflush(stdout);
					}
					debug_show_board();
					break;
				default:
					break;
			}
			if (IdleComm == CommandQuit) {
				break;
			}
		}
		printf("bye\n");
		fflush(stdout);
		return 0;
	}
	printf("return");
	return 0;
}
#else
// ��ڹ���
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int i;
	MSG msg;
	WNDCLASSEX wce;



	// ��ʼ��ȫ�ֱ���
	Xqwl.hInst = hInstance;
	Xqwl.bFlipped = FALSE;
	LoadBook();
	Startup();


	// װ��ͼƬ
	Xqwl.bmpBoard = LoadResBmp(IDB_BOARD);
	Xqwl.bmpSelected = LoadResBmp(IDB_SELECTED);
	for (i = KING; i <= PAWN; i++) {
		Xqwl.bmpPieces[PieceFlag(0) + i] = LoadResBmp(IDB_RK + i);
		Xqwl.bmpPieces[PieceFlag(1) + i] = LoadResBmp(IDB_BK + i);
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
	return (int)msg.wParam;
}
#endif