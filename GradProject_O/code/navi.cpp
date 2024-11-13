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
	m_Effects.Clear();
	m_Path.clear();
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CNavi::Uninit(void)
{
	for (int i = m_Effects.GetNum() - 1; i >= 0; i--)
	{
		SEffect* pEffect = m_Effects.Get(i);
		SAFE_DELETE(pEffect->pLine);
		SAFE_DELETE(pEffect->pPin);
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
		UpdateNavigation();
		Reach();
	}
}
//==========================================================
// 開始処理
//==========================================================
void CNavi::StartNavigation(void)
{
	//近いゴールを探索
	Clist<CGole*>* List = CGole::GetInstance();
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	float fDis = FLT_MAX;
	m_pGole = nullptr;
	for (int i = 0; i < List->GetNum(); i++)
	{
		if (!List->Get(i)->GetEnd())
		{
			float F = GetDistance(List->Get(i)->GetPos(), pPlayer->GetPosition());
			if (F < fDis)
			{
				fDis = F;
				m_pGole = List->Get(i);
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
		m_Path = AStar(pStart->GetSearchSelf(), m_pGole->GetRoad()->GetSearchSelf());
	}

}
//==========================================================
// エフェクト生成処理
//==========================================================
void CNavi::CreateEffect(void)
{
	
	for (int i = 0; i < m_Path.size(); i++)
	{
		SEffect* pEffect = DEBUG_NEW SEffect;
		pEffect->pPin = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\marker_pin.efkefc", m_Path[i]->pos, VECTOR3_ZERO, VECTOR3_ZERO, 200.0f, true, false);

	
			pEffect->pLine = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\marker_laser.efkefc", m_Path[i]->pos, VECTOR3_ZERO, VECTOR3_ZERO,0.0f, true, false);
			if (i > 0)
			{
				D3DXVECTOR3 vec = m_Path[i - 1]->pos - m_Path[i]->pos;
				pEffect->pLine->m_Scale = D3DXVECTOR3(20.0f, 20.0f, D3DXVec3Length(&vec));
				D3DXVec3Normalize(&vec, &vec);
				pEffect->pLine->m_rot = VectorToAngles(-vec);
			}
		

		m_Effects.Regist(pEffect);
	}
}
//==========================================================
// 更新処理
//==========================================================
void CNavi::UpdateNavigation(void)
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
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
			SAFE_DELETE(m_Effects.Get(i)->pLine);
			SAFE_DELETE(m_Effects.Get(i)->pPin);
			m_Effects.Get(i)->pTarget = nullptr;
			SEffect* pEffect = m_Effects.Get(i);
			m_Effects.Delete(m_Effects.Get(i));
			SAFE_DELETE(pEffect);
		}
		m_Path.shrink_to_fit();
	}
	else
	{
		if (fDis < 1500.0f && m_Path.size()>1)
		{
			m_Path.erase(std::find(m_Path.begin(), m_Path.end(), m_Path[0]));
			SAFE_DELETE(m_Effects.Get(0)->pLine);
			SAFE_DELETE(m_Effects.Get(0)->pPin);
			m_Effects.Get(0)->pTarget = nullptr;

			SEffect* pEffect = m_Effects.Get(0);
			m_Effects.Delete(m_Effects.Get(0));
			SAFE_DELETE(pEffect);
		}
	}
	if (!m_Effects.Empty())
	{
		SEffect* pEffect = m_Effects.Get(0);
		D3DXVECTOR3 vec = pPlayer->GetPosition() - m_Path[0]->pos;
		pEffect->pLine->m_Scale = D3DXVECTOR3(20.0f, 20.0f, D3DXVec3Length(&vec));
		D3DXVec3Normalize(&vec, &vec);
		pEffect->pLine->m_rot = VectorToAngles(-vec);
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
				m_Path.erase(std::find(m_Path.begin(), m_Path.end(), m_Path[i]));
				SEffect* pEffect = m_Effects.Get(i);
				SAFE_DELETE(pEffect->pLine);
				SAFE_DELETE(pEffect->pPin);
				pEffect->pTarget = nullptr;
				m_Effects.Delete(m_Effects.Get(i));
				SAFE_DELETE(pEffect);
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
