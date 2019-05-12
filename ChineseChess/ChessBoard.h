#include "RESOURCE.H"
#include "Zobrist.h"

#ifdef __cplusplus
extern "C"
{
#endif

	void readRegmark(char *regmark); //����д��������

#ifdef __cplusplus
}
#endif

struct MoveInfo
{
	int thisMove;		// �����߷�
	BYTE pieceCaptured; // ���Ե�����
	bool Check;			// �Ƿ񱻽���
	DWORD thisKey;		// ���߷���Ӧ����ļ�ֵ

	void push(int move, int pcCap, bool check, DWORD key)
	{
		this->thisMove = move;
		this->pieceCaptured = pcCap;
		this->Check = check;
		this->thisKey = key;
	}
};
// ����ṹ
struct PositionStruct
{
	int sdPlayer;				  // �ֵ�˭�� 0 : �췽��1 : �ڷ�
	BYTE Board[256];			  // �����ϵ�����
	int valueRed, valueBlack;	 // �졢��˫����������ֵ
	int RootDistance;			  // ������ڵ�Ĳ���
	int MoveNum;				  // ���ߵĲ���
	MoveInfo AllMoves[MAX_MOVES]; // �������߹����߷�
	ZobristStruct zobr;

	void ClearBoard();   //�������
	void InitAllMoves(); // ��� AllMoves
	void Startup();		 // ��ʼ������
	void ChangeSide();   // �������ӷ�

	void AddPiece(int pos, int piece); // ���������������
	void DelPiece(int pos, int piece); // ɾ�������ϵ�����
	void AddDelPiece(int src, int dst, int piece);	
	int Evaluate() const;							 // �������ۺ���
	int MovePiece(int move);						 // ��һ���������
	void UndoMovePiece(int move, int PieceCaptured); // ������һ���������
	bool MakeMove(int move, int &PieceCaptured);	 // ��һ����
	void UndoMakeMove(int move, int PieceCaptured);  // ������һ����
	int GenerateMoves(int *moves) const;			 // ���������߷�
	bool LegalMove(int move) const;					 // �ж��߷��Ƿ����
	bool Checked() const;							 // �ж��Ƿ񱻽���
	bool IsMate();									 // �ж��Ƿ�ɱ
	bool Captured() const;							 // �Ƿ����

	void MoveNull();				  // ����
	void UndoMoveNull();			  // ��������
	bool LastCheck();				  // ���һ���Ƿ񽫾�
	int DrawValue();				  // ƽ�ַ���
	int IsRepetitive(int ReLoop = 1); // ����ظ�����
	int RepeatValue(int ReNum);		  //�ظ�����ķ���
	bool NullOkay();				  // �Ƿ�ɿ�������
};

extern PositionStruct pos;

// ��ø��ӵĺ�����
int Row(int posIndex);
// ��ø��ӵ�������
int Column(int posIndex);
// ����������ͺ�������λ��
int PositionIndex(int col, int row);
// ������λ�ö�Ӧ���Է�λ��
int CorrespondPos(int posIndex);
// ������ˮƽ����
int MirrorCol(int col);
// �����괹ֱ����
int MirrorRow(int row);
// ��λ�õ�ͬ�ж�Ӧλ��
int MirrorPosRow(int posIndex);
// ��λ�õ�ͬ�е���һ��λ��(��˫����˵����ǰ��һ��)
int NextPosCol(int posIndex, int player);
// ��/˧���߷��Ƿ����
bool LegalMoveKing(int src, int dst);
// �˵��߷��Ƿ����
bool LegalMoveAdvisor(int src, int dst);
// ��/����߷��Ƿ����
bool LegalMoveBishop(int src, int dst);
// ��(��)�۵�λ��
int BishopCenter(int src, int dst);
// ���ȵ�λ��
int KNIGHT_PIN(int src, int dst);
// �Ƿ���� true : ���ӣ� false : δ����
bool CrossRiver(int posIndex, int player);
// �Ƿ��ںӵ�ͬһ��
bool SameSide(int src, int dst);
// �Ƿ���ͬһ��
bool SameRow(int src, int dst);
// �Ƿ���ͬһ��
bool SameCol(int src, int dst);
// ��ú�ڱ��(������8��������16)
int PieceFlag(int player);
// ��öԷ���ڱ��
int OppPieceFlag(int player);
// ����߷������
int SrcPos(int move);
// ����߷����յ�
int DstPos(int move);
// ���������յ����߷�
int Move(int src, int dst);
// �߷�ˮƽ����
int MirrorMove(int move);