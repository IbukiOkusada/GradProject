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
	float fDis = 0;
	m_pGole = nullptr;
	for (int i = 0; i < List->GetNum(); i++)
	{
		float F = GetDistance(List->Get(i)->GetPos(), pPlayer->GetPosition());
		if (F > fDis)
		{
			fDis = F;
			m_pGole = List->Get(i);
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
	m_Path = AStar(pStart->GetSearchSelf(), m_pGole->GetRoad()->GetSearchSelf());
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

		if (i > 0)
		{
			pEffect->pLine = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\marker_laser.efkefc", m_Path[i]->pos, VECTOR3_ZERO, VECTOR3_ZERO, 200.0f, true, false);
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
