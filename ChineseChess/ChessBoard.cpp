#include "ChessBoard.h"
#include "ChessData.h"
#include "Search.h"
#include <cstring>

PositionStruct pos;

/**
 * description: 初始化棋盘
 * parameter: void
 * return: void
 */
void PositionStruct::Startup()
{
    int posIndex, piece;
    // 由于需要计算初始局面的zobrist键值，所以必须清空棋盘
    ClearBoard();
    // 将棋子加入棋盘
    const int begPos = PositionIndex(FILE_LEFT, RANK_TOP);
    const int endPos = PositionIndex(FILE_RIGHT, RANK_BOTTOM);
    for (posIndex = begPos; posIndex <= endPos; ++posIndex){
        piece = StartupBoard[posIndex];
        if (piece != 0){
            // 非空棋子加入，在addpiece中计算zobrist键值
            AddPiece(posIndex, piece);
        }
    }
    // 初始化记录历史走法的数组
    this->InitAllMoves();
}

/**
 * description: 移动棋子
 * parameter: 走法信息
 * return: 被吃的棋子
 */
int PositionStruct::MovePiece(int move)
{
    int src, dst, piece, pieceCaptured;
    src = SrcPos(move); // 获得该走法的起点
    dst = DstPos(move); // 获得该走法的终点
    pieceCaptured = this->Board[dst];   // 获取终点的棋子(即被吃的子)
    if (pieceCaptured != 0)    // 如果有有棋子则吃掉(已检查过合理性，不会吃己方棋子)
        this->DelPiece(dst, pieceCaptured); // 从棋盘上删除棋子
    piece = this->Board[src];   // 获取起点的棋子
    this->DelPiece(src, piece); // 删除起点棋子
    this->AddPiece(dst, piece); // 将起点的棋子放到终点位置
    return pieceCaptured;
}

/**
 * description: 撤销前一步的走子
 * parameter: move-走法，pieceCaptured-被吃的棋子
 * return: void
 */
void PositionStruct::UndoMovePiece(int move, int pieceCaptured)
{
    int src, dst, piece;
    src = SrcPos(move);
    dst = DstPos(move);
    piece = Board[dst];         // 获取终点的棋子
    this->DelPiece(dst, piece); // 删除
    this->AddPiece(src, piece); // 将该棋子放到起点
    if (pieceCaptured != 0)
        AddPiece(dst, pieceCaptured);   // 若被吃子，重置终点棋子
}

/**
 * description: 走一步棋
 * parameter: move-走法，change-是否换到对方(默认为false)
 * return: 若被将军返回false，否则true
 */
bool PositionStruct::MakeMove(int move, bool change)
{
    int pieceCaptured;
    DWORD key;        

    key = zobr.dwKey;   // 记录当前局面的键值
    pieceCaptured = MovePiece(move);    // 记录被吃子
    if (Checked()){ // 被将军则撤销走子
        UndoMovePiece(move, pieceCaptured);
        return false;
    }
    if (change)
        this->ChangeSide();
    
    // 记录到历史走法中, 含义为(当前局面要走的走法 执行走法后将被吃掉的子 此时对面是否被将军 未执行该走法前的局面键值)
    this->AllMoves[this->MoveNum++].push(move, pieceCaptured, Checked(), key);
    ++this->RootDistance;       // 已走的步数
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

    if (piece >= 16){
        this->valueBlack += PiecePosValue[piece - 16][CorrespondPos(position)];
        zobr ^= Zrand.Table[piece - 9][position];
    }
    else{
        valueRed += PiecePosValue[piece - 8][position];
        zobr ^= Zrand.Table[piece - 8][position];
    }
}

