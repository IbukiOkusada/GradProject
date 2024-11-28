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
#include "a_star.h"
#include "police_AI.h"

// マクロ定義

// 無名名前空間を定義
namespace
{
	const float LENGTH_POINT = (200.0f);		// 到達判定距離
	const float CHASE_SPEED = (17.0f);			// 追跡時の加速
}

//==========================================================================
// 関数ポインタ
//==========================================================================
// 状態管理
CPolice::STATE_FUNC CPolice::m_StateFunc[] =
{
	&CPolice::StateNormal,		// 通常
	&CPolice::StateChase,		// 追跡
	&CPolice::StateSearch,		// 警戒
	&CPolice::StateFadeOut,		// フェードアウト
	&CPolice::StateStop,		// 停止
};

// 状態設定
CPolice::SETSTATE_FUNC CPolice::m_SetStateFunc[] =
{
	&CPolice::SetStateNormal,		// 通常
	&CPolice::SetStateChase,		// 追跡
	&CPolice::SetStateSearch,		// 警戒
	&CPolice::SetStateFadeOut,		// フェードアウト
	&CPolice::SetStateStop,			// 停止
};

//==========================================================
// コンストラクタ
//==========================================================
CPolice::CPolice()
{
	// 値のクリア
	m_Info = SInfo();
	m_pPatrolLamp = nullptr;
	m_pSiren = nullptr;
	m_stateInfo = SState();

	CPoliceManager::GetInstance()->GetList()->Regist(this);
}

//==========================================================
// デストラクタ
//==========================================================
CPolice::~CPolice()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CPolice::Init(void)
{
	TailLamp();
	m_pSiren = CMasterSound::CObjectSound::Create("data\\SE\\siren.wav", -1);
	m_pSiren->Stop();
	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, "data\\MODEL\\car003.x");
	m_pPoliceAI = CPoliceAI::Create(this);
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CPolice::Uninit(void)
{
	CCar::Uninit();
	CPoliceManager::GetInstance()->GetList()->Delete(this);
	SAFE_DELETE(m_pPatrolLamp);
	SAFE_UNINIT_DELETE(m_pSiren);
	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CPolice::Update(void)
{
	// 停止状態なら動かない
	if (m_stateInfo.state == STATE::STATE_STOP) 
	{ 
		MoveRoad();
		return;
	}

	CCar::Update();
	if (m_Info.bChase)
	{
		if (m_pPatrolLamp == nullptr)
		{
			m_pPatrolLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\patrollamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
		}
		m_pPatrolLamp->m_pos = GetPosition();
		m_pPatrolLamp->m_rot = GetRotation();
	}
	else
	{
		SAFE_DELETE(m_pPatrolLamp);
	}

	UpdateState();
}

//==========================================================
// 生成
//==========================================================
CPolice *CPolice::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move)
{
	CPolice *pCar = nullptr;

	pCar = DEBUG_NEW CPolice;

	if (pCar != nullptr)
	{
		// 初期化処理
		pCar->Init();

		// 座標設定
		pCar->SetPosition(pos);

		// 向き設定
		pCar->SetRotation(rot);

		// 移動量設定
		pCar->SetMove(move);
	}

	return pCar;
}

//==========================================================
// 道移動処理
//==========================================================
void CPolice::MoveRoad()
{
	CRoad* pRoadStart = GetRoadStart();
	CRoad* pRoadTarget = GetRoadTarget();

	if (pRoadTarget == nullptr)
		SearchRoad();

	SearchPlayer();

	if (m_Info.bChase)
	{
		m_pSiren->Start();

		ChasePlayer();

		float dis = GetDistance(m_Info.pPlayer->GetPosition() , GetPosition());
		m_pSiren->SetVolume((2000.0f - dis) * 0.00075f);

		if (pRoadTarget != nullptr)
		{
			SetSpeedDest(GetSpeedDest() + CHASE_SPEED);
			SetPosTarget(pRoadTarget->GetPosition());
		}
		else
		{
			SetSpeedDest(0.0f);
			if (m_Info.pPlayer != nullptr)
			{
				SetPosTarget(m_Info.pPlayer->GetPosition());
			}
		}

		CDebugProc::GetInstance()->Print("追跡中\n");
	}
	else
	{
		m_pSiren->Stop();
		if (pRoadTarget != nullptr)
		{
			pRoadStart = GetRoadStart();
			pRoadTarget = GetRoadTarget();

			float length = D3DXVec3Length(&(pRoadTarget->GetPosition() - GetPosition()));
			if (length < LENGTH_POINT)
				ReachRoad();

			SetPosTarget(pRoadTarget->GetPosition());
		}
	}
}

