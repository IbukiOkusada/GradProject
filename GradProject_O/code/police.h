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
// �T���v���̃N���X��`
//==========================================================
class CPolice : public CCar
{
private:	// �����������A�N�Z�X�\

	// ���\����
	struct SInfo
	{
		CPlayer* pPlayer;
		int nChaseCount;
		bool bChase;
	};

public:	// �N�ł��A�N�Z�X�\

	CPolice();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CPolice();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void) override;
	void Uninit(void);
	void Update(void);
	static CPolice*Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move);

	// �����o�֐�(�擾)

	// �����o�֐�(�ݒ�)

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void MoveRoad() override;
	void ReachRoad() override;
	void ChasePlayer();
	void SearchPlayer();
	void Collision();

	// �����o�ϐ�
	SInfo m_Info;					// �������g�̏��
	std::list<CRoad*> listRoad;		// �ǐ՗p���X�g
	std::list<CRoad*> listChase;	// �ǐ՗p���X�g

	CEffekseer::CEffectData* m_pPatrolLamp;
};

#endif
