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
#include "gimmick.h"
#include "add_police.h"
#include "car_manager.h"

// マクロ定義

// 無名名前空間を定義
namespace
{
	const float CHASE_SECURE = (400.0f);			// 追跡確保距離
	const float CHASE_CROSS = (750.0f);				// すれ違い判定距離
	const float CHASE_NEAR = (3000.0f);				// 近距離判定
	const float CHASE_FAR = (5000.0f);				// 遠距離判定

	const float	LIFE_DAMAGE = (80.0f);				// 傷判定のダメージ
	const float	LIFE_SMOKE = (50.0f);				// 煙判定のダメージ

	const float	LEVEL_MAX = (50.0f);				// 警戒度の最大値
	const float	LEVEL_MIN = (0.0f);					// 警戒度の最小値
	const float	LEVEL_NORMAL = (-0.1f);				// 通常時の警戒度減少量
	const float	LEVEL_NEAR = (0.3f);				// 近距離時の警戒度増加量
	const float	LEVEL_FAR = (0.2f);					// 遠距離時の警戒度増加量
	const float	LEVEL_NITRO = (1.5f);				// ニトロ時の警戒度増加量
	const float	LEVEL_DAMAGE = (0.3f);				// 傷状態時の警戒度増加量
	const float	LEVEL_SMOKE = (0.5f);				// 煙状態時の警戒度増加量

	const int CHASE_TIME = (300);					// 追跡時間
	const int NUM_BACKUP = (2);						// 増援の数

	const float CHASE_SPEED[CPoliceAI::TYPE_MAX] =
	{
		(28.0f),		// デフォルトの追跡時の加速
		(28.0f),		// 通常タイプの追跡時の加速
		(30.0f),		// 回り込みタイプの追跡時の加速
		(25.0f),		// 緩やかタイプの追跡時の加速
	};

	const float ATTACK_SPEED[CPoliceAI::TYPE_MAX] =
	{
		(1.5f),		// デフォルトの攻撃時の加速
		(1.5f),		// 通常タイプの攻撃時の加速
		(2.5f),		// 回り込みタイプの攻撃時の加速
		(3.0f),		// 緩やかタイプの攻撃時の加速
	};

	const float SEARCH_TIME[CPoliceAI::TYPE_MAX] =
	{
		(3.0f),		// デフォルトの経路確認間隔
		(3.0f),		// 通常タイプの経路確認間隔
		(2.5f),		// 回り込みタイプの経路確認間隔
		(5.0f),		// 緩やかタイプの経路確認間隔
	};

