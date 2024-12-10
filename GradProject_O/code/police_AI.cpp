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
#include "police_AI_manager.h"
#include "deltatime.h"
#include "a_star.h"
#include "pred_route.h"

// �}�N����`

// �������O��Ԃ��`
namespace
{
	const float SECURE_SPEEDDEST = (-35.0f);		// �m�ێ��̖ڕW���x
	const float SECURE_SPEED = (0.8f);				// �m�ێ��̉����{��

	const float CHASE_SECURE = (400.0f);			// �ǐՊm�ۋ���
	const float CHASE_CROSS = (500.0f);				// ����Ⴂ���苗��
	const float CHASE_NEAR = (2000.0f);				// �ߋ�������
	const float CHASE_FAR = (3500.0f);				// ����������

	const float	LIFE_DAMAGE = (80.0f);				// ������̃_���[�W
	const float	LIFE_SMOKE = (50.0f);				// ������̃_���[�W
	const float	LEVEL_MAX = (30.0f);				// �x���x�̍ő�l
	const float	LEVEL_MIN = (0.0f);					// �x���x�̍ŏ��l
	const float	LEVEL_NORMAL = (-0.1f);				// �ʏ펞�̌x���x������
	const float	LEVEL_NEAR = (0.3f);				// �ߋ������̌x���x������
	const float	LEVEL_FAR = (0.2f);					// ���������̌x���x������
	const float	LEVEL_NITRO = (1.0f);				// �j�g�����̌x���x������
	const float	LEVEL_DAMAGE = (0.3f);				// ����Ԏ��̌x���x������
	const float	LEVEL_SMOKE = (0.5f);				// ����Ԏ��̌x���x������

	const int CHASE_TIME = (100);					// �ǐՎ���

	const float CHASE_SPEED_DEF = (22.0f);			// �f�t�H���g�̒ǐՎ��̉���
	const float CHASE_SPEED_NORMAL = (22.0f);		// �ʏ�^�C�v�̒ǐՎ��̉���
	const float CHASE_SPEED_ELITE = (24.0f);		// ��荞�݃^�C�v�̒ǐՎ��̉���
	const float CHASE_SPEED_GENTLE = (18.0f);		// �ɂ₩�^�C�v�̒ǐՎ��̉���

