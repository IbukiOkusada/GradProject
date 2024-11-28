//==========================================================
//
// ナビ [navi.cpp]
// Author : 丹野竜之介
//
//==========================================================
#include "navi.h"
#include "player.h"
#include "player_manager.h"
#include "manager.h"
#include "object.h"
#include "object_manager.h"
#include "a_star.h"
#include "goal.h"
#include "road_manager.h"
#include "goal_manager.h"
// マクロ定義
namespace
{
 	const D3DVIEWPORT9 VIEWPORT = (D3DVIEWPORT9{ (DWORD)(SCREEN_WIDTH * 0.7f), (DWORD)(SCREEN_WIDTH * 0.7f), 100,100,1.0,1000});
}
//==========================================================
// コンストラクタ
//==========================================================
CNavi::CNavi()
{
	// 値のクリア
}

//==========================================================
// デストラクタ
//==========================================================
CNavi::~CNavi()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CNavi::Init(void)
{
	m_pGole = nullptr;
	m_IdxPath = 0;
	m_Effects.Clear();
	m_Path.clear();
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CNavi::Uninit(void)
{
	for (int i = m_Effects.GetNum()-1 ; i >= 0; i--)
	{
		SEffect* pEffect = m_Effects.Get(i);
		SAFE_DELETE(pEffect->pLine);
		pEffect->pTarget = nullptr;
		m_Effects.Delete(m_Effects.Get(i));
		SAFE_DELETE(pEffect);
	}

	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CNavi::Update(void)
{
	if (m_Path.empty())
	{
		StartNavigation();
		CreateEffect();
	}
	else
	{
		//UpdateNavigation();
		Reach();
	}
}
//==========================================================
// 開始処理
//==========================================================
void CNavi::StartNavigation(void)
{
	//近いゴールを探索
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
	if (pPlayer == nullptr) { return; }
	float fDis = FLT_MAX;
	m_pGole = nullptr;

	auto mgr = CGoalManager::GetInstance()->GetList();

	// ゴールを全て確認
	for (auto ite = mgr->begin(); ite != mgr->end(); ite++)
	{
		CGoal* pGoal = ite->second;
		if (!pGoal->GetEnd())
		{
			float F = GetDistance(pGoal->GetPos(), pPlayer->GetPosition());
			if (F < fDis)
			{
				fDis = F;
				m_pGole = pGoal;
			}
		}
		
		
	}
	//近い道路を探索
	Clist<CRoad*>* ListRoad = CRoadManager::GetInstance()->GetList();
	float fDis2 = FLT_MAX;
	CRoad* pStart = nullptr;
	for (int i = 0; i < ListRoad->GetNum(); i++)
	{
		float F = GetDistance(ListRoad->Get(i)->GetPosition(), pPlayer->GetPosition());
		if (F < fDis2)
		{
			fDis2 = F;
			pStart = ListRoad->Get(i);
		}
	}
	if (m_pGole != nullptr && pStart != nullptr)
	{
		m_IdxPath = 0;
		m_Path = AStar::AStar(pStart->GetSearchSelf(), m_pGole->GetRoad()->GetSearchSelf());
	}

}
//==========================================================
// エフェクト生成処理
//==========================================================
void CNavi::CreateEffect(void)
{
	if (!m_Path.empty())
	{
		for (int i = 0; i < m_Path.size()-1; i++)
		{
			SEffect* pEffect = DEBUG_NEW SEffect;

		
				pEffect->pLine = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\straight.efkefc", m_Path[i]->pos, VECTOR3_ZERO, VECTOR3_ZERO, 200.0f, true, false);
			
		
			if (i < m_Path.size() - 1)
			{
				D3DXVECTOR3 vec = m_Path[i]->pos - m_Path[i + 1]->pos;

				D3DXVec3Normalize(&vec, &vec);
				pEffect->pLine->m_rot = VectorToAngles(vec);
			}

			m_Effects.Regist(pEffect);
		}
	}
}
//==========================================================
// 更新処理
//==========================================================
void CNavi::UpdateNavigation(void)
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
	float fDis = FLT_MAX;
	int nID = 0;
	for (int i = 0; i < m_Path.size(); i++)
	{
		float F = GetDistance(m_Path[i]->pos, pPlayer->GetPosition());
		if (F < fDis)
		{
			fDis = F;
			nID = i;
		}
	}
	if (nID != 0)
	{
		for (int i = nID - 1; i >= 0; i--)
		{
			m_Path.erase(std::find(m_Path.begin(), m_Path.end(), m_Path[i]));
		}
		m_Path.shrink_to_fit();
	}
	else
	{
		if (fDis < 1000.0f && m_Path.size()>1)
		{
			m_Path.erase(std::find(m_Path.begin(), m_Path.end(), m_Path[0]));
		}
	}
	if (!m_Effects.Empty())
	{
		SEffect* pEffect = m_Effects.Get(m_Effects.GetNum()-1);
		D3DXVECTOR3 vec = pPlayer->GetPosition() - m_Path[0]->pos;
		pEffect->pLine->m_pos = pPlayer->GetPosition();
		D3DXVec3Normalize(&vec, &vec);
		pEffect->pLine->m_rot = VectorToAngles(vec);
	}

}
void CNavi::Reach(void)
{
	if (m_pGole != nullptr)
	{
		if (m_pGole->GetEnd())
		{
			for (int i = m_Effects.GetNum() - 1; i >= 0; i--)
			{
				
				SEffect* pEffect = m_Effects.Get(i);
				SAFE_DELETE(pEffect->pLine);
				pEffect->pTarget = nullptr;
				m_Effects.Delete(m_Effects.Get(i));
				SAFE_DELETE(pEffect);
			}
			for (int i = m_Path.size() - 1; i >= 0; i--)
			{
				m_Path.erase(std::find(m_Path.begin(), m_Path.end(), m_Path[i]));
			}
		}
	}
}
//==========================================================
// 生成
//==========================================================
CNavi* CNavi::Create(void)
{
	CNavi* pNavi = nullptr;

	pNavi = DEBUG_NEW CNavi;

	if (pNavi != nullptr)
	{
		// 初期化処理
		pNavi->Init();
	}

	return pNavi;
}