//==========================================================
// 道到達時処理
//==========================================================
void CPolice::ReachRoad()
{
	CRoad* pRoadStart = GetRoadStart();
	CRoad* pRoadTarget = GetRoadTarget();

	CRoad* pRoadNext = nullptr;

	while (1)
	{// 地点が入るまで
		int roadPoint = rand() % CRoad::DIC_MAX;

		pRoadNext = pRoadTarget->GetConnectRoad((CRoad::DIRECTION)roadPoint);

		if (pRoadTarget->GetType() == CRoad::TYPE_STOP)
		{

		}
		else
		{
			if (pRoadNext == pRoadStart) { continue; }
		}

		if (pRoadNext != nullptr)
			break;
	}

	pRoadStart = pRoadTarget;
	pRoadTarget = pRoadNext;

	SetRoadStart(pRoadStart);
	SetRoadTarget(pRoadTarget);
}

//==========================================================
// プレイヤー発見処理
//==========================================================
void CPolice::SearchPlayer()
{
	m_pPoliceAI->Search();
}

//==========================================================
// 追跡経路探索処理
//==========================================================
void CPolice::ChasePlayer()
{
	m_pPoliceAI->Chase();

	if (m_pPoliceAI->GetSearchRoad() != nullptr)
	{
		if (m_pPoliceAI->GetSearchRoad()->pRoad != nullptr)
		{
			SetRoadTarget(m_pPoliceAI->GetSearchRoad()->pRoad);
		}
		else
		{
			SetRoadTarget(nullptr);
		}
	}
}

//==========================================================
// 当たり判定処理
//==========================================================
void CPolice::Collision()
{
	
}

//==========================================================
// 破壊
//==========================================================
void CPolice::Break()
{
	CPlayer* p = CPlayerManager::GetInstance()->GetPlayer();
	p->Damage(p->GetLifeOrigin() * 0.1f);
}

//==========================================================
// 状態設定
//==========================================================
void CPolice::UpdateState(void)
{
	// タイマー減少
	m_stateInfo.fTimer -= CDeltaTime::GetInstance()->GetDeltaTime();
	if(m_stateInfo.fTimer < 0.0f)
	{ 
		m_stateInfo.fTimer = 0.0f;
	}

	// 状態ごとの関数を呼ぶ
	(this->*(m_StateFunc[m_stateInfo.state]))();
}

//==========================================================
// 状態設定
//==========================================================
void CPolice::SetState(const STATE state)
{
	// 状態ごとの関数を呼ぶ
	if (m_stateInfo.state == STATE::STATE_FADEOUT) { return; }

	(this->*(m_SetStateFunc[state]))();
}

//==========================================================
// 通常状態
//==========================================================
void CPolice::StateNormal(void)
{
	
}

//==========================================================
// 追跡状態
//==========================================================
void CPolice::StateChase(void)
{
	if (m_pObj == nullptr) { return; }
}

//==========================================================
// 警戒状態
//==========================================================
void CPolice::StateSearch(void)
{
	// カウントなくなるまで
	if (m_stateInfo.fTimer > 0.0f) { return; }

	SetState(STATE::STATE_NORMAL);
}

//==========================================================
// フェードアウト状態
//==========================================================
void CPolice::StateFadeOut(void)
{

}

//==========================================================
// 停止状態
//==========================================================
void CPolice::StateStop(void)
{

}

//==========================================================
// 通常状態設定
//==========================================================
void CPolice::SetStateNormal(void)
{
	// 初期化
	m_stateInfo = SState();

	// 状態変更
	m_stateInfo.state = STATE::STATE_NORMAL;
}

//==========================================================
// 追跡状態設定
//==========================================================
void CPolice::SetStateChase(void)
{
	// 初期化
	m_stateInfo = SState();

	// 状態変更
	m_stateInfo.state = STATE::STATE_CHASE;
}

//==========================================================
// 警戒状態設定
//==========================================================
void CPolice::SetStateSearch(void)
{
	// 初期化
	m_stateInfo = SState();

	// 状態変更
	m_stateInfo.state = STATE::STATE_SEARCH;
	SetStateTimer(5.0f);
}

//==========================================================
// フェードアウト状態設定
//==========================================================
void CPolice::SetStateFadeOut(void)
{
	// 初期化
	m_stateInfo = SState();

	// 状態変更
	m_stateInfo.state = STATE::STATE_FADEOUT;
}

//==========================================================
// 停止状態設定
//==========================================================
void CPolice::SetStateStop(void)
{
	// 初期化
	m_stateInfo = SState();

	// 状態変更
	m_stateInfo.state = STATE::STATE_STOP;
}