
// ����ṹ
struct PositionStruct {
	int sdPlayer;                   // �ֵ�˭�ߣ�0=�췽��1=�ڷ�
	BYTE ucpcSquares[256];          // �����ϵ�����
	int vlWhite, vlBlack;           // �졢��˫����������ֵ
	int nDistance;                  // ������ڵ�Ĳ���

	void Startup(void);             // ��ʼ������
	void ChangeSide(void) {         // �������ӷ�
		sdPlayer = 1 - sdPlayer;
	}
	void AddPiece(int sq, int pc) { // �������Ϸ�һö����
		ucpcSquares[sq] = pc;
		// �췽�ӷ֣��ڷ�(ע��"cucvlPiecePos"ȡֵҪ�ߵ�)����
		if (pc < 16) {
			vlWhite += cucvlPiecePos[pc - 8][sq];
		}
		else {
			vlBlack += cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
		}
	}
	void DelPiece(int sq, int pc) { // ������������һö����
		ucpcSquares[sq] = 0;
		// �췽���֣��ڷ�(ע��"cucvlPiecePos"ȡֵҪ�ߵ�)�ӷ�
		if (pc < 16) {
			vlWhite -= cucvlPiecePos[pc - 8][sq];
		}
		else {
			vlBlack -= cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
		}
	}
	int Evaluate(void) const {      // �������ۺ���
		return (sdPlayer == 0 ? vlWhite - vlBlack : vlBlack - vlWhite) + ADVANCED_VALUE;
	}
	int MovePiece(int mv);                      // ��һ���������
	void UndoMovePiece(int mv, int pcCaptured); // ������һ���������
	BOOL MakeMove(int mv, int &pcCaptured);     // ��һ����
	void UndoMakeMove(int mv, int pcCaptured) { // ������һ����
		nDistance--;
		ChangeSide();
		UndoMovePiece(mv, pcCaptured);
	}
	int GenerateMoves(int *mvs) const;          // ���������߷�
	BOOL LegalMove(int mv) const;               // �ж��߷��Ƿ����
	BOOL Checked(void) const;                   // �ж��Ƿ񱻽���
	BOOL IsMate(void);                          // �ж��Ƿ�ɱ
};