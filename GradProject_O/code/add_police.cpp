//==========================================================
//
// パトカー処理 [police.cpp]
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
#include "add_police.h"

// 状態管理時間用名前空間
namespace STATETIME
{
	const float NORMAL = (10.0f);	// 通常
	const float SEARCH = (5.0f);	// 警戒
	const float FADEOUT = (2.0f);	// フェードアウト
}

//==========================================================
// コンストラクタ
//==========================================================
CAddPolice::CAddPolice()
{
	m_SpawnPos = VECTOR3_ZERO;
}

//==========================================================
// デストラクタ
//==========================================================
CAddPolice::~CAddPolice()
{

}

//==========================================================
// 生成
//==========================================================
CAddPolice* CAddPolice::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move)
{
	CAddPolice* pCar = nullptr;

	pCar = DEBUG_NEW CAddPolice;

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

	CDebugProc::GetInstance()->Print("もうすぐ消えるよ～！\n");

	// 削除
	Uninit();
}

//==========================================================
// 警戒状態設定
//==========================================================
void CAddPolice::SetStateNormal(void)
{
	SetStateTimer(STATETIME::NORMAL);
}

//==========================================================
// 警戒状態設定
//==========================================================
void CAddPolice::SetStateSearch(void)
{
	SetStateTimer(STATETIME::SEARCH);
}

//==========================================================
// フェードアウト状態設定
//==========================================================
void CAddPolice::SetStateFadeOut(void)
{
	SetStateTimer(STATETIME::FADEOUT);
}