void PositionStruct::DelPiece(int position, int piece)
{
    this->Board[position] = 0;
    if (piece >= 16){
        this->valueBlack -= PiecePosValue[piece - 16][CorrespondPos(position)];
        zobr ^= Zrand.Table[piece - 9][position];
    }
    else{
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
int PositionStruct::GenerateMoves(int *moves, bool OnlyCapture) const
{
    int NumGenerate, src, dst;
    int SelfSide, OppSide, pieceSrc, pieceDst;
    // 生成所有走法，需要经过以下几个步骤：

    NumGenerate = 0;
    SelfSide = PieceFlag(this->player);
    OppSide = OppPieceFlag(this->player);

    for (int row = RANK_TOP; row <= RANK_BOTTOM; ++row){
        for (int col = FILE_LEFT; col <= FILE_RIGHT; ++col){
            src = PositionIndex(col, row);

            // 1. 找到一个本方棋子，再做以下判断：
            pieceSrc = this->Board[src];
            if ((pieceSrc & SelfSide) == 0){
                continue;
            }

            // 2. 根据棋子确定走法
            switch (pieceSrc - SelfSide){
            case KING:
                for (int i = 0; i < 4; i++){
                    dst = src + KingStep[i];
                    if (!InFort[dst]){
                        continue;
                    }
                    pieceDst = this->Board[dst];
                    if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0){
                        moves[NumGenerate++] = Move(src, dst);
                    }
                }
                break;
            case ADVISOR:
                for (int i = 0; i < 4; i++){
                    dst = src + AdvisorStep[i];
                    if (!InFort[dst]){
                        continue;
                    }
                    pieceDst = this->Board[dst];
                    if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0){
                        moves[NumGenerate++] = Move(src, dst);
                    }
                }
                break;
            case BISHOP:
                for (int i = 0; i < 4; i++){
                    dst = src + AdvisorStep[i];
                    if (!(InBoard[dst] && !CrossRiver(dst, this->player) && this->Board[dst] == 0)){
                        continue;
                    }
                    dst += AdvisorStep[i];
                    pieceDst = this->Board[dst];
                    if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0){
                        moves[NumGenerate++] = Move(src, dst);
                    }
                }
                break;
            case KNIGHT:
                for (int i = 0; i < 4; i++){
                    dst = src + KingStep[i];
                    if (this->Board[dst] != 0){
                        continue;
                    }
                    for (int j = 0; j < 2; j++){
                        dst = src + KnightStep[i][j];
                        if (!InBoard[dst]){
                            continue;
                        }
                        pieceDst = this->Board[dst];
                        if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0){
                            moves[NumGenerate++] = Move(src, dst);
                        }
                    }
                }
                break;
            case ROOK:
                for (int i = 0; i < 4; i++){
                    int delta = KingStep[i];
                    dst = src + delta;
                    while (InBoard[dst]){
                        pieceDst = this->Board[dst];
                        if (pieceDst == 0){
                            if (!OnlyCapture){
                                moves[NumGenerate++] = Move(src, dst);
                            }
                        }
                        else{
                            if ((pieceDst & OppSide) != 0){
                                moves[NumGenerate++] = Move(src, dst);
                            }
                            break;
                        }
                        dst += delta;
                    }
                }
                break;
            case CANNON:
                for (int i = 0; i < 4; i++){
                    int delta = KingStep[i];
                    dst = src + delta;
                    while (InBoard[dst]){
                        pieceDst = this->Board[dst];
                        if (pieceDst == 0){
                            if (!OnlyCapture){
                                moves[NumGenerate++] = Move(src, dst);
                            }
                        }
                        else{
                            break;
                        }
                        dst += delta;
                    }
                    dst += delta;
                    while (InBoard[dst]){
                        pieceDst = this->Board[dst];
                        if (pieceDst != 0){
                            if ((pieceDst & OppSide) != 0){
                                moves[NumGenerate++] = Move(src, dst);
                            }
                            break;
                        }
                        dst += delta;
                    }
                }
                break;
            case PAWN:
                dst = NextPosCol(src, this->player);
                if (InBoard[dst]){
                    pieceDst = this->Board[dst];
                    if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0){
                        moves[NumGenerate] = Move(src, dst);
                        NumGenerate++;
                    }
                }
                if (CrossRiver(src, this->player)){
                    for (int delta = -1; delta <= 1; delta += 2){
                        dst = src + delta;
                        if (InBoard[dst]){
                            pieceDst = this->Board[dst];
                            if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0){
                                moves[NumGenerate] = Move(src, dst);
                                NumGenerate++;
                            }
                        }
                    }
                }
                break;
            }
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
    if ((pieceSrc & SelfSide) == 0){
        return false;
    }

    // 2. 判断目标格是否有自己的棋子
    dst = DstPos(mv);
    pieceDst = this->Board[dst];
    if ((pieceDst & SelfSide) != 0){
        return false;
    }

    // 3. 根据棋子的类型检查走法是否合理
    switch (pieceSrc - SelfSide){
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
        if (SameRow(src, dst)){
            delta = (dst < src ? -1 : 1);
        }
        else if (SameCol(src, dst)){
            delta = (dst < src ? -16 : 16);
        }
        else{
            return false;
        }
        pin = src + delta;
        while (pin != dst && this->Board[pin] == 0){
            pin += delta;
        }
        if (pin == dst){
            return pieceDst == 0 || pieceSrc - SelfSide == ROOK;
        }
        else if (pieceDst != 0 && pieceSrc - SelfSide == CANNON){
            pin += delta;
            while (pin != dst && this->Board[pin] == 0){
                pin += delta;
            }
            return pin == dst;
        }
        else{
            return false;
        }
    case PAWN:
        if (CrossRiver(dst, this->player) && (dst == src - 1 || dst == src + 1)){
            return true;
        }
        return dst == NextPosCol(src, this->player);
    default:
        return false;
    }
}

