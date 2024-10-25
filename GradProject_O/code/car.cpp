//==========================================================
//
// 車処理 [car.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "car.h"
#include "car_manager.h"
#include "road.h"
#include "road_manager.h"
#include "manager.h"
#include "debugproc.h"
#include "collision.h"
#include "deltatime.h"
#include "slow.h"
#include "player.h"
#include "player_manager.h"

// マクロ定義

// 無名名前空間を定義
namespace
{
	const float SPEED_STREET = (15.0f);		// 直進時の速度
	const float SPEED_CURVE = (10.0f);		// カーブ時の速度
	const float SPEED_INER = (0.05f);		// 速度の慣性
	const float ROT_MULTI = (0.06f);		// 向き補正倍率
	const float ROT_CURVE = (0.15f);		// カーブ判定角度
	const float LENGTH_POINT = (200.0f);	// 到達判定距離
	const float FRAME_RATE_SCALER = 60.0f;  // フレームレートを考慮した速度の調整
}

//==========================================================
// コンストラクタ
//==========================================================
CCar::CCar()
{
	// 値のクリア
	m_Info.pos = VECTOR3_ZERO;
	m_Info.posOld = VECTOR3_ZERO;
	m_Info.rot = VECTOR3_ZERO;
	m_Info.rotDest = VECTOR3_ZERO;
	m_Info.move = VECTOR3_ZERO;
	m_Info.pRoadStart = nullptr;
	m_Info.pRoadTarget = nullptr;
	m_Info.speed = 0.0f;
	m_Info.speedDest = 0.0f;
	m_Info.bBreak = false;
	m_Info.bBack = false;
}

//==========================================================
// デストラクタ
//==========================================================
CCar::~CCar()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CCar::Init(void)
{
	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, "data\\MODEL\\car002.x");
	TailLamp();
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CCar::Uninit(void)
{
	// 描画オブジェクトを廃棄
	SAFE_UNINIT(m_pObj);
	SAFE_DELETE(m_pTailLamp);
	CCarManager::GetInstance()->ListOut(this);

	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CCar::Update(void)
{
	m_Info.posOld = m_Info.pos;
	m_Info.speedDest = 0.0f;

	// 移動先の決定
	MoveRoad();

	// 移動処理
	Move();

	// 当たり判定処理
	Collision();

	if (m_pObj != nullptr)
	{
		m_Info.rot.y += D3DX_PI;
		m_pObj->SetPosition(m_Info.pos);
		m_pObj->SetRotation(m_Info.rot);
		m_Info.rot.y -= D3DX_PI;
	}
	m_pTailLamp->m_pos = m_Info.pos;
	m_pTailLamp->m_rot = m_Info.rot;
}

//==========================================================
// 生成
//==========================================================
CCar *CCar::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move)
{
	CCar *pCar = nullptr;

	pCar = DEBUG_NEW CCar;

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
void CCar::TailLamp()
{
	m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\taillamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);

}
//==========================================================
// 移動処理
//==========================================================
void CCar::Move()
{
	if (!m_Info.bBreak)
	{
		// 角度調整
		Rot();

		m_Info.speed += (m_Info.speedDest - m_Info.speed) * SPEED_INER;
		CManager::GetInstance()->GetDebugProc()->Print("車の速度 [ %f ]\n", m_Info.speed);

		m_Info.move.x = m_Info.speed * sinf(m_Info.rot.y);
		m_Info.move.y = 0.0f;
		m_Info.move.z = m_Info.speed * cosf(m_Info.rot.y);

		// デルタタイム取得
		float DeltaTime = CDeltaTime::GetInstance()->GetDeltaTime();

		m_Info.pos += m_Info.move * FRAME_RATE_SCALER * DeltaTime;
	}
}

