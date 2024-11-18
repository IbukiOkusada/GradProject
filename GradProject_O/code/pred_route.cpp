//==========================================================
//
// 道予測処理 [pred_route.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "pred_route.h"
#include "player.h"
#include "road.h"
#include "a_star.h"

// 名前空間定義
namespace
{
	const int MIN_PRED_NUM = 4;	// 最低限の道数
}

//==========================================================
// コンストラクタ
//==========================================================
CPredRoute::CPredRoute()
{
	// 値のクリア
	m_PassRoad.clear();
	m_pPlayer = nullptr;
	m_pOldRoad = nullptr;
}

//==========================================================
// デストラクタ
//==========================================================
CPredRoute::~CPredRoute()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CPredRoute::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CPredRoute::Uninit(void)
{
	// 開放
	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CPredRoute::Update(void)
{
	if (m_pPlayer == nullptr) { return; }

	// 移動した
	if (m_pPlayer->GetRoad() != m_pOldRoad)
	{
		m_pOldRoad = m_pPlayer->GetRoad();

		// 道路の上
		if (m_pOldRoad != nullptr)
		{
			// 間を補間
			Interp();

			// 確認
			TurnCheck();
		}
	}
}

//==========================================================
// 生成
//==========================================================
CPredRoute* CPredRoute::Create(CPlayer* pPlayer)
{
	CPredRoute* pPredRoute = nullptr;

	pPredRoute = DEBUG_NEW CPredRoute;

	if (pPredRoute != nullptr)
	{
		// 初期化処理
		pPredRoute->m_pPlayer = pPlayer;
		pPredRoute->Init();
	}

	return pPredRoute;
}

//==========================================================
// 回転確認
//==========================================================
void CPredRoute::TurnCheck()
{
	// 確認数に満たない
	if (m_PassRoad.size() < MIN_PRED_NUM) { return; }
}

//==========================================================
// 間を補完する
//==========================================================
void CPredRoute::Interp()
{
	// まだ未登録
	if (m_PassRoad.size() == 0) {
		// 通った道を覚える
		m_PassRoad.push_back(m_pOldRoad);
		return;
	}

	CRoad* pOld = m_PassRoad[m_PassRoad.size() - 1];

	// 間を補完する
	std::vector<CRoad::SSearch*> path = AStar(pOld->GetSearchSelf(), m_pOldRoad->GetSearchSelf());

	if (path.size() <= 4)
	{
		for (int i = 1; i < path.size(); i++)
		{
			m_PassRoad.push_back(path[i]->pRoad);
		}
	}

	// 通った道を覚える
	m_PassRoad.push_back(m_pOldRoad);
}

//==========================================================
// 次を予測立てる
//==========================================================
void CPredRoute::Pred()
{

}