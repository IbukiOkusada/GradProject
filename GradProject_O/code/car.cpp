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
#include "bridge.h"
#include "meshfield.h"
#include "magic_enum/magic_enum.hpp"

// マクロ定義

// 無名名前空間を定義
namespace
{
	const float SPEED_STREET = (15.0f);			// 直進時の速度
	const float SPEED_CURVE_RIGHT = (9.0f);		// 左折時の速度
	const float SPEED_CURVE_LEFT = (3.0f);		// 右折時の速度
	const float SPEED_CURVE_UTURN = (7.0f);		// Uターン時の速度
	const float SPEED_STREET_BACK = (-10.0f);	// バックで直進時の速度
	const float SPEED_CURVE_BACK = (-10.0f);	// バックでカーブ時の速度
	const int TIME_BACK = (80);					// バックする時間
	const float SPEED_INER = (0.05f);			// 速度の慣性
	const float ROT_MULTI = (0.015f);			// 向き補正倍率
	const float ROT_MULTI_BACK = (0.015f);		// バック時の向き補正倍率
	const float ROT_CURVE = (0.3f);				// カーブ判定角度
	const float LENGTH_POINT = (900.0f);		// 到達判定距離
	const float LENGTH_LANE = (-400.0f);		// 車線の幅
	const float FRAME_RATE_SCALER = 60.0f;		// フレームレートを考慮した速度の調整
	const float RECV_INER = (0.35f);			// 受信したデータの慣性
	const float GRAVITY = (-24.0f);		//プレイヤー重力

	const int SET_COL_MAX = (100);				// 設定する色の最大値（100 = 1.0）
	const float SET_FLOAT_COL = (100.0f);		// 色のintの値をfloatに直す用
}

//==========================================================
// コンストラクタ
//==========================================================
CCar::CCar(int nId, CAR_TYPE type) : 
m_Info(SInfo()),
m_RecvInfo(SRecvInfo()),
m_pObj(nullptr),
m_pTailLamp(nullptr),
m_type(type)
{
	// 値のクリア
	m_Info.nId = nId;

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
	// 車体の色をランダムに
	float fColR = (float)(rand() % SET_COL_MAX) / SET_FLOAT_COL;
	float fColG = (float)(rand() % SET_COL_MAX) / SET_FLOAT_COL;
	float fColB = (float)(rand() % SET_COL_MAX) / SET_FLOAT_COL;

	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, "data\\MODEL\\car002.x");
	m_pObj->SetColMulti(D3DXCOLOR(fColR, fColG, fColB, 1.0f));

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
	if (m_pObj == nullptr) { return; }
	if (m_Info.nId < 0) { 
		Uninit();
		return; 
	}

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

		Uninit();
		return;
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

	// 座標系設定
	Set();
}

