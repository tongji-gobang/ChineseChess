
#include	"Ucci.h"

void debug_show_board()
{
	int i;
	int j;
	printf("\n");
	for (i = 3; i < 13; i++) {
		for (j = 3; j < 12; j++) {
			if (0 != pos.Board[i * 16 + j]) {
				printf("%-3d", pos.Board[i * 16 + j]);
			}
			else
				printf("0  ");
		}
		printf("\n");
	}
}
// 读取指令
char * read_line()
{
	static char s[MAX_BUFF]; // 注意是静态的 
	fgets(s, MAX_BUFF, stdin);  // 到底行不行
	s[strlen(s) - 1] = '\0'; // fgets会读\n
	return s;
}

//extern PositionStruct pos;
// 初始化ucci 废弃
inline void init_ucci()
{
//	UcciInAtty = _isatty(0);  // 还不知道做什么用
							  //UcciInAtty = 1;
}


// 接收第一条指令
CommandEnum first_line()
{
	char *p;
	init_ucci();
	p = read_line();

	//while (p) {
	//	printf("read_line");
	//	p = read_line();
	//}
	//printf("??????");
	if (strcmp(p, "ucci") == 0) { // 默认指令没有错
		return CommandUcci; // 上层得到指令后 返回ucciok
	}
	else {
		return CommandNone;
	}
}


// 接收后续指令
CommandEnum idle_line(CommandInfo &comm)
{
	char *p; //指针

	p = read_line();
	if (strncmp(p, "isready", 7) == NULL) {
		return CommandIsReady; // 上层得到指令后 返回isready
	}
	else if (strncmp(p, "position", 8) == NULL) { // CommandPosition
		p += 9;
		if (strncmp(p, "startpos", 8) == 0) {
			comm.fen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";

			//p += 2;
			//if (strncmp(p, "moves", 5) == NULL) {
			//	comm.fen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";
			//}
			//else {
			//	memcpy()
			//}

#ifdef DEBUG
			printf("[%s]\n", comm.fen);
#endif

		}
		else if (strncmp(p, "fen ", 4) == 0) {
			comm.fen = p + 4;

#ifdef DEBUG
			printf("[%s]\n", comm.fen);
#endif

		}
		else {
			return CommandNone;
		}
		return CommandPosition;
	}
	else if (strncmp(p, "go ", 3) == 0) { // word中只要实现go time 
		if (strncmp(p + 3, "time ", 5) == 0) {
			// 还要设计数据结构把sec传出去
			char *q = p + 8;
			clock_t sec = 0;
			while (1) {
				if (*q >= '0' && *q <= '9') {
					sec *= 10;
					sec += (*q - '0');
					q++;
				}
				else {
					break;
				}
			}
			comm.go_time = sec;
#ifdef DEBUG
			//printf("%d\n", sec);
#endif

			return CommandGoTime;
		}
		return CommandNone; // 只要指令正确无误不会这里return

	}
	else if (strcmp(p, "quit") == 0) {
		return CommandQuit; // 上层得到指令后 返回bye
	}
	else {
		return CommandNone;
	}
}


// 能给出bestmove的字符串形式
long best_move2str(int mv)
{
	int src = mv & 255;
	int dst = mv >> 8;

	char bestmove[4];

	//// 获得格子的横坐标
	//int RANK_Y(int sq) {
	//	return sq >> 4;
	//}

	//// 获得格子的纵坐标
	//int FILE_X(int sq) {
	//	return sq & 15;

	int src_x = (src & 15) - 3; // 获得格子的纵坐标0-0
	int src_y = (src >> 4) - 3; // 获得格子的横坐标a-i
	int dst_x = (dst & 15) - 3;
	int dst_y = (dst >> 4) - 3;
#ifdef DEBUG
	//printf("%d %d ", src, dst);
	//printf("%d %d", src_x, src_y);
#endif // DEBUG
	bestmove[0] = src_x + 'a';
	bestmove[1] = '9' - src_y;
	bestmove[2] = dst_x + 'a';
	bestmove[3] = '9' - dst_y;

	return *(long *)bestmove;
}


// nobestmove直接在顶层实现


