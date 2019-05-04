﻿#include"ChessBoard.h"
#include"ChessData.h"
#include"Search.h"


PositionStruct pos;




// 判断棋子是否在棋盘中
  bool IN_BOARD(int sq) {
	return ccInBoard[sq] != 0;
}

// 判断棋子是否在九宫中
  bool IN_FORT(int sq) {
	return ccInFort[sq] != 0;
}

// 获得格子的横坐标
  int RANK_Y(int sq) {
	return sq >> 4;
}

// 获得格子的纵坐标
  int FILE_X(int sq) {
	return sq & 15;
}

// 根据纵坐标和横坐标获得格子
  int COORD_XY(int x, int y) {
	return x + (y << 4);
}

// 翻转格子
  int SQUARE_FLIP(int sq) {
	return 254 - sq;
}

// 纵坐标水平镜像
  int FILE_FLIP(int x) {
	return 14 - x;
}

// 横坐标垂直镜像
  int RANK_FLIP(int y) {
	return 15 - y;
}

// 格子水平镜像
  int MIRROR_SQUARE(int sq) {
	return COORD_XY(FILE_FLIP(FILE_X(sq)), RANK_Y(sq));
}

// 格子水平镜像
  int SQUARE_FORWARD(int sq, int sd) {
	return sq - 16 + (sd << 5);
}

// 走法是否符合帅(将)的步长
  bool KING_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 1;
}

// 走法是否符合仕(士)的步长
  bool ADVISOR_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 2;
}

// 走法是否符合相(象)的步长
  bool BISHOP_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 3;
}

// 相(象)眼的位置
  int BISHOP_PIN(int sqSrc, int sqDst) {
	return (sqSrc + sqDst) >> 1;
}

// 马腿的位置
  int KNIGHT_PIN(int sqSrc, int sqDst) {
	return sqSrc + ccKnightPin[sqDst - sqSrc + 256];
}

// 是否未过河
  bool HOME_HALF(int sq, int sd) {
	return (sq & 0x80) != (sd << 7);
}

// 是否已过河
  bool AWAY_HALF(int sq, int sd) {
	return (sq & 0x80) == (sd << 7);
}

// 是否在河的同一边
  bool SAME_HALF(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0x80) == 0;
}

// 是否在同一行
  bool SAME_RANK(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0xf0) == 0;
}

// 是否在同一列
  bool SAME_FILE(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0x0f) == 0;
}

// 获得红黑标记(红子是8，黑子是16)
  int SIDE_TAG(int sd) {
	return 8 + (sd << 3);
}

// 获得对方红黑标记
  int OPP_SIDE_TAG(int sd) {
	return 16 - (sd << 3);
}

// 获得走法的起点
  int SRC(int mv) {
	return mv & 255;
}

  int DST(int mv) {
	return mv >> 8;
}

// 根据起点和终点获得走法
  int MOVE(int sqSrc, int sqDst) {
	return sqSrc + (sqDst <<8);
}

