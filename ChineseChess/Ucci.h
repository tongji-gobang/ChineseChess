#pragma once

#define MAX_BUFF	1024	// 一次读入一行有多少个
#define MAX_MOVE_NUM  64	// 跟在fen串后的moves有几个
//#define DEBUG
// UCCI 协议
#include <time.h>
#include <stdio.h>
#include <string.h>
#include	"ChessBoard.h"


void debug_show_board();


// 接收指令 按照规则word 实现这些：
/*
	指令；
	1.ucci
	3.isready
	5.position ...
	6.go time
	9.quit

	反馈：
	2.ucciok
	4.readyok
	7.bestmove
	8.nobestmove
	10.bye
*/

// 定义各种状态
enum CommandEnum {
	CommandNone, 
	CommandUcci,
	CommandIsReady,
	CommandPosition,
	CommandGoTime, 
	CommandQuit
};

struct CommandInfo {
	char *fen;
	clock_t go_time;		//担心大小不够
	int movelist[MAX_MOVE_NUM];
	int nmv;					// 从moves指令中读到多少个moves
	char *last_fen;				// 上一次的指令

	bool is_captured;			//
	char *last_point;			// 上一次的指针位置
	int cur_opp_move;
};


// 读取指令
char * read_line();

// 接收第一条指令
CommandEnum first_line();

// 接收后续指令
CommandEnum idle_line(CommandInfo &comm);

// 能给出bestmove的字符串形式
long best_move2str(int mv);


// nobestmove直接在顶层实现


// fen字符串的译码，直接返回pc值
int char2type(char ch);


// 将fen中四个字符转为int型的mv
void str2move(long str, int &mv);

// fen串处理

//rnbakabnr / 9 / 1c5c1 / p1p1p1p1p / 9 / 9 / P1P1P1P1P / 1C5C1 / 9 / RNBAKABNR w - -0 1
//
//(1) 红色区域，表示棋盘布局，小写表示黑方，大写表示红方，其他规则同国际象棋的FEN规范(请参阅《国际象棋译文苑》文摘——关于PGN和FEN记谱规范(下)一文)。这里要注意两点，一是中国象棋棋盘有10行，所以要用9个“ / ”把每一行隔开；二是棋子名称用英文字母表示，国际象棋中没有的棋子是仕(士)和炮，这里分别用字母A(a)和C(c)表示。
//(2) 绿色区域，表示轮到哪一方走子，“w”表示红方，“b”表示黑方。(有人认为红方应该用“r”表示，很多象棋软件确实是这样表示的。ElephantBoard尽管用“w”表示，但识别时采取灵活的办法，即“b”表示黑方，除此以外都表示红方。)
//(3) 深紫色区域，空缺，始终用“ - ”表示。
//(4) 紫红色区域，空缺，始终用“ - ”表示。
//(5) 蓝色区域，表示双方没有吃子的走棋步数(半回合数)，通常该值达到120就要判和(六十回合自然限着)，一旦形成局面的上一步是吃子，这里就标记“0”。(这个参数对于普通局面的意义不大，ElephantBoard的规则处理器并不是根据这一项来判断和棋的，所以总是计为“0”。)
//(6) 棕色区域，表示当前的回合数，在研究中局或排局时，作为研究对象的局面，这一项可以写1，随着局势的发展逐渐增加。
void process_fen(CommandInfo &comm);