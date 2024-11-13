//==========================================================
//
// �x�@���M�~�b�N [gimmick_policestation.h]
// Author : Ibuki Okusada
// 
//==========================================================
#ifndef _POLICESTATION_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _POLICESTATION_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "gimmick.h"

// �O���錾
class CObjectX;
class CRoad;

//==========================================================
// �x�@���̃N���X��`
//==========================================================
class CGimmickPoliceStation : public CGimmick
{
private:

	struct SInfo
	{
		float fInterVal;	// �C���^�[�o��
		float fSpawnTime;	// �����^�C�~���O
		CRoad* pRoad;		// ���ʂ̓�

		// �R���X�g���N�^
		SInfo() : fInterVal(0.0f), fSpawnTime(0.0f), pRoad(nullptr) {}
	};

public:	// �N�ł��A�N�Z�X�\

	CGimmickPoliceStation(); // �R���X�g���N�^(�I�[�o�[���[�h)
	~CGimmickPoliceStation() override;	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	static CGimmickPoliceStation* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale);
	void SetColMulti(const D3DXCOLOR& col) override;

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	virtual void SetObjPos(const D3DXVECTOR3& pos) override;
	virtual void SetObjRot(const D3DXVECTOR3& rot) override;
	virtual void SetObjScale(const D3DXVECTOR3& scale) override;

	// �����o�ϐ�
	CObjectX* m_pObj;			// �`��I�u�W�F�N�g
	SInfo m_Info;
};

#endif