// 走法水平镜像
  int MIRROR_MOVE(int mv) {
	return MOVE(MIRROR_SQUARE(SRC(mv)), MIRROR_SQUARE(DST(mv)));
}



  // 生成所有走法
  int PositionStruct::GenerateMoves(int *mvs) const {
	  int i, j, nGenMoves, nDelta, sqSrc, sqDst;
	  int pcSelfSide, pcOppSide, pcSrc, pcDst;
	  // 生成所有走法，需要经过以下几个步骤：

	  nGenMoves = 0;
	  pcSelfSide = SIDE_TAG(sdPlayer);
	  pcOppSide = OPP_SIDE_TAG(sdPlayer);
	  for (sqSrc = 0; sqSrc < 256; sqSrc++) {

		  // 1. 找到一个本方棋子，再做以下判断：
		  pcSrc = ucpcSquares[sqSrc];
		  if ((pcSrc & pcSelfSide) == 0) {
			  continue;
		  }

		  // 2. 根据棋子确定走法
		  switch (pcSrc - pcSelfSide) {
		  case PIECE_KING:
			  for (i = 0; i < 4; i++) {
				  sqDst = sqSrc + ccKingDelta[i];
				  if (!IN_FORT(sqDst)) {
					  continue;
				  }
				  pcDst = ucpcSquares[sqDst];
				  if ((pcDst & pcSelfSide) == 0) {
					  mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					  nGenMoves++;
				  }
			  }
			  break;
		  case PIECE_ADVISOR:
			  for (i = 0; i < 4; i++) {
				  sqDst = sqSrc + ccAdvisorDelta[i];
				  if (!IN_FORT(sqDst)) {
					  continue;
				  }
				  pcDst = ucpcSquares[sqDst];
				  if ((pcDst & pcSelfSide) == 0) {
					  mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					  nGenMoves++;
				  }
			  }
			  break;
		  case PIECE_BISHOP:
			  for (i = 0; i < 4; i++) {
				  sqDst = sqSrc + ccAdvisorDelta[i];
				  if (!(IN_BOARD(sqDst) && HOME_HALF(sqDst, sdPlayer) && ucpcSquares[sqDst] == 0)) {
					  continue;
				  }
				  sqDst += ccAdvisorDelta[i];
				  pcDst = ucpcSquares[sqDst];
				  if ((pcDst & pcSelfSide) == 0) {
					  mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					  nGenMoves++;
				  }
			  }
			  break;
		  case PIECE_KNIGHT:
			  for (i = 0; i < 4; i++) {
				  sqDst = sqSrc + ccKingDelta[i];
				  if (ucpcSquares[sqDst] != 0) {
					  continue;
				  }
				  for (j = 0; j < 2; j++) {
					  sqDst = sqSrc + ccKnightDelta[i][j];
					  if (!IN_BOARD(sqDst)) {
						  continue;
					  }
					  pcDst = ucpcSquares[sqDst];
					  if ((pcDst & pcSelfSide) == 0) {
						  mvs[nGenMoves] = MOVE(sqSrc, sqDst);
						  nGenMoves++;
					  }
				  }
			  }
			  break;
		  case PIECE_ROOK:
			  for (i = 0; i < 4; i++) {
				  nDelta = ccKingDelta[i];
				  sqDst = sqSrc + nDelta;
				  while (IN_BOARD(sqDst)) {
					  pcDst = ucpcSquares[sqDst];
					  if (pcDst == 0) {
						  mvs[nGenMoves] = MOVE(sqSrc, sqDst);
						  nGenMoves++;
					  }
					  else {
						  if ((pcDst & pcOppSide) != 0) {
							  mvs[nGenMoves] = MOVE(sqSrc, sqDst);
							  nGenMoves++;
						  }
						  break;
					  }
					  sqDst += nDelta;
				  }
			  }
			  break;
		  case PIECE_CANNON:
			  for (i = 0; i < 4; i++) {
				  nDelta = ccKingDelta[i];
				  sqDst = sqSrc + nDelta;
				  while (IN_BOARD(sqDst)) {
					  pcDst = ucpcSquares[sqDst];
					  if (pcDst == 0) {
						  mvs[nGenMoves] = MOVE(sqSrc, sqDst);
						  nGenMoves++;
					  }
					  else {
						  break;
					  }
					  sqDst += nDelta;
				  }
				  sqDst += nDelta;
				  while (IN_BOARD(sqDst)) {
					  pcDst = ucpcSquares[sqDst];
					  if (pcDst != 0) {
						  if ((pcDst & pcOppSide) != 0) {
							  mvs[nGenMoves] = MOVE(sqSrc, sqDst);
							  nGenMoves++;
						  }
						  break;
					  }
					  sqDst += nDelta;
				  }
			  }
			  break;
		  case PIECE_PAWN:
			  sqDst = SQUARE_FORWARD(sqSrc, sdPlayer);
			  if (IN_BOARD(sqDst)) {
				  pcDst = ucpcSquares[sqDst];
				  if ((pcDst & pcSelfSide) == 0) {
					  mvs[nGenMoves] = MOVE(sqSrc, sqDst);
					  nGenMoves++;
				  }
			  }
			  if (AWAY_HALF(sqSrc, sdPlayer)) {
				  for (nDelta = -1; nDelta <= 1; nDelta += 2) {
					  sqDst = sqSrc + nDelta;
					  if (IN_BOARD(sqDst)) {
						  pcDst = ucpcSquares[sqDst];
						  if ((pcDst & pcSelfSide) == 0) {
							  mvs[nGenMoves] = MOVE(sqSrc, sqDst);
							  nGenMoves++;
						  }
					  }
				  }
			  }
			  break;
		  }
	  }
	  return nGenMoves;
  }






//!？ wanring add和delpiece总是成对出现，考虑整合一下？（总体少一次判断）

void PositionStruct::AddPiece(int sq, int pc) { // 在棋盘上放一枚棋子
	ucpcSquares[sq] = pc;
	// 红方加分，黑方(注意"cucvlPiecePos"取值要颠倒)减分
	if (pc < 16) {
		vlWhite += cucvlPiecePos[pc - 8][sq];
	}
	else {
		vlBlack += cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
	}
}
void PositionStruct::DelPiece(int sq, int pc) { // 从棋盘上拿走一枚棋子
	ucpcSquares[sq] = 0;
	// 红方减分，黑方(注意"cucvlPiecePos"取值要颠倒)加分
	if (pc < 16) {
		vlWhite -= cucvlPiecePos[pc - 8][sq];
	}
	else {
		vlBlack -= cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
	}
}

