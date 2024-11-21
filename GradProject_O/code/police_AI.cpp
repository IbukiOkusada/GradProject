//==========================================================
//
// パトカーAI処理 [police_AI.cpp]
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

// マクロ定義

// 無名名前空間を定義
namespace
{

}

//==========================================================
// コンストラクタ
//==========================================================
CPoliceAI::CPoliceAI()
{
	// 値のクリア
	m_pPolice = nullptr;
	m_pRoadStart = nullptr;
	m_pRoadTarget = nullptr;
	m_pSearchTarget = nullptr;
	m_fSearchTimer = 0.0f;
}

//==========================================================
// デストラクタ
//==========================================================
CPoliceAI::~CPoliceAI()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CPoliceAI::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CPoliceAI::Uninit(void)
{
	CDeltaTime::GetInstance()->GetDeltaTime();
}

//==========================================================
// 更新処理
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
// 生成
//==========================================================
CPoliceAI *CPoliceAI::Create(CPolice* pPolice)
{
	CPoliceAI *pPoliceAI = nullptr;

	pPoliceAI = DEBUG_NEW CPoliceAI;

	if (pPoliceAI != nullptr)
	{
		pPoliceAI->m_pPolice = pPolice;

		// 初期化処理
		pPoliceAI->Init();
	}

	return pPoliceAI;
}

//==========================================================
// 移動ルート用の道選択処理
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
// 目標地点到達時処理処理
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
