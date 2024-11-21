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

// �}�N����`

// �������O��Ԃ��`
namespace
{

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
// �X�V����
//==========================================================
void CPoliceAI::Update(void)
{
	float fDeltaTime = CDeltaTime::GetInstance()->GetDestTime();
	float fSlow = CDeltaTime::GetInstance()->GetSlow();

	SelectRoad();

	ReachRoad();

	if (m_fSearchTimer > 3.0f || m_pSearchTarget == nullptr)
	{
		m_searchRoad = AStar::AStar(m_pRoadStart->GetSearchSelf(), m_pRoadTarget->GetSearchSelf());

		if (!m_searchRoad.empty())
		{
			m_pSearchTarget = m_searchRoad.front();
		}

		m_fSearchTimer = 0;
	}

	m_fSearchTimer += fDeltaTime * fSlow;
}

//==========================================================
// ����
//==========================================================
CPoliceAI *CPoliceAI::Create(CPolice* pPolice)
{
	CPoliceAI *pPoliceAI = nullptr;

	pPoliceAI = DEBUG_NEW CPoliceAI;

	if (pPoliceAI != nullptr)
	{
		pPoliceAI->m_pPolice = pPolice;

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
	m_pRoadStart = m_pPolice->GetRoadTarget();

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
// �ڕW�n�_���B����������
//==========================================================
void CPoliceAI::ReachRoad(void)
{
	if (m_pSearchTarget != nullptr)
	{
		CRoad* pRoadTarget = m_pPolice->GetRoadTarget();
		D3DXVECTOR3 posRoad = pRoadTarget->GetPosition();
		D3DXVECTOR3 posPolice = m_pPolice->GetPosition();
		float length = D3DXVec3Length(&(posRoad - posPolice));
		if (length < 3000.0f)
		{
			m_pSearchTarget = m_pSearchTarget->pChaild;
		}
	}
}
