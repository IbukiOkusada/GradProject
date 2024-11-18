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

// �}�N����`

// �������O��Ԃ��`
namespace
{
	const float LENGTH_POINT = (200.0f);		// ���B���苗��
	const float CHASE_SPEED = (17.0f);			// �ǐՎ��̉���
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
	const float CHASE_CONTINUE = (2000.0f);		// �ǐՌp������
	const float CHASE_END = (3000.0f);			// �ǐՏI������
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
CPolice::CPolice()
{
	// �l�̃N���A
	m_Info = SInfo();
	m_pPatrolLamp = nullptr;
	m_stateInfo = SState();

	CPoliceManager::GetInstance()->GetList()->Regist(this);
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
	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, "data\\MODEL\\car003.x");
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CPolice::Uninit(void)
{
	CCar::Uninit();
	CPoliceManager::GetInstance()->GetList()->Delete(this);
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
	if (m_stateInfo.state == STATE::STATE_STOP) { 
		SearchPlayer();
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
CPolice *CPolice::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move)
{
	CPolice *pCar = nullptr;

	pCar = DEBUG_NEW CPolice;

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

	if (pRoadTarget == nullptr && !m_Info.bChase)
		SearchRoad();

	SearchPlayer();

	if (m_Info.bChase)
	{
		m_pSiren->Start();
		if (m_Info.pPlayer != nullptr)
			SetPosTarget(m_Info.pPlayer->GetPosition());
		float dis = GetDistance(m_Info.pPlayer->GetPosition() , GetPosition());
		m_pSiren->SetVolume((2000.0f - dis) * 0.00075f);
		SetSpeedDest(GetSpeedDest() + CHASE_SPEED);
	}
	else
	{
		m_pSiren->Stop();
		if (pRoadTarget != nullptr)
		{
			pRoadStart = GetRoadStart();
			pRoadTarget = GetRoadTarget();

			float length = D3DXVec3Length(&(pRoadTarget->GetPosition() - GetPosition()));
			if (length < LENGTH_POINT)
				ReachRoad();
			SetPosTarget(pRoadTarget->GetPosition());
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

		if (pRoadTarget->GetType() == CRoad::TYPE_STOP)
		{

		}
		else
		{
			if (pRoadNext == pRoadStart)
			{
				continue;
			}
		}

		if (pRoadNext != nullptr)
			break;
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
	m_Info.pPlayer = CPlayerManager::GetInstance()->GetTop();
	
	if (m_Info.pPlayer != nullptr)
	{
		float length = 0.0f;
		length = D3DXVec3Length(&(GetPosition() - m_Info.pPlayer->GetPosition()));
		CManager::GetInstance()->GetDebugProc()->Print("�ԂƎԂ̋��� [ %f ]\n", length);

		if (length < CHASE_SECURE)
		{// �ǐՊJ�n

			m_Info.bChase = true;
			m_Info.nChaseCount = CHASE_TIME;

			SetRoadStart(nullptr);
			SetRoadTarget(nullptr);

			SetSpeedDest(SECURE_SPEEDDEST);
			SetSpeed(GetSpeed() * SECURE_SPEED);

			// ��Ԑݒ�
			SetState(STATE::STATE_CHASE);
		}
		else if (length < CHASE_BEGIN[m_stateInfo.state])
		{// �ǐՊJ�n

			m_Info.bChase = true;
			m_Info.nChaseCount = CHASE_TIME;

			SetRoadStart(nullptr);
			SetRoadTarget(nullptr);

			// ��Ԑݒ�
			SetState(STATE::STATE_CHASE);
		}
		else if (length < CHASE_CONTINUE)
		{// �ǐՌp��

			if (m_Info.bChase)
			{
				m_Info.nChaseCount = CHASE_TIME;

				SetRoadStart(nullptr);
				SetRoadTarget(nullptr);
			}

			// ��Ԑݒ�
			SetState(STATE::STATE_CHASE);
		}
		else if (length < CHASE_END)
		{// �ǐՏI��

			if (m_Info.bChase)
			{
				m_Info.nChaseCount--;

				SetRoadStart(nullptr);
				SetRoadTarget(nullptr);

				if (m_Info.nChaseCount < 0)
				{
					// �S�����x����Ԃ�
					SetState(STATE::STATE_SEARCH);
					CPoliceManager::GetInstance()->Warning(this);
					m_Info.bChase = false;
					m_Info.nChaseCount = 0;
				}
			}
		}
		else
		{// �ǐՋ����I��

			// �ǐՂ��Ă�
			if (m_Info.bChase)
			{
				SetState(STATE::STATE_SEARCH);
				CPoliceManager::GetInstance()->Warning(this);
			}

			m_Info.bChase = false;
			m_Info.nChaseCount = 0;
		}
	}
}

//==========================================================
// �ǐՌo�H�T������
//==========================================================
void CPolice::ChasePlayer()
{
	CRoadManager* pRoadManager = CRoadManager::GetInstance();
	auto list = pRoadManager->GetList();
	
	for (int i = 0; i < list->GetNum() - 1; i++)
	{
		CRoad* pRoad = list->Get(i);
	}
}

//==========================================================
// �����蔻�菈��
//==========================================================
void CPolice::Collision()
{
	
}

//==========================================================
// �j��
//==========================================================
void CPolice::Break()
{
	CPlayer* p = CPlayerManager::GetInstance()->GetTop();
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