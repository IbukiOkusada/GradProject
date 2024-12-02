//==========================================================
//
// �p�g�J�[AI���� [police_AI.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "police_AI.h"
#include "road.h"
#include "road_manager.h"
#include "debugproc.h"
#include "player.h"
#include "player_manager.h"
#include "police.h"
#include "police_manager.h"
#include "deltatime.h"
#include "a_star.h"
#include "pred_route.h"

// �}�N����`

// �������O��Ԃ��`
namespace
{
	const float SECURE_SPEEDDEST = (-35.0f);	// �m�ێ��̖ڕW���x
	const float SECURE_SPEED = (0.8f);			// �m�ێ��̉����{��
	const int CHASE_TIME = (300);				// �ǐՎ���
	const float CHASE_SECURE = (400.0f);		// �ǐՊm�ۋ���
	const float CHASE_BEGIN[CPolice::STATE::STATE_MAX] = {
		(700.0f),
		(700.0f),
		(2000.0f),
		(400.0f),
	};			// �ǐՊJ�n����
	const float CHASE_CONTINUE = (200000.0f);		// �ǐՌp������
	const float CHASE_END = (300000.0f);			// �ǐՏI������
	const float CHASE_CROSS = (500.0f);			// �ǐՏI������
	const float CHASE_NEAR = (2000.0f);			// �ǐՏI������
	const float CHASE_FAR = (5000.0f);			// �ǐՏI������
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CPoliceAI::CPoliceAI()
{
	// �l�̃N���A
	m_pPolice = nullptr;
	m_pRoadStart = nullptr;
	m_pRoadTarget = nullptr;
	m_pSearchTarget = nullptr;
	m_fSearchTimer = 0.0f;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CPoliceAI::~CPoliceAI()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CPoliceAI::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CPoliceAI::Uninit(void)
{
	CDeltaTime::GetInstance()->GetDeltaTime();
}

//==========================================================
// ��������
//==========================================================
void CPoliceAI::Search(void)
{
	// �v���C���[�̏��������Ă��Ȃ��ꍇ�v���C���[���擾
	if (m_pPolice->GetPlayer() == nullptr)
	{
		m_pPolice->SetPlayer(CPlayerManager::GetInstance()->GetPlayer());
	}

	// �v���C���[�����݂��Ȃ��Ȃ甲����
	if (m_pPolice->GetPlayer() == nullptr) { return; }

	// �v���C���[���擾
	CPlayer* pPlayer = m_pPolice->GetPlayer();
	D3DXVECTOR3 vecPlayer = pPlayer->GetPosition() - m_pPolice->GetPosition();		// �v���C���[�ƌx�@�Ԃ̃x�N�g���v�Z
	float length = D3DXVec3Length(&vecPlayer);										// �����v�Z
	float rotVec = atan2f(vecPlayer.x, vecPlayer.z);								// �p�x�v�Z
	float rotView = fabs(pPlayer->GetRotation().y - rotVec);						// �����Ă�����Ƃ̍����v�Z

	// �p�x�␳
	if (rotView > D3DX_PI)
	{
		rotView -= D3DX_PI * 2.0f;
	}
	else if (rotView < -D3DX_PI)
	{
		rotView += D3DX_PI * 2.0f;
	}

	// �����ɂ���Ĕ��f
	if (length < CHASE_CROSS)
	{// ���ߋ���

		// �ǐՏ�ԂɕύX
		m_pPolice->SetChase(true);

		// �ǐՏ�ԂłȂ���Δ�����
		if (!m_pPolice->GetChase()) { return; }

		// �ǐՎ��Ԃ�ݒ�
		m_pPolice->SetChaseCount(CHASE_TIME);

		// ���x��ݒ�
		m_pPolice->SetSpeedDest(SECURE_SPEEDDEST);
		m_pPolice->SetSpeed(m_pPolice->GetSpeed() * SECURE_SPEED);

		// ��Ԑݒ�
		m_pPolice->SetState(CPolice::STATE::STATE_CHASE);
	}
	else if (length < CHASE_NEAR)
	{// �ߋ���

		// ���E���ɓ����Ă��邩�ǂ���
		if (rotView > D3DX_PI * 0.5f && !m_pPolice->GetChase()) { return; }

		// �e�󋵊m�F
		CheckSpeed(pPlayer);
		CheckTurn(pPlayer);
		CheckDamage(pPlayer);
		CheckCollision(pPlayer);

		// �ǐՏ�ԂłȂ���Δ�����
		if (!m_pPolice->GetChase()) { return; }

		// �ǐՎ��Ԃ�ݒ�
		m_pPolice->SetChaseCount(CHASE_TIME);

		// ��Ԑݒ�
		m_pPolice->SetState(CPolice::STATE::STATE_CHASE);
	}
	else if (length < CHASE_FAR)
	{// ������

		// ���E���ɓ����Ă��邩�ǂ���
		if (rotView > D3DX_PI * 0.5f && !m_pPolice->GetChase())
		{
			return;
		}

		// �e�󋵊m�F
		CheckSpeed(pPlayer);
		CheckSmoke(pPlayer);
		CheckCollision(pPlayer);

		// �ǐՏ�ԂłȂ���Δ�����
		if (!m_pPolice->GetChase()) { return; }

		// �ǐՎ��Ԃ�ݒ�
		m_pPolice->SetChaseCount(CHASE_TIME);

		// ��Ԑݒ�
		m_pPolice->SetState(CPolice::STATE::STATE_CHASE);
	}
	else
	{// �͈͊O

		// �ǐՏ�ԂłȂ���Δ�����
		if (!m_pPolice->GetChase()) { return; }

		// �ǐՎ��Ԃ����炷
		m_pPolice->SetChaseCount(m_pPolice->GetChaseCount() - 1);

		// �ǐՎ��Ԃ�0�ɂȂ�����x����ԂɈڍs
		if (m_pPolice->GetChaseCount() < 0)
		{
			// �S�����x����Ԃ�
			m_pPolice->SetState(CPolice::STATE::STATE_SEARCH);
			CPoliceManager::GetInstance()->Warning(m_pPolice);

			// �J�E���g��0�ɌŒ�
			m_pPolice->SetChaseCount(0);

			// �ǐՏ�Ԃ�����
			m_pPolice->SetChase(false);
		}
	}
}

//==========================================================
// ���x�v������
//==========================================================
void CPoliceAI::CheckSpeed(CPlayer* pPlayer)
{
	// �v���C���[�����݂��Ȃ��Ȃ甲����
	if (pPlayer == nullptr) { return; }

	 // �j�g�����g���Ă��鎞
	if (pPlayer->GetState() == CPlayer::STATE_NITRO)
	{
		m_pPolice->SetChase(true);
	}
}

//==========================================================
// ����m�F����
//==========================================================
void CPoliceAI::CheckTurn(CPlayer* pPlayer)
{
	// �v���C���[�����݂��Ȃ��Ȃ甲����
	if (pPlayer == nullptr) { return; }
}

//==========================================================
// ���m�F����
//==========================================================
void CPoliceAI::CheckDamage(CPlayer* pPlayer)
{
	// �v���C���[�����݂��Ȃ��Ȃ甲����
	if (pPlayer == nullptr) { return; }

	// ���C�t�����l�ȉ��̎�
	if (pPlayer->GetLife() < 50.0f)
	{
		m_pPolice->SetChase(true);
	}
}

//==========================================================
// ���m�F����
//==========================================================
void CPoliceAI::CheckSmoke(CPlayer* pPlayer)
{
	// �v���C���[�����݂��Ȃ��Ȃ甲����
	if (pPlayer == nullptr) { return; }

	// ���C�t�����l�ȉ��̎�
	if (pPlayer->GetLife() < 20.0f)
	{
		m_pPolice->SetChase(true);
	}
}

//==========================================================
// ���̊m�F����
//==========================================================
void CPoliceAI::CheckCollision(CPlayer* pPlayer)
{
	// �v���C���[�����݂��Ȃ��Ȃ甲����
	if (pPlayer == nullptr) { return; }
}

//==========================================================
// �ǐՏ���
//==========================================================
void CPoliceAI::Chase(void)
{
	// �ϐ��錾
	float fDeltaTime = CDeltaTime::GetInstance()->GetDestTime();
	float fSlow = CDeltaTime::GetInstance()->GetSlow();

	// �ړI�n�I��
	SelectRoad();

	// ��莞�Ԃ��Ƃ������̓^�[�Q�b�g�����݂��Ȃ���
	if (m_fSearchTimer > 3.0f || m_pSearchTarget == nullptr)
	{
		// ���ݒn�ƖړI�n���ʂ̎�
		if (m_pRoadStart->GetSearchSelf() != m_pRoadTarget->GetSearchSelf())
		{
			// �o�H�T��
			m_searchRoad = AStar::AStarPolice(m_pRoadStart, m_pRoadTarget);

			// ���X�g����łȂ���Έړ���ݒ�
			if (!m_searchRoad.empty())
			{
				m_pSearchTarget = m_searchRoad.front();
			}
		}
		m_fSearchTimer = 0;
	}

	// �ړI�n���B����
	ReachRoad();

	m_fSearchTimer += fDeltaTime * fSlow;
}

//==========================================================
// ��������
//==========================================================
CPoliceAI *CPoliceAI::Create(CPolice* pPolice, TYPE type)
{
	CPoliceAI *pPoliceAI = nullptr;

	switch (type)
	{
	case TYPE_NORMAL:
		pPoliceAI = DEBUG_NEW CPoliceAINomal;
		break;

	case TYPE_ELITE:
		pPoliceAI = DEBUG_NEW CPoliceAIElite;
		break;

	default:
		pPoliceAI = DEBUG_NEW CPoliceAI;
		break;
	}

	if (pPoliceAI != nullptr)
	{
		pPoliceAI->m_pPolice = pPolice;
		pPoliceAI->m_type = type;

		// ����������
		pPoliceAI->Init();
	}

	return pPoliceAI;
}

//==========================================================
// �ړ����[�g�p�̓��I������
//==========================================================
void CPoliceAI::SelectRoad(void)
{
	// �T���J�n�n�_�Ɍ��݂̖ړI�n��ݒ�
	m_pRoadStart = m_pPolice->GetRoadTarget();

	// �v���C���[�̍Ŋ��̓���ڕW�n�_�ɐݒ�
	CPlayer* pPlayer = m_pPolice->GetPlayer();
	if (pPlayer != nullptr)
	{
		m_pRoadTarget = pPlayer->GetRoad();
	}
	else
	{
		m_pRoadTarget = m_pPolice->GetRoadTarget();
	}
}

//==========================================================
// �ړ����[�g�p�̓��I������
//==========================================================
void CPoliceAINomal::SelectRoad(void)
{
	// �T���J�n�n�_�Ɍ��݂̖ړI�n��ݒ�
	m_pRoadStart = m_pPolice->GetRoadTarget();

	// �v���C���[�̍Ŋ��̓���ڕW�n�_�ɐݒ�
	CPlayer* pPlayer = m_pPolice->GetPlayer();
	if (pPlayer != nullptr)
	{
		m_pRoadTarget = pPlayer->GetRoad();
	}
	else
	{
		m_pRoadTarget = m_pPolice->GetRoadTarget();
	}
}

//==========================================================
// �ړ����[�g�p�̓��I������
//==========================================================
void CPoliceAIElite::SelectRoad(void)
{
	// �T���J�n�n�_�Ɍ��݂̖ړI�n��ݒ�
	m_pRoadStart = m_pPolice->GetRoadTarget();

	// �v���C���[�̍Ŋ��̓���ڕW�n�_�ɐݒ�
	CPlayer* pPlayer = m_pPolice->GetPlayer();
	if (pPlayer != nullptr)
	{
		m_pRoadTarget = pPlayer->GetPredRoute()->GetPredRoad();
	}
	else
	{
		m_pRoadTarget = m_pPolice->GetRoadTarget();
	}
}

//==========================================================
// �ڕW�n�_���B������
//==========================================================
void CPoliceAI::ReachRoad(void)
{
	// �ړI�n�����݂��鎞
	if (m_pSearchTarget != nullptr)
	{
		// ���̖ړI�n��ݒ�
		D3DXVECTOR3 posRoad = m_pSearchTarget->pConnectRoad->GetPosition();
		D3DXVECTOR3 posPolice = m_pPolice->GetPosition();
		float length = D3DXVec3Length(&(posRoad - posPolice));
		if (length < 500.0f)
		{
			m_pSearchTarget = m_pSearchTarget->pChaild;
		}
	}
}
