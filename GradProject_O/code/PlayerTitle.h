//<=================================================
//�^�C�g���ł̃v���C���\�̓������� [PlayerTitle.h]
//
//Author : Kazuki Watanabe
//<=================================================
#ifndef _PLAYERTITLE_H_
#define _PLAYERTITLE_H_

#include "player.h"

//<***************************************
//�^�C�g���֘A�Ŏg���֐�
//<***************************************
namespace Function
{
	//�ړI�n�ɒ��������̔���
	bool BoolToDest(const D3DXVECTOR3 Pos, const D3DXVECTOR3 DestPos, const float Distance, bool bZuse = true);
}
class CTitleBaggage;
class CTitleGoal;
//<========================================
//�v���C���[�N���X���p�����Ē�`
//<========================================
class CPlayerTitle: public CPlayer
{
public:

	//�X�e�[�g
	enum STATE
	{

		STATE_NONE=0,	//�ʏ�̓���
		STATE_DEBUG,	//�ʒu�����Ȃǂ̃f�o�b�O��
		STATE_MAX
	};

	//�ړI�n�i���o�[
	enum DEST
	{
		DEST_FIRST = 0,	//��Ԗ�
		DEST_SECOND,	//��Ԗ�
		DEST_MAX,

	};


	CPlayerTitle();
	~CPlayerTitle();

	// �����o�֐�
	HRESULT Init(void);
	HRESULT Init(const char* pBodyName, const char* pLegName);	// �I�[�o�[���[�h
	void Uninit(void);
	void Update(void);
	static CPlayerTitle* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move, const char* pBodyName, const char* pLegName);

	void Moving(const int nNum);
	void BaggageMove(void);

	//�������Ɋւ���Set�EGet�֐�
	void SetReached(bool bReach) { m_bReached = bReach; }
	bool GetReached(void) {return m_bReached;}

	//�S�[�������擾
	CTitleGoal* GetTitleGoal(void) { return m_pTitleGoal; }

private:

	float m_fBDustValue;				//���̑傫���̒l
	bool m_bNextMove;					//���̓����Ɉڍs����Ƃ��ɂȂ�����
	STATE m_eState;						//�f�o�b�O�p�̃X�e�[�g
	bool m_bReached;					//���������ǂ���
	CTitleBaggage* m_pTitleBaggage;		//�^�C�g���p�̉ו�
	CTitleGoal *m_pTitleGoal;			//�^�C�g���p�̃S�[��


};

#endif
