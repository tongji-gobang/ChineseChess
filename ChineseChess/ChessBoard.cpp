#include"ChessBoard.h"
#include"ChessData.h"
#include"Search.h"
#include <cstring>


PositionStruct pos;

// 获得格子的横坐标
int Row(int posIndex)
{
    return posIndex >> 4;  // posIndex / 16, 对应二维棋盘的行
}

// 获得格子的纵坐标
int Column(int posIndex)
{
    return posIndex & 15; // posIndex % 16, 对应二维棋盘的列
}

// 根据纵坐标和横坐标获得位置
int PositionIndex(int col, int row)
{
    return (row << 4) + col;
}

// 将己方位置对应到对方位置
int CorrespondPos(int posIndex)
{
    return 254 - posIndex;
}

// 纵坐标水平镜像
int MirrorCol(int col)
{
    return 14 - col;
}

// 横坐标垂直镜像
int MirrorRow(int row)
{
    return 15 - row;
}

// 该位置的同行对应位置
int MirrorPosRow(int posIndex)
{
    return PositionIndex(MirrorCol(Column(posIndex)), Row(posIndex));
}

// 该位置的同列的下一个位置(对双方来说均是前进一行)
int NextPosCol(int posIndex, int player)
{
    return posIndex - 16 + (player << 5);
}

// 将/帅的走法是否合理
bool LegalMoveKing(int src, int dst)
{
    return LegalSpan[dst - src + 256] == 1;
}

// 仕的走法是否合理
bool LegalMoveAdvisor(int src, int dst)
{
    return LegalSpan[dst - src + 256] == 2;
}

// 相/象的走法是否合理
bool LegalMoveBishop(int src, int dst) {
    return LegalSpan[dst - src + 256] == 3;
}

// 相(象)眼的位置
int BishopCenter(int src, int dst)
{
    return (src + dst) >> 1;
}

// 马腿的位置
int KnightPinPos(int src, int dst)
{
    return src + KnightPin[dst - src + 256];
}

// 是否过河 true : 过河， false : 未过河
bool CrossRiver(int posIndex, int player)
{
    // 以 128(0x80) 为界, 0-128 为 0 玩家， 128-256为 1 玩家
    return (posIndex & 0x80) == (player << 7);
}

// 是否在河的同一边
bool SameSide(int src, int dst)
{
    // 若在河的同一边，异或后 0x80 对应的bit位为 0 
    return ((src ^ dst) & 0x80) == 0;
}

// 是否在同一行
bool SameRow(int src, int dst)
{
    // 若在同一行, 第 4-7 bit位一定相同
    return ((src ^ dst) & 0xf0) == 0;
}

// 是否在同一列
bool SameCol(int src, int dst)
{
    // 若在同一行, 第 0-3 bit位一定相同
    return ((src ^ dst) & 0x0f) == 0;
}

// 获得红黑标记(红子是8，黑子是16)
int PieceFlag(int player)
{
    return 8 + (player << 3);
}

// 获得对方红黑标记
int OppPieceFlag(int player)
{
    return 16 - (player << 3);
}

// 获得走法的起点
int SrcPos(int move)
{
    // move % 256
    return move & 255;
}
// 获得走法的终点
int DstPos(int move)
{
    // move / 256
    return move >> 8;
}

// 根据起点和终点获得走法
int Move(int src, int dst)
{
    return src + (dst << 8);
}

// 走法水平镜像
int MirrorMove(int move)
{
    return Move(MirrorPosRow(SrcPos(move)), MirrorPosRow(DstPos(move)));
}

// 初始化棋盘
void PositionStruct::Startup() {
    int posIndex, piece;
    ClearBoard();
    for (posIndex = 0; posIndex < 256; ++posIndex) {
        piece = StartupBoard[posIndex];
        if (piece != 0) {
            AddPiece(posIndex, piece);
        }
    }
    this->InitAllMoves();
}

// 搬一步棋的棋子
int PositionStruct::MovePiece(int move) {
    int src, dst, piece, pieceCaptured;
    src = SrcPos(move);
    dst = DstPos(move);
    pieceCaptured = this->Board[dst];
    if (pieceCaptured != 0) {
        this->DelPiece(dst, pieceCaptured);
    }
    piece = this->Board[src];
    this->DelPiece(src, piece);
    this->AddPiece(dst, piece);
    return pieceCaptured;
}

// 撤消搬一步棋的棋子
void PositionStruct::UndoMovePiece(int move, int pieceCaptured) {
    int src, dst, piece;
    src = SrcPos(move);
    dst = DstPos(move);
    piece = Board[dst];
    this->DelPiece(dst, piece);
    this->AddPiece(src, piece);
    if (pieceCaptured != 0) {
        AddPiece(dst, pieceCaptured);
    }
}