	const float SEARCH_TIME_DEF = (3.0f);			// �f�t�H���g�̌o�H�m�F�Ԋu
	const float SEARCH_TIME_NORMAL = (3.0f);		// �ʏ�^�C�v�̌o�H�m�F�Ԋu
	const float SEARCH_TIME_ELITE = (2.5f);			// ��荞�݃^�C�v�̌o�H�m�F�Ԋu
	const float SEARCH_TIME_GENTLE = (5.0f);		// �ɂ₩�^�C�v�̌o�H�m�F�Ԋu
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
	m_fLevelSearch = 0.0f;
	m_fChaseSpeed = 0.0f;
	m_fSearchInterval = 0.0f;
	m_nCntThread = 0;
	m_bCross = false;
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
	CPoliceAIManager::GetInstance()->ListIn(this);
	m_fChaseSpeed = CHASE_SPEED_DEF;
	m_fSearchInterval = SEARCH_TIME_DEF;

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CPoliceAI::Uninit(void)
{
	while (m_nCntThread == 1) {}
	CPoliceAIManager::GetInstance()->ListOut(this);
}

//==========================================================
// ��������
//==========================================================
void CPoliceAI::Search(void)
{
	auto mgr = CPlayerManager::GetInstance();
	auto list = mgr->GetList();

	for (const auto& pair : *list->GetList())
	{
		// �ǐՂ���v���C���[������ꍇ���̃v���C���[�ȊO�͔��肵�Ȃ�
		if (m_pPolice->GetPlayer() != nullptr && m_pPolice->GetPlayer() != pair.second) { continue; }

		// �v���C���[���擾
		CPlayer* pPlayer = pair.second;
		D3DXVECTOR3 vecPlayer = pPlayer->GetPosition() - m_pPolice->GetPosition();		// �v���C���[�ƌx�@�Ԃ̃x�N�g���v�Z
		float length = D3DXVec3Length(&vecPlayer);										// �����v�Z
		float rotVec = atan2f(vecPlayer.x, vecPlayer.z);								// �p�x�v�Z
		float rotView = m_pPolice->GetRotation().y - rotVec;							// �����Ă�����Ƃ̍����v�Z

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
			if (!m_pPolice->GetChase()) { continue; }

			// �ǐՊJ�n����
			BeginChase(pPlayer);

			// �ڋߏ�Ԃ�ݒ�
			m_bCross = true;
		}
		else if (length < CHASE_NEAR)
		{// �ߋ���

			// ���E���ɓ����Ă��邩�ǂ���
			if (fabs(rotView) > D3DX_PI * 0.3f && !m_pPolice->GetChase()) { continue; }

			// �x���x�𑝉�������
			m_fLevelSearch += LEVEL_NEAR;

			// �e�󋵊m�F
			CheckSpeed(pPlayer);
			CheckTurn(pPlayer);
			CheckDamage(pPlayer);
			CheckCollision(pPlayer);
			CheckLevel(pPlayer);

			// �ǐՏ�ԂłȂ���Δ�����
			if (!m_pPolice->GetChase()) { continue; }

			// �ǐՊJ�n����
			BeginChase(pPlayer);

			// �ڋߏ�Ԃ�ݒ�
			m_bCross = true;
		}
		else if (length < CHASE_FAR)
		{// ������

			// ���E���ɓ����Ă��邩�ǂ���
			if (fabs(rotView) > D3DX_PI * 0.3f && !m_pPolice->GetChase()) { continue; }

			// �x���x�𑝉�������
			m_fLevelSearch += LEVEL_FAR;

			// �e�󋵊m�F
			CheckSpeed(pPlayer);
			CheckSmoke(pPlayer);
			CheckCollision(pPlayer);
			CheckLevel(pPlayer);

			// �ǐՏ�ԂłȂ���Δ�����
			if (!m_pPolice->GetChase()) { continue; }

			// �ǐՊJ�n����
			BeginChase(pPlayer);
		}
		else
		{// �͈͊O

			// �ǐՏ�ԂłȂ���Δ�����
			if (!m_pPolice->GetChase()) { continue; }

			// ��x���ڋ߂��Ă��Ȃ���Δ�����
			if (!m_bCross) { continue; }

			// �ǐՎ��Ԃ����炷
			m_pPolice->SetChaseCount(m_pPolice->GetChaseCount() - 1);

			// �ǐՎ��Ԃ�0�ɂȂ�����x����ԂɈڍs
			if (m_pPolice->GetChaseCount() >= 0) { continue; }

			// �ǐՏI������
			EndChase();

			// �ǐՏ�Ԃ𑗐M
			if (m_pPolice->IsActive())
			{
				m_pPolice->SendChaseEnd();
			}
		}
	}

	// �x���x������������
	m_fLevelSearch += LEVEL_NORMAL;

	// �ŏ��l�����������␳����
	if (m_fLevelSearch < LEVEL_MIN)
	{
		m_fLevelSearch = LEVEL_MIN;
	}
}

//==========================================================
// �ǐՊJ�n����
//==========================================================
void CPoliceAI::BeginChase(CPlayer* pPlayer)
{
	// �ǐՂ���v���C���[�����Ȃ��ꍇ
	if (m_pPolice->GetPlayer() == nullptr)
	{
		// �ǐՂ���v���C���[������
		m_pPolice->SetPlayer(pPlayer);

		// �ǐՏ�Ԃ𑗐M
		if (m_pPolice->IsActive())
		{
			m_pPolice->SendChase();
		}
	}

	// �ǐՎ��Ԃ�ݒ�
	m_pPolice->SetChaseCount(CHASE_TIME);

	// ��Ԑݒ�
	m_pPolice->SetState(CPolice::STATE::STATE_CHASE);
}

//==========================================================
// �ǐՏI������
//==========================================================
void CPoliceAI::EndChase(void)
{
	// �ǐՂ���v���C���[������
	m_pPolice->SetPlayer(nullptr);

	// �J�E���g��0�ɌŒ�
	m_pPolice->SetChaseCount(0);

	// �ǐՏ�Ԃ�����
	m_pPolice->SetChase(false);

	// �ڋߏ�Ԃ�����
	m_bCross = false;

	// �x���x�����Z�b�g
	m_fLevelSearch = LEVEL_MIN;

	// �x����Ԃ�
	m_pPolice->SetState(CPolice::STATE::STATE_SEARCH);

	// �S�����x����Ԃɂ���
	CPoliceManager::GetInstance()->Warning(m_pPolice);
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
		m_fLevelSearch += LEVEL_NITRO;
	}
}

