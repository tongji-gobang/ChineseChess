#pragma once
#include "RESOURCE.H"
#include "Zobrist.h"
#include "search.h"
#include "ChessData.h"

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
class PositionStruct
{
public:
	int player;				  // �ֵ�˭�� 0 : �췽��1 : �ڷ�
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

	void AddPiece(int position, int piece); // ���������������
	void DelPiece(int position, int piece); // ɾ�������ϵ�����
	int Evaluate() const;							 // �������ۺ���
	int MovePiece(int move);						 // ��һ���������
	void UndoMovePiece(int move, int pieceCaptured); // ������һ���������
	bool MakeMove(int move, bool change = true);	 // ��һ����
	void UndoMakeMove();  // ������һ����
	int GenerateMoves(int *moves, bool OnlyCapture = false) const;			 // ���������߷�
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

	void Mirror(PositionStruct &posMirror);
};

extern PositionStruct pos;

// ��ø��ӵĺ�����
inline int Row(int posIndex)
{
    return posIndex >> 4; // posIndex / 16, ��Ӧ��ά���̵���
}

// ��ø��ӵ�������
inline int Column(int posIndex)
{
    return posIndex & 15; // posIndex % 16, ��Ӧ��ά���̵���
}

// ����������ͺ�������λ��
inline int PositionIndex(int col, int row)
{
    return col + (row << 4);
}

// ������λ�ö�Ӧ���Է�λ��
inline int CorrespondPos(int posIndex)
{
    return 254 - posIndex;
}

// ������ˮƽ����
inline int MirrorCol(int col)
{
    return 14 - col;
}

// �����괹ֱ����
inline int MirrorRow(int row)
{
    return 15 - row;
}

// ��λ�õ�ͬ�ж�Ӧλ��
inline int MirrorPosRow(int posIndex)
{
    return PositionIndex(MirrorCol(Column(posIndex)), Row(posIndex));
}

// ��λ�õ�ͬ�е���һ��λ��(��˫����˵����ǰ��һ��)
inline int NextPosCol(int posIndex, int player)
{
    return posIndex - 16 + (player << 5);
}

// ��/˧���߷��Ƿ����
inline bool LegalMoveKing(int src, int dst)
{
    return LegalSpan[dst - src + 256] == 1;
}

// �˵��߷��Ƿ����
inline bool LegalMoveAdvisor(int src, int dst)
{
    return LegalSpan[dst - src + 256] == 2;
}

// ��/����߷��Ƿ����
inline bool LegalMoveBishop(int src, int dst)
{
    return LegalSpan[dst - src + 256] == 3;
}

// ��(��)�۵�λ��
inline int BishopCenter(int src, int dst)
{
    return (src + dst) >> 1;
}

// ���ȵ�λ��
inline int KnightPinPos(int src, int dst)
{
    return src + KnightPin[dst - src + 256];
}

// �Ƿ���� true : ���ӣ� false : δ����
inline bool CrossRiver(int posIndex, int player)
{
    // �� 128(0x80) Ϊ��, 0-128 Ϊ 0 ��ң� 128-256Ϊ 1 ���
    return (posIndex & 0x80) == (player << 7);
}

// �Ƿ��ںӵ�ͬһ��
inline bool SameSide(int src, int dst)
{
    // ���ںӵ�ͬһ�ߣ����� 0x80 ��Ӧ��bitλΪ 0
    return ((src ^ dst) & 0x80) == 0;
}

// �Ƿ���ͬһ��
inline bool SameRow(int src, int dst)
{
    // ����ͬһ��, �� 4-7 bitλһ����ͬ
    return ((src ^ dst) & 0xf0) == 0;
}

// �Ƿ���ͬһ��
inline bool SameCol(int src, int dst)
{
    // ����ͬһ��, �� 0-3 bitλһ����ͬ
    return ((src ^ dst) & 0x0f) == 0;
}

// ��ú�ڱ��(������8��������16)
inline int PieceFlag(int player)
{
    return 8 + (player << 3);
}

// ��öԷ���ڱ��
inline int OppPieceFlag(int player)
{
    return 16 - (player << 3);
}

// ����߷������
inline int SrcPos(int move)
{
    // move % 256
    return move & 255;
}
// ����߷����յ�
inline int DstPos(int move)
{
    // move / 256
    return move >> 8;
}

// ���������յ����߷�
inline int Move(int src, int dst)
{
    return src + (dst << 8);
}

// �߷�ˮƽ����
inline int MirrorMove(int move)
{
    return Move(MirrorPosRow(SrcPos(move)), MirrorPosRow(DstPos(move)));
}