// fen字符串的译码，直接返回pc值
int char2type(char ch)
{
	// pc:  
	// 8~14依次表示红方的帅、仕、相、马、车、炮和兵；
	// 16~22依次表示黑方的将、士、象、马、车、炮和卒。

	// ucci：
	// 小写的是黑方

	// 字母缩写对应：
	/*const int PIECE_KING = 0;
	const int PIECE_ADVISOR = 1;
	const int PIECE_BISHOP = 2;	// 象
	const int PIECE_KNIGHT = 3;	// 马
	const int PIECE_ROOK = 4;
	const int PIECE_CANNON = 5;
	const int PIECE_PAWN = 6;	//
	*/
	switch (ch) {
		case 'k': return 16;
		case 'K': return 8;

		case 'a': return 17;
		case 'A': return 9;

		case 'b': return 18;
		case 'B': return 10;

		case 'n': return 19;
		case 'N': return 11;

		case 'r': return 20;
		case 'R': return 12;

		case 'c': return 21;
		case 'C': return 13;

		case 'p': return 22;
		case 'P': return 14;

		default: return 7;
	}
}


// 将fen中四个字符转为int型的mv
void str2move(long str, int &mv)
{
	char *ch;
	mv = 0;
	ch = (char *)&str;
	//return x + (y << 4)
	//move: return sqSrc + (sqDst <<8);

	// 3 是256棋盘边界的3
#ifdef DEBUG
	// dst
	printf("%d ", ('9' - ch[1] + 3));
	printf("%d ", ch[0] - 'a' + 3);
#endif // DEBUG
	mv = (ch[2] - 'a' + 3) + (('9' - ch[3] + 3) << 4);
	mv <<= 8;
	mv += (ch[0] - 'a' + 3) + (('9' - ch[1] + 3) << 4);
}


// fen串处理

//rnbakabnr / 9 / 1c5c1 / p1p1p1p1p / 9 / 9 / P1P1P1P1P / 1C5C1 / 9 / RNBAKABNR w - -0 1
//
//(1) 红色区域，表示棋盘布局，小写表示黑方，大写表示红方，其他规则同国际象棋的FEN规范(请参阅《国际象棋译文苑》文摘——关于PGN和FEN记谱规范(下)一文)。这里要注意两点，一是中国象棋棋盘有10行，所以要用9个“ / ”把每一行隔开；二是棋子名称用英文字母表示，国际象棋中没有的棋子是仕(士)和炮，这里分别用字母A(a)和C(c)表示。
//(2) 绿色区域，表示轮到哪一方走子，“w”表示红方，“b”表示黑方。(有人认为红方应该用“r”表示，很多象棋软件确实是这样表示的。ElephantBoard尽管用“w”表示，但识别时采取灵活的办法，即“b”表示黑方，除此以外都表示红方。)
//(3) 深紫色区域，空缺，始终用“ - ”表示。
//(4) 紫红色区域，空缺，始终用“ - ”表示。
//(5) 蓝色区域，表示双方没有吃子的走棋步数(半回合数)，通常该值达到120就要判和(六十回合自然限着)，一旦形成局面的上一步是吃子，这里就标记“0”。(这个参数对于普通局面的意义不大，ElephantBoard的规则处理器并不是根据这一项来判断和棋的，所以总是计为“0”。)
//(6) 棕色区域，表示当前的回合数，在研究中局或排局时，作为研究对象的局面，这一项可以写1，随着局势的发展逐渐增加。
void process_fen(CommandInfo &comm) {
	// 每次进入这个字符串和原来比较
	//if (strcmp(comm.last_fen, comm.fen) == NULL) {

	//}
	//if (comm.is_captured == FALSE) {		// 没有产生吃子

	//}

	const char *p = comm.fen;


	// Read Board:
	int i = 0;
	int k;
	int j = 9; // 记录有多少/

			   // j 横看
			   // i 竖看
	while (*p != ' ') {
		if (*p == '/') {
			i = 0;
			j--;
			if (j < 0) {
				break; // 九行都读完了
			}
		}
		else if (*p >= '1' && *p <= '9') {
			for (k = 0; k < (*p - '0'); k++) {
				//if (i >= 9) { // 有必要吗？
				//	break;
				//}
				int sq = ((9 - j + 3) << 4) + i + 3;
				int cur_pc = pos.Board[sq];
				if (0 != cur_pc) {
					pos.DeletePiece(sq, cur_pc);
					//pos.AddPiece(sq, 0);
				}
				i++;
			}
		}
		else if ((*p >= 'A' && *p <= 'Z') ||
			(*p >= 'a' && *p <= 'z')) {
			k = char2type(*p);

#ifdef DEBUG
			printf("%d ", k);
			printf("9*10式的棋盘坐标为(%c, %d),", i + 'a', j);
			printf("256式的棋盘坐标为%x\n", ((9 - j + 3) << 4) + i + 3);
#endif // DEBUG
			

			int sq = ((9 - j + 3) << 4) + i + 3;
			// 根据横纵坐标得到落子的地方
			int cur_pc = pos.Board[sq];
			if (k != cur_pc) {
				pos.DeletePiece(sq, cur_pc);
				pos.AddPiece(sq, k);  // 需要再讨论
			}
			

			i++;
			//if (i < 9) {  废弃
			//if (RedPiece[k] < 16) {
			//SetPiece(i * 10 + j, RedPiece[k]);
			//	RedPiece[k] ++;
			//}
			//}

		}
		p++;
	}

	p += 1;
	//pos.sdPlayer = (*p == 'w' ? 0 : 1);
#ifdef DEBUG
	printf("<%d>\n", pos.player);
#endif // DEBUG
	
	
	if (pos.player == (*p == 'b' ? 0 : 1)) {
		pos.ChangeSide();
	}
	// ("w - - 0 1 ") 
	p += 6; // 越过空格和-
			//防止数字不止一位
	while (*p >= '0' && *p <= '9')
		p++;
	p++;
	while (*p >= '0' && *p <= '9')
		p++;
	p++;
	//p += 10;

#ifdef DEBUG
	printf("%s\n", p);  // 确认指针加得正确
#endif // DEBUG

						// Read Counter  // 双方没有吃子的走棋步数
						/*moves选项是为了防止引擎着出长打着法而设的，
						UCCI界面传递局面时，通常fen选项为最后一个吃过子的局面(或开始局面)，
						然后moves选项列出该局面到当前局面的所有着法。*/
						//i = 0;
						//while (*p != ' ') {
						//	if (*p >= '0' && *p <= '9') {
						//		i *= 10;
						//		i += *p - '0';
						//	}
						//	p++;
						//}
						//p++; //跳过空格 

						// Read Moves

	int mv;
	if (strncmp(p, "moves ", 6) == 0) { //若是moves后为空，那么无法进入if
		p += 6;
		// int 是为了消warning
		comm.nmv = int(strlen(p) + 1) / 5; // "moves"后面的每个着法都是1个空格和4个字符
#ifdef DEBUG
		printf("%d\n", comm.nmv);
#endif // DEBUG
		for (i = 0; i < comm.nmv; i++) {
			mv = 0;
			//printf("%c%c%c%c ", p[0], p[1], p[2], p[3]);
			str2move(*(long *)p, mv);

			pos.MakeMove(mv, false);

			comm.movelist[i] = mv; // 4个字符可转换为一个long
#ifdef DEBUG
			printf("%x ", mv);
			long move_str = best_move2str(mv);

			printf("交叉验证%.4s\n", (char *)&(move_str));
#endif // DEBUG
			p += 5;
		}
#ifdef DEBUG
		printf("\n");
#endif // DEBUG
	}
	else {
		comm.nmv = 0; // 没有moves，说明对手吃子了，需要改变的棋子已经通过AddPiece实现了
		return; ////若是moves后为空，那么无法进入if 文档要求实现
	}
	

	// 换方
	// 红为0 pos.sdPlayer
	if ((pos.player == 0) && (comm.nmv & 1) == 1) {
		pos.player = 1;
	}
	else if ((pos.player == 1) && (comm.nmv & 1) == 1) {
		pos.player = 0;
	}
#ifdef DEBUG
	debug_show_board();
	printf("[%d]", pos.player);
#endif // DEBUG
}

/*
position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r - - 0 1 moves h2e2

position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r - - 0 1 moves h2e2 h9g7
*/