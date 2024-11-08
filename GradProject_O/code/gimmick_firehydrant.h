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
class CFireHydrant : public CGimmick
{
private:

public:	// �N�ł��A�N�Z�X�\

	CFireHydrant();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CFireHydrant() override;	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	static CFireHydrant* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale);

private:	// �����������A�N�Z�X�\

	// �����o�ϐ�
	CObjectX* m_pObj;
};

#endif