void PositionStruct::AddDelPiece(int src, int dst, int pc) { // 在棋盘上放一枚棋子
	ucpcSquares[src] = pc;
	ucpcSquares[dst] = 0;
	// 红方加分，黑方(注意"cucvlPiecePos"取值要颠倒)减分
	if (pc < 16) {
		vlWhite += cucvlPiecePos[pc - 8][src];
		vlWhite -= cucvlPiecePos[pc - 8][dst];
	}
	else {
		vlBlack += cucvlPiecePos[pc - 16][SQUARE_FLIP(src)];
		vlBlack -= cucvlPiecePos[pc - 16][SQUARE_FLIP(dst)];
	}
}



int PositionStruct::Evaluate(void) const { // 局面评价函数
	return (sdPlayer == 0 ? vlWhite - vlBlack : vlBlack - vlWhite) + ADVANCED_VALUE;
}








// 初始化棋盘
void PositionStruct::Startup(void) {
	int sq, pc;
	sdPlayer = vlWhite = vlBlack = nDistance = 0;
	memset(ucpcSquares, 0, 256);
	for (sq = 0; sq < 256; ++sq) {
		pc = cucpcStartup[sq];
		if (pc != 0) {
			AddPiece(sq, pc);
		}
	}
}

// 搬一步棋的棋子
int PositionStruct::MovePiece(int mv) {
	int sqSrc, sqDst, pc, pcCaptured;
	sqSrc = SRC(mv);
	sqDst = DST(mv);
	pcCaptured = ucpcSquares[sqDst];
	if (pcCaptured != 0) {
		DelPiece(sqDst, pcCaptured);
	}
	pc = ucpcSquares[sqSrc];
	//DelPiece(sqSrc, pc);
	//AddPiece(sqDst, pc);
	AddDelPiece(sqDst, sqSrc, pc);
	return pcCaptured;
}

// 撤消搬一步棋的棋子
void PositionStruct::UndoMovePiece(int mv, int pcCaptured) {
	int sqSrc, sqDst, pc;
	sqSrc = SRC(mv);
	sqDst = DST(mv);
	pc = ucpcSquares[sqDst];
	//DelPiece(sqDst, pc);
	//AddPiece(sqSrc, pc);
	AddDelPiece(sqSrc, sqDst, pc);
	if (pcCaptured != 0) {
		AddPiece(sqDst, pcCaptured);
	}
}

// 交换走子方
void PositionStruct::ChangeSide(void) {
	sdPlayer = 1 - sdPlayer;
}

// 判断走法是否合理
bool PositionStruct::LegalMove(int mv) const {
	int sqSrc, sqDst, sqPin;
	int pcSelfSide, pcSrc, pcDst, nDelta;
	// 判断走法是否合法，需要经过以下的判断过程：

	// 1. 判断起始格是否有自己的棋子
	sqSrc = SRC(mv);
	pcSrc = ucpcSquares[sqSrc];
	pcSelfSide = SIDE_TAG(sdPlayer);
	if ((pcSrc & pcSelfSide) == 0) {
		return FALSE;
	}

	// 2. 判断目标格是否有自己的棋子
	sqDst = DST(mv);
	pcDst = ucpcSquares[sqDst];
	if ((pcDst & pcSelfSide) != 0) {
		return FALSE;
	}

	// 3. 根据棋子的类型检查走法是否合理
	switch (pcSrc - pcSelfSide) {
	case PIECE_KING:
		return IN_FORT(sqDst) && KING_SPAN(sqSrc, sqDst);
	case PIECE_ADVISOR:
		return IN_FORT(sqDst) && ADVISOR_SPAN(sqSrc, sqDst);
	case PIECE_BISHOP:
		return SAME_HALF(sqSrc, sqDst) && BISHOP_SPAN(sqSrc, sqDst) &&
			ucpcSquares[BISHOP_PIN(sqSrc, sqDst)] == 0;
	case PIECE_KNIGHT:
		sqPin = KNIGHT_PIN(sqSrc, sqDst);
		return sqPin != sqSrc && ucpcSquares[sqPin] == 0;
	case PIECE_ROOK:
	case PIECE_CANNON:
		if (SAME_RANK(sqSrc, sqDst)) {
			nDelta = (sqDst < sqSrc ? -1 : 1);
		}
		else if (SAME_FILE(sqSrc, sqDst)) {
			nDelta = (sqDst < sqSrc ? -16 : 16);
		}
		else {
			return FALSE;
		}
		sqPin = sqSrc + nDelta;
		while (sqPin != sqDst && ucpcSquares[sqPin] == 0) {
			sqPin += nDelta;
		}
		if (sqPin == sqDst) {
			return pcDst == 0 || pcSrc - pcSelfSide == PIECE_ROOK;
		}
		else if (pcDst != 0 && pcSrc - pcSelfSide == PIECE_CANNON) {
			sqPin += nDelta;
			while (sqPin != sqDst && ucpcSquares[sqPin] == 0) {
				sqPin += nDelta;
			}
			return sqPin == sqDst;
		}
		else {
			return FALSE;
		}
	case PIECE_PAWN:
		if (AWAY_HALF(sqDst, sdPlayer) && (sqDst == sqSrc - 1 || sqDst == sqSrc + 1)) {
			return TRUE;
		}
		return sqDst == SQUARE_FORWARD(sqSrc, sdPlayer);
	default:
		return FALSE;
	}
}

