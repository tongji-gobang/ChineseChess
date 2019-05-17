#include "ChessBoard.h"
#include "ChessData.h"
#include "Search.h"
#include <cstring>

CurrentBoard pos;

/**
 * description: 初始化棋盘
 * parameter: void
 * return: void
 */
void CurrentBoard::Startup()
{
    int posIndex, piece;
    // 由于需要计算初始局面的zobrist键值，所以必须清空棋盘
    ClearBoard();
    // 将棋子加入棋盘
    const int begPos = PositionIndex(FILE_LEFT, RANK_TOP);
    const int endPos = PositionIndex(FILE_RIGHT, RANK_BOTTOM);
    for (posIndex = begPos; posIndex <= endPos; ++posIndex) {
        piece = StartupBoard[posIndex];
        if (piece != 0) {
            // 非空棋子加入，在addpiece中计算zobrist键值
            AddPiece(posIndex, piece);
        }
    }
    // 初始化记录历史走法的数组
    this->InitAllMoves();
}

/**
 * description: 向棋盘中添加指定棋子
 * parameter: position - 位置， piece - 棋子
 * return: void
 */
void CurrentBoard::AddPiece(int position, int piece)
{
    // 加入棋子
    this->Board[position] = piece;
    // 计算子力价值 当前局面键值
    if (piece >= 16) {
        this->valueBlack += PiecePosValue[piece - 16][CorrespondPos(position)];
        zobr ^= Zrand.Table[piece - 9][position];
    }
    else {
        valueRed += PiecePosValue[piece - 8][position];
        zobr ^= Zrand.Table[piece - 8][position];
    }
}
/**
 * description: 删除棋盘中指定位置的棋子
 * parameter: pisition - 位置， piece - 棋子(计算子力价值)
 * return: void
 */
void CurrentBoard::DeletePiece(int position, int piece)
{
    // 删除
    this->Board[position] = 0;
    // 重新计算子力价值
    if (piece >= 16) {
        this->valueBlack -= PiecePosValue[piece - 16][CorrespondPos(position)];
        zobr ^= Zrand.Table[piece - 9][position];
    }
    else {
        valueRed -= PiecePosValue[piece - 8][position];
        zobr ^= Zrand.Table[piece - 8][position];
    }
}

/**
 * description: 移动棋子
 * parameter: 走法信息
 * return: 被吃的棋子
 */
int CurrentBoard::MovePiece(int move)
{
    int src, dst, piece, pieceCaptured;
    src = SrcPos(move); // 获得该走法的起点
    dst = DstPos(move); // 获得该走法的终点
    pieceCaptured = this->Board[dst];   // 获取终点的棋子(即被吃的子)
    if (pieceCaptured != 0)    // 如果有有棋子则吃掉(已检查过合理性，不会吃己方棋子)
        this->DeletePiece(dst, pieceCaptured); // 从棋盘上删除棋子
    piece = this->Board[src];   // 获取起点的棋子
    this->DeletePiece(src, piece); // 删除起点棋子
    this->AddPiece(dst, piece); // 将起点的棋子放到终点位置
    return pieceCaptured;
}

/**
 * description: 撤销前一步的走子
 * parameter: move-走法，pieceCaptured-被吃的棋子
 * return: void
 */
void CurrentBoard::UndoMovePiece(int move, int pieceCaptured)
{
    int src, dst, piece;
    src = SrcPos(move);
    dst = DstPos(move);
    piece = Board[dst];         // 获取终点的棋子
    this->DeletePiece(dst, piece); // 删除
    this->AddPiece(src, piece); // 将该棋子放到起点
    if (pieceCaptured != 0)
        AddPiece(dst, pieceCaptured);   // 若被吃子，重置终点棋子
}

/**
 * description: 走一步棋
 * parameter: move-走法，change-是否换到对方(默认为false)
 * return: 若被将军返回false，否则true
 */
