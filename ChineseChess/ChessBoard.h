#include"RESOURCE.H"


#ifdef __cplusplus
extern "C" {
#endif

	void readRegmark(char *regmark);  //这里写函数声明

#ifdef __cplusplus
}
#endif
// 局面结构
struct PositionStruct {
	int sdPlayer;                   // 轮到谁走，0=红方，1=黑方
	BYTE ucpcSquares[256];          // 棋盘上的棋子
	int vlWhite, vlBlack;           // 红、黑双方的子力价值
	int nDistance;                  // 距离根节点的步数

	void Startup(void);             // 初始化棋盘
	void ChangeSide(void);         // 交换走子方

	void AddPiece(int sq, int pc);// 在棋盘上放一枚棋子 
	void DelPiece(int sq, int pc);// 从棋盘上拿走一枚棋子 
	int Evaluate(void) const;// 局面评价函数 
	int MovePiece(int mv);                      // 搬一步棋的棋子
	void UndoMovePiece(int mv, int pcCaptured); // 撤消搬一步棋的棋子
	BOOL MakeMove(int mv, int &pcCaptured);     // 走一步棋
	void UndoMakeMove(int mv, int pcCaptured); // 撤消走一步棋
	int GenerateMoves(int *mvs) const;          // 生成所有走法
	BOOL LegalMove(int mv) const;               // 判断走法是否合理
	BOOL Checked(void) const;                   // 判断是否被将军
	BOOL IsMate(void);                          // 判断是否被杀
};

extern PositionStruct pos;

// 判断棋子是否在棋盘中
BOOL IN_BOARD(int sq);

// 判断棋子是否在九宫中
BOOL IN_FORT(int sq);

// 获得格子的横坐标
 int RANK_Y(int sq);

// 获得格子的纵坐标
 int FILE_X(int sq);

// 根据纵坐标和横坐标获得格子
 int COORD_XY(int x, int y);

// 翻转格子
int SQUARE_FLIP(int sq);

// 纵坐标水平镜像
 int FILE_FLIP(int x);

// 横坐标垂直镜像
 int RANK_FLIP(int y);

// 格子水平镜像
 int MIRROR_SQUARE(int sq);

// 格子水平镜像
 int SQUARE_FORWARD(int sq, int sd);

// 走法是否符合帅(将)的步长
BOOL KING_SPAN(int sqSrc, int sqDst);

// 走法是否符合仕(士)的步长
 BOOL ADVISOR_SPAN(int sqSrc, int sqDst);

// 走法是否符合相(象)的步长
 BOOL BISHOP_SPAN(int sqSrc, int sqDst);

// 相(象)眼的位置
int BISHOP_PIN(int sqSrc, int sqDst);

// 马腿的位置
int KNIGHT_PIN(int sqSrc, int sqDst);

// 是否未过河
BOOL HOME_HALF(int sq, int sd);

// 是否已过河
 BOOL AWAY_HALF(int sq, int sd);

// 是否在河的同一边
 BOOL SAME_HALF(int sqSrc, int sqDst);

// 是否在同一行
BOOL SAME_RANK(int sqSrc, int sqDst);

// 是否在同一列
 BOOL SAME_FILE(int sqSrc, int sqDst);

// 获得红黑标记(红子是8，黑子是16)
int SIDE_TAG(int sd);

// 获得对方红黑标记
int OPP_SIDE_TAG(int sd);

// 获得走法的起点
int SRC(int mv);

// 获得走法的终点
int DST(int mv);

// 根据起点和终点获得走法
int MOVE(int sqSrc, int sqDst);

// 走法水平镜像
int MIRROR_MOVE(int mv);