//==========================================================
// 角度決定処理
//==========================================================
void CCar::Rot()
{
	float fRotMove, fRotDest, fRotDiff;				//角度調整用変数

	D3DXVECTOR3 vecTarget = m_Info.posTarget - m_Info.pos;

	fRotMove = m_Info.rot.y;
	fRotDest = atan2f(vecTarget.x, vecTarget.z);
	fRotDiff = fRotDest - fRotMove;

	if (fRotDiff > D3DX_PI)
	{
		fRotDiff -= D3DX_PI * 2.0f;
	}
	else if (fRotDiff < -D3DX_PI)
	{
		fRotDiff += D3DX_PI * 2.0f;
	}

	//差分追加
	fRotMove += fRotDiff * ROT_MULTI;

	//角度一致判定
	if (fabsf(fRotDiff) >= ROT_CURVE)
	{
		m_Info.speedDest += SPEED_CURVE;
	}
	else
	{
		m_Info.speedDest += SPEED_STREET;
	}

	if (fRotMove > D3DX_PI)
	{
		fRotMove -= D3DX_PI * 2.0f;
	}
	else if (fRotMove <= -D3DX_PI)
	{
		fRotMove += D3DX_PI * 2.0f;
	}

	m_Info.rot.y = fRotMove;

	if (m_Info.rot.y > D3DX_PI)
	{
		m_Info.rot.y -= D3DX_PI * 2.0f;
	}
	else if (m_Info.rot.y <= -D3DX_PI)
	{
		m_Info.rot.y += D3DX_PI * 2.0f;
	}
}

//==========================================================
// 道移動処理
//==========================================================
void CCar::MoveRoad()
{
	if (m_Info.pRoadTarget == nullptr)
		SearchRoad();

	float length = D3DXVec3Length(&(m_Info.pRoadTarget->GetPosition() - m_Info.pos));
	if (length < LENGTH_POINT)
		ReachRoad();

	m_Info.posTarget = m_Info.pRoadTarget->GetPosition();
}

//==========================================================
// 道探索処理
//==========================================================
void CCar::SearchRoad()
{
	CRoadManager* pRoadManager = CRoadManager::GetInstance();

	CRoad* pRoad = pRoadManager->GetTop();
	CRoad* pRoadClose = pRoadManager->GetTop();
	float length = D3DXVec3Length(&(pRoadClose->GetPosition() - m_Info.pos));
	float lengthClose = 0.0f;

	while (pRoad != nullptr)
	{// 使用されていない状態まで

		CRoad* pRoadNext = pRoad->GetNext();	// 次のオブジェクトへのポインタを取得

		// 距離判定処理
		lengthClose = D3DXVec3Length(&(pRoad->GetPosition() - m_Info.pos));

		if (length > lengthClose)
		{
			length = lengthClose;
			pRoadClose = pRoad;
		}

		pRoad = pRoadNext;	// 次のオブジェクトに移動
	}

	m_Info.pRoadTarget = pRoadClose;
}

//==========================================================
// 道到達時処理
//==========================================================
void CCar::ReachRoad()
{
	CRoadManager* pRoadManager = CRoadManager::GetInstance();
	CRoad* pRoadNext = nullptr;

	while (1)
	{// 地点が入るまで
		int roadPoint = rand() % CRoad::DIC_MAX;

		pRoadNext = m_Info.pRoadTarget->GetConnectRoad((CRoad::DIRECTION)roadPoint);

		if (m_Info.pRoadTarget->GetType() == CRoad::TYPE_STOP)
		{
			
		}
		else
		{
			if (pRoadNext == m_Info.pRoadStart)
			{
				continue;
			}
		}

		if (pRoadNext != nullptr)
			break;
	}

	m_Info.pRoadStart = m_Info.pRoadTarget;
	m_Info.pRoadTarget = pRoadNext;
}

//==========================================================
// 当たり判定処理
//==========================================================
bool CCar::Collision()
{
	CObjectX* pObjectX = CObjectX::GetTop();	// 先頭を取得

	while (pObjectX != nullptr)
	{// 使用されていない状態まで

		CObjectX* pObjectXNext = pObjectX->GetNext();	// 次のオブジェクトへのポインタを取得

		D3DXVECTOR3 posObjectX = pObjectX->GetPosition();
		D3DXVECTOR3 rotObjectX = pObjectX->GetRotation();
		D3DXVECTOR3 sizeMax = pObjectX->GetVtxMax();
		D3DXVECTOR3 sizeMin = pObjectX->GetVtxMin();

		bool bCollision = collision::CollidePointToOBB(&m_Info.pos, m_Info.posOld, posObjectX, rotObjectX, (sizeMax - sizeMin) * 0.5f);

		if (bCollision)
		{
			CRoad* pRoadNext = m_Info.pRoadTarget;
			m_Info.pRoadTarget = m_Info.pRoadStart;
			m_Info.pRoadStart = pRoadNext;

			CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
			if (pPlayer->GetModelIndex() == pObjectX->GetIdx())
			{
				m_Info.bBreak = true;
			}

			return true;
		}

		pObjectX = pObjectXNext;	// 次のオブジェクトに移動
	}

	return false;
}