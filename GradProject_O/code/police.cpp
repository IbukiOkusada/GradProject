//==========================================================
//
// �p�g�J�[���� [police.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "police.h"
#include "car.h"
#include "car_manager.h"
#include "road.h"
#include "road_manager.h"
#include "manager.h"
#include "debugproc.h"
#include "collision.h"
#include "player.h"
#include "player_manager.h"
#include "debugproc.h"
#include "police_manager.h"
#include "deltatime.h"
#include "a_star.h"
#include "police_AI.h"

// �}�N����`

// �������O��Ԃ��`
namespace
{
	const float LENGTH_POINT = (200.0f);			// ���B���苗��
	const float LENGTH_POINT_CHASE = (500.0f);		// ����Ⴂ���苗��
	const float SECURE_SPEEDDEST = (-35.0f);		// �m�ێ��̖ڕW���x
	const float SECURE_SPEED = (0.8f);				// �m�ێ��̉����{��
	const float ROT_MULTI_DEF = (0.06f);			// �ʏ펞�̌����␳�{��
	const float ROT_MULTI_CHASE = (0.13f);			// �ǐՎ��̌����␳�{��
}

//==========================================================================
// �֐��|�C���^
//==========================================================================
// ��ԊǗ�
CPolice::STATE_FUNC CPolice::m_StateFunc[] =
{
	&CPolice::StateNormal,		// �ʏ�
	&CPolice::StateChase,		// �ǐ�
	&CPolice::StateSearch,		// �x��
	&CPolice::StateFadeOut,		// �t�F�[�h�A�E�g
	&CPolice::StateStop,		// ��~
};

// ��Ԑݒ�
CPolice::SETSTATE_FUNC CPolice::m_SetStateFunc[] =
{
	&CPolice::SetStateNormal,		// �ʏ�
	&CPolice::SetStateChase,		// �ǐ�
	&CPolice::SetStateSearch,		// �x��
	&CPolice::SetStateFadeOut,		// �t�F�[�h�A�E�g
	&CPolice::SetStateStop,			// ��~
};