// 走一步棋
bool PositionStruct::MakeMove(int move, bool change) {
    int pieceCaptured;
    DWORD dwKey;

    dwKey = zobr.dwKey;
    pieceCaptured = MovePiece(move);
    if (Checked()) {
        UndoMovePiece(move, pieceCaptured);
        return false;
    }
    if ( change )
        this->ChangeSide();
    this->AllMoves[this->MoveNum].push(move, pieceCaptured, Checked(), dwKey);
    this->MoveNum++;
    this->RootDistance++;
    return true;
}

void PositionStruct::UndoMakeMove()
{
    --this->MoveNum;
    --this->RootDistance;
    this->ChangeSide();
    this->UndoMovePiece(this->AllMoves[this->MoveNum].thisMove, this->AllMoves[this->MoveNum].pieceCaptured);
}

void PositionStruct::ChangeSide()
{
    this->player = 1 - this->player;
    zobr ^= Zrand.Player;
}

void PositionStruct::ClearBoard()
{
    this->player = 0;
    this->valueRed = 0;
    this->valueBlack = 0;
    this->RootDistance = 0;
    memset(this->Board, 0, sizeof(this->Board));
    zobr.InitZero();
}

void PositionStruct::InitAllMoves()
{
    this->AllMoves[0].push(0, 0, Checked(), zobr.dwKey);
    this->MoveNum = 1;
}

void PositionStruct::AddPiece(int position, int piece)
{
    this->Board[position] = piece;

    if (piece >= 16) {
        this->valueBlack += PiecePosValue[piece - 16][CorrespondPos(position)];
        zobr ^= Zrand.Table[piece - 9][position];
    }
    else {
        valueRed += PiecePosValue[piece - 8][position];
        zobr ^= Zrand.Table[piece - 8][position];
    }
}

void PositionStruct::DelPiece(int position, int piece)
{
    this->Board[position] = 0;
    if (piece >= 16) {
        this->valueBlack -= PiecePosValue[piece - 16][CorrespondPos(position)];
        zobr ^= Zrand.Table[piece - 9][position];
    }
    else {
        valueRed -= PiecePosValue[piece - 8][position];
        zobr ^= Zrand.Table[piece - 8][position];
    }
}

int PositionStruct::Evaluate() const
{
    return (this->player == 0 ? valueRed - valueBlack : valueBlack - valueRed) + ADVANCED_VALUE;
}

bool PositionStruct::LastCheck()
{
    return this->AllMoves[this->MoveNum - 1].Check;
}

bool PositionStruct::Captured() const
{
    return this->AllMoves[this->MoveNum - 1].pieceCaptured != 0;
}

void PositionStruct::MoveNull()
{
    DWORD key;
    key = this->zobr.dwKey;
    this->ChangeSide();
    this->AllMoves[this->MoveNum].push(0, 0, false, key);
    ++this->MoveNum;
    ++this->RootDistance;
}

void PositionStruct::UndoMoveNull()
{
    --this->RootDistance;
    --this->MoveNum;
    this->ChangeSide();
}

int PositionStruct::DrawValue()
{
    return (this->RootDistance & 1) == 0 ? -DRAW_VALUE : DRAW_VALUE;
}

int PositionStruct::RepeatValue(int ReNum)
{
    int value;
    if ((ReNum & 2) == 0)
        value = 0;
    else
        value = this->RootDistance - BAN_VALUE + ((ReNum & 4) == 0 ? 0 : BAN_VALUE - this->RootDistance);

    return value ? value : this->DrawValue();
}

bool PositionStruct::NullOkay()
{
    return (this->player ? valueBlack : valueRed) > NULL_MARGIN;
}


