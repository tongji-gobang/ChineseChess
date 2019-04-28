
// 局面结构
struct PositionStruct {
	int sdPlayer;                   // 轮到谁走，0=红方，1=黑方
	BYTE ucpcSquares[256];          // 棋盘上的棋子
	int vlWhite, vlBlack;           // 红、黑双方的子力价值
	int nDistance;                  // 距离根节点的步数

	void Startup(void);             // 初始化棋盘
	void ChangeSide(void) {         // 交换走子方
		sdPlayer = 1 - sdPlayer;
	}
	void AddPiece(int sq, int pc) { // 在棋盘上放一枚棋子
		ucpcSquares[sq] = pc;
		// 红方加分，黑方(注意"cucvlPiecePos"取值要颠倒)减分
		if (pc < 16) {
			vlWhite += cucvlPiecePos[pc - 8][sq];
		}
		else {
			vlBlack += cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
		}
	}
	void DelPiece(int sq, int pc) { // 从棋盘上拿走一枚棋子
		ucpcSquares[sq] = 0;
		// 红方减分，黑方(注意"cucvlPiecePos"取值要颠倒)加分
		if (pc < 16) {
			vlWhite -= cucvlPiecePos[pc - 8][sq];
		}
		else {
			vlBlack -= cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
		}
	}
	int Evaluate(void) const {      // 局面评价函数
		return (sdPlayer == 0 ? vlWhite - vlBlack : vlBlack - vlWhite) + ADVANCED_VALUE;
	}
	int MovePiece(int mv);                      // 搬一步棋的棋子
	void UndoMovePiece(int mv, int pcCaptured); // 撤消搬一步棋的棋子
	BOOL MakeMove(int mv, int &pcCaptured);     // 走一步棋
	void UndoMakeMove(int mv, int pcCaptured) { // 撤消走一步棋
		nDistance--;
		ChangeSide();
		UndoMovePiece(mv, pcCaptured);
	}
	int GenerateMoves(int *mvs) const;          // 生成所有走法
	BOOL LegalMove(int mv) const;               // 判断走法是否合理
	BOOL Checked(void) const;                   // 判断是否被将军
	BOOL IsMate(void);                          // 判断是否被杀
};