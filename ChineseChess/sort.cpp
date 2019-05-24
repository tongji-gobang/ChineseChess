#include"sort.h"

//��ʷ��ȽϺ���
struct compare {
    bool operator()(const int& a, const int& b) const
    {
        return Search.nHistoryTable[b] < Search.nHistoryTable[a];
    }
};

void SortMoves::Init(int hash_mv_) {
	this->hash_mv = hash_mv_;		//��ʼ����ϣ�߷�
	this->killer_mv1 = Search.mvKillers[pos.RootDistance][0]; //��ɱ�ֱ��ȡ�ֵܽڵ��ɱ���߷�
	this->killer_mv2 = Search.mvKillers[pos.RootDistance][1];
	this->which_phrase = PHASE_HASH;				//���ʼ״̬����Ϊ��ȡ��ϣ�߷�
}

//��ȡ��һ���߷�
int SortMoves::GetNextMv() {
	int mv;

	//��״̬Ϊȡɢ�б��߷�
	if (which_phrase == PHASE_HASH) {
		which_phrase = PHASE_KILLER_1;	//��״̬��Ϊɱ���߷�1
		if (hash_mv)					//���ɢ�б��߷����� �򷵻ظ��߷�
			return hash_mv;
	}

	// ɱ���ŷ�����(Killer Heuristic)�ǻ�������һ��˼�룬����ĳ�����ʱ���ȳ����ŷ�a1��
	// ��a1�ĺ����ŷ�b1�����ضϣ��ص�ԭ���Ľ��ʱ������a1���ֵܽ��a2ʱ��
	// ���b1�Ծ���a2�ĺ����ŷ�����ôb1���п���Ҳ������ضϡ�
	// ����������Ϊÿ�����2��ɱ���ŷ����������Ƚ��ȳ��ķ�ʽ����
	// 1. ɱ���ŷ�����(��һ��ɱ���ŷ�)����ɺ�����������һ�׶Σ�

	//��״̬Ϊȡɱ���߷�1
	if (which_phrase == PHASE_KILLER_1) {
		which_phrase = PHASE_KILLER_2;										//��״̬��Ϊɱ���߷�2

		if (killer_mv1&&killer_mv1 != hash_mv&& pos.LegalMove(killer_mv1))	//�ж�ɱ��1�߷��Ƿ�Ϊ0����ɢ�б��߷���ͬ
																		//��Ϊ�Ϸ��߷�
			return killer_mv1;											//����ɱ��1�߷�
	}


	//��״̬Ϊȡɱ���߷�1
	if (which_phrase == PHASE_KILLER_2) {
		which_phrase = PHASE_GEN_MOVES;										//��״̬��Ϊ���������߷�״̬

		if (killer_mv2&&killer_mv2 != hash_mv&& pos.LegalMove(killer_mv2))	//�ж�ɱ��2�߷��Ƿ�Ϊ0����ɢ�б��߷���ͬ
																		//��Ϊ�Ϸ��߷�
			return killer_mv2;											//����ɱ��2�߷�
	}

	// ����ʷ��������(History Heuristic)��ɱ���ŷ���������չ��
	// ��ʷ���¼�����������������ŷ��ĺû�����ʷ���˼���ǣ�
	// ��������ĳ������ϵ�һ���õ��ŷ�����������������Ҳ�Ǻõġ�
	// û��ʲô�ǳ��ɿ���������֧�����˼�룬��������ʷ���������ŷ���
	// �ܱȲ�����Ҫ�õö࣬����ʵ��֤������һ��Ч���ǳ��õ������㷨������ÿ�������õ���

	//��״̬Ϊ���������߷�
	if (which_phrase == PHASE_GEN_MOVES) {
		which_phrase = PHASE_REST;											//��״̬����Ϊ��һ��ȡ�߷�״̬

		this->n_mvs = pos.GenerateMoves(mvs);						//���ɴ˾��������߷�
		std::sort(std::begin(mvs), std::begin(mvs) + n_mvs, compare());					//����Щ�߷�������ʷ������
		this->idx = 0;												//�������߷����±���0
	}


	//��״̬Ϊ��һ��ȡ�߷�״̬
	if (which_phrase == PHASE_REST) {
		while(idx < n_mvs) {										//�������߷��±�С�����ɵ����߷���
			mv = mvs[idx];
			idx++; 											//�������߷����±��һ
			if(mv != hash_mv && mv!= killer_mv1 && mv != killer_mv2)//�����߷�����ɢ�б��߷���ɱ���߷�
				return mv;										//���ش��߷�
			
		}
	}

	return 0;
}