// 判断是否被将军
bool PositionStruct::Checked() const
{
    int src, dst;
    int SelfSide, OppSide, pieceDst;
    SelfSide = PieceFlag(this->player);
    OppSide = OppPieceFlag(this->player);
    // 找到棋盘上的帅(将)，再做以下判断：

    const int begFortRow = this->player ? RANK_TOP : RANK_BOTTOM - 2;
    const int endFortRow = this->player ? RANK_TOP + 2 : RANK_BOTTOM;
    const int begFortCol = FILE_LEFT + 3;
    const int endFortCol = FILE_RIGHT - 3;

    for (int row = begFortRow; row <= endFortRow; ++row){
        for (int col = begFortCol; col <= endFortCol; ++col){
            src = PositionIndex(col, row);

            if (this->Board[src] != SelfSide + KING)
                continue;

            // 1. 判断是否被对方的兵(卒)将军
            if (this->Board[NextPosCol(src, this->player)] == OppSide + PAWN)
                return true;

            if (this->Board[src - 1] == OppSide + PAWN)
                return true;
            if (this->Board[src + 1] == OppSide + PAWN)
                return true;

            // 2. 判断是否被对方的马将军(以仕(士)的步长当作马腿)
            for (int i = 0; i < 4; i++){
                if (this->Board[src + AdvisorStep[i]]){
                    continue;
                }
                for (int j = 0; j < 2; j++){
                    pieceDst = this->Board[src + KnightCheckStep[i][j]];
                    if (pieceDst == OppSide + KNIGHT){
                        return TRUE;
                    }
                }
            }

            // 3. 判断是否被对方的车或炮将军(包括将帅对脸)
            for (int i = 0; i < 4; i++){
                int delta = KingStep[i];
                dst = src + delta;
                while (InBoard[dst]){
                    pieceDst = this->Board[dst];
                    if (pieceDst != 0){
                        if (pieceDst == OppSide + ROOK || pieceDst == OppSide + KING){
                            return true;
                        }
                        break;
                    }
                    dst += delta;
                }
                dst += delta;
                while (InBoard[dst]){
                    int pieceDst = this->Board[dst];
                    if (pieceDst != 0){
                        if (pieceDst == OppSide + CANNON){
                            return true;
                        }
                        break;
                    }
                    dst += delta;
                }
            }
            return false;
        }
    }
    return false;
}

// 判断是否被杀
bool PositionStruct::IsMate()
{
    int i, nGenMoveNum, pcCaptured;
    int moves[MAX_GEN_MOVES];

    nGenMoveNum = GenerateMoves(moves, false);
    for (i = 0; i < nGenMoveNum; i++){
        pcCaptured = MovePiece(moves[i]);
        if (!Checked()){
            UndoMovePiece(moves[i], pcCaptured);
            return false;
        }
        else{
            UndoMovePiece(moves[i], pcCaptured);
        }
    }
    return true;
}

// 检测重复局面
int PositionStruct::IsRepetitive(int ReLoop)
{
    bool SelfSide, PerpetualCheck, OppPerpetualCheck;
    const MoveInfo *ptrMoves;

    SelfSide = false;
    PerpetualCheck = true;
    OppPerpetualCheck = true;
    ptrMoves = this->AllMoves + this->MoveNum - 1;
    while (ptrMoves->thisMove != 0 && ptrMoves->pieceCaptured == 0){
        if (SelfSide){
            PerpetualCheck = PerpetualCheck && ptrMoves->Check;
            if (ptrMoves->thisKey == zobr.dwKey){
                --ReLoop;
                if (ReLoop == 0){
                    return 1 + (PerpetualCheck ? 2 : 0) + (OppPerpetualCheck ? 4 : 0);
                }
            }
        }
        else{
            OppPerpetualCheck = OppPerpetualCheck && ptrMoves->Check;
        }
        SelfSide = !SelfSide;
        ptrMoves--;
    }
    return 0;
}

void PositionStruct::Mirror(PositionStruct &posMirror)
{
    int posIndex, piece;
    posMirror.ClearBoard();
    for (posIndex = 0; posIndex < 256; posIndex++){
        piece = this->Board[posIndex];
        if (piece){
            posMirror.AddPiece(MirrorPosRow(posIndex), piece);
        }
    }
    if (this->player){
        posMirror.ChangeSide();
    }
    posMirror.InitAllMoves();
}