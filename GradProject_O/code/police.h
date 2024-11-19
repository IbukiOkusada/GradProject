//==========================================================
//
// �p�g�J�[���� [police.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _POLICE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _POLICE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "main.h"
#include "object.h"
#include "objectX.h"
#include "task.h"
#include "car.h"
#include "player.h"
#include <list>

// �O���錾
class CRoad;

//==========================================================
// �x�@�̃N���X��`
//==========================================================
class CPolice : public CCar
{
public:

	// ���
	enum STATE
	{
		STATE_NORMAL = 0,
		STATE_CHASE,
		STATE_SEARCH,
		STATE_FADEOUT,
		STATE_STOP,
		STATE_MAX
	};

private:	// �����������A�N�Z�X�\

	// ���\����
	struct SInfo
	{
		CPlayer* pPlayer;
		int nChaseCount;
		bool bChase;

		// �R���X�g���N�^
		SInfo() : pPlayer(nullptr), nChaseCount(0), bChase(false) {}
	};

protected:

	// ��ԊǗ��\����
	struct SState
	{
		STATE state;
		float fTimer;
		float fTimerOrigin;

		// �R���X�g���N�^
		SState() : state(STATE::STATE_NORMAL), fTimer(0.0f) {}
	};

public:	// �N�ł��A�N�Z�X�\

	CPolice();	// �R���X�g���N�^(�I�[�o�[���[�h)
	virtual ~CPolice() override;	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void) override;
	void Uninit(void);
	void Update(void);
	static CPolice*Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move);

	// �����o�֐�(�擾)
	STATE GetState() { return m_stateInfo.state; }

	// �����o�֐�(�ݒ�)
	void SetState(const STATE state);

protected:

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԋ֐�
	virtual void UpdateState();		// ��ԍX�V
	virtual void StateNormal();		// �Ȃ�
	virtual void StateChase();		// �ǐ�
	virtual void StateSearch();		// �x��
	virtual void StateFadeOut();	// �t�F�[�h�A�E�g
	virtual void StateStop();		// ��~

	// ��Ԑݒ�֐�
	virtual void SetStateNormal();		// �Ȃ�
	virtual void SetStateChase();		// �ǐ�
	virtual void SetStateSearch();		// �x��
	virtual void SetStateFadeOut();		// �t�F�[�h�A�E�g
	virtual void SetStateStop();		// ��~
	
	// �����o�֐�
	void SetStateTimer(const float fTimer) { // ��ԃ^�C�}�[�ݒ�
		m_stateInfo.fTimer = fTimer; 
		m_stateInfo.fTimerOrigin = fTimer; 
	}

	// �����o�ϐ�
	SState m_stateInfo;	// ��ԊǗ����

private:	// �����������A�N�Z�X�\

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CPolice::* STATE_FUNC)();
	static STATE_FUNC m_StateFunc[];
	typedef void(CPolice::* SETSTATE_FUNC)();
	static SETSTATE_FUNC m_SetStateFunc[];

	// �����o�֐�
	void MoveRoad() override;
	void ReachRoad() override;
	void ChasePlayer();
	void SearchPlayer();
	void Collision();
	void Break() override;

	// �����o�ϐ�
	SInfo m_Info;					// �������g�̏��
	std::list<CRoad*> listRoad;		// �ǐ՗p���X�g
	std::list<CRoad*> listChase;	// �ǐ՗p���X�g

	CMasterSound::CObjectSound* m_pSiren;
	CEffekseer::CEffectData* m_pPatrolLamp;
};

#endif
