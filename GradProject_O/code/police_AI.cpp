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
#include "police_AI_manager.h"
#include "deltatime.h"
#include "a_star.h"
#include "pred_route.h"

// マクロ定義

// 無名名前空間を定義
namespace
{
	const float SECURE_SPEEDDEST = (-35.0f);		// 確保時の目標速度
	const float SECURE_SPEED = (0.8f);				// 確保時の加速倍率
	const float CHASE_SECURE = (400.0f);			// 追跡確保距離
	const float CHASE_CROSS = (500.0f);				// すれ違い判定距離
	const float CHASE_NEAR = (2000.0f);				// 近距離判定
	const float CHASE_FAR = (3500.0f);				// 遠距離判定
	const int CHASE_TIME = (100);					// 追跡時間
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
	m_nCntThread = 0;
	bCross = false;
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
	CPoliceAIManager::GetInstance()->ListIn(this);

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CPoliceAI::Uninit(void)
{
	while (m_nCntThread == 1) {}
	CPoliceAIManager::GetInstance()->ListOut(this);
}

//==========================================================
// 発見処理
//==========================================================
void CPoliceAI::Search(void)
{
	auto mgr = CPlayerManager::GetInstance();
	auto list = mgr->GetList();

	for (const auto& pair : *list->GetList())
	{
		// 追跡するプレイヤーがいる場合そのプレイヤー以外は判定しない
		if (m_pPolice->GetPlayer() != nullptr && m_pPolice->GetPlayer() != pair.second) { continue; }

		// プレイヤー情報取得
		CPlayer* pPlayer = pair.second;
		D3DXVECTOR3 vecPlayer = pPlayer->GetPosition() - m_pPolice->GetPosition();		// プレイヤーと警察間のベクトル計算
		float length = D3DXVec3Length(&vecPlayer);										// 距離計算
		float rotVec = atan2f(vecPlayer.x, vecPlayer.z);								// 角度計算
		float rotView =m_pPolice->GetRotation().y - rotVec;								// 向いてる方向との差を計算

		// 角度補正
		if (rotView > D3DX_PI)
		{
			rotView -= D3DX_PI * 2.0f;
		}
		else if (rotView < -D3DX_PI)
		{
			rotView += D3DX_PI * 2.0f;
		}

		// 距離によって判断
		if (length < CHASE_CROSS)
		{// 至近距離

			// 追跡状態に変更
			m_pPolice->SetChase(true);

			// 追跡状態でなければ抜ける
			if (!m_pPolice->GetChase()) { continue; }

			// 追跡開始処理
			BeginChase(pPlayer);

			// 接近状態を設定
			bCross = true;

			// 速度を設定
			m_pPolice->SetSpeedDest(SECURE_SPEEDDEST);
			m_pPolice->SetSpeed(m_pPolice->GetSpeed() * SECURE_SPEED);
		}
		else if (length < CHASE_NEAR)
		{// 近距離

			// 視界内に入っているかどうか
			if (fabs(rotView) > D3DX_PI * 0.3f && !m_pPolice->GetChase()) { continue; }

			// 各状況確認
			CheckSpeed(pPlayer);
			CheckTurn(pPlayer);
			CheckDamage(pPlayer);
			CheckCollision(pPlayer);

			// 追跡状態でなければ抜ける
			if (!m_pPolice->GetChase()) { continue; }

			// 追跡開始処理
			BeginChase(pPlayer);

			// 接近状態を設定
			bCross = true;
		}
		else if (length < CHASE_FAR)
		{// 遠距離

			// 視界内に入っているかどうか
			if (fabs(rotView) > D3DX_PI * 0.3f && !m_pPolice->GetChase()) { continue; }

			// 各状況確認
			CheckSpeed(pPlayer);
			CheckSmoke(pPlayer);
			CheckCollision(pPlayer);

			// 追跡状態でなければ抜ける
			if (!m_pPolice->GetChase()) { continue; }

			// 追跡開始処理
			BeginChase(pPlayer);
		}
		else
		{// 範囲外

			// 追跡状態でなければ抜ける
			if (!m_pPolice->GetChase()) { continue; }

			// 一度も接近していなければ抜ける
			if (!bCross) { continue; }

			// 追跡時間を減らす
			m_pPolice->SetChaseCount(m_pPolice->GetChaseCount() - 1);

			// 追跡時間が0になったら警戒状態に移行
			if (m_pPolice->GetChaseCount() >= 0) { continue; }

			// 追跡終了処理
			EndChase();

			// 追跡状態を送信
			if (m_pPolice->IsActive())
			{
				m_pPolice->SendChaseEnd();
			}
		}
	}
}

//==========================================================
// 追跡開始処理
//==========================================================
void CPoliceAI::BeginChase(CPlayer* pPlayer)
{
	// 追跡するプレイヤーがいない場合
	if (m_pPolice->GetPlayer() == nullptr)
	{
		// 追跡するプレイヤーを決定
		m_pPolice->SetPlayer(pPlayer);

		// 追跡状態を送信
		if (m_pPolice->IsActive())
		{
			m_pPolice->SendChase();
		}
	}

	// 追跡時間を設定
	m_pPolice->SetChaseCount(CHASE_TIME);

	// 状態設定
	m_pPolice->SetState(CPolice::STATE::STATE_CHASE);
}

//==========================================================
// 追跡終了処理
//==========================================================
void CPoliceAI::EndChase(void)
{
	// 追跡するプレイヤーを消す
	m_pPolice->SetPlayer(nullptr);

	// カウントを0に固定
	m_pPolice->SetChaseCount(0);

	// 追跡状態を解除
	m_pPolice->SetChase(false);

	// 接近状態を解除
	bCross = false;

	// 警戒状態に
	m_pPolice->SetState(CPolice::STATE::STATE_SEARCH);

	// 全員を警戒状態にする
	CPoliceManager::GetInstance()->Warning(m_pPolice);
}

//==========================================================
// 速度計測処理
//==========================================================
void CPoliceAI::CheckSpeed(CPlayer* pPlayer)
{
	// プレイヤーが存在しないなら抜ける
	if (pPlayer == nullptr) { return; }

	 // ニトロを使っている時
	if (pPlayer->GetState() == CPlayer::STATE_NITRO)
	{
		m_pPolice->SetChase(true);
	}
}

//==========================================================
// 旋回確認処理
//==========================================================
void CPoliceAI::CheckTurn(CPlayer* pPlayer)
{
	// プレイヤーが存在しないなら抜ける
	if (pPlayer == nullptr) { return; }
}

//==========================================================
// 傷確認処理
//==========================================================
void CPoliceAI::CheckDamage(CPlayer* pPlayer)
{
	// プレイヤーが存在しないなら抜ける
	if (pPlayer == nullptr) { return; }

	// ライフが一定値以下の時
	if (pPlayer->GetLife() < 80.0f)
	{
		m_pPolice->SetChase(true);
	}
}

//==========================================================
// 煙確認処理
//==========================================================
void CPoliceAI::CheckSmoke(CPlayer* pPlayer)
{
	// プレイヤーが存在しないなら抜ける
	if (pPlayer == nullptr) { return; }

	// ライフが一定値以下の時
	if (pPlayer->GetLife() < 50.0f)
	{
		m_pPolice->SetChase(true);
	}
}

//==========================================================
// 事故確認処理
//==========================================================
void CPoliceAI::CheckCollision(CPlayer* pPlayer)
{
	// プレイヤーが存在しないなら抜ける
	if (pPlayer == nullptr) { return; }
}

//==========================================================
// 追跡処理
//==========================================================
void CPoliceAI::Chase(void)
{
	// 変数宣言
	float fDeltaTime = CDeltaTime::GetInstance()->GetDestTime();
	float fSlow = CDeltaTime::GetInstance()->GetSlow();

	// 目的地選択
	SelectRoad();

	// 一定時間ごともしくはターゲットが存在しない時
	if (m_fSearchTimer > 3.0f || m_pSearchTarget == nullptr)
	{
		// リストが空でなければ移動先設定
		if (!m_searchRoad.empty())
		{
			m_pSearchTarget = m_searchRoad.front();
		}
		else
		{
			m_pSearchTarget = nullptr;
		}
		
		m_fSearchTimer = 0.0f;
	}

	// 目的地到達判定
	ReachRoad();

	m_fSearchTimer += fDeltaTime * fSlow;
}

//==========================================================
// 追跡時の経路探索処理
//==========================================================
void CPoliceAI::ChaseAStar(void)
{
	m_nCntThread = 1;

	// 現在地と目的地が別の時
	if (m_pRoadStart != m_pRoadTarget || m_pRoadStart == nullptr || m_pRoadTarget == nullptr)
	{
		// 経路探索
		m_searchRoad = AStar::AStarPolice(m_pRoadStart, m_pRoadTarget);
	}
	else
	{
		m_searchRoad.clear();
	}
	
	m_nCntThread--;
}

//==========================================================
// 生成処理
//==========================================================
CPoliceAI *CPoliceAI::Create(CPolice* pPolice, TYPE type)
{
	CPoliceAI *pPoliceAI = nullptr;

	switch (type)
	{
	case TYPE_NORMAL:
		pPoliceAI = DEBUG_NEW CPoliceAINomal;
		break;

	case TYPE_ELITE:
		pPoliceAI = DEBUG_NEW CPoliceAIElite;
		break;

	case TYPE_GENTLE:
		pPoliceAI = DEBUG_NEW CPoliceAIGentle;
		break;

	default:
		pPoliceAI = DEBUG_NEW CPoliceAI;
		break;
	}

	if (pPoliceAI != nullptr)
	{
		pPoliceAI->m_pPolice = pPolice;
		pPoliceAI->m_type = type;

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
	// 探索開始地点に現在の目的地を設定
	m_pRoadStart = m_pPolice->GetRoadTarget();

	// プレイヤーの最寄りの道を目標地点に設定
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
// 目標地点到達時処理
//==========================================================
void CPoliceAI::ReachRoad(void)
{
	// 目的地が存在する時
	if (m_pSearchTarget != nullptr)
	{
		// 次の目的地を設定
		D3DXVECTOR3 posRoad = m_pSearchTarget->pConnectRoad->GetPosition();
		D3DXVECTOR3 posPolice = m_pPolice->GetPosition();
		float length = D3DXVec3Length(&(posRoad - posPolice));
		if (length < 1500.0f)
		{
			m_pSearchTarget = m_pSearchTarget->pChild;
		}
	}
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CPoliceAINomal::Init(void)
{
	CPoliceAI::Init();
	return S_OK;
}

//==========================================================
// 移動ルート用の道選択処理
//==========================================================
void CPoliceAINomal::SelectRoad(void)
{
	// 探索開始地点に現在の目的地を設定
	m_pRoadStart = m_pPolice->GetRoadTarget();

	// プレイヤーの最寄りの道を目標地点に設定
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
// 初期化処理
//==========================================================
HRESULT CPoliceAIElite::Init(void)
{
	CPoliceAI::Init();
	return S_OK;
}

//==========================================================
// 移動ルート用の道選択処理
//==========================================================
void CPoliceAIElite::SelectRoad(void)
{
	// 探索開始地点に現在の目的地を設定
	m_pRoadStart = m_pPolice->GetRoadTarget();

	// プレイヤーの最寄りの道を目標地点に設定
	CPlayer* pPlayer = m_pPolice->GetPlayer();
	if (pPlayer != nullptr)
	{
		m_pRoadRelay = pPlayer->GetPredRoute()->GetPredRoad();
		m_pRoadTarget = pPlayer->GetRoad();
	}
	else
	{
		m_pRoadTarget = m_pPolice->GetRoadTarget();
	}
}

//==========================================================
// 追跡時の経路探索処理
//==========================================================
void CPoliceAIElite::ChaseAStar(void)
{
	m_nCntThread = 1;

	// 現在地と目的地が別の時
	if (m_pRoadStart != m_pRoadTarget || m_pRoadStart == nullptr || m_pRoadTarget == nullptr)
	{
		// 経路探索
		m_searchRoad = AStar::AStarPoliceDetour(m_pRoadStart, m_pRoadRelay, m_pRoadTarget);
	}
	else
	{
		m_searchRoad.clear();
	}

	m_nCntThread--;
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CPoliceAIGentle::Init(void)
{
	CPoliceAI::Init();
	return S_OK;
}

//==========================================================
// 移動ルート用の道選択処理
//==========================================================
void CPoliceAIGentle::SelectRoad(void)
{
	// 探索開始地点に現在の目的地を設定
	m_pRoadStart = m_pPolice->GetRoadTarget();

	// プレイヤーの最寄りの道を目標地点に設定
	CPlayer* pPlayer = m_pPolice->GetPlayer();
	if (pPlayer != nullptr)
	{
		m_pRoadTarget = pPlayer->GetPredRoute()->GetPredRoad();
	}
	else
	{
		m_pRoadTarget = m_pPolice->GetRoadTarget();
	}
}
