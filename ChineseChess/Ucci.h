#pragma once

#define MAX_BUFF	1024	// һ�ζ���һ���ж��ٸ�
#define MAX_MOVE_NUM  64	// ����fen�����moves�м���
#define DEBUG
// UCCI Э��
#include <time.h>
#include <stdio.h>
#include <string.h>
#include	"ChessBoard.h"





// ����ָ�� ���չ���word ʵ����Щ��
/*
	ָ�
	1.ucci
	3.isready
	5.position ...
	6.go time
	9.quit

	������
	2.ucciok
	4.readyok
	7.bestmove
	8.nobestmove
	10.bye
*/

// �������״̬
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
	clock_t go_time;		//���Ĵ�С����
	int movelist[MAX_MOVE_NUM];
	int nmv;					// ��movesָ���ж������ٸ�moves
	char *last_fen;				// ��һ�ε�ָ��

	bool is_captured;			//
	char *last_point;			// ��һ�ε�ָ��λ��
	int cur_opp_move;
};


// ��ȡָ��
char * read_line();

// ���յ�һ��ָ��
CommandEnum first_line();

// ���պ���ָ��
CommandEnum idle_line(CommandInfo &comm);

// �ܸ���bestmove���ַ�����ʽ
long best_move2str(int mv);


// nobestmoveֱ���ڶ���ʵ��


// fen�ַ��������룬ֱ�ӷ���pcֵ
int char2type(char ch);


// ��fen���ĸ��ַ�תΪint�͵�mv
void str2move(long str, int &mv);

// fen������

//rnbakabnr / 9 / 1c5c1 / p1p1p1p1p / 9 / 9 / P1P1P1P1P / 1C5C1 / 9 / RNBAKABNR w - -0 1
//
//(1) ��ɫ���򣬱�ʾ���̲��֣�Сд��ʾ�ڷ�����д��ʾ�췽����������ͬ���������FEN�淶(����ġ�������������Է����ժ��������PGN��FEN���׹淶(��)һ��)������Ҫע�����㣬һ���й�����������10�У�����Ҫ��9���� / ����ÿһ�и�������������������Ӣ����ĸ��ʾ������������û�е���������(ʿ)���ڣ�����ֱ�����ĸA(a)��C(c)��ʾ��
//(2) ��ɫ���򣬱�ʾ�ֵ���һ�����ӣ���w����ʾ�췽����b����ʾ�ڷ���(������Ϊ�췽Ӧ���á�r����ʾ���ܶ��������ȷʵ��������ʾ�ġ�ElephantBoard�����á�w����ʾ����ʶ��ʱ��ȡ���İ취������b����ʾ�ڷ����������ⶼ��ʾ�췽��)
//(3) ����ɫ���򣬿�ȱ��ʼ���á� - ����ʾ��
//(4) �Ϻ�ɫ���򣬿�ȱ��ʼ���á� - ����ʾ��
//(5) ��ɫ���򣬱�ʾ˫��û�г��ӵ����岽��(��غ���)��ͨ����ֵ�ﵽ120��Ҫ�к�(��ʮ�غ���Ȼ����)��һ���γɾ������һ���ǳ��ӣ�����ͱ�ǡ�0����(�������������ͨ��������岻��ElephantBoard�Ĺ������������Ǹ�����һ�����жϺ���ģ��������Ǽ�Ϊ��0����)
//(6) ��ɫ���򣬱�ʾ��ǰ�Ļغ��������о��оֻ��ž�ʱ����Ϊ�о�����ľ��棬��һ�����д1�����ž��Ƶķ�չ�����ӡ�
void process_fen(CommandInfo &comm);