// 走一步棋
bool PositionStruct::MakeMove(int mv, int &pcCaptured) {
	pcCaptured = MovePiece(mv);
	if (Checked()) {
		UndoMovePiece(mv, pcCaptured);
		return FALSE;
	}
	ChangeSide();
	nDistance++;
	return TRUE;
}

void PositionStruct::UndoMakeMove(int mv, int pcCaptured) { // 撤消走一步棋
	--nDistance;
	ChangeSide();
	UndoMovePiece(mv, pcCaptured);
}



// 判断是否被将军
bool PositionStruct::Checked() const {
	int i, j, sqSrc, sqDst;
	int pcSelfSide, pcOppSide, pcDst, nDelta;
	pcSelfSide = SIDE_TAG(sdPlayer);
	pcOppSide = OPP_SIDE_TAG(sdPlayer);
	// 找到棋盘上的帅(将)，再做以下判断：

	for (sqSrc = 0; sqSrc < 256; sqSrc++) {
		if (ucpcSquares[sqSrc] != pcSelfSide + PIECE_KING) {
			continue;
		}

		// 1. 判断是否被对方的兵(卒)将军
		if (ucpcSquares[SQUARE_FORWARD(sqSrc, sdPlayer)] == pcOppSide + PIECE_PAWN) {
			return TRUE;
		}
		for (nDelta = -1; nDelta <= 1; nDelta += 2) {
			if (ucpcSquares[sqSrc + nDelta] == pcOppSide + PIECE_PAWN) {
				return TRUE;
			}
		}

		// 2. 判断是否被对方的马将军(以仕(士)的步长当作马腿)
		for (i = 0; i < 4; i++) {
			if (ucpcSquares[sqSrc + ccAdvisorDelta[i]] != 0) {
				continue;
			}
			for (j = 0; j < 2; j++) {
				pcDst = ucpcSquares[sqSrc + ccKnightCheckDelta[i][j]];
				if (pcDst == pcOppSide + PIECE_KNIGHT) {
					return TRUE;
				}
			}
		}

		// 3. 判断是否被对方的车或炮将军(包括将帅对脸)
		for (i = 0; i < 4; i++) {
			nDelta = ccKingDelta[i];
			sqDst = sqSrc + nDelta;
			while (IN_BOARD(sqDst)) {
				pcDst = ucpcSquares[sqDst];
				if (pcDst != 0) {
					if (pcDst == pcOppSide + PIECE_ROOK || pcDst == pcOppSide + PIECE_KING) {
						return TRUE;
					}
					break;
				}
				sqDst += nDelta;
			}
			sqDst += nDelta;
			while (IN_BOARD(sqDst)) {
				int pcDst = ucpcSquares[sqDst];
				if (pcDst != 0) {
					if (pcDst == pcOppSide + PIECE_CANNON) {
						return TRUE;
					}
					break;
				}
				sqDst += nDelta;
			}
		}
		return FALSE;
	}
	return FALSE;
}



// 判断是否被杀
bool PositionStruct::IsMate(void) {
	int i, nGenMoveNum, pcCaptured;
	int mvs[MAX_GEN_MOVES];

	nGenMoveNum = GenerateMoves(mvs);
	for (i = 0; i < nGenMoveNum; ++i) {
		pcCaptured = MovePiece(mvs[i]);
		if (!Checked()) {
			UndoMovePiece(mvs[i], pcCaptured);
			return FALSE;
		}
		else {
			UndoMovePiece(mvs[i], pcCaptured);
		}
	}
	return TRUE;
}