//==========================================================
//
// 追加パトカー処理 [police.cpp]
// Author : Ibuki Okusada
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
#include "add_police.h"

// 状態管理時間用名前空間
namespace STATETIME
{
	const float NORMAL = (40.0f);	// 通常
	const float SEARCH = (5.0f);	// 警戒
	const float FADEOUT = (2.0f);	// フェードアウト
}

// 静的メンバ変数宣言
Clist<CAddPolice*> CAddPolice::m_List = {};

//==========================================================
// コンストラクタ
//==========================================================
CAddPolice::CAddPolice(int nId) : CPolice(nId)
{
	m_SpawnPos = VECTOR3_ZERO;
	m_List.Regist(this);
	m_Path.clear();
	m_nNowRoad = 0;
}

//==========================================================
// デストラクタ
//==========================================================
CAddPolice::~CAddPolice()
{
	m_List.Delete(this);
}

//==========================================================
// 生成
//==========================================================
CAddPolice* CAddPolice::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move, int nId)
{
	CAddPolice* pCar = nullptr;

	pCar = DEBUG_NEW CAddPolice(nId);

	if (pCar != nullptr)
	{
		// 初期化処理
		pCar->Init();

		// 座標設定
		pCar->SetPosition(pos);
		pCar->m_SpawnPos = pos;

		// 向き設定
		pCar->SetRotation(rot);

		// 移動量設定
		pCar->SetMove(move);

		// 状態設定
		pCar->SetState(STATE::STATE_NORMAL);
	}

	return pCar;
}

//==========================================================
// 通常状態
//==========================================================
void CAddPolice::StateNormal(void)
{
	// カウントなくなるまで
	if (m_stateInfo.fTimer > 0.0f) { return; }

	if (m_Path.size() > 0) { return; }

	SetState(STATE::STATE_FADEOUT);
}

//==========================================================
// 警戒状態
//==========================================================
void CAddPolice::StateSearch(void)
{
	// カウントなくなるまで
	if (m_stateInfo.fTimer > 0.0f) { return; }

	SetState(STATE::STATE_FADEOUT);
}

//==========================================================
// フェードアウト状態
//==========================================================
void CAddPolice::StateFadeOut(void)
{
	// カウントなくなるまで
	if (m_stateInfo.fTimer > 0.0f) 
	{
		float fMulti = m_stateInfo.fTimer / m_stateInfo.fTimerOrigin;

		m_pObj->SetColMulti(D3DXCOLOR(1.0f, 1.0f, 1.0f, fMulti));
		return; 
	}

	// 削除
	Uninit();
}

//==========================================================
// 警戒状態設定
//==========================================================
void CAddPolice::SetStateNormal(void)
{
	// 初期化
	m_stateInfo = SState();

	// 状態変更
	m_stateInfo.state = STATE::STATE_NORMAL;

	SetStateTimer(STATETIME::NORMAL);
}

//==========================================================
// 警戒状態設定
//==========================================================
void CAddPolice::SetStateSearch(void)
{
	if (m_Path.size() > 0) { return; }

	// 初期化
	m_stateInfo = SState();

	// 状態変更
	m_stateInfo.state = STATE::STATE_SEARCH;

	SetStateTimer(STATETIME::SEARCH);
}

//==========================================================
// フェードアウト状態設定
//==========================================================
void CAddPolice::SetStateFadeOut(void)
{
	// 初期化
	m_stateInfo = SState();

	// 状態変更
	m_stateInfo.state = STATE::STATE_FADEOUT;

	SetStateTimer(STATETIME::FADEOUT);
}

//==========================================================
// 探索
//==========================================================
void CAddPolice::SearchRoad()
{
	if (m_Path.size() == 0)
	{
		CCar::SearchRoad();
		return;
	}

	if (m_nNowRoad >= m_Path.size() - 1) {

		//if (GetState() == STATE::STATE_NORMAL)
		{
			SetState(STATE::STATE_STOP);
			m_Path.clear();
		}

		return; 
	}
	SetRoadTarget(m_Path[m_nNowRoad]->pRoad);
}

//==========================================================
// 探索終了後
//==========================================================
void CAddPolice::ReachRoad()
{
	if (m_Path.size() == 0)
	{
		CCar::ReachRoad();
		return;
	}

	if (m_nNowRoad >= m_Path.size() - 1)
	{ 
			SetState(STATE::STATE_STOP);
			m_Path.clear();

		return; 
	}
	m_nNowRoad++;
	SetRoadTarget(m_Path[m_nNowRoad]->pRoad);
}