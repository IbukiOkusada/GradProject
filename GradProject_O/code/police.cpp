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
	const float CHASE_BEGIN = (700.0f);			// �ǐՊJ�n����
	const float CHASE_CONTINUE = (2000.0f);		// �ǐՌp������
	const float CHASE_END = (3000.0f);			// �ǐՏI������
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CPolice::CPolice()
{
	// �l�̃N���A
	m_Info.pPlayer = nullptr;
	m_Info.nChaseCount = 0;
	m_Info.bChase = false;
	m_pPatrolLamp = nullptr;
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
	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, "data\\MODEL\\car003.x");
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CPolice::Uninit(void)
{
	CCar::Uninit();
	SAFE_DELETE(m_pPatrolLamp);
	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CPolice::Update(void)
{
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
}

//==========================================================
// ����
//==========================================================
CPolice *CPolice::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move)
{
	CPolice *pCar = nullptr;

	pCar = new CPolice;

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
		if (m_Info.pPlayer != nullptr)
			SetPosTarget(m_Info.pPlayer->GetPosition());

		SetSpeedDest(GetSpeedDest() + CHASE_SPEED);
	}
	else
	{
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

	CRoadManager* pRoadManager = CRoadManager::GetInstance();
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
		}
		else if (length < CHASE_BEGIN)
		{// �ǐՊJ�n

			m_Info.bChase = true;
			m_Info.nChaseCount = CHASE_TIME;

			SetRoadStart(nullptr);
			SetRoadTarget(nullptr);
		}
		else if (length < CHASE_CONTINUE)
		{// �ǐՌp��

			if (m_Info.bChase)
			{
				m_Info.nChaseCount = CHASE_TIME;

				SetRoadStart(nullptr);
				SetRoadTarget(nullptr);
			}
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
					m_Info.bChase = false;
					m_Info.nChaseCount = 0;
				}
			}
		}
		else
		{// �ǐՋ����I��

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
	CRoad* pRoad = pRoadManager->GetTop();

	while (pRoad != nullptr)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		CRoad* pRoadNext = pRoad->GetNext();	// ���̃I�u�W�F�N�g�ւ̃|�C���^���擾

		pRoad = pRoadNext;	// ���̃I�u�W�F�N�g�Ɉړ�
	}
}

//==========================================================
// �����蔻�菈��
//==========================================================
void CPolice::Collision()
{
	
}