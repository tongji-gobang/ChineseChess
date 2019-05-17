#include"UI.h"
#include"ChessBoard.h"
#include"ChessData.h"
#include"Search.h"
#include"Ucci.h"
//#define DEBUG_UCCI
//#define DEBUG


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
		LoadBook();		// 加载开局库
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
					process_fen(comm); // 将move提取出来 存到Command的movelist里

									   //for (int i = 0; i < Command.nmv; i++) {
									   //	printf("%d ", Command.movelist[i]);
									   //	fflush(stdout);
									   //}
					break;
				case CommandGoTime:
					//ThisSearch.Search(Command.Search.DepthTime.Depth, 2/* Set by User */);
					TopSearch(comm.go_time);
					mv_rst = Search.mvResult;
#ifdef DEBUG
					printf("go time: %d\n", comm.go_time);
					printf("[!!%x!!]", mv_rst);
#endif // DEBUG
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
	
#ifdef DEBUG
	printf("return");
#endif // DEBUG
	return 0;
}
#else
// 入口过程
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int i;
	MSG msg;
	WNDCLASSEX wce;

	// 初始化全局变量
	Xqwl.hInst = hInstance;
	Xqwl.bFlipped = FALSE;
	LoadBook();
	Startup();


	// 装入图片
	Xqwl.bmpBoard = LoadResBmp(IDB_BOARD);
	Xqwl.bmpSelected = LoadResBmp(IDB_SELECTED);
	for (i = KING; i <= PAWN; i++) {
		Xqwl.bmpPieces[PieceFlag(0) + i] = LoadResBmp(IDB_RK + i);
		Xqwl.bmpPieces[PieceFlag(1) + i] = LoadResBmp(IDB_BK + i);
	}

	// 设置窗口
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

	// 打开窗口
	Xqwl.hWnd = CreateWindow("XQWLIGHT", "象棋小巫师", WINDOW_STYLES,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if (Xqwl.hWnd == NULL) {
		return 0;
	}
	ShowWindow(Xqwl.hWnd, nCmdShow);
	UpdateWindow(Xqwl.hWnd);

	// 接收消息
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
#endif