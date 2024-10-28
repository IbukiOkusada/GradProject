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

// マクロ定義

// 無名名前空間を定義
namespace
{
	const float LENGTH_POINT = (200.0f);		// 到達判定距離
	const float CHASE_SPEED = (17.0f);			// 追跡時の加速
	const float SECURE_SPEEDDEST = (-35.0f);	// 確保時の目標速度
	const float SECURE_SPEED = (0.8f);			// 確保時の加速倍率
	const int CHASE_TIME = (300);				// 追跡時間
	const float CHASE_SECURE = (400.0f);		// 追跡確保距離
	const float CHASE_BEGIN = (700.0f);			// 追跡開始距離
	const float CHASE_CONTINUE = (2000.0f);		// 追跡継続距離
	const float CHASE_END = (3000.0f);			// 追跡終了距離
}

//==========================================================
// コンストラクタ
//==========================================================
CPolice::CPolice()
{
	// 値のクリア
	m_Info.pPlayer = nullptr;
	m_Info.nChaseCount = 0;
	m_Info.bChase = false;
	m_pPatrolLamp = nullptr;
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
	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, "data\\MODEL\\car003.x");
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CPolice::Uninit(void)
{
	CCar::Uninit();
	SAFE_DELETE(m_pPatrolLamp);
	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CPolice::Update(void)
{
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
}

//==========================================================
// 生成
//==========================================================
CPolice *CPolice::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move)
{
	CPolice *pCar = nullptr;

	pCar = new CPolice;

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

	if (pRoadTarget == nullptr && !m_Info.bChase)
		SearchRoad();

	SearchPlayer();

	if (m_Info.bChase)
	{
		if (m_Info.pPlayer != nullptr)
			SetPosTarget(m_Info.pPlayer->GetPosition());

		SetSpeedDest(GetSpeedDest() + CHASE_SPEED);
	}
	else
	{
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

	CRoadManager* pRoadManager = CRoadManager::GetInstance();
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
			if (pRoadNext == pRoadStart)
			{
				continue;
			}
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
	m_Info.pPlayer = CPlayerManager::GetInstance()->GetTop();
	
	if (m_Info.pPlayer != nullptr)
	{
		float length = 0.0f;
		length = D3DXVec3Length(&(GetPosition() - m_Info.pPlayer->GetPosition()));
		CManager::GetInstance()->GetDebugProc()->Print("車と車の距離 [ %f ]\n", length);

		if (length < CHASE_SECURE)
		{// 追跡開始

			m_Info.bChase = true;
			m_Info.nChaseCount = CHASE_TIME;

			SetRoadStart(nullptr);
			SetRoadTarget(nullptr);

			SetSpeedDest(SECURE_SPEEDDEST);
			SetSpeed(GetSpeed() * SECURE_SPEED);
		}
		else if (length < CHASE_BEGIN)
		{// 追跡開始

			m_Info.bChase = true;
			m_Info.nChaseCount = CHASE_TIME;

			SetRoadStart(nullptr);
			SetRoadTarget(nullptr);
		}
		else if (length < CHASE_CONTINUE)
		{// 追跡継続

			if (m_Info.bChase)
			{
				m_Info.nChaseCount = CHASE_TIME;

				SetRoadStart(nullptr);
				SetRoadTarget(nullptr);
			}
		}
		else if (length < CHASE_END)
		{// 追跡終了

			if (m_Info.bChase)
			{
				m_Info.nChaseCount--;

				SetRoadStart(nullptr);
				SetRoadTarget(nullptr);

				if (m_Info.nChaseCount < 0)
				{
					m_Info.bChase = false;
					m_Info.nChaseCount = 0;
				}
			}
		}
		else
		{// 追跡強制終了

			m_Info.bChase = false;
			m_Info.nChaseCount = 0;
		}
	}
}

//==========================================================
// 追跡経路探索処理
//==========================================================
void CPolice::ChasePlayer()
{
	CRoadManager* pRoadManager = CRoadManager::GetInstance();
	CRoad* pRoad = pRoadManager->GetTop();

	while (pRoad != nullptr)
	{// 使用されていない状態まで

		CRoad* pRoadNext = pRoad->GetNext();	// 次のオブジェクトへのポインタを取得

		pRoad = pRoadNext;	// 次のオブジェクトに移動
	}
}

//==========================================================
// 当たり判定処理
//==========================================================
void CPolice::Collision()
{
	
}