// 生成所有走法，如果"OnlyCapture"为"true"则只生成吃子走法
int PositionStruct::GenerateMoves(int* moves, bool OnlyCapture) const {
    int i, j, NumGenerate, delta, src, dst;
    int SelfSide, OppSide, pieceSrc, pieceDst;
    // 生成所有走法，需要经过以下几个步骤：

    NumGenerate = 0;
    SelfSide = PieceFlag(this->player);
    OppSide = OppPieceFlag(this->player);
    for (src = 0; src < 256; ++src) {

        // 1. 找到一个本方棋子，再做以下判断：
        pieceSrc = this->Board[src];
        if ((pieceSrc & SelfSide) == 0) {
            continue;
        }

        // 2. 根据棋子确定走法 
        switch (pieceSrc - SelfSide) {
            case KING:
                for (i = 0; i < 4; i++) {
                    dst = src + KingStep[i];
                    if (!InFort[dst]) {
                        continue;
                    }
                    pieceDst = this->Board[dst];
                    if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0) {
                        moves[NumGenerate] = Move(src, dst);
                        NumGenerate++;
                    }
                }
                break;
            case ADVISOR:
                for (i = 0; i < 4; i++) {
                    dst = src + AdvisorStep[i];
                    if (!InFort[dst]) {
                        continue;
                    }
                    pieceDst = this->Board[dst];
                    if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0) {
                        moves[NumGenerate] = Move(src, dst);
                        NumGenerate++;
                    }
                }
                break;
            case BISHOP:
                for (i = 0; i < 4; i++) {
                    dst = src + AdvisorStep[i];
                    if (!(InBoard[dst] && !CrossRiver(dst, this->player) && this->Board[dst] == 0)) {
                        continue;
                    }
                    dst += AdvisorStep[i];
                    pieceDst = this->Board[dst];
                    if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0) {
                        moves[NumGenerate] = Move(src, dst);
                        NumGenerate++;
                    }
                }
                break;
            case KNIGHT:
                for (i = 0; i < 4; i++) {
                    dst = src + KingStep[i];
                    if (this->Board[dst] != 0) {
                        continue;
                    }
                    for (j = 0; j < 2; j++) {
                        dst = src + KnightStep[i][j];
                        if (!InBoard[dst]) {
                            continue;
                        }
                        pieceDst = this->Board[dst];
                        if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0) {
                            moves[NumGenerate] = Move(src, dst);
                            NumGenerate++;
                        }
                    }
                }
                break;
            case ROOK:
                for (i = 0; i < 4; i++) {
                    delta = KingStep[i];
                    dst = src + delta;
                    while (InBoard[dst]) {
                        pieceDst = this->Board[dst];
                        if (pieceDst == 0) {
                            if (!OnlyCapture) {
                                moves[NumGenerate] = Move(src, dst);
                                NumGenerate++;
                            }
                        }
                        else {
                            if ((pieceDst & OppSide) != 0) {
                                moves[NumGenerate] = Move(src, dst);
                                NumGenerate++;
                            }
                            break;
                        }
                        dst += delta;
                    }
                }
                break;
            case CANNON:
                for (i = 0; i < 4; i++) {
                    delta = KingStep[i];
                    dst = src + delta;
                    while (InBoard[dst]) {
                        pieceDst = this->Board[dst];
                        if (pieceDst == 0) {
                            if (!OnlyCapture) {
                                moves[NumGenerate] = Move(src, dst);
                                NumGenerate++;
                            }
                        }
                        else {
                            break;
                        }
                        dst += delta;
                    }
                    dst += delta;
                    while (InBoard[dst]) {
                        pieceDst = this->Board[dst];
                        if (pieceDst != 0) {
                            if ((pieceDst & OppSide) != 0) {
                                moves[NumGenerate] = Move(src, dst);
                                NumGenerate++;
                            }
                            break;
                        }
                        dst += delta;
                    }
                }
                break;
            case PAWN:
                dst = NextPosCol(src, this->player);
                if (InBoard[dst]) {
                    pieceDst = this->Board[dst];
                    if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0) {
                        moves[NumGenerate] = Move(src, dst);
                        NumGenerate++;
                    }
                }
                if (CrossRiver(src, this->player)) {
                    for (delta = -1; delta <= 1; delta += 2) {
                        dst = src + delta;
                        if (InBoard[dst]) {
                            pieceDst = this->Board[dst];
                            if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0) {
                                moves[NumGenerate] = Move(src, dst);
                                NumGenerate++;
                            }
                        }
                    }
                }
                break;
        }
    }
    return NumGenerate;
}

// 判断走法是否合理
bool PositionStruct::LegalMove(int mv) const
{
    int src, dst, pin;
    int SelfSide, pieceSrc, pieceDst, delta;
    // 判断走法是否合法，需要经过以下的判断过程：

    // 1. 判断起始格是否有自己的棋子
    src = SrcPos(mv);
    pieceSrc = this->Board[src];
    SelfSide = PieceFlag(this->player);
    if ((pieceSrc & SelfSide) == 0) {
        return false;
    }

    // 2. 判断目标格是否有自己的棋子
    dst = DstPos(mv);
    pieceDst = this->Board[dst];
    if ((pieceDst & SelfSide) != 0) {
        return false;
    }

    // 3. 根据棋子的类型检查走法是否合理
    switch (pieceSrc - SelfSide) {
        case KING:
            return InFort[dst] && LegalMoveKing(src, dst);
        case ADVISOR:
            return InFort[dst] && LegalMoveAdvisor(src, dst);
        case BISHOP:
            return !CrossRiver(src, dst) && LegalMoveBishop(src, dst) &&
                this->Board[BishopCenter(src, dst)] == 0;
        case KNIGHT:
            pin = KnightPinPos(src, dst);
            return pin != src && this->Board[pin] == 0;
        case ROOK:
        case CANNON:
            if (SameRow(src, dst)) {
                delta = (dst < src ? -1 : 1);
            }
            else if (SameCol(src, dst)) {
                delta = (dst < src ? -16 : 16);
            }
            else {
                return false;
            }
            pin = src + delta;
            while (pin != dst && this->Board[pin] == 0) {
                pin += delta;
            }
            if (pin == dst) {
                return pieceDst == 0 || pieceSrc - SelfSide == ROOK;
            }
            else if (pieceDst != 0 && pieceSrc - SelfSide == CANNON) {
                pin += delta;
                while (pin != dst && this->Board[pin] == 0) {
                    pin += delta;
                }
                return pin == dst;
            }
            else {
                return false;
            }
        case PAWN:
            if (CrossRiver(dst, this->player) && (dst == src - 1 || dst == src + 1)) {
                return true;
            }
            return dst == NextPosCol(src, this->player);
        default:
            return false;
    }
}

