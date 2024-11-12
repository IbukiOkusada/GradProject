//==========================================================
//
// �K�[�h���[���M�~�b�N [gimmick_guardrail.h]
// Author : Ibuki Okusada
// 
//==========================================================
#ifndef _GUARDRAIL_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _GUARDRAIL_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "gimmick.h"

// �O���錾
class CObjectX;

//==========================================================
// ���ΐ��̃N���X��`
//==========================================================
class CGimmickGuardRail : public CGimmick
{
private:

public:	// �N�ł��A�N�Z�X�\

	CGimmickGuardRail();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CGimmickGuardRail() override;	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	static CGimmickGuardRail* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale);
	
	// �Փˎ��̔���
	void Hit(const D3DXVECTOR3& HitPos);
	void SetColMulti(const D3DXCOLOR& col) override;

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	virtual void SetObjPos(const D3DXVECTOR3& pos) override;
	virtual void SetObjRot(const D3DXVECTOR3& rot) override;
	virtual void SetObjScale(const D3DXVECTOR3& scale) override;
	void Away();

	// �����o�ϐ�
	D3DXVECTOR3 m_TargetRot;	// ������ь����
	D3DXVECTOR3 m_TargetPos;	// ������ь���W
	CObjectX* m_pObj;			// �`��I�u�W�F�N�g
	bool m_bHit;				// �Փ˂���
};

#endif

