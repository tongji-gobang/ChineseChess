#pragma once
#include "RESOURCE.H"
#include "Zobrist.h"
#include "search.h"
#include "ChessData.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void readRegmark(char* regmark); //这里写函数声明

#ifdef __cplusplus
}
#endif

struct MoveInfo
{
    int thisMove;		// 本次走法
    BYTE pieceCaptured; // 被吃的棋子
    bool Check;			// 是否被将军
    DWORD thisKey;		// 此走法对应局面的键值

    void push(int move, int pcCap, bool check, DWORD key)
    {
        this->thisMove = move;
        this->pieceCaptured = pcCap;
        this->Check = check;
        this->thisKey = key;
    }
};
// 局面结构
class CurrentBoard
{
public:
    int player;				  // 轮到谁走 0 : 红方，1 : 黑方
    BYTE Board[256];			  // 棋盘上的棋子
    int valueRed, valueBlack;	 // 红、黑双方的子力价值
    int RootDistance;			  // 距离根节点的步数
    int MoveNum;				  // 已走的步数
    MoveInfo AllMoves[MAX_MOVES]; // 所有已走过的走法
    ZobristStruct zobr;

    void ClearBoard();   //清空棋盘
    void InitAllMoves(); // 清空 AllMoves
    void Startup();		 // 初始化棋盘
    void ChangeSide();   // 交换走子方

    void AddPiece(int position, int piece); // 给棋盘上添加棋子
    void DeletePiece(int position, int piece); // 删除棋盘上的棋子
    int Evaluate() const;							 // 局面评价函数
    int MovePiece(int move);						 // 搬一步棋的棋子
    void UndoMovePiece(int move, int pieceCaptured); // 撤消搬一步棋的棋子
    bool MakeMove(int move, bool change = true);	 // 走一步棋
    void UndoMakeMove();  // 撤消走一步棋
    int GenerateMoves(int* moves, bool OnlyCapture = false) const;			 // 生成所有走法
    bool LegalMove(int move) const;					 // 判断走法是否合理
    bool Checked() const;							 // 判断是否被将军
    bool IsMating();									 // 判断是否被杀
    bool Capture() const;							 // 是否吃子

    void UselessMove();				  // 空着
    void UndoUselessMove();			  // 撤销空着
    bool LastCheck();				  // 最后一步是否将军
    int DrawValue();				  // 平局分数
    int IsRepetitive(int ReLoop = 1); // 检测重复局面
    int RepeatValue(int ReNum);		  //重复局面的分数
    bool CanUselessMove();				  // 是否可空着搜索

    void Mirror(CurrentBoard& posMirror);
};

extern CurrentBoard pos;

// 获得格子的横坐标
inline int Row(int posIndex)
{
    return posIndex >> 4; // posIndex / 16, 对应二维棋盘的行
}

// 获得格子的纵坐标
inline int Column(int posIndex)
{
    return posIndex & 15; // posIndex % 16, 对应二维棋盘的列
}

// 根据纵坐标和横坐标获得位置
inline int PositionIndex(int col, int row)
{
    return col + (row << 4);
}

// 将己方位置对应到对方位置
inline int CorrespondPos(int posIndex)
{
    return 254 - posIndex;
}

// 纵坐标水平镜像
inline int MirrorCol(int col)
{
    return 14 - col;
}

// 横坐标垂直镜像
inline int MirrorRow(int row)
{
    return 15 - row;
}

// 该位置的同行对应位置
inline int MirrorPosRow(int posIndex)
{
    return PositionIndex(MirrorCol(Column(posIndex)), Row(posIndex));
}

// 该位置的同列的下一个位置(对双方来说均是前进一行)
inline int NextPosCol(int posIndex, int player)
{
    return posIndex - 16 + (player << 5);
}

// 将/帅的走法是否合理
inline bool LegalMoveKing(int src, int dst)
{
    return LegalSpan[dst - src + 256] == 1;
}

// 仕的走法是否合理
inline bool LegalMoveAdvisor(int src, int dst)
{
    return LegalSpan[dst - src + 256] == 2;
}

// 相/象的走法是否合理
inline bool LegalMoveBishop(int src, int dst)
{
    return LegalSpan[dst - src + 256] == 3;
}

// 相(象)眼的位置
inline int BishopCenter(int src, int dst)
{
    return (src + dst) >> 1;
}

// 马腿的位置
inline int KnightPinPos(int src, int dst)
{
    return src + KnightPin[dst - src + 256];
}

// 是否过河 true : 过河， false : 未过河
inline bool CrossRiver(int posIndex, int player)
{
    // 以 128(0x80) 为界, 0-128 为 0 玩家， 128-256为 1 玩家
    return (posIndex & 0x80) == (player << 7);
}

// 是否在河的同一边
inline bool SameSide(int src, int dst)
{
    // 若在河的同一边，异或后 0x80 对应的bit位为 0
    return ((src ^ dst) & 0x80) == 0;
}

// 是否在同一行
inline bool SameRow(int src, int dst)
{
    // 若在同一行, 第 4-7 bit位一定相同
    return ((src ^ dst) & 0xf0) == 0;
}

// 是否在同一列
inline bool SameCol(int src, int dst)
{
    // 若在同一行, 第 0-3 bit位一定相同
    return ((src ^ dst) & 0x0f) == 0;
}

// 获得红黑标记(红子是8，黑子是16)
inline int PieceFlag(int player)
{
    return 8 + (player << 3);
}

// 获得对方红黑标记
inline int OppPieceFlag(int player)
{
    return 16 - (player << 3);
}

// 获得走法的起点
inline int SrcPos(int move)
{
    // move % 256
    return move & 255;
}
// 获得走法的终点
inline int DstPos(int move)
{
    // move / 256
    return move >> 8;
}

// 根据起点和终点获得走法
inline int Move(int src, int dst)
{
    return src + (dst << 8);
}

// 走法水平镜像
inline int MirrorMove(int move)
{
    return Move(MirrorPosRow(SrcPos(move)), MirrorPosRow(DstPos(move)));
}