//==========================================================
// �R���X�g���N�^
//==========================================================
CPolice::CPolice(int nId) : CCar(nId)
{
	// �l�̃N���A
	m_Info = SInfo();
	m_pPatrolLamp = nullptr;
	m_pSiren = nullptr;
	m_stateInfo = SState();
	m_pPoliceAI = nullptr;
	CPoliceManager::GetInstance()->ListIn(this);
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CPolice::~CPolice()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CPolice::Init(void)
{
	TailLamp();
	m_pSiren = CMasterSound::CObjectSound::Create("data\\SE\\siren.wav", -1);
	m_pSiren->Stop();
	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, "data\\MODEL\\police.x");
	m_pPoliceAI = CPoliceAI::Create(this, CPoliceAI::TYPE_NORMAL);
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CPolice::Uninit(void)
{
	if (m_pPoliceAI != nullptr)
	{
		m_pPoliceAI->Uninit();
		delete m_pPoliceAI;
		m_pPoliceAI = nullptr;
	}
	
	CCar::Uninit();
	CPoliceManager::GetInstance()->ListOut(this);
	SAFE_DELETE(m_pPatrolLamp);
	SAFE_UNINIT_DELETE(m_pSiren);
	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CPolice::Update(void)
{
	// ��~��ԂȂ瓮���Ȃ�
	if (m_stateInfo.state == STATE::STATE_STOP) 
	{ 
		MoveRoad();
		return;
	}

	CCar::Update();
	if (m_Info.bChase)
	{
		if (m_pPatrolLamp == nullptr)
		{
			m_pPatrolLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\patrollamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
		}
		m_pPatrolLamp->m_pos = GetPosition();
		m_pPatrolLamp->m_rot = GetRotation();
	}
	else
	{
		SAFE_DELETE(m_pPatrolLamp);
	}

	UpdateState();
}

//==========================================================
// ����
//==========================================================
CPolice *CPolice::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move, int nId)
{
	CPolice *pCar = nullptr;

	pCar = DEBUG_NEW CPolice(nId);

	if (pCar != nullptr)
	{
		// ����������
		pCar->Init();

		// ���W�ݒ�
		pCar->SetPosition(pos);

		// �����ݒ�
		pCar->SetRotation(rot);

		// �ړ��ʐݒ�
		pCar->SetMove(move);
	}

	return pCar;
}

//==========================================================
// ���ړ�����
//==========================================================
void CPolice::MoveRoad()
{
	CRoad* pRoadStart = GetRoadStart();
	CRoad* pRoadTarget = GetRoadTarget();

	if (pRoadTarget == nullptr && IsActive())
		SearchRoad();

	if (m_Info.bChase)
	{
		m_pSiren->Start();

		ChasePlayer();
	
		float dis = GetDistance(m_Info.pPlayer->GetPosition() , GetPosition());
		float vol = 8000.0f - dis;
		vol /= 8000.0f;
		if (vol < 0.0f)
		{
			vol = 0.0f;
		}
		m_pSiren->SetVolume(vol);

		if (pRoadTarget != nullptr)
		{
			SetSpeedDest(GetSpeedDest() + m_pPoliceAI->GetChaseSpeed());
			SetPosTarget(pRoadTarget->GetPosition());
			SetRotMulti(ROT_MULTI_CHASE);
		}
		else
		{
			if (m_Info.pPlayer == nullptr) { return; }

			SetPosTarget(m_Info.pPlayer->GetPosition());

			if (D3DXVec3Length(&(m_Info.pPlayer->GetPosition() - GetPosition())) > LENGTH_POINT_CHASE) { return; }

			// ���x��ݒ�
			SetSpeedDest(SECURE_SPEEDDEST);
			SetSpeed(GetSpeed() * SECURE_SPEED);
		}
	}
	else
	{
		if (m_pSiren != nullptr)
		{
			m_pSiren->Stop();
		}

		if (pRoadTarget != nullptr || IsActive())
		{
			pRoadStart = GetRoadStart();
			pRoadTarget = GetRoadTarget();

			float length = D3DXVec3Length(&(pRoadTarget->GetPosition() - GetPosition()));
			if (length < LENGTH_POINT)
				ReachRoad();

			SetPosTarget(pRoadTarget->GetPosition());
			SetRotMulti(ROT_MULTI_DEF);
		}
	}
}

//==========================================================
// �����B������
//==========================================================
void CPolice::ReachRoad()
{
	CRoad* pRoadStart = GetRoadStart();
	CRoad* pRoadTarget = GetRoadTarget();

	CRoad* pRoadNext = nullptr;

	while (1)
	{// �n�_������܂�
		int roadPoint = rand() % CRoad::DIC_MAX;

		pRoadNext = pRoadTarget->GetConnectRoad((CRoad::DIRECTION)roadPoint);

		if (pRoadNext == pRoadStart && pRoadTarget->GetType() != CRoad::TYPE_STOP) { continue; }

		if (pRoadNext != nullptr) { break; }
	}

	pRoadStart = pRoadTarget;
	pRoadTarget = pRoadNext;

	SetRoadStart(pRoadStart);
	SetRoadTarget(pRoadTarget);
}

//==========================================================
// �v���C���[��������
//==========================================================
void CPolice::SearchPlayer()
{
	if (m_pPoliceAI != nullptr)
	{
		m_pPoliceAI->Search();
	}
}

//==========================================================
// �ǐՌo�H�T������
//==========================================================
void CPolice::ChasePlayer()
{
	// �ǐՂ���
	if (m_pPoliceAI == nullptr || !IsActive()) { return; }

	m_pPoliceAI->Chase();

	// �ǐՌo�H�����݂���Ȃ�ΖڕW�n�_�ɐݒ肷��
	if (m_pPoliceAI->GetSearchRoad() != nullptr)
	{
		SetRoadTarget(m_pPoliceAI->GetSearchRoad()->pConnectRoad);
	}
	else
	{
		SetRoadTarget(nullptr);
	}
}

//==========================================================
// �����蔻�菈��
//==========================================================
void CPolice::Collision()
{
	
}

//==========================================================
// �ڐG������
//==========================================================
void CPolice::Hit()
{
	if (GetBack()) { return; }

	if (!m_Info.bChase)
	{
		CRoad* pRoadNext = GetRoadTarget();
		SetRoadTarget(GetRoadStart());
		SetRoadStart(pRoadNext);
	}

	SetBack(true);
	SetBackTime(80);
}

//==========================================================
// �j��
//==========================================================
void CPolice::Break()
{
	CPlayer* p = CPlayerManager::GetInstance()->GetPlayer();
	p->Damage(p->GetLifeOrigin() * 0.1f);
}

//==========================================================
// ��Ԑݒ�
//==========================================================
void CPolice::UpdateState(void)
{
	// �^�C�}�[����
	m_stateInfo.fTimer -= CDeltaTime::GetInstance()->GetDeltaTime();
	if(m_stateInfo.fTimer < 0.0f)
	{ 
		m_stateInfo.fTimer = 0.0f;
	}

	// ��Ԃ��Ƃ̊֐����Ă�
	(this->*(m_StateFunc[m_stateInfo.state]))();
}

//==========================================================
// ��Ԑݒ�
//==========================================================
void CPolice::SetState(const STATE state)
{
	// ��Ԃ��Ƃ̊֐����Ă�
	if (m_stateInfo.state == STATE::STATE_FADEOUT) { return; }

	(this->*(m_SetStateFunc[state]))();
}

//==========================================================
// �ʏ���
//==========================================================
void CPolice::StateNormal(void)
{
	
}

//==========================================================
// �ǐՏ��
//==========================================================
void CPolice::StateChase(void)
{
	if (m_pObj == nullptr) { return; }
}

//==========================================================
// �x�����
//==========================================================
void CPolice::StateSearch(void)
{
	// �J�E���g�Ȃ��Ȃ�܂�
	if (m_stateInfo.fTimer > 0.0f) { return; }

	SetState(STATE::STATE_NORMAL);
}

//==========================================================
// �t�F�[�h�A�E�g���
//==========================================================
void CPolice::StateFadeOut(void)
{

}

//==========================================================
// ��~���
//==========================================================
void CPolice::StateStop(void)
{

}

//==========================================================
// �ʏ��Ԑݒ�
//==========================================================
void CPolice::SetStateNormal(void)
{
	// ������
	m_stateInfo = SState();

	// ��ԕύX
	m_stateInfo.state = STATE::STATE_NORMAL;
}

//==========================================================
// �ǐՏ�Ԑݒ�
//==========================================================
void CPolice::SetStateChase(void)
{
	// ������
	m_stateInfo = SState();

	// ��ԕύX
	m_stateInfo.state = STATE::STATE_CHASE;
}

//==========================================================
// �x����Ԑݒ�
//==========================================================
void CPolice::SetStateSearch(void)
{
	// ������
	m_stateInfo = SState();

	// ��ԕύX
	m_stateInfo.state = STATE::STATE_SEARCH;
	SetStateTimer(5.0f);
}

//==========================================================
// �t�F�[�h�A�E�g��Ԑݒ�
//==========================================================
void CPolice::SetStateFadeOut(void)
{
	// ������
	m_stateInfo = SState();

	// ��ԕύX
	m_stateInfo.state = STATE::STATE_FADEOUT;
}

//==========================================================
// ��~��Ԑݒ�
//==========================================================
void CPolice::SetStateStop(void)
{
	// ������
	m_stateInfo = SState();

	// ��ԕύX
	m_stateInfo.state = STATE::STATE_STOP;
}

//===============================================
// �x�@�̍��W�̑��M
//===============================================
void CPolice::SendPosition()
{
	CNetWork* pNet = CNetWork::GetInstance();

	pNet->SendPdPos(GetId(), GetPosition(), GetRotation());
}

//===============================================
// �x�@�̒ǐՊJ�n���M
//===============================================
void CPolice::SendChase()
{
	if (m_Info.pPlayer == nullptr) { return; }
	CNetWork* pNet = CNetWork::GetInstance();

	pNet->SendPdChase(GetId(), m_Info.pPlayer->GetId());
}

//===============================================
// �x�@�̒ǐՏI�����M
//===============================================
void CPolice::SendChaseEnd()
{
	CNetWork* pNet = CNetWork::GetInstance();

	pNet->SendPdChaseEnd(GetId());
}

//===============================================
// ��M������ސݒ�
//===============================================
void CPolice::RecvTypeSet()
{
	// ��ސݒ�
	CCar::RecvTypeSet();

	if (m_stateInfo.chasenext == CHASE::CHASE_MAX) { return; }

	// ���̎w�肳�ꂽ��Ԃł̐ݒ�
	switch (m_stateInfo.chasenext)
	{
	case CHASE::CHASE_BEGIN:
	{
		// �ǐՊJ�n
		m_pPoliceAI->BeginChase(m_stateInfo.pNextPlayer);
		m_stateInfo.pNextPlayer = nullptr;
	}
	break;

	case CHASE::CHASE_END:
	{
		m_pPoliceAI->EndChase();
	}
	break;

	default:
		break;
	}

	m_stateInfo.chasenext = CHASE::CHASE_MAX;
}