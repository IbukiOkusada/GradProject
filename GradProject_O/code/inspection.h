//==========================================================
//
// ���⏈�� [instpection.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _INSTPECTION_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _INSTPECTION_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"

// �O���錾
class CAddPolice;
class CGimmickPoliceStation;
class CRoad;

//==========================================================
// �ǉ��x�@�̃N���X��`
//==========================================================
class CInstpection : public CTask
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

		// �R���X�g���N�^
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO) {}
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
		float fTimer;			// �^�C�}�[
		float fRotateTimer;		// ��]�^�C�}�[
		bool bHit;				// �Փ˔���

		// �R���X�g���N�^
		SLagerInfo() : fTimer(0.0f), fRotateTimer(0.0f), bHit(false) {}
	};


public:	// �N�ł��A�N�Z�X�\

	CInstpection();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CInstpection() override;	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;

	// �ÓI�����o�֐�
	static CInstpection* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, CRoad* pRoad);

	// �����o�֐�(�擾)
	D3DXVECTOR3& GetPosition() { return m_Info.pos; }
	D3DXVECTOR3& GetRotation() { return m_Info.rot; }

	// �����o�֐�(�ݒ�)
	void SetPosition(const D3DXVECTOR3& pos) { m_Info.pos = pos; }
	void SetRotation(const D3DXVECTOR3& rot) { m_Info.rot = rot; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void Away();
	void LagerSet();
	void LagerSetRotation();

	// �����o�ϐ�
	SInfo m_Info;			// ��{���
	CRoad* m_pRoad;			// �ݒu���铹
	CGimmickPoliceStation* m_pNearStation;	// �߂��x�@��
	SPoliceInfo m_aPoliceInfo[TYPE_MAX];	// �x�@���
	SLagerInfo m_LagerInfo;			// ��������
};

#endif