bool CurrentBoard::MakeMove(int move, bool change)
{
    int pieceCaptured;
    DWORD key;

    key = zobr.key0;   // 记录当前局面的键值
    pieceCaptured = MovePiece(move);    // 记录被吃子
    if (Checked()) { // 被将军则撤销走子
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

/**
 * description: 撤销上步走法
 * parameter: void
 * return: void
 */
void CurrentBoard::UndoMakeMove()
{
    // 走法数 - 1
    --this->MoveNum;
    // 移动距离 - 1
    --this->RootDistance;
    // 交换走子方
    this->ChangeSide();
    // 取消走子
    this->UndoMovePiece(this->AllMoves[this->MoveNum].thisMove, this->AllMoves[this->MoveNum].pieceCaptured);
}

/**
 * description: 交换走子方 红为0，黑为1
 * parameter: void
 * return: void
 */
void CurrentBoard::ChangeSide()
{
    this->player = !this->player;
    // 每次交换时，局面键值异或当前玩家的局面键值
    zobr ^= Zrand.Player;
}

/**
 * description: 清空棋盘
 * parameter: void
 * return: void
 */
void CurrentBoard::ClearBoard()
{
    // 黑方先走
    this->player = 0;
    // 红方子力价值
    this->valueRed = 0;
    // 黑方子力价值
    this->valueBlack = 0;
    // 已执行走法距根节点的距离
    this->RootDistance = 0;
    // 清空棋盘
    memset(this->Board, 0, sizeof(this->Board));
    // 生成初始密码流(随机数)
    zobr.InitZero();
}

/**
 * description: 初始化(重置)历史走法
 * parameter: void
 * return: void
 */
void CurrentBoard::InitAllMoves()
{
    // 加入空走法
    this->AllMoves[0].push(0, 0, Checked(), zobr.key0);
    // 走法数 + 1
    this->MoveNum = 1;
}
/**
 * description: 对当前局面进行评价
 * parameter: void
 * return: 局面分数
 */
int CurrentBoard::Evaluate() const
{
    // 取子力价值差 并加上先行的优势分值
    return (this->player == 0 ? valueRed - valueBlack : valueBlack - valueRed) + ADVANCED_VALUE;
}
/**
 * description: 判断最后一次走法是否将军
 * parameter: void
 * return: true - 将军， false - 未将军
 */
bool CurrentBoard::LastCheck()
{
    // this->MoveNum 指向下一个还未加入的位置，需 - 1
    return this->AllMoves[this->MoveNum - 1].Check;
}
/**
 * description: 判断最后一步走法是否吃子
 * parameter: void
 * return: void
 */
bool CurrentBoard::Capture() const
{
    return this->AllMoves[this->MoveNum - 1].pieceCaptured != 0;
}
/**
 * description: 执行一步空着(不走棋)
 * parameter: void
 * return: void
 */
void CurrentBoard::UselessMove()
{
    // 记录当前局面键值
    DWORD key = this->zobr.key0;
    // 换走子方
    this->ChangeSide();
    // 加入历史走法
    this->AllMoves[this->MoveNum++].push(0, 0, false, key);
    ++this->RootDistance;
}
/**
 * description: 撤销上步空着
 * parameter: void
 * return: void
 */
void CurrentBoard::UndoUselessMove()
{
    --this->RootDistance;
    --this->MoveNum;
    this->ChangeSide();
}

/**
 * description: 判断是否允许空着
 * parameter: void
 * return: true OR false
 */
bool CurrentBoard::CanUselessMove()
{
    return (this->player ? valueBlack : valueRed) > NULL_MARGIN;
}
/**
 * description: 计算平局分值
 * parameter: void
 * return: 平局分值
 */
int CurrentBoard::DrawValue()
{
    // 红方先行(分值越大越好)，黑方越小越好
    // 若 this->RootDistance 为偶数, 返回最小
    return (this->RootDistance & 1) == 0 ? -DRAW_VALUE : DRAW_VALUE;
}

/**
 * description: 生成所有走法
 * parameter: moves - 存储生成的走法， OnlyCapture - 是否只生成吃子走法(默认为false)
 * return: 生成的走法数
 */
int CurrentBoard::GenerateMoves(int* moves, bool OnlyCapture) const
{
    int NumGenerate, src, dst;
    int SelfSide, OppSide, pieceSrc, pieceDst;
    // 生成所有走法，需要经过以下几个步骤：

    // 生成的走法数
    NumGenerate = 0;
    // 获取本方棋子标记
    SelfSide = PieceFlag(this->player);
    // 获取对方棋子标记
    OppSide = OppPieceFlag(this->player);
    // 将搜索范围限制在棋盘内部
    for (int row = RANK_TOP; row <= RANK_BOTTOM; ++row) {
        for (int col = FILE_LEFT; col <= FILE_RIGHT; ++col) {
            // 获取行列对应的数组位置
            src = PositionIndex(col, row);

            // 寻找本方棋子
            pieceSrc = this->Board[src];
            // 若该位置为空或为对方棋子，继续寻找
            if ((pieceSrc & SelfSide) == 0)
                continue;

            // 生成走法
            switch (pieceSrc - SelfSide) {
                case KING:      // 将/帅
                    for (int i = 0; i < 4; i++) {
                        dst = src + KingStep[i];    // 四个方向可走
                        if (!InFort[dst])           // 若不在九宫格内，继续找
                            continue;
                        pieceDst = this->Board[dst]; // 获取终点棋子
                        // 存储走法(若仅生成吃子走法, 需额外判断目标位置的棋子)
                        if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0)
                            moves[NumGenerate++] = Move(src, dst);
                    }
                    break;
                case ADVISOR:   // 仕/士
                    for (int i = 0; i < 4; i++) {
                        dst = src + AdvisorStep[i]; // 四个方向
                        if (!InFort[dst])   // 必须在九宫格内
                            continue;
                        pieceDst = this->Board[dst];
                        if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0)
                            moves[NumGenerate++] = Move(src, dst);
                    }
                    break;
                case BISHOP:    // 相/象
                    for (int i = 0; i < 4; i++) {
                        dst = src + AdvisorStep[i]; // 相眼位置
                        // 相眼必须在棋盘内，未过河且无棋子
                        if (!(InBoard[dst] && !CrossRiver(dst, this->player) && this->Board[dst] == 0))
                            continue;
                        dst += AdvisorStep[i];
                        pieceDst = this->Board[dst];
                        if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0)
                            moves[NumGenerate++] = Move(src, dst);
                    }
                    break;
                case KNIGHT:    // 马
                    for (int i = 0; i < 4; i++) {
                        dst = src + KingStep[i];    // 马腿位置
                        if (this->Board[dst] != 0)  // 马腿必须为空
                            continue;
                        for (int j = 0; j < 2; j++) {
                            dst = src + KnightStep[i][j]; // 目标位置
                            if (!InBoard[dst])
                                continue;
                            pieceDst = this->Board[dst];
                            if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0)
                                moves[NumGenerate++] = Move(src, dst);
                        }
                    }
                    break;
                case ROOK:      // 车
                    for (int i = 0; i < 4; i++) {
                        int delta = KingStep[i];    // 每次走一步
                        dst = src + delta;
                        while (InBoard[dst]) {       // 直到不在棋盘中
                            pieceDst = this->Board[dst];
                            if (pieceDst == 0) {     // 位置为空
                                if (!OnlyCapture)
                                    moves[NumGenerate++] = Move(src, dst);
                            }
                            else {
                                // 位置不为空必须吃子
                                if ((pieceDst & OppSide) != 0)
                                    moves[NumGenerate++] = Move(src, dst);
                                break;
                            }
                            dst += delta;   // 每次走一步
                        }
                    }
                    break;
                case CANNON:    // 炮
                    for (int i = 0; i < 4; i++) {
                        int delta = KingStep[i];    // 每次走一步
                        dst = src + delta;
                        while (InBoard[dst]) {
                            pieceDst = this->Board[dst];
                            if (pieceDst == 0) {
                                if (!OnlyCapture)
                                    moves[NumGenerate++] = Move(src, dst);
                            }
                            else   // 不为空(以此位置为炮架)
                                break;
                            dst += delta;
                        }
                        dst += delta;
                        while (InBoard[dst]) {
                            pieceDst = this->Board[dst];
                            if (pieceDst != 0) {
                                // 已有炮架，必须吃子
                                if ((pieceDst & OppSide) != 0)
                                    moves[NumGenerate++] = Move(src, dst);
                                break;
                            }
                            dst += delta;
                        }
                    }
                    break;
                case PAWN:      // 兵/卒
                    dst = NextPosCol(src, this->player);    // 只能向前走(此时默认未过河)
                    if (InBoard[dst]) {
                        pieceDst = this->Board[dst];
                        if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0)
                            moves[NumGenerate++] = Move(src, dst);
                    }
                    // 若已过河，可左右走
                    if (CrossRiver(src, this->player)) {
                        for (int delta = -1; delta <= 1; delta += 2) {
                            dst = src + delta;
                            if (InBoard[dst]) {
                                pieceDst = this->Board[dst];
                                if (OnlyCapture ? (pieceDst & OppSide) != 0 : (pieceDst & SelfSide) == 0)
                                    moves[NumGenerate++] = Move(src, dst);
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
    return NumGenerate;
}

/**
 * description: 判断走法是否合理
 * parameter: mv - 走法
 * return: true OR false
 */
bool CurrentBoard::LegalMove(int mv) const
{
    int src, dst, pin;
    int SelfSide, pieceSrc, pieceDst, delta;

    // 起点是否是自己的棋子
    src = SrcPos(mv);
    pieceSrc = this->Board[src];
    SelfSide = PieceFlag(this->player);
    if ((pieceSrc & SelfSide) == 0)
        return false;

    // 目标位置是否为空或是对分棋子
    dst = DstPos(mv);
    pieceDst = this->Board[dst];
    if ((pieceDst & SelfSide) != 0)
        return false;

    switch (pieceSrc - SelfSide) {
        case KING:      // 将必须在九宫格内且只能向四个方向移动一步
            return InFort[dst] && LegalMoveKing(src, dst);
        case ADVISOR:   // 仕必须在九宫格内且只能向四个方向移动一步
            return InFort[dst] && LegalMoveAdvisor(src, dst);
        case BISHOP:    // 相必须未过河且符合田字走法，相眼为空
            return !CrossRiver(src, dst) && LegalMoveBishop(src, dst) &&
                this->Board[BishopCenter(src, dst)] == 0;
        case KNIGHT:    // 马腿必须为空且符合日字走法
            pin = KnightPinPos(src, dst);
            return pin != src && this->Board[pin] == 0;
        case ROOK:      // 车和炮的移动必须在同行/列
        case CANNON:
            if (SameRow(src, dst))
                delta = (dst < src ? -1 : 1);
            else if (SameCol(src, dst))
                delta = (dst < src ? -16 : 16);
            else
                return false;
            pin = src + delta;
            while (pin != dst && this->Board[pin] == 0)
                pin += delta;
            // 已到目标位置
            if (pin == dst)
                // 目标位置为空 或者是车的移动
                return pieceDst == 0 || pieceSrc - SelfSide == ROOK;
            // 否则判断炮(还未到目标位置)
            else if (pieceDst != 0 && pieceSrc - SelfSide == CANNON) {
                // 此时目标位置一定为对方棋子
                pin += delta;
                while (pin != dst && this->Board[pin] == 0)
                    pin += delta;
                return pin == dst;
            }
            else
                return false;
        case PAWN:
            // 若已过河 可左右移动
            if (CrossRiver(dst, this->player) && (dst == src - 1 || dst == src + 1))
                return true;
            // 否则只能向前
            return dst == NextPosCol(src, this->player);
        default:
            return false;
    }
    return false;
}

/**
 * description: 判断是否被将军
 * parameter: void
 * return: bool
 */
bool CurrentBoard::Checked() const
{
    int src, dst;
    int SelfSide, OppSide, pieceDst;
    // 本方和对方的棋子标记
    SelfSide = PieceFlag(this->player);
    OppSide = OppPieceFlag(this->player);

    // 在九宫格内查找
    const int begFortRow = this->player ? RANK_TOP : RANK_BOTTOM - 2;
    const int endFortRow = this->player ? RANK_TOP + 2 : RANK_BOTTOM;
    const int begFortCol = FILE_LEFT + 3;
    const int endFortCol = FILE_RIGHT - 3;

    for (int row = begFortRow; row <= endFortRow; ++row) {
        for (int col = begFortCol; col <= endFortCol; ++col) {
            src = PositionIndex(col, row);
            // 先找到自己的将/帅
            if (this->Board[src] != SelfSide + KING)
                continue;
            // 若被兵/卒将军
            if (this->Board[NextPosCol(src, this->player)] == OppSide + PAWN)
                return true;
            if (this->Board[src - 1] == OppSide + PAWN)
                return true;
            if (this->Board[src + 1] == OppSide + PAWN)
                return true;
            // 被马将军
            for (int i = 0; i < 4; i++) {
                // 马腿位置必须为空
                if (this->Board[src + AdvisorStep[i]])
                    continue;
                for (int j = 0; j < 2; j++) {
                    pieceDst = this->Board[src + KnightCheckStep[i][j]];
                    if (pieceDst == OppSide + KNIGHT)
                        return true;
                }
            }
            // 被车/炮/将/帅将军
            for (int i = 0; i < 4; i++) {
                int delta = KingStep[i];
                dst = src + delta;
                while (InBoard[dst]) {
                    pieceDst = this->Board[dst];
                    if (pieceDst != 0) {
                        // 此时被车/帅/将将军
                        if (pieceDst == OppSide + ROOK || pieceDst == OppSide + KING)
                            return true;
                        break;
                    }
                    dst += delta;
                }
                dst += delta;
                while (InBoard[dst]) {
                    int pieceDst = this->Board[dst];
                    if (pieceDst != 0) {
                        // 被炮将军
                        if (pieceDst == OppSide + CANNON)
                            return true;
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

/**
 * description: 是否被将死
 * parameter: void
 * return: bool
 */
bool CurrentBoard::IsMating()
{
    int i, nGenMoveNum, pcCaptured;
    int moves[MAX_GEN_MOVES];

    // 先生成所有走法
    nGenMoveNum = GenerateMoves(moves, false);
    for (i = 0; i < nGenMoveNum; i++) {
        pcCaptured = MovePiece(moves[i]);
        // 如果存在一步活棋，返回false
        if (!Checked()) {
            UndoMovePiece(moves[i], pcCaptured);
            return false;
        }
        else
            UndoMovePiece(moves[i], pcCaptured);
    }
    // 到这里表示必死
    return true;
}

/**
 * description: 计算重复局面分数
 * parameter: ReNum --- IsRepetitive 的返回值，表示双方是否长将
 * return: 重复局面的分值
 */
int CurrentBoard::RepeatValue(int ReNum)
{
    int value;
    if ((ReNum & 2) == 0)
        value = 0;      // 此时对方长将
    else                // 否则己方长将
        value = this->RootDistance - BAN_VALUE;

    if ((ReNum & 4) == 0)
        value += 0;     // 此时本方长将
    else                // 对方长将
        value += BAN_VALUE - this->RootDistance;

    // 若分数不为零 表示某方单方面长将，否则喝起
    return value ? value : this->DrawValue();
}

/**
 * description: 判断局面是否重复
 * parameter: ReLoop - 以几次局面重复表示最终局面重复信息
 * return: 双方长将信息
 */
int CurrentBoard::IsRepetitive(int ReLoop)
{
    bool SelfSide, PerpetualCheck, OppPerpetualCheck;
    const MoveInfo* ptrMoves;

    // 先从对方开始
    SelfSide = false;
    // 此时默认双方均长将
    PerpetualCheck = true;
    OppPerpetualCheck = true;
    // 获取最后一步走法
    ptrMoves = this->AllMoves + this->MoveNum - 1;
    // 走法不为空且未吃子
    while (ptrMoves->thisMove != 0 && ptrMoves->pieceCaptured == 0) {
        if (SelfSide) {
            PerpetualCheck = PerpetualCheck && ptrMoves->Check;
            if (ptrMoves->thisKey == zobr.key0) {   // 局面重复
                --ReLoop;
                if (!ReLoop)
                    return 1 + (PerpetualCheck ? 2 : 0) + (OppPerpetualCheck ? 4 : 0);
            }
        }
        else
            OppPerpetualCheck = OppPerpetualCheck && ptrMoves->Check;
        SelfSide = !SelfSide;
        ptrMoves--;     // 回溯走法
    }
    return 0;
}

/**
 * description: 求当前局面的镜像
 * parameter: posMirror - 存储镜像局面
 * return: void
 */
void CurrentBoard::Mirror(CurrentBoard & posMirror)
{
    // 求当前局面的镜像
    int posIndex, piece;
    posMirror.ClearBoard();
    for (posIndex = 0; posIndex < 256; posIndex++) {
        piece = this->Board[posIndex];
        if (piece)
            posMirror.AddPiece(MirrorPosRow(posIndex), piece);
    }
    if (this->player)
        posMirror.ChangeSide();
    posMirror.InitAllMoves();
}