#include "RESOURCE.H"
#include "Zobrist.h"

#ifdef __cplusplus
extern "C"
{
#endif

	void readRegmark(char *regmark); //这里写函数声明

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
struct PositionStruct
{
	int sdPlayer;				  // 轮到谁走 0 : 红方，1 : 黑方
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

	void AddPiece(int pos, int piece); // 给棋盘上添加棋子
	void DelPiece(int pos, int piece); // 删除棋盘上的棋子
	int Evaluate() const;							 // 局面评价函数
	int MovePiece(int move);						 // 搬一步棋的棋子
	void UndoMovePiece(int move, int PieceCaptured); // 撤消搬一步棋的棋子
	bool MakeMove(int move);	 // 走一步棋
	void UndoMakeMove(int move, int PieceCaptured);  // 撤消走一步棋
	int GenerateMoves(int *moves, bool OnlyCapture) const;			 // 生成所有走法
	bool LegalMove(int move) const;					 // 判断走法是否合理
	bool Checked() const;							 // 判断是否被将军
	bool IsMate();									 // 判断是否被杀
	bool Captured() const;							 // 是否吃子

	void MoveNull();				  // 空着
	void UndoMoveNull();			  // 撤销空着
	bool LastCheck();				  // 最后一步是否将军
	int DrawValue();				  // 平局分数
	int IsRepetitive(int ReLoop = 1); // 检测重复局面
	int RepeatValue(int ReNum);		  //重复局面的分数
	bool NullOkay();				  // 是否可空着搜索
};

extern PositionStruct pos;

// 获得格子的横坐标
int Row(int posIndex);
// 获得格子的纵坐标
int Column(int posIndex);
// 根据纵坐标和横坐标获得位置
int PositionIndex(int col, int row);
// 将己方位置对应到对方位置
int CorrespondPos(int posIndex);
// 纵坐标水平镜像
int MirrorCol(int col);
// 横坐标垂直镜像
int MirrorRow(int row);
// 该位置的同行对应位置
int MirrorPosRow(int posIndex);
// 该位置的同列的下一个位置(对双方来说均是前进一行)
int NextPosCol(int posIndex, int player);
// 将/帅的走法是否合理
bool LegalMoveKing(int src, int dst);
// 仕的走法是否合理
bool LegalMoveAdvisor(int src, int dst);
// 相/象的走法是否合理
bool LegalMoveBishop(int src, int dst);
// 相(象)眼的位置
int BishopCenter(int src, int dst);
// 马腿的位置
int KnightPinPos(int src, int dst);
// 是否过河 true : 过河， false : 未过河
bool CrossRiver(int posIndex, int player);
// 是否在河的同一边
bool SameSide(int src, int dst);
// 是否在同一行
bool SameRow(int src, int dst);
// 是否在同一列
bool SameCol(int src, int dst);
// 获得红黑标记(红子是8，黑子是16)
int PieceFlag(int player);
// 获得对方红黑标记
int OppPieceFlag(int player);
// 获得走法的起点
int SrcPos(int move);
// 获得走法的终点
int DstPos(int move);
// 根据起点和终点获得走法
int Move(int src, int dst);
// 走法水平镜像
int MirrorMove(int move);