//==========================================================
// ����m�F����
//==========================================================
void CPoliceAI::CheckTurn(CPlayer* pPlayer)
{
	// �v���C���[�����݂��Ȃ��Ȃ甲����
	if (pPlayer == nullptr) { return; }

	D3DXVECTOR3 rotTurn = pPlayer->GetRotation() - pPlayer->GetOldRotation();

	m_fLevelSearch += fabs(rotTurn.y);
}

//==========================================================
// ���m�F����
//==========================================================
void CPoliceAI::CheckDamage(CPlayer* pPlayer)
{
	// �v���C���[�����݂��Ȃ��Ȃ甲����
	if (pPlayer == nullptr) { return; }

	// ���C�t�����l�ȉ��̎�
	if (pPlayer->GetLife() < LIFE_DAMAGE)
	{
		m_fLevelSearch += LEVEL_DAMAGE;
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
	if (pPlayer->GetLife() < LIFE_SMOKE)
	{
		m_fLevelSearch += LEVEL_SMOKE;
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
// �x���x�m�F����
//==========================================================
void CPoliceAI::CheckLevel(CPlayer* pPlayer)
{
	// �v���C���[�����݂��Ȃ��Ȃ甲����
	if (pPlayer == nullptr) { return; }

	if (m_fLevelSearch > LEVEL_MAX)
	{
		m_pPolice->SetChase(true);
		m_fLevelSearch = LEVEL_MAX;
	}
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
	if (m_fSearchTimer > m_fSearchInterval || m_pSearchTarget == nullptr)
	{
		// ���X�g����łȂ���Έړ���ݒ�
		if (!m_searchRoad.empty())
		{
			m_pSearchTarget = m_searchRoad.front();
		}
		else
		{
			m_pSearchTarget = nullptr;
		}
		
		m_fSearchTimer = 0.0f;
	}

	// �ړI�n���B����
	ReachRoad();

	m_fSearchTimer += fDeltaTime * fSlow;
}

//==========================================================
// �ǐՎ��̌o�H�T������
//==========================================================
void CPoliceAI::ChaseAStar(void)
{
	m_nCntThread = 1;

	// ���ݒn�ƖړI�n���ʂ̎�
	if (m_pRoadStart != m_pRoadTarget || m_pRoadStart == nullptr || m_pRoadTarget == nullptr)
	{
		// �o�H�T��
		m_searchRoad = AStar::AStarPolice(m_pRoadStart, m_pRoadTarget);
	}
	else
	{
		m_searchRoad.clear();
	}
	
	m_nCntThread--;
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

	case TYPE_GENTLE:
		pPoliceAI = DEBUG_NEW CPoliceAIGentle;
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
		if (length < 1500.0f)
		{
			m_pSearchTarget = m_pSearchTarget->pChild;
		}
	}
}

//==========================================================
// ����������
//==========================================================
HRESULT CPoliceAINomal::Init(void)
{
	CPoliceAI::Init();

	m_fChaseSpeed = CHASE_SPEED_NORMAL;
	m_fSearchInterval = SEARCH_TIME_NORMAL;

	return S_OK;
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
// ����������
//==========================================================
HRESULT CPoliceAIElite::Init(void)
{
	CPoliceAI::Init();

	m_fChaseSpeed = CHASE_SPEED_ELITE;
	m_fSearchInterval = SEARCH_TIME_ELITE;

	return S_OK;
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
		m_pRoadRelay = pPlayer->GetPredRoute()->GetPredRoad();
		m_pRoadTarget = pPlayer->GetRoad();
	}
	else
	{
		m_pRoadTarget = m_pPolice->GetRoadTarget();
	}
}

//==========================================================
// �ǐՎ��̌o�H�T������
//==========================================================
void CPoliceAIElite::ChaseAStar(void)
{
	m_nCntThread = 1;

	// ���ݒn�ƖړI�n���ʂ̎�
	if (m_pRoadStart != m_pRoadTarget || m_pRoadStart == nullptr || m_pRoadTarget == nullptr)
	{
		// �o�H�T��
		m_searchRoad = AStar::AStarPoliceDetour(m_pRoadStart, m_pRoadRelay, m_pRoadTarget);
	}
	else
	{
		m_searchRoad.clear();
	}

	m_nCntThread--;
}

//==========================================================
// ����������
//==========================================================
HRESULT CPoliceAIGentle::Init(void)
{
	CPoliceAI::Init();

	m_fChaseSpeed = CHASE_SPEED_GENTLE;
	m_fSearchInterval = SEARCH_TIME_GENTLE;

	return S_OK;
}

//==========================================================
// �ړ����[�g�p�̓��I������
//==========================================================
void CPoliceAIGentle::SelectRoad(void)
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