	const int CALL_TIME[CPoliceAI::TYPE_MAX] =
	{
		(450),		// デフォルトの応援呼び出し時間
		(450),		// 通常タイプの応援呼び出し時間
		(420),		// 回り込みタイプの応援呼び出し時間
		(480),		// 緩やかタイプの応援呼び出し時間
	};
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
	m_pPoliceBackUp = nullptr;
	m_fSearchTimer = 0.0f;
	m_fLevelSearch = 0.0f;
	m_fChaseSpeed = 0.0f;
	m_fSearchInterval = 0.0f;
	m_nCntThread = 0;
	nAttackTime = 0;
	m_bCross = false;
	m_bCall = false;
	m_state = STATE_NORMAL;
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
	m_fChaseSpeed = CHASE_SPEED[m_type];
	m_fSearchInterval = SEARCH_TIME[m_type];
	m_nCntCall = 0;
	nAttackTime = 420;

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
		float rotView = m_pPolice->GetRotation().y - rotVec;							// 向いてる方向との差を計算

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
			m_bCross = true;
		}
		else if (length < CHASE_NEAR)
		{// 近距離

			// 視界内に入っているかどうか
			if (fabs(rotView) > D3DX_PI * 0.3f && !m_pPolice->GetChase()) { continue; }

			// 警戒度を増加させる
			m_fLevelSearch += LEVEL_NEAR;

			// 各状況確認
			CheckSpeed(pPlayer);
			CheckTurn(pPlayer);
			CheckDamage(pPlayer);
			CheckCollision(pPlayer);
			CheckLevel(pPlayer);

			// 追跡状態でなければ抜ける
			if (!m_pPolice->GetChase()) { continue; }

			// 追跡開始処理
			BeginChase(pPlayer);

			// 接近状態を設定
			m_bCross = true;
		}
		else if (length < CHASE_FAR)
		{// 遠距離

			// 視界内に入っているかどうか
			if (fabs(rotView) > D3DX_PI * 0.3f && !m_pPolice->GetChase()) { continue; }

			// 警戒度を増加させる
			m_fLevelSearch += LEVEL_FAR;

			// 各状況確認
			CheckSpeed(pPlayer);
			CheckSmoke(pPlayer);
			CheckCollision(pPlayer);
			CheckLevel(pPlayer);

			// 追跡状態でなければ抜ける
			if (!m_pPolice->GetChase()) { continue; }

			// 追跡開始処理
			BeginChase(pPlayer);
		}
		else
		{// 範囲外

			nAttackTime = rand() % 120 + 360;

			// 追跡状態でなければ抜ける
			if (!m_pPolice->GetChase()) { continue; }

			// 一度も接近していなければ抜ける
			if (!m_bCross) { continue; }

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

	// 警戒度を減少させる
	m_fLevelSearch += LEVEL_NORMAL;

	// 最小値を下回ったら補正する
	if (m_fLevelSearch < LEVEL_MIN)
	{
		m_fLevelSearch = LEVEL_MIN;
	}

	// 応援を呼んだ警察がいなければ抜ける
	if (m_pPoliceBackUp == nullptr) { return; }

	// 応援を呼んだ警察が追跡を終了していたら
	if (!m_pPoliceBackUp->GetChase())
	{
		// 追跡終了処理
		EndChase();

		// 追跡状態を送信
		if (m_pPolice->IsActive())
		{
			m_pPolice->SendChaseEnd();
		}

		// 応援を呼んだ警察をリセット
		m_pPoliceBackUp = nullptr;
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

	// 追跡経路をリセット
	m_searchRoad.clear();

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

	m_pPolice->SetLaneCount(0);
	m_pPolice->SetLaneTime(0);

	// 接近状態を解除
	m_bCross = false;

	// 警戒度をリセット
	m_fLevelSearch = LEVEL_MIN;

	// 応援要請をリセット
	m_nCntCall = 0;
	if (m_bCall)
	{
		m_bCall = false;
		CPoliceAIManager::GetInstance()->SetCall(false);
	}

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
		m_fLevelSearch += LEVEL_NITRO;
	}
}

//==========================================================
// 旋回確認処理
//==========================================================
void CPoliceAI::CheckTurn(CPlayer* pPlayer)
{
	// プレイヤーが存在しないなら抜ける
	if (pPlayer == nullptr) { return; }

	D3DXVECTOR3 rotTurn = pPlayer->GetRotation() - pPlayer->GetOldRotation();

	m_fLevelSearch += fabs(rotTurn.y);
}