// 判断是否被将军
bool PositionStruct::Checked() const {
    int i, j, src, dst;
    int SelfSide, OppSide, pieceDst, delta;
    SelfSide = PieceFlag(this->player);
    OppSide = OppPieceFlag(this->player);
    // 找到棋盘上的帅(将)，再做以下判断：

    for (src = 0; src < 256; src++) {
        if (this->Board[src] != SelfSide + KING) {
            continue;
        }

        // 1. 判断是否被对方的兵(卒)将军
        if (this->Board[NextPosCol(src, this->player)] == OppSide + PAWN) {
            return TRUE;
        }
        for (delta = -1; delta <= 1; delta += 2) {
            if (this->Board[src + delta] == OppSide + PAWN) {
                return TRUE;
            }
        }

        // 2. 判断是否被对方的马将军(以仕(士)的步长当作马腿)
        for (i = 0; i < 4; i++) {
            if (this->Board[src + AdvisorStep[i]] != 0) {
                continue;
            }
            for (j = 0; j < 2; j++) {
                pieceDst = this->Board[src + KnightCheckStep[i][j]];
                if (pieceDst == OppSide + KNIGHT) {
                    return TRUE;
                }
            }
        }

        // 3. 判断是否被对方的车或炮将军(包括将帅对脸)
        for (i = 0; i < 4; i++) {
            delta = KingStep[i];
            dst = src + delta;
            while (InBoard[dst]) {
                pieceDst = this->Board[dst];
                if (pieceDst != 0) {
                    if (pieceDst == OppSide + ROOK || pieceDst == OppSide + KING) {
                        return TRUE;
                    }
                    break;
                }
                dst += delta;
            }
            dst += delta;
            while (InBoard[dst]) {
                int pieceDst = this->Board[dst];
                if (pieceDst != 0) {
                    if (pieceDst == OppSide + CANNON) {
                        return TRUE;
                    }
                    break;
                }
                dst += delta;
            }
        }
        return FALSE;
    }
    return FALSE;
}

// 判断是否被杀
bool PositionStruct::IsMate() {
    int i, nGenMoveNum, pcCaptured;
    int moves[MAX_GEN_MOVES];

    nGenMoveNum = GenerateMoves(moves, false);
    for (i = 0; i < nGenMoveNum; i++) {
        pcCaptured = MovePiece(moves[i]);
        if (!Checked()) {
            UndoMovePiece(moves[i], pcCaptured);
            return FALSE;
        }
        else {
            UndoMovePiece(moves[i], pcCaptured);
        }
    }
    return TRUE;
}

// 检测重复局面
int PositionStruct::IsRepetitive(int ReLoop)
{
    bool SelfSide, PerpetualCheck, OppPerpetualCheck;
    const MoveInfo* ptrMoves;

    SelfSide = false;
    PerpetualCheck = true;
    OppPerpetualCheck = true;
    ptrMoves = this->AllMoves + this->MoveNum - 1;
    while (ptrMoves->thisMove != 0 && ptrMoves->pieceCaptured == 0) {
        if (SelfSide) {
            PerpetualCheck = PerpetualCheck && ptrMoves->Check;
            if (ptrMoves->thisKey == zobr.dwKey) {
                --ReLoop;
                if (ReLoop == 0)
                {
                    return 1 + (PerpetualCheck ? 2 : 0) + (OppPerpetualCheck ? 4 : 0);
                }
            }
        }
        else {
            OppPerpetualCheck = OppPerpetualCheck && ptrMoves->Check;
        }
        SelfSide = !SelfSide;
        ptrMoves--;
    }
    return 0;
}
