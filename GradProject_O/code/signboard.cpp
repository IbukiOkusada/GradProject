//==========================================================
//
// �ŔM�~�b�N���� [signboard.cpp]
// Author : Kenta Hashimoto
//
//==========================================================
#include "manager.h"
#include "signboard.h"
#include "deltatime.h"
#include "objectX.h"
#include "camera.h"
#include "camera_manager.h"
#include "renderer.h"
#include "particle3D.h"
#include "player.h"
#include "player_manager.h"
#include "network.h"
#include "effekseerControl.h"

// �萔��`
namespace
{
	// �t�@�C����
	const float RANGE_WIDTH = SCREEN_WIDTH * 0.1f;
	const float RANGE_HEIGHT = SCREEN_HEIGHT * 0.1f;
	const char* FILENAME = "data\\MODEL\\map\\sign_speed_limit.x";
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CGimmickSignBoard::CGimmickSignBoard()
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
CGimmickSignBoard::~CGimmickSignBoard()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CGimmickSignBoard::Init(void)
{
	m_pObj = CObjectX::Create(GetPos(), GetRot(), FILENAME);
	m_pObj->SetScale(VECTOR3_ONE);
	SetVtxMax(m_pObj->GetVtxMax());
	SetVtxMin(m_pObj->GetVtxMin());
	m_pObj->SetScale(GetScale());
	SetType(TYPE::TYPE_SIGNBOARD);

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CGimmickSignBoard::Uninit(void)
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
void CGimmickSignBoard::Update(void)
{
	if (m_pObj == nullptr) { return; }

	// �Փ˂���
	if (!m_bHit)
	{
		if (m_pObj->GetHit() || m_pObj->GetHitOld())
		{
			CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
			D3DXVECTOR3 pos = pPlayer->GetPosition();
			float speed = CPlayerManager::GetInstance()->GetPlayer()->GetEngine();

			Hit(pos, speed);

			// �l�b�g���[�N�ł̏Փˑ��M
			CNetWork::GetInstance()->SendGmHit(GetId(), pos, speed);
		}
	}

	// �������
	Away();
}

//==========================================================
// ����
//==========================================================
CGimmickSignBoard* CGimmickSignBoard::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale)
{
	CGimmickSignBoard* pGuardRail = nullptr;

	pGuardRail = DEBUG_NEW CGimmickSignBoard;

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
void CGimmickSignBoard::Away()
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
	correction::Adjust(&rotdiff);
	D3DXVECTOR3 setrot = m_pObj->GetRotation() + rotdiff * 0.1f;
	correction::Adjust(&setrot);
	m_pObj->SetRotation(setrot);
}

//==========================================================
// �Փ�
//==========================================================
void CGimmickSignBoard::Hit(const D3DXVECTOR3& HitPos, const float fSpeed)
{
	if (m_bHit) { return; }

	m_bHit = true;
	m_pObj->SetEnableCollision(false);
	float rot = atan2f(GetPos().x - HitPos.x, GetPos().z - HitPos.z);

	// ���W�ݒ�
	m_TargetPos = {
		GetPos().x + sinf(rot) * fSpeed * 500.0f,
		GetPos().y,
		GetPos().z + cosf(rot) * fSpeed * 500.0f,
	};

	m_TargetRot = {
		D3DX_PI * 0.5f,
		rot,
		0.0f
	};

	if (m_pObj->GetHit() || m_pObj->GetHitOld()) { return; }
	//CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\spark.efkefc", GetPos(), VECTOR3_ZERO, VECTOR3_ZERO, 300.0f);
}

//==========================================================
// �F�{���ύX
//==========================================================
void CGimmickSignBoard::SetColMulti(const D3DXCOLOR& col)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetColMulti(col);
}

//==========================================================
// ���W���f
//==========================================================
void CGimmickSignBoard::SetObjPos(const D3DXVECTOR3& pos)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetPosition(pos);
}

//==========================================================
// �������f
//==========================================================
void CGimmickSignBoard::SetObjRot(const D3DXVECTOR3& rot)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetRotation(rot);
}

//==========================================================
// �X�P�[�����f
//==========================================================
void CGimmickSignBoard::SetObjScale(const D3DXVECTOR3& scale)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetScale(scale);
}