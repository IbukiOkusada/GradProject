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
	const float SPEED_STREET = (15.0f);			// 直進時の速度
	const float SPEED_CURVE = (10.0f);			// カーブ時の速度
	const float SPEED_STREET_BACK = (-10.0f);	// バックで直進時の速度
	const float SPEED_CURVE_BACK = (-10.0f);	// バックでカーブ時の速度
	const int TIME_BACK = (80);					// バックする時間
	const float SPEED_INER = (0.05f);			// 速度の慣性
	const float ROT_MULTI = (0.06f);			// 向き補正倍率
	const float ROT_MULTI_BACK = (0.015f);		// バック時の向き補正倍率
	const float ROT_CURVE = (0.15f);			// カーブ判定角度
	const float LENGTH_POINT = (200.0f);		// 到達判定距離
	const float FRAME_RATE_SCALER = 60.0f;		// フレームレートを考慮した速度の調整
	const float RECV_INER = (0.35f);			// 受信したデータの慣性
}

//==========================================================
// コンストラクタ
//==========================================================
CCar::CCar(int nId)
{
	// 値のクリア
	m_Info = SInfo();
	m_Info.nId = nId;
	m_RecvInfo = SRecvInfo();
	m_pObj = nullptr;
	m_pTailLamp = nullptr;

	// リストに入れる
	CCarManager::GetInstance()->ListIn(this);
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
	m_Info.fRotMulti = ROT_MULTI;
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
	m_Info.fSpeedDest = 0.0f;

	// 受信した種類を設定
	RecvTypeSet();

	// 移動先の決定
	MoveRoad();

	// 移動処理
	Move();

	// 当たり判定処理
	Collision();

	if (m_Info.type == TYPE::TYPE_RECV)
	{
		RecvInerSet();
	}
	else if(m_Info.type != TYPE::TYPE_NONE)
	{
		CNetWork* pNet = CNetWork::GetInstance();

		// 座標送信
		if (pNet->GetTime()->IsOK())
		{
			SendPosition();
		}
	}

	CDebugProc::GetInstance()->Print("車の座標 : [ %f, %f, %f ]\n", m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);

	// 座標系設定
	Set();
}

//==========================================================
// 生成
//==========================================================
CCar *CCar::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move, int nId)
{
	CCar *pCar = nullptr;

	pCar = DEBUG_NEW CCar(nId);

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
// テールランプ生成
//==========================================================
void CCar::TailLamp()
{
	m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\taillamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
}

//==========================================================
// 移動処理
//==========================================================
void CCar::Move()
{
	if (!IsActive()) { return; }
	if (!m_Info.bBreak)
	{
		// 角度調整
		Rot();

		// バック中の処理
		if (m_Info.bBack)
		{
			m_Info.nBackTime--;

			if (m_Info.nBackTime < 0)
			{
				m_Info.bBack = false;
			}
		}

		// 速度を追加
		m_Info.fSpeed += (m_Info.fSpeedDest - m_Info.fSpeed) * SPEED_INER;
		CManager::GetInstance()->GetDebugProc()->Print("一般車の速度 [ %f ]\n", m_Info.fSpeed);

		// 移動量設定
		m_Info.move.x = m_Info.fSpeed * sinf(m_Info.rot.y);
		m_Info.move.y = 0.0f;
		m_Info.move.z = m_Info.fSpeed * cosf(m_Info.rot.y);

		// デルタタイム取得
		float DeltaTime = CDeltaTime::GetInstance()->GetDeltaTime();

		// 位置に加算
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

	// 計算用変数
	fRotMove = m_Info.rot.y;
	fRotDest = atan2f(vecTarget.x, vecTarget.z);
	fRotDiff = fRotDest - fRotMove;

	// 角度補正
	if (fRotDiff > D3DX_PI)
	{
		fRotDiff -= D3DX_PI * 2.0f;
	}
	else if (fRotDiff < -D3DX_PI)
	{
		fRotDiff += D3DX_PI * 2.0f;
	}

	//角度一致判定
	if (m_Info.bBack)
	{
		// 補正の値を変更
		m_Info.fRotMulti = ROT_MULTI_BACK;

		//差分追加
		if (m_Info.fSpeed < -5.0f)
		{
			fRotMove += fRotDiff * m_Info.fRotMulti;
		}

		// 状況によって速度を変更する
		if (fabsf(fRotDiff) >= ROT_CURVE)
		{
			m_Info.fSpeedDest += SPEED_CURVE_BACK;
		}
		else
		{
			m_Info.fSpeedDest += SPEED_STREET_BACK;
		}
	}
	else
	{
		//差分追加
		fRotMove += fRotDiff * m_Info.fRotMulti;

		// 状況によって速度を変更する
		if (fabsf(fRotDiff) >= ROT_CURVE)
		{
			m_Info.fSpeedDest += SPEED_CURVE;
		}
		else
		{
			m_Info.fSpeedDest += SPEED_STREET;
		}
	}

	// 角度補正
	if (fRotMove > D3DX_PI)
	{
		fRotMove -= D3DX_PI * 2.0f;
	}
	else if (fRotMove <= -D3DX_PI)
	{
		fRotMove += D3DX_PI * 2.0f;
	}

	// 角度を加算
	m_Info.rot.y = fRotMove;

	// 角度補正
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
	if (!IsActive()) { return; }

	// 目的地が存在しなければ最寄りの道を設定
	if (m_Info.pRoadTarget == nullptr)
		SearchRoad();

	// 最寄りの道が見つからなかった場合抜ける
	if (m_Info.pRoadTarget == nullptr) { return; }

	// 目的地への到着判定処理
	float length = D3DXVec3Length(&(m_Info.pRoadTarget->GetPosition() - m_Info.pos));
	if (length < LENGTH_POINT)
		ReachRoad();

	// 目標地点の座標を取得
	m_Info.posTarget = m_Info.pRoadTarget->GetPosition();
}

//==========================================================
// 道探索処理
//==========================================================
void CCar::SearchRoad()
{
	if (!IsActive()) { return; }

	// 道のリスト取得
	CRoadManager* pRoadManager = CRoadManager::GetInstance();
	auto list = pRoadManager->GetList();

	CRoad* pRoad = pRoadManager->GetList()->Get(0);
	CRoad* pRoadClose = pRoadManager->GetList()->Get(0);

	// 道が存在しなければ抜ける
	if (pRoad == nullptr) { return; }

	// 距離計算用変数
	float length = D3DXVec3Length(&(pRoadClose->GetPosition() - m_Info.pos));
	float lengthClose = 0.0f;

	// 最寄りの道を見つける
	for (int i = 0; i < pRoadManager->GetList()->GetNum() - 1; i++)
	{
		pRoad = list->Get(i);

		// 距離判定処理
		lengthClose = D3DXVec3Length(&(pRoad->GetPosition() - m_Info.pos));

		if (length > lengthClose)
		{
			length = lengthClose;
			pRoadClose = pRoad;
		}
	}
	
	// 目的地を最寄りの道に設定する
	m_Info.pRoadTarget = pRoadClose;
}

//==========================================================
// 道到達時処理
//==========================================================
void CCar::ReachRoad()
{
	if (!IsActive()) { return; }
	CRoad* pRoadNext = nullptr;

	while (1)
	{// 地点が入るまで
		int roadPoint = rand() % CRoad::DIC_MAX;

		pRoadNext = m_Info.pRoadTarget->GetConnectRoad((CRoad::DIRECTION)roadPoint);

		// 直線の道はそのまま進
		if (m_Info.pRoadTarget->GetType() == CRoad::TYPE_STOP)
		{
			
		}
		else
		{
			// Uターンする場合道を探しなおす
			if (pRoadNext == m_Info.pRoadStart) { continue; }
		}

		if (pRoadNext != nullptr) { break; }
	}

	// 目標地点と出発地点をずらす
	m_Info.pRoadStart = m_Info.pRoadTarget;
	m_Info.pRoadTarget = pRoadNext;
}

//==========================================================
// 当たり判定処理
//==========================================================
bool CCar::Collision()
{
	if (!IsActive()) { return false; }

	auto mgr = CObjectX::GetList();
	for(int i = 0; i < mgr->GetNum(); i++)
	{// 使用されていない状態まで

		CObjectX* pObjectX = mgr->Get(i);	// 先頭を取得

		if (!pObjectX->GetEnableCollision()) { continue; }

		// オブジェクトの情報取得
		D3DXVECTOR3 posObjectX = pObjectX->GetPosition();
		D3DXVECTOR3 rotObjectX = pObjectX->GetRotation();
		D3DXVECTOR3 sizeMax = pObjectX->GetVtxMax();
		D3DXVECTOR3 sizeMin = pObjectX->GetVtxMin();

		// OBBとの当たり判定を実行
		bool bCollision = collision::CollidePointToOBB(&m_Info.pos, m_Info.posOld, posObjectX, rotObjectX, (sizeMax - sizeMin) * 0.5f);
		
		// 衝突していない場合繰り返す
		if (!bCollision) { continue; }

		if (pObjectX->GetType() == TYPE_PLAYER)
		{
			Break();
		}

		return true;
	}

	return false;
}

//==========================================================
// 接触時処理
//==========================================================
void CCar::Hit()
{
	if (!m_Info.bBack) { return; }

	CRoad* pRoadNext = m_Info.pRoadTarget;
	m_Info.pRoadTarget = m_Info.pRoadStart;
	m_Info.pRoadStart = pRoadNext;
	m_Info.bBack = true;
	m_Info.nBackTime = TIME_BACK;
}

//==========================================================
// 破壊
//==========================================================
void CCar::Break()
{
	m_Info.bBreak = true;
}

//==========================================================
// 設定
//==========================================================
void CCar::Set()
{
	if (m_pObj != nullptr)
	{
		m_Info.rot.y += D3DX_PI;
		m_pObj->SetPosition(m_Info.pos);
		m_pObj->SetRotation(m_Info.rot);
		m_Info.rot.y -= D3DX_PI;
	}

	if (m_pTailLamp != nullptr)
	{
		m_pTailLamp->m_pos = m_Info.pos;
		m_pTailLamp->m_rot = m_Info.rot;
	}
}

//==========================================================
// 座標設定
//==========================================================
void CCar::SetPosition(const D3DXVECTOR3& pos)
{ 
	m_Info.pos = pos; 

	if (m_pObj != nullptr)
	{
		m_pObj->SetPosition(m_Info.pos);
	}
}

//==========================================================
// 向き設定
//==========================================================
void CCar::SetRotation(const D3DXVECTOR3& rot)
{ 
	m_Info.rot = rot;

	if (m_pObj != nullptr)
	{
		m_pObj->SetRotation(m_Info.rot);
	}
}

//===============================================
// 受信したデータに慣性をつけて補正
//===============================================
void CCar::RecvInerSet()
{
	// 位置
	{
		D3DXVECTOR3 diff = m_RecvInfo.pos - m_Info.pos;
		D3DXVECTOR3 pos = m_Info.pos + diff * RECV_INER;
		m_Info.pos = pos;
	}

	// 向き
	{
		D3DXVECTOR3 diff = m_RecvInfo.rot - m_Info.rot;
		Adjust(diff);

		D3DXVECTOR3 rot = m_Info.rot + diff;
		Adjust(rot);
		m_Info.rot = rot;
		Adjust(m_Info.rot);
	}
}

//===============================================
// 車の座標の更新
//===============================================
void CCar::SendPosition()
{
	CNetWork* pNet = CNetWork::GetInstance();

	pNet->SendCarPos(GetId(), GetPosition(), GetRotation());
}