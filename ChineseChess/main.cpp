#include"ChessBoard.h"
#include"ChessData.h"
#include"Search.h"
#include"Ucci.h"

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
					//printf("enter process fen\n");
					process_fen(comm); // 将move提取出来 存到Command的movelist里

									   //for (int i = 0; i < Command.nmv; i++) {
									   //	printf("%d ", Command.movelist[i]);
									   //	fflush(stdout);
									   //}
					break;
				case CommandGoTime:
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
#ifdef DEBUG
					debug_show_board();
#endif // DEBUG
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