//==========================================================
// 生成
//==========================================================
CCar *CCar::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move, int nId)
{
	CCar *pCar = nullptr;

	pCar = DEBUG_NEW CCar(nId, CAR_TYPE::CAR_TYPE_CAR);

	if (pCar != nullptr)
	{
		// 座標設定
		pCar->SetPosition(pos);
		pCar->SetRecvPosition(pos);

		// 初期化処理
		pCar->Init();

		// 向き設定
		pCar->SetRotation(rot);
		pCar->SetRecvRotation(rot);

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
	if (m_pTailLamp == nullptr)
	{
		m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\taillamp.efkefc", m_Info.pos, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	}
}

//==========================================================
// テールランプ削除
//==========================================================
void CCar::DeleteTailLamp()
{
	SAFE_DELETE(m_pTailLamp);
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
		//CManager::GetInstance()->GetDebugProc()->Print("一般車の速度 [ %f ]\n", m_Info.fSpeed);

		// デルタタイム取得
		float DeltaTime = CDeltaTime::GetInstance()->GetDeltaTime();

		// 移動量設定
		m_Info.move.x = m_Info.fSpeed * sinf(m_Info.rot.y);
		m_Info.move.y += GRAVITY * DeltaTime;
		m_Info.move.z = m_Info.fSpeed * cosf(m_Info.rot.y);

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

	// 角度補正
	if (fRotDest > D3DX_PI)
	{
		fRotDest -= D3DX_PI * 2.0f;
	}
	else if (fRotDest < -D3DX_PI)
	{
		fRotDest += D3DX_PI * 2.0f;
	}

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

	// デルタタイム取得
	float DeltaTime = CDeltaTime::GetInstance()->GetDeltaTime();

	//角度一致判定
	if (m_Info.bBack)
	{
		//差分追加
		if (m_Info.fSpeed < -5.0f)
		{
			fRotMove += fRotDiff * m_Info.fRotMulti * FRAME_RATE_SCALER * DeltaTime;
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
		fRotMove += fRotDiff * m_Info.fRotMulti * FRAME_RATE_SCALER * DeltaTime;

		// 状況によって速度を変更する
		if (fabsf(fRotDiff) >= ROT_CURVE)
		{
			m_Info.fSpeedDest += m_Info.fSpeedCurve;
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
	m_Info.posTarget = m_Info.pRoadTarget->GetPosition() + m_Info.offsetLane;
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

	CRoad* pRoad = list->Get(0);
	CRoad* pRoadClose = list->Get(0);

	// 道が存在しなければ抜ける
	if (pRoad == nullptr) { return; }

	// 距離計算用変数
	float length = D3DXVec3Length(&(pRoadClose->GetPosition() - m_Info.pos));
	float lengthClose = 0.0f;

	// 最寄りの道を見つける
	for (int i = 0; i < list->GetNum() - 1; i++)
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
	Clist<int> listStation;

	// 道の数を取り出す
	for (int i = 0; i < CRoad::DIC_MAX; i++)
	{
		if (m_Info.pRoadTarget->GetConnectRoad((CRoad::DIRECTION)i) == nullptr) { continue; }

		if (m_Info.pRoadTarget->GetType() != CRoad::TYPE_STOP)
		{
			if (m_Info.pRoadTarget->GetConnectRoad((CRoad::DIRECTION)i) == m_Info.pRoadStart) { continue; }
		}

		listStation.Regist(i);
	}

	// 進む方向をランダムで決定
	int roadPoint = listStation.Get(rand() % listStation.GetNum());
	pRoadNext = m_Info.pRoadTarget->GetConnectRoad((CRoad::DIRECTION)roadPoint);

	// 進行方向によって車線の幅分目的地をずらす
	if ((CRoad::DIRECTION)roadPoint == CRoad::DIC_UP)
	{// 上
		m_Info.offsetLane = D3DXVECTOR3(-LENGTH_LANE, 0.0f, 0.0f);
	}
	else if ((CRoad::DIRECTION)roadPoint == CRoad::DIC_DOWN)
	{// 下
		m_Info.offsetLane = D3DXVECTOR3(LENGTH_LANE, 0.0f, 0.0f);
	}
	else if ((CRoad::DIRECTION)roadPoint == CRoad::DIC_LEFT)
	{// 左
		m_Info.offsetLane = D3DXVECTOR3(0.0f, 0.0f, -LENGTH_LANE);
	}
	else if ((CRoad::DIRECTION)roadPoint == CRoad::DIC_RIGHT)
	{// 右
		m_Info.offsetLane = D3DXVECTOR3(0.0f, 0.0f, LENGTH_LANE);
	}

	// 目的地と出発地点が存在する時
	if (m_Info.pRoadStart != nullptr && m_Info.pRoadTarget != nullptr)
	{
		D3DXVECTOR3 vecTarget, vecNext, vecTemp;
		vecTarget = m_Info.pRoadTarget->GetPosition() - m_Info.pRoadStart->GetPosition();
		vecNext = pRoadNext->GetPosition() - m_Info.pRoadStart->GetPosition();

		// 曲がる方向に対して速度を設定
		if (m_Info.pRoadTarget->GetType() == CRoad::TYPE_STOP)
		{// Uターン
			m_Info.fSpeedCurve = SPEED_CURVE_UTURN;
		}
		else if (D3DXVec3Cross(&vecTemp, &vecTarget, &vecNext)->y > 0.0f)
		{// 左折
			m_Info.fSpeedCurve = SPEED_CURVE_LEFT;
		}
		else if (D3DXVec3Cross(&vecTemp, &vecTarget, &vecNext)->y < 0.0f)
		{// 右折
			m_Info.fSpeedCurve = SPEED_CURVE_RIGHT;
		}
		else
		{// 直進
			m_Info.fSpeedCurve = SPEED_CURVE_LEFT;
		}
	}
	else
	{
		m_Info.fSpeedCurve = SPEED_CURVE_LEFT;
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

	bool bCollision = false;

	// オブジェクトとの当たり判定
	if (CollisionObjX())
		bCollision = true;

	// 道との当たり判定
	if (CollisionRoad())
		bCollision = true;

	// 地面との当たり判定
	if (CollisionField())
		bCollision = true;

	// ギミックとの当たり判定
	if (CollisionGimick())
		bCollision = true;

	return bCollision;
}

//==========================================================
// オブジェクトとの当たり判定処理
//==========================================================
bool CCar::CollisionObjX(void)
{
	auto mgr = CObjectX::GetList();
	for (int i = 0; i < mgr->GetNum(); i++)
	{// 使用されていない状態まで

		CObjectX* pObjectX = mgr->Get(i);	// 取得

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
// 道との当たり判定処理
//==========================================================
bool CCar::CollisionRoad(void)
{
	// 道との判定
	auto listRoad = CRoadManager::GetInstance()->GetList();
	for (int i = 0; i < listRoad->GetNum(); i++)
	{// 使用されていない状態まで

		// 道確認
		CRoad* pRoad = listRoad->Get(i);	// 先頭を取得
		if (pRoad == nullptr) { continue; }

		D3DXVECTOR3* pVtx = pRoad->GetVtxPos();
		D3DXVECTOR3 pos = pRoad->GetPosition();

		float height = m_Info.pos.y - 0.1f;
		D3DXVECTOR3 vec1 = pVtx[1] - pVtx[0], vec2 = pVtx[2] - pVtx[0];
		D3DXVECTOR3 nor0, nor1;

		D3DXVec3Cross(&nor0, &vec1, &vec2);
		D3DXVec3Normalize(&nor0, &nor0);	// ベクトルを正規化する

		vec1 = pVtx[2] - pVtx[3];
		vec2 = pVtx[1] - pVtx[3];
		D3DXVec3Cross(&nor1, &vec1, &vec2);
		D3DXVec3Normalize(&nor1, &nor1);	// ベクトルを正規化する

		// 判定
		collision::IsOnSquarePolygon(pos + pVtx[0], pos + pVtx[1], pos + pVtx[2], pos + pVtx[3],
			nor0, nor1, m_Info.pos, m_Info.posOld, height);

		if (height >= m_Info.pos.y)
		{
			m_Info.pos.y = height;
			m_Info.move.y = 0.0f;
		}
	}

	return false;
}

//==========================================================
// ギミックとの当たり判定処理
//==========================================================
bool CCar::CollisionGimick(void)
{
	auto listGimmick = CGimmick::GetList();
	for (int i = 0; i < listGimmick->GetNum(); i++)
	{// 使用されていない状態まで

		// 道確認
		CGimmick* pGimmick = listGimmick->Get(i);	// 先頭を取得
		if (pGimmick == nullptr) { continue; }

		if (pGimmick->GetType() != CGimmick::TYPE_BRIDGE) { continue; }

		CBridge* pBridge = dynamic_cast <CBridge*> (pGimmick);

		for (int bridge = 0; bridge < BRIDGE_NUM; bridge++)
		{
			// ポリゴンの大きさを計算
			CObjectX* pObjectX = pBridge->GetObjectX(bridge);
			D3DXVECTOR3 posGimmick = pObjectX->GetPosition();
			D3DXVECTOR3 rotGimmick = pObjectX->GetRotation();
			D3DXVECTOR3 sizeMax = pObjectX->GetVtxMax();
			D3DXVECTOR3 sizeMin = pObjectX->GetVtxMin();
			sizeMin.y = 0.0f;

			sizeMax = collision::PosRelativeMtx(VECTOR3_ZERO, rotGimmick, sizeMax);
			sizeMin = collision::PosRelativeMtx(VECTOR3_ZERO, rotGimmick, sizeMin);

			float height = m_Info.pos.y - 0.1f;
			D3DXVECTOR3 pVtx[4];
			pVtx[0] = D3DXVECTOR3(sizeMax.x, sizeMax.y, sizeMax.z);
			pVtx[1] = D3DXVECTOR3(sizeMin.x, sizeMin.y, sizeMax.z);
			pVtx[2] = D3DXVECTOR3(sizeMax.x, sizeMax.y, sizeMin.z);
			pVtx[3] = D3DXVECTOR3(sizeMin.x, sizeMin.y, sizeMin.z);

			// 法線ベクトルを計算する
			D3DXVECTOR3 vec1, vec2;
			D3DXVECTOR3 nor0, nor1;

			vec1 = pVtx[1] - pVtx[0];
			vec2 = pVtx[2] - pVtx[0];
			D3DXVec3Cross(&nor0, &vec1, &vec2);
			D3DXVec3Normalize(&nor0, &nor0);	// ベクトルを正規化する

			vec1 = pVtx[2] - pVtx[3];
			vec2 = pVtx[1] - pVtx[3];
			D3DXVec3Cross(&nor1, &vec1, &vec2);
			D3DXVec3Normalize(&nor1, &nor1);	// ベクトルを正規化する

			// 判定
			collision::IsOnSquarePolygon(posGimmick + pVtx[0], posGimmick + pVtx[1], posGimmick + pVtx[2], posGimmick + pVtx[3],
				nor0, nor1, m_Info.pos, m_Info.posOld, height);

			if (height >= m_Info.pos.y)
			{
				m_Info.pos.y = height;
				m_Info.move.y = 0.0f;
			}
		}
	}

	return false;
}

//==========================================================
// 地面との当たり判定処理
//==========================================================
bool CCar::CollisionField(void)
{
	// 道との判定
	auto listRoad = CMeshField::GetList();
	for (int i = 0; i < listRoad->GetNum(); i++)
	{// 使用されていない状態まで

		// 道確認
		CMeshField* pRoad = listRoad->Get(i);	// 先頭を取得
		if (pRoad == nullptr) { continue; }

		D3DXVECTOR3 pVtx[4] = {};
		D3DXVECTOR3 pos = pRoad->GetPosition();

		pVtx[0] = D3DXVECTOR3(-(pRoad->GetWidth() * pRoad->GetNumWidth()), 0.0f, (pRoad->GetHeight() * pRoad->GetNumHeight()));
		pVtx[1] = D3DXVECTOR3((pRoad->GetWidth() * pRoad->GetNumWidth()), 0.0f, (pRoad->GetHeight() * pRoad->GetNumHeight()));
		pVtx[2] = D3DXVECTOR3(-(pRoad->GetWidth() * pRoad->GetNumWidth()), 0.0f, -(pRoad->GetHeight() * pRoad->GetNumHeight()));
		pVtx[3] = D3DXVECTOR3((pRoad->GetWidth() * pRoad->GetNumWidth()), 0.0f, -(pRoad->GetHeight() * pRoad->GetNumHeight()));

		float height = m_Info.pos.y - 0.1f;
		D3DXVECTOR3 vec1 = pVtx[1] - pVtx[0], vec2 = pVtx[2] - pVtx[0];
		D3DXVECTOR3 nor0, nor1;

		D3DXVec3Cross(&nor0, &vec1, &vec2);
		D3DXVec3Normalize(&nor0, &nor0);	// ベクトルを正規化する

		vec1 = pVtx[2] - pVtx[3];
		vec2 = pVtx[1] - pVtx[3];
		D3DXVec3Cross(&nor1, &vec1, &vec2);
		D3DXVec3Normalize(&nor1, &nor1);	// ベクトルを正規化する

		// 判定
		collision::IsOnSquarePolygon(pos + pVtx[0], pos + pVtx[1], pos + pVtx[2], pos + pVtx[3],
			nor0, nor1, m_Info.pos, m_Info.posOld, height);

		if (height >= m_Info.pos.y)
		{
			m_Info.pos.y = height;
			m_Info.move.y = 0.0f;
		}
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

	CDebugProc::GetInstance()->Print(" 受信しています %d 座標 : [%f, %f, %f]\n", m_Info.nId,
		m_RecvInfo.pos.x, m_RecvInfo.pos.y, m_RecvInfo.pos.z);
}

//===============================================
// 車の座標の更新
//===============================================
void CCar::SendPosition()
{
	CNetWork* pNet = CNetWork::GetInstance();

	pNet->SendCarPos(GetId(), GetPosition(), GetRotation());
}