//==========================================================
// 傷確認処理
//==========================================================
void CPoliceAI::CheckDamage(CPlayer* pPlayer)
{
	// プレイヤーが存在しないなら抜ける
	if (pPlayer == nullptr) { return; }

	// ライフが一定値以下の時
	if (pPlayer->GetLife() < LIFE_DAMAGE)
	{
		m_fLevelSearch += LEVEL_DAMAGE;
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
	if (pPlayer->GetLife() < LIFE_SMOKE)
	{
		m_fLevelSearch += LEVEL_SMOKE;
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
// 警戒度確認処理
//==========================================================
void CPoliceAI::CheckLevel(CPlayer* pPlayer)
{
	// プレイヤーが存在しないなら抜ける
	if (pPlayer == nullptr) { return; }

	if (m_fLevelSearch > LEVEL_MAX)
	{
		m_pPolice->SetChase(true);
		m_fLevelSearch = LEVEL_MAX;
	}
}

//==========================================================
// 応援要請処理
//==========================================================
void CPoliceAI::CallBackup(void)
{
	// 応援要請時間になっていなければ時間を加算し抜ける
	if (m_nCntCall < CALL_TIME[m_type]) 
	{ 
		m_nCntCall++;
		return; 
	}

	// 応援要請をしているなら抜ける
	if (m_bCall || CPoliceAIManager::GetInstance()->GetCall()) { return; }

	// 応援要請の判定をtrueにする
	m_bCall = true;
	CPoliceAIManager::GetInstance()->SetCall(true);

	// ギミックのリスト
	auto listGimmick = CGimmick::GetList();
	Clist<CGimmick*> listStation;
	int nNumStation = 0;

	// ギミックのリストから警察署を取り出す
	for (int i = 0; i < listGimmick->GetNum(); i++)
	{// 使用されていない状態まで
		// 道確認
		CGimmick* pGimmick = listGimmick->Get(i);	// 先頭を取得
		if (pGimmick == nullptr) { continue; }

		if (pGimmick->GetType() != CGimmick::TYPE_POLICESTATION) { continue; }

		listStation.Regist(pGimmick);
		nNumStation++;
	}

	// 応援の警察を指定回数生成
	for (int i = 0; i < NUM_BACKUP; i++)
	{
		// ランダムな警察署を選択
		CGimmick* pPoliceStation = listStation.Get(rand() % nNumStation);

		// 応援の警察を生成
		CAddPolice* pP = CAddPolice::Create(pPoliceStation->GetPos(), pPoliceStation->GetRot(), VECTOR3_ZERO, CCarManager::GetInstance()->GetMapList()->GetInCnt());

		if (pP == nullptr) { continue; }

		// 応援の警察のタイプを設定
		pP->SetTypeAI((CPoliceAI::TYPE)(rand() % CPoliceAI::TYPE_MAX));
		pP->SetType(CCar::TYPE::TYPE_ACTIVE);

		// 目的地設定
		pP->SetRoadTarget(CRoadManager::GetInstance()->GetNearRoad(pPoliceStation->GetPos()));

		// 追跡状態に変更
		pP->SetChase(true);
		pP->GetAi()->BeginChase(m_pPolice->GetPlayer());

		// 応援の警察は応援を呼ばないようにする
		pP->GetAi()->m_bCall = true;

		// 応援を呼んだ警察を保存する
		pP->GetAi()->m_pPoliceBackUp = m_pPolice;
	}
}

//==========================================================
// 攻撃処理
//==========================================================
void CPoliceAI::Attack(void)
{
	// 攻撃インターバルを減少させる
	nAttackTime--;

	if (m_state == STATE_ATTACK)
	{
		// 速度を設定
		m_fChaseSpeed += ATTACK_SPEED[m_type];
		m_pSearchTarget = nullptr;

		CDebugProc::GetInstance()->Print(" 攻撃中 %d インターバル : [%d]\n", (int)m_state, nAttackTime);
	}
	else if (m_state == STATE_PREP)
	{
		m_fChaseSpeed = 0.0f;
		CDebugProc::GetInstance()->Print(" 準備中 %d インターバル : [%d]\n", (int)m_state, nAttackTime);
	}
	else if (m_state == STATE_FINISH)
	{
		m_fChaseSpeed -= 5.0f;
		CDebugProc::GetInstance()->Print(" 減速中 %d インターバル : [%d]\n", (int)m_state, nAttackTime);
	}
	else
	{
		m_fChaseSpeed = CHASE_SPEED[m_type];

		CDebugProc::GetInstance()->Print(" 追跡中 %d インターバル : [%d]\n", (int)m_state, nAttackTime);
	}

	if (nAttackTime < 0)
	{
		D3DXVECTOR3 vecPlayer = m_pPolice->GetPlayer()->GetPosition() - m_pPolice->GetPosition();		// プレイヤーと警察間のベクトル計算

		if (m_state == STATE_ATTACK)
		{
			nAttackTime = 30;
			m_state = STATE_FINISH;
		}
		else if (m_state == STATE_PREP)
		{
			float rotVec = atan2f(vecPlayer.x, vecPlayer.z);								// 角度計算
			float rotView = m_pPolice->GetRotation().y - rotVec;							// 向いてる方向との差を計算
			
			if (fabs(rotView) < D3DX_PI * 0.3f)
			{// 近距離
				nAttackTime = 90;
				m_state = STATE_ATTACK;
			}
		}
		else if (m_state == STATE_FINISH)
		{
			nAttackTime = rand() % 120 + 360;
			m_state = STATE_NORMAL;
		}
		else
		{
			float length = D3DXVec3Length(&vecPlayer);										// 距離計算
			
			if (length < CHASE_NEAR)
			{// 近距離
				nAttackTime = 20;
				m_state = STATE_PREP;
			}
		}
	}
}

//==========================================================
// 攻撃中止処理
//==========================================================
void CPoliceAI::StopAttack(void)
{
	nAttackTime = rand() % 120 + 360;
	m_state = STATE_NORMAL;
	m_fSearchTimer += SEARCH_TIME[m_type];
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
	if (m_fSearchTimer > m_fSearchInterval || m_pSearchTarget == nullptr)
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
	if (m_pSearchTarget == nullptr) { return; }

	// 次の目的地を設定
	D3DXVECTOR3 posRoad = m_pSearchTarget->pConnectRoad->GetPosition();
	D3DXVECTOR3 posPolice = m_pPolice->GetPosition();
	float length = D3DXVec3Length(&(posRoad - posPolice));
	if (length < 1500.0f)
	{
		m_pSearchTarget = m_pSearchTarget->pChild;
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
	
	m_pRoadRelay = nullptr;
	m_bRelay = false;

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
		m_pRoadTarget = pPlayer->GetRoad();

		if (m_bRelay || m_bCross) { return; }

		CRoad* pRoadRelay = pPlayer->GetRoad();

		if (pRoadRelay == nullptr) { return; }

		int nConnectDic = 0;
		float rotDif = 100.0f;

		for (int i = 0; i < CRoad::DIC_MAX; i++)
		{
			CRoad* pRoadRelayConnect = pRoadRelay->GetConnectRoad((CRoad::DIRECTION)i);

			if (pRoadRelayConnect == nullptr) { continue; }

			D3DXVECTOR3 vecRoad = pRoadRelay->GetPosition() - pRoadRelayConnect->GetPosition();

			float targetrot = atan2f(vecRoad.x, vecRoad.z);

			// 角度補正
			if (targetrot > D3DX_PI)
			{
				targetrot -= D3DX_PI * 2.0f;
			}
			else if (targetrot < -D3DX_PI)
			{
				targetrot += D3DX_PI * 2.0f;
			}

			targetrot -= m_pPolice->GetPlayer()->GetRotation().y;

			// 角度補正
			if (targetrot > D3DX_PI)
			{
				targetrot -= D3DX_PI * 2.0f;
			}
			else if (targetrot < -D3DX_PI)
			{
				targetrot += D3DX_PI * 2.0f;
			}

			if (targetrot < rotDif)
			{
				rotDif = targetrot;
				nConnectDic = i;
				pRoadRelay = pRoadRelayConnect;
			}
		}

		while (pRoadRelay->GetType() == CRoad::TYPE_NONE)
		{
			pRoadRelay = pRoadRelay->GetConnectRoad((CRoad::DIRECTION)nConnectDic);
		}

		m_pRoadRelay = pRoadRelay;
	}
	else
	{
		m_pRoadTarget = m_pPolice->GetRoadTarget();
	}
}

//==========================================================
// 目標地点到達時処理
//==========================================================
void CPoliceAIElite::ReachRoad(void)
{
	// 目的地が存在する時
	if (m_pSearchTarget == nullptr) { return; }

	// 次の目的地を設定
	D3DXVECTOR3 posRoad = m_pSearchTarget->pConnectRoad->GetPosition();
	D3DXVECTOR3 posPolice = m_pPolice->GetPosition();
	float length = D3DXVec3Length(&(posRoad - posPolice));
	if (length < 1500.0f)
	{
		if (m_pSearchTarget->pConnectRoad == m_pRoadRelay && !m_bRelay && !m_bCross)
		{ 
			m_bRelay = true;
			m_searchRoad.clear();
			m_pSearchTarget = nullptr;
			return;
		}

		m_pSearchTarget = m_pSearchTarget->pChild;
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
		if (m_bRelay || m_bCross)
		{
			// 経路探索
			m_searchRoad = AStar::AStarPolice(m_pRoadStart, m_pRoadTarget);
		}
		else
		{
			// 経路探索
			m_searchRoad = AStar::AStarPoliceDetour(m_pRoadStart, m_pRoadRelay, m_pRoadTarget);
		}
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
		m_pRoadTarget = pPlayer->GetRoad();
	}
	else
	{
		m_pRoadTarget = m_pPolice->GetRoadTarget();
	}
}
