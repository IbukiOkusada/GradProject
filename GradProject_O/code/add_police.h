//==========================================================
//
// �ǉ��p�g�J�[���� [add_police.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _ADD_POLICE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _ADD_POLICE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "main.h"
#include "task.h"
#include "police.h"
#include "player.h"
#include <list>

// �O���錾
class CRoad;

//==========================================================
// �ǉ��x�@�̃N���X��`
//==========================================================
class CAddPolice : public CPolice
{
public:	// �N�ł��A�N�Z�X�\

	CAddPolice();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CAddPolice() override;	// �f�X�g���N�^

	// �����o�֐�
	static CAddPolice* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move);

private:	// �����������A�N�Z�X�\

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԋ֐�
	void StateNormal() override;	// �ʏ�
	void StateSearch() override;	// �x��
	void StateFadeOut() override;	// �t�F�[�h�A�E�g

	// ��Ԑݒ�֐�
	virtual void SetStateNormal() override;		// �ʏ�
	virtual void SetStateSearch() override;		// �x��
	virtual void SetStateFadeOut() override;	// �t�F�[�h�A�E�g

	// �����o�֐�
	D3DXVECTOR3 m_SpawnPos;
};

#endif