#include"RESOURCE.H"


#ifdef __cplusplus
extern "C" {
#endif

	void readRegmark(char *regmark);  //����д��������

#ifdef __cplusplus
}
#endif
// ����ṹ
struct PositionStruct {
	int sdPlayer;                   // �ֵ�˭�ߣ�0=�췽��1=�ڷ�
	BYTE ucpcSquares[256];          // �����ϵ�����
	int vlWhite, vlBlack;           // �졢��˫����������ֵ
	int nDistance;                  // ������ڵ�Ĳ���

	void Startup(void);             // ��ʼ������
	void ChangeSide(void);         // �������ӷ�

	void AddPiece(int sq, int pc);// �������Ϸ�һö���� 
	void DelPiece(int sq, int pc);// ������������һö���� 
	void AddDelPiece(int Src, int Dst, int pc);
	int Evaluate(void) const;// �������ۺ��� 
	int MovePiece(int mv);                      // ��һ���������
	void UndoMovePiece(int mv, int pcCaptured); // ������һ���������
	bool MakeMove(int mv, int &pcCaptured);     // ��һ����
	void UndoMakeMove(int mv, int pcCaptured); // ������һ����
	int GenerateMoves(int *mvs) const;          // ���������߷�
	bool LegalMove(int mv) const;               // �ж��߷��Ƿ����
	bool Checked() const;                   // �ж��Ƿ񱻽���
	bool IsMate(void);                          // �ж��Ƿ�ɱ
};

extern PositionStruct pos;

// �ж������Ƿ���������
bool IN_BOARD(int sq);

// �ж������Ƿ��ھŹ���
bool IN_FORT(int sq);

// ��ø��ӵĺ�����
int RANK_Y(int sq);

// ��ø��ӵ�������
int FILE_X(int sq);

// ����������ͺ������ø���
int COORD_XY(int x, int y);

// ��ת����
int SQUARE_FLIP(int sq);

// ������ˮƽ����
int FILE_FLIP(int x);

// �����괹ֱ����
int RANK_FLIP(int y);

// ����ˮƽ����
int MIRROR_SQUARE(int sq);

// ����ˮƽ����
int SQUARE_FORWARD(int sq, int sd);

// �߷��Ƿ����˧(��)�Ĳ���
bool KING_SPAN(int sqSrc, int sqDst);

// �߷��Ƿ������(ʿ)�Ĳ���
bool ADVISOR_SPAN(int sqSrc, int sqDst);

// �߷��Ƿ������(��)�Ĳ���
bool BISHOP_SPAN(int sqSrc, int sqDst);

// ��(��)�۵�λ��
int BISHOP_PIN(int sqSrc, int sqDst);

// ���ȵ�λ��
int KNIGHT_PIN(int sqSrc, int sqDst);

// �Ƿ�δ����
bool HOME_HALF(int sq, int sd);

// �Ƿ��ѹ���
bool AWAY_HALF(int sq, int sd);

// �Ƿ��ںӵ�ͬһ��
bool SAME_HALF(int sqSrc, int sqDst);

// �Ƿ���ͬһ��
bool SAME_RANK(int sqSrc, int sqDst);

// �Ƿ���ͬһ��
bool SAME_FILE(int sqSrc, int sqDst);

// ��ú�ڱ��(������8��������16)
int SIDE_TAG(int sd);

// ��öԷ���ڱ��
int OPP_SIDE_TAG(int sd);

// ����߷������
int SRC(int mv);

// ����߷����յ�
int DST(int mv);

// ���������յ����߷�
int MOVE(int sqSrc, int sqDst);

// �߷�ˮƽ����
int MIRROR_MOVE(int mv);

