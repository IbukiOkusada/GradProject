//==========================================================
//
// �S�~���M�~�b�N [trashcan.h]
// Author : Kenta Hashimoto
// 
//==========================================================
#ifndef _TRASHCAN_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _TRASHCAN_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "gimmick.h"

// �O���錾
class CObjectX;

//==========================================================
// ���ΐ��̃N���X��`
//==========================================================
class CGimmickTrashCan : public CGimmick
{
private:

public:	// �N�ł��A�N�Z�X�\

	CGimmickTrashCan();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CGimmickTrashCan() override;	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	static CGimmickTrashCan* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale);

	// �Փˎ��̔���
	void Hit(const D3DXVECTOR3& HitPos, const float fSpeed) override;
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

