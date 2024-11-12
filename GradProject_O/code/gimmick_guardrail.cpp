//==========================================================
//
// �K�[�h���[���M�~�b�N���� [gimmick_guardrail.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "manager.h"
#include "gimmick_guardrail.h"
#include "deltatime.h"
#include "objectX.h"
#include "camera.h"
#include "camera_manager.h"
#include "renderer.h"
#include "particle3D.h"
#include "player.h"
#include "player_manager.h"

// �萔��`
namespace
{
	// �t�@�C����
	const float RANGE_WIDTH = SCREEN_WIDTH * 0.1f;
	const float RANGE_HEIGHT = SCREEN_HEIGHT * 0.1f;
	const char* FILENAME = "data\\MODEL\\map\\guardrail001.x";
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CGimmickGuardRail::CGimmickGuardRail()
{
	// �l�̃N���A
	m_pObj = nullptr;
	m_bHit = false;
	m_TargetPos = VECTOR3_ZERO;
	m_TargetRot = VECTOR3_ZERO;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CGimmickGuardRail::~CGimmickGuardRail()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CGimmickGuardRail::Init(void)
{
	m_pObj = CObjectX::Create(GetPos(), GetRot(), FILENAME);
	m_pObj->SetScale(GetScale());

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CGimmickGuardRail::Uninit(void)
{
	// �I�u�W�F�N�g�p��
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	CGimmick::Uninit();
}

//==========================================================
// �X�V����
//==========================================================
void CGimmickGuardRail::Update(void)
{
	if (m_pObj == nullptr) { return; }

	// �Փ˂���
	if (!m_bHit)
	{
		if (m_pObj->GetHit() || m_pObj->GetHitOld())
		{
			m_bHit = true;
			m_pObj->SetEnableCollision(false);
			CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
			D3DXVECTOR3 pos = pPlayer->GetPosition();

			Hit(pos);
		}
	}

	// �������
	Away();
}

//==========================================================
// ����
//==========================================================
CGimmickGuardRail* CGimmickGuardRail::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale)
{
	CGimmickGuardRail* pGuardRail = nullptr;

	pGuardRail = DEBUG_NEW CGimmickGuardRail;

	if (pGuardRail != nullptr)
	{
		pGuardRail->SetPos(pos);
		pGuardRail->SetRot(rot);
		pGuardRail->SetScale(scale);

		// ����������
		pGuardRail->Init();
	}

	return pGuardRail;
}

//==========================================================
// �������
//==========================================================
void CGimmickGuardRail::Away()
{
	// ���ΐ����ړ�����
	if (m_pObj == nullptr) { return; }
	if (!m_bHit) { return; }

	// �ʒu�X�V
	D3DXVECTOR3 posdiff = m_TargetPos - m_pObj->GetPosition();
	D3DXVECTOR3 setpos = m_pObj->GetPosition() + posdiff * 0.1f;
	m_pObj->SetPosition(setpos);

	// �����X�V
	D3DXVECTOR3 rotdiff = m_TargetRot - m_pObj->GetRotation();
	Adjust(rotdiff);
	D3DXVECTOR3 setrot = m_pObj->GetRotation() + rotdiff * 0.1f;
	Adjust(setrot);
	m_pObj->SetRotation(setrot);
}

//==========================================================
// �Փ�
//==========================================================
void CGimmickGuardRail::Hit(const D3DXVECTOR3& HitPos)
{
	float rot = atan2f(GetPos().x - HitPos.x, GetPos().z - HitPos.z);
	float speed = CPlayerManager::GetInstance()->GetTop()->GetEngine();

	// ���W�ݒ�
	m_TargetPos = {
		GetPos().x + sinf(rot) * speed * 500.0f,
		GetPos().y,
		GetPos().z + cosf(rot) * speed * 500.0f,
	};

	m_TargetRot = {
		D3DX_PI * 0.5f,
		rot,
		0.0f
	};
}

