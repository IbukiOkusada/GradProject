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
class CObjectX;

//==========================================================
// �ǉ��x�@�̃N���X��`
//==========================================================
class CInstpection : public CTask
{
private:


	// ��{���\����
	struct SInfo
	{
		D3DXVECTOR3 pos;	// ���W
		D3DXVECTOR3 rot;	// ����

		// �R���X�g���N�^
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO) {}
	};

	// fence�p���
	struct SGuardInfo
	{
		float fTimer;	// �J�ڃ^�C�}�[
		D3DXVECTOR3 targetpos;	// �ڕW���W
		D3DXVECTOR3 targetrot;	// �ڕW����
		D3DXVECTOR3 startpos;	// �J�n���W
		D3DXVECTOR3 startrot;	// �J�n����
		CObjectX* pObj;	// �`��I�u�W�F�N�g

		// �R���X�g���N�^
		SGuardInfo() : fTimer(0.0f), targetpos(VECTOR3_ZERO), 
			targetrot(VECTOR3_ZERO), startpos(VECTOR3_ZERO), startrot(VECTOR3_ZERO), pObj(nullptr) {}
	};

public:	// �N�ł��A�N�Z�X�\

	CInstpection();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CInstpection() override;	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;

	// �ÓI�����o�֐�
	static CInstpection* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);

	// �����o�֐�(�擾)
	D3DXVECTOR3& GetPosition() { return m_Info.pos; }
	D3DXVECTOR3& GetRotation() { return m_Info.rot; }

	// �����o�֐�(�ݒ�)
	void SetPosition(const D3DXVECTOR3& pos) { m_Info.pos = pos; }
	void SetRotation(const D3DXVECTOR3& rot) { m_Info.rot = rot; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void Away();

	// �����o�ϐ�
	CAddPolice* m_pPolice;	// ����ꏊ�ɂ���x�@
	SGuardInfo m_Guard;		// �t�F���X�p
	SInfo m_Info;			// ��{���
	bool m_bHit;			// ��x�ł��Փ˂���
};

#endif