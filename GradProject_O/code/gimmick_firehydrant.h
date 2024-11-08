//==========================================================
//
// ���ΐ��M�~�b�N [gimmick_firehydrant.h]
// Author : Ibuki Okusada
// 
//==========================================================
#ifndef _FIREHYDRANT_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _FIREHYDRANT_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "gimmick.h"

// �O���錾
class CObjectX;

//==========================================================
// ���ΐ��̃N���X��`
//==========================================================
class CGimmickFireHydrant : public CGimmick
{
private:

public:	// �N�ł��A�N�Z�X�\

	CGimmickFireHydrant();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CGimmickFireHydrant() override;	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	static CGimmickFireHydrant* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale);
	
	// �Փˎ��̔���
	virtual void Hit(const D3DXVECTOR3& HitPos) {}

private:	// �����������A�N�Z�X�\

	void SetEffect();

	// �����o�ϐ�
	CObjectX* m_pObj;	// �`��I�u�W�F�N�g
	bool m_bHit;		// �Փ˂���
};

#endif

