//==========================================================
//
// ���⏈�� [inspection.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _INSPECTION_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _INSPECTION_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include "effekseerControl.h"

// �O���錾
class CAddPolice;
class CGimmickPoliceStation;
class CRoad;

// �萔��`
namespace InstpectionData
{
	const int NUM_EFFECT = 2;
}

//==========================================================
// �ǉ��x�@�̃N���X��`
//==========================================================
class CInspection : public CTask
{
private:

	// �o���x�@���
	enum TYPE
	{
		TYPE_SEARCH_L = 0,	// �����Ď�
		TYPE_SEARCH_R,		// �E���Ď�
		TYPE_MAX
	};

	// ��{���\����
	struct SInfo
	{
		D3DXVECTOR3 pos;	// ���W
		D3DXVECTOR3 rot;	// ����
		int nId;
		int nStartPdId;		// �x�@�J�nID

		// �R���X�g���N�^
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), nId(-1) {}
	};

	// �o�΃p�g�J�[���
	struct SPoliceInfo
	{
		CAddPolice* pPolice;	// �x�@�N���X�̃|�C���^
		D3DXVECTOR3 goalpos;	// �S�[���n�_�̍��W
		D3DXVECTOR3 goalrot;	// �S�[���n�_�̍��W
		D3DXVECTOR3 startpos;	// �J�n�n�_�̍��W
		float fTimer;			// �ړ��^�C�}�[

		// �R���X�g���N�^
		SPoliceInfo() : pPolice(nullptr), goalpos(VECTOR3_ZERO), goalrot(VECTOR3_ZERO), startpos(VECTOR3_ZERO), fTimer(0.0f) {}
	};

	// ��������
	struct SLagerInfo
	{
		CEffekseer::CEffectData* apEffect[InstpectionData::NUM_EFFECT];
		float fTimer;			// �^�C�}�[
		float fRotateTimer;		// ��]�^�C�}�[
		float fEndTimer;		// �I���^�C�}�[
		float scale;			// �����X�P�[��

		// �R���X�g���N�^
		SLagerInfo() : apEffect(), fTimer(0.0f), fRotateTimer(0.0f), scale(0.0f), fEndTimer(0.0f) {}
	};


public:	// �N�ł��A�N�Z�X�\

	CInspection(int nId = -1);	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CInspection() override;	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Start();

	// �ÓI�����o�֐�
	static CInspection* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, CRoad* pRoad, int nId, int startpdid);

	// �����o�֐�(�擾)
	D3DXVECTOR3& GetPosition() { return m_Info.pos; }
	D3DXVECTOR3& GetRotation() { return m_Info.rot; }
	int GetId() { return m_Info.nId; }

	// �����o�֐�(�ݒ�)
	void SetPosition(const D3DXVECTOR3& pos) { m_Info.pos = pos; }
	void SetRotation(const D3DXVECTOR3& rot) { m_Info.rot = rot; }
	void BindId(const int nId) { m_Info.nId; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void Away();
	void LagerSet();
	void LagerSetRotation();
	void Collision();

	// �����o�ϐ�
	SInfo m_Info;			// ��{���
	CRoad* m_pRoad;			// �ݒu���铹
	CGimmickPoliceStation* m_pNearStation;	// �߂��x�@��
	SPoliceInfo m_aPoliceInfo[TYPE_MAX];	// �x�@���
	SLagerInfo m_LagerInfo;			// ��������
};

#endif