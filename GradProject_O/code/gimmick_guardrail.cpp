//==========================================================
//
// ���ΐ��M�~�b�N���� [gimmick_firehydrant.cpp]
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
	const char* FILENAME = "data\\MODEL\\map\\fire_hydrant_00.x";
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
	if (m_pObj->GetHit() || m_pObj->GetHitOld())
	{
		m_bHit = true;
		m_pObj->SetEnableCollision(false);
		D3DXVECTOR3 pos = CPlayerManager::GetInstance()->GetTop()->GetPosition();
		float rot = atan2f(GetPos().x - pos.x, GetPos().z - pos.z);
		float speed = CPlayerManager::GetInstance()->GetTop()->GetEngine();

		// ���W�ݒ�
		m_TargetPos = {
			GetPos().x + sinf(rot) * speed * 500.0f,
			GetPos().y,
			GetPos().z + cosf(rot) * speed * 500.0f,
		};

		m_TargetRot = {
			0.0f,
			rot,
			D3DX_PI * 0.5f
		};
	}

	// �������
	Away();

	// �G�t�F�N�g�ݒ�
	SetEffect();
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
// �G�t�F�N�g����
//==========================================================
void CGimmickGuardRail::SetEffect()
{
	// ���ΐ����ړ�����
	if (m_pObj == nullptr) { return; }
	if (!m_bHit) { return; }

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();
	D3DXMATRIX mtxProjection, mtxView, mtxWorld;
	D3DVIEWPORT9 Viewport;
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	D3DXVECTOR3 mypos = GetPos();

	// �K�v�ȏ��擾
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetViewport(&Viewport);

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxWorld);

	// �X�N���[�����W�擾
	D3DXVec3Project(&pos, &mypos, &Viewport, &mtxProjection, &mtxView, &mtxWorld);

	// ��ʊO�Ȃ�o���Ȃ�
	if (pos.x < 0.0f - RANGE_WIDTH || pos.x > SCREEN_WIDTH + RANGE_WIDTH ||
		pos.y < 0.0f - RANGE_HEIGHT || pos.y > SCREEN_HEIGHT + RANGE_HEIGHT) {
		return;
	}

	D3DXVECTOR3 objpos = GetPos();
	CParticle3D::Create(objpos, CEffect3D::TYPE_SPLASH);
}
