//==========================================================
//
// 車処理 [car.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "car.h"
#include "road.h"
#include "road_manager.h"

// マクロ定義

//==========================================================
// コンストラクタ
//==========================================================
CCar::CCar()
{
	// 値のクリア
	m_Info.pos = VECTOR3_ZERO;
	m_Info.posOld = VECTOR3_ZERO;
	m_Info.rot = VECTOR3_ZERO;
	m_Info.move = VECTOR3_ZERO;
	m_Info.pRoadStart = nullptr;
	m_Info.pRoadTarget = nullptr;
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
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CCar::Uninit(void)
{
	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CCar::Update(void)
{
	MoveRoad();

	D3DXVECTOR3 vecTarget = m_Info.pos - m_Info.pRoadTarget->GetPosition();
	m_Info.rot.y = atan2f(vecTarget.x, vecTarget.z);

	m_Info.move.x += 0.05f * sinf(m_Info.rot.y);
	m_Info.move.z += 0.05f * cosf(m_Info.rot.y);
	//m_Info.pos += m_Info.move;

	m_Info.pos += (m_Info.pRoadTarget->GetPosition() - m_Info.pos) * 0.1f;

	if (m_pObj != nullptr)
	{
		m_pObj->SetPosition(m_Info.pos);
		m_pObj->SetRotation(m_Info.rot);
	}
}

//==========================================================
// 生成
//==========================================================
CCar *CCar::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move)
{
	CCar *pCar = nullptr;

	pCar = new CCar;

	if (pCar != nullptr)
	{
		// 初期化処理
		pCar->Init();

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
void CCar::MoveRoad()
{
	if (m_Info.pRoadTarget == nullptr)
		SearchRoad();

	float length = D3DXVec3Length(&(m_Info.pRoadTarget->GetPosition() - m_Info.pos));
	if (length < 250.0f)
		ReachRoad();
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

		if (pRoadNext- pRoadNext->TYPE_CROSSING || pRoadNext->TYPE_T_JUNCTION)
		{
			if (pRoadNext == m_Info.pRoadStart)
			{

			}
		}
		else
		{
			
		}

		if (pRoadNext == m_Info.pRoadStart)
		{
			continue;
		}

		if (pRoadNext != nullptr)
			break;
	}

	m_Info.pRoadStart = m_Info.pRoadTarget;
	m_Info.pRoadTarget = pRoadNext;
}