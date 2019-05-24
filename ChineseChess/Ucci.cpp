
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
// ��ȡָ��
char * read_line()
{
	static char s[MAX_BUFF]; // ע���Ǿ�̬�� 
	fgets(s, MAX_BUFF, stdin);  // �����в���
	s[strlen(s) - 1] = '\0'; // fgets���\n
	return s;
}

// ���յ�һ��ָ��
CommandEnum first_line()
{
	char *p;
	p = read_line();

	//while (p) {
	//	printf("read_line");
	//	p = read_line();
	//}
	//printf("??????");
	if (strcmp(p, "ucci") == 0) { // Ĭ��ָ��û�д�
		return CommandUcci; // �ϲ�õ�ָ��� ����ucciok
	}
	else {
		return CommandNone;
	}
}


// ���պ���ָ��
CommandEnum idle_line(CommandInfo &comm)
{
	char *p; //ָ��

	p = read_line();
	if (strncmp(p, "isready", 7) == NULL) {
		return CommandIsReady; // �ϲ�õ�ָ��� ����isready
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
	else if (strncmp(p, "go ", 3) == 0) { // word��ֻҪʵ��go time 
		if (strncmp(p + 3, "time ", 5) == 0) {
			// ��Ҫ������ݽṹ��sec����ȥ
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
		return CommandNone; // ֻҪָ����ȷ���󲻻�����return

	}
	else if (strcmp(p, "quit") == 0) {
		return CommandQuit; // �ϲ�õ�ָ��� ����bye
	}
	else {
		return CommandNone;
	}
}


// �ܸ���bestmove���ַ�����ʽ
long best_move2str(int mv)
{
	int src = mv & 255;
	int dst = mv >> 8;

	char bestmove[4];

	//// ��ø��ӵĺ�����
	//int RANK_Y(int sq) {
	//	return sq >> 4;
	//}

	//// ��ø��ӵ�������
	//int FILE_X(int sq) {
	//	return sq & 15;

	int src_x = (src & 15) - 3; // ��ø��ӵ�������0-0
	int src_y = (src >> 4) - 3; // ��ø��ӵĺ�����a-i
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


// nobestmoveֱ���ڶ���ʵ��


// fen�ַ��������룬ֱ�ӷ���pcֵ
int char2type(char ch)
{
	// pc:  
	// 8~14���α�ʾ�췽��˧���ˡ��ࡢ�������ںͱ���
	// 16~22���α�ʾ�ڷ��Ľ���ʿ�����������ں��䡣

	// ucci��
	// Сд���Ǻڷ�

	// ��ĸ��д��Ӧ��
	/*const int PIECE_KING = 0;
	const int PIECE_ADVISOR = 1;
	const int PIECE_BISHOP = 2;	// ��
	const int PIECE_KNIGHT = 3;	// ��
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


// ��fen���ĸ��ַ�תΪint�͵�mv
void str2move(long str, int &mv)
{
	char *ch;
	mv = 0;
	ch = (char *)&str;
	//return x + (y << 4)
	//move: return sqSrc + (sqDst <<8);

	// 3 ��256���̱߽��3
#ifdef DEBUG
	// dst
	printf("%d ", ('9' - ch[1] + 3));
	printf("%d ", ch[0] - 'a' + 3);
#endif // DEBUG
	mv = (ch[2] - 'a' + 3) + (('9' - ch[3] + 3) << 4);
	mv <<= 8;
	mv += (ch[0] - 'a' + 3) + (('9' - ch[1] + 3) << 4);
}


// fen������

//rnbakabnr / 9 / 1c5c1 / p1p1p1p1p / 9 / 9 / P1P1P1P1P / 1C5C1 / 9 / RNBAKABNR w - -0 1
//
//(1) ��ɫ���򣬱�ʾ���̲��֣�Сд��ʾ�ڷ�����д��ʾ�췽����������ͬ���������FEN�淶(����ġ�������������Է����ժ��������PGN��FEN���׹淶(��)һ��)������Ҫע�����㣬һ���й�����������10�У�����Ҫ��9���� / ����ÿһ�и�������������������Ӣ����ĸ��ʾ������������û�е���������(ʿ)���ڣ�����ֱ�����ĸA(a)��C(c)��ʾ��
//(2) ��ɫ���򣬱�ʾ�ֵ���һ�����ӣ���w����ʾ�췽����b����ʾ�ڷ���(������Ϊ�췽Ӧ���á�r����ʾ���ܶ��������ȷʵ��������ʾ�ġ�ElephantBoard�����á�w����ʾ����ʶ��ʱ��ȡ���İ취������b����ʾ�ڷ����������ⶼ��ʾ�췽��)
//(3) ����ɫ���򣬿�ȱ��ʼ���á� - ����ʾ��
//(4) �Ϻ�ɫ���򣬿�ȱ��ʼ���á� - ����ʾ��
//(5) ��ɫ���򣬱�ʾ˫��û�г��ӵ����岽��(��غ���)��ͨ����ֵ�ﵽ120��Ҫ�к�(��ʮ�غ���Ȼ����)��һ���γɾ������һ���ǳ��ӣ�����ͱ�ǡ�0����(�������������ͨ��������岻��ElephantBoard�Ĺ������������Ǹ�����һ�����жϺ���ģ��������Ǽ�Ϊ��0����)
//(6) ��ɫ���򣬱�ʾ��ǰ�Ļغ��������о��оֻ��ž�ʱ����Ϊ�о�����ľ��棬��һ�����д1�����ž��Ƶķ�չ�����ӡ�
void process_fen(CommandInfo &comm) {
	// ÿ�ν�������ַ�����ԭ���Ƚ�
	//if (strcmp(comm.last_fen, comm.fen) == NULL) {

	//}
	//if (comm.is_captured == FALSE) {		// û�в�������

	//}

	const char *p = comm.fen;


	// Read Board:
	int i = 0;
	int k;
	int j = 9; // ��¼�ж���/

			   // j �ῴ
			   // i ����
	while (*p != ' ') {
		if (*p == '/') {
			i = 0;
			j--;
			if (j < 0) {
				break; // ���ж�������
			}
		}
		else if (*p >= '1' && *p <= '9') {
			for (k = 0; k < (*p - '0'); k++) {
				//if (i >= 9) { // �б�Ҫ��
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
			printf("9*10ʽ����������Ϊ(%c, %d),", i + 'a', j);
			printf("256ʽ����������Ϊ%x\n", ((9 - j + 3) << 4) + i + 3);
#endif // DEBUG
			

			int sq = ((9 - j + 3) << 4) + i + 3;
			// ���ݺ�������õ����ӵĵط�
			int cur_pc = pos.Board[sq];
			if (k != cur_pc) {
				pos.DeletePiece(sq, cur_pc);
				pos.AddPiece(sq, k);  // ��Ҫ������
			}
			

			i++;
			//if (i < 9) {  ����
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
	p += 6; // Խ���ո��-
			//��ֹ���ֲ�ֹһλ
	while (*p >= '0' && *p <= '9')
		p++;
	p++;
	while (*p >= '0' && *p <= '9')
		p++;
	p++;
	//p += 10;

#ifdef DEBUG
	printf("%s\n", p);  // ȷ��ָ��ӵ���ȷ
#endif // DEBUG

						// Read Counter  // ˫��û�г��ӵ����岽��
						/*movesѡ����Ϊ�˷�ֹ�����ų������ŷ�����ģ�
						UCCI���洫�ݾ���ʱ��ͨ��fenѡ��Ϊ���һ���Թ��ӵľ���(��ʼ����)��
						Ȼ��movesѡ���г��þ��浽��ǰ����������ŷ���*/
						//i = 0;
						//while (*p != ' ') {
						//	if (*p >= '0' && *p <= '9') {
						//		i *= 10;
						//		i += *p - '0';
						//	}
						//	p++;
						//}
						//p++; //�����ո� 

						// Read Moves

	int mv;
	if (strncmp(p, "moves ", 6) == 0) { //����moves��Ϊ�գ���ô�޷�����if
		p += 6;
		// int ��Ϊ����warning
		comm.nmv = int(strlen(p) + 1) / 5; // "moves"�����ÿ���ŷ�����1���ո��4���ַ�
#ifdef DEBUG
		printf("%d\n", comm.nmv);

#endif // DEBUG
		// ����
		// ��Ϊ0 pos.sdPlayer
		if ((pos.player == 0) && (comm.nmv & 1) == 1) {
			pos.player = 1;
		}
		else if ((pos.player == 1) && (comm.nmv & 1) == 1) {
			pos.player = 0;
		}
		for (i = 0; i < comm.nmv; i++) {
			mv = 0;
			//printf("%c%c%c%c ", p[0], p[1], p[2], p[3]);
			str2move(*(long *)p, mv);

			pos.MakeMove(mv, false);

			comm.movelist[i] = mv; // 4���ַ���ת��Ϊһ��long
#ifdef DEBUG
			printf("%x ", mv);
			long move_str = best_move2str(mv);

			printf("������֤%.4s\n", (char *)&(move_str));
#endif // DEBUG
			p += 5;
		}
#ifdef DEBUG
		printf("\n");
#endif // DEBUG
	}
	else {
		comm.nmv = 0; // û��moves��˵�����ֳ����ˣ���Ҫ�ı�������Ѿ�ͨ��AddPieceʵ����
		return; ////����moves��Ϊ�գ���ô�޷�����if �ĵ�Ҫ��ʵ��
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