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
#include "spot_light.h"

// マクロ定義

// 無名名前空間を定義
namespace
{
	const float LENGTH_POINT = (900.0f);			// 到達判定距離
	const float LENGTH_POINT_CHASE = (500.0f);		// すれ違い判定距離
	const float SECURE_SPEEDDEST = (-20.0f);		// 確保時の目標速度
	const float SECURE_SPEED = (0.8f);				// 確保時の加速倍率
	const float SPEED_CURVE_RIGHT = (9.0f);			// 左折時の速度
	const float SPEED_CURVE_LEFT = (3.0f);			// 右折時の速度
	const float SPEED_CURVE_UTURN = (7.0f);			// Uターン時の速度
	const float ROT_MULTI_DEF = (0.02f);			// 通常時の向き補正倍率
	const float ROT_MULTI_CHASE = (0.04f);			// 追跡時の向き補正倍率
	const float ROT_MULTI_ATTACK = (0.005f);		// 攻撃時の向き補正倍率
	const float ROT_MULTI_PREP = (0.08f);			// 準備時の向き補正倍率
	const float LENGTH_LANE = (-400.0f);			// 車線の幅
	const D3DXVECTOR3 LIGHT_OFFSET = D3DXVECTOR3(0.0f, 100.0f, 250.0f);
	const D3DXVECTOR3 LIGHT_VEC = D3DXVECTOR3(0.0f, -0.25f, 1.0f);
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
CPolice::CPolice(int nId, CAR_TYPE type) : CCar(nId, type),
m_Info(SInfo()),
m_pPatrolLamp(nullptr),
m_pSiren(nullptr),
m_pPoliceAI(nullptr),
m_pShaderLight(nullptr),
m_pSpotLight(nullptr),
m_stateInfo(SState())
{
	// 値のクリア
	CPoliceManager::GetInstance()->ListIn(this);
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
	m_pObj = CObjectX::Create(GetPosition(), VECTOR3_ZERO, "data\\MODEL\\police.x");
	m_pObj->SetType(CObject::TYPE_ENEMY);

	// AIを生成
	m_pPoliceAI = CPoliceAI::Create(this, CPoliceAI::TYPE_NORMAL);
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CPolice::Uninit(void)
{
	// AIを破棄
	if (m_pPoliceAI != nullptr)
	{
		m_pPoliceAI->Uninit();
		delete m_pPoliceAI;
		m_pPoliceAI = nullptr;
	}
	
	CCar::Uninit();
	CPoliceManager::GetInstance()->ListOut(this);
	SAFE_DELETE(m_pPatrolLamp);
	m_pSiren = nullptr;
	if (m_pShaderLight != nullptr)
	{
		CShaderLight::Delete(m_pShaderLight);
	}
	SAFE_UNINIT(m_pSpotLight);
	SAFE_DELETE(m_pShaderLight);
	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CPolice::Update(void)
{
	if (m_pObj == nullptr) { return; }

	// 停止状態なら動かない
	if (m_stateInfo.state == STATE::STATE_STOP) 
	{ 
		MoveRoad();
		return;
	}

	// アップデート
	CCar::Update();

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	D3DXMatrixRotationYawPitchRoll(&mat, GetRotation().y, GetRotation().x, GetRotation().z);
	D3DXVECTOR3 lightpos = LIGHT_OFFSET;
	D3DXVECTOR3 lightvec = LIGHT_VEC;
	D3DXVec3Normalize(&lightvec, &lightvec);
	D3DXVec3TransformCoord(&lightpos, &lightpos, &mat);
	D3DXVec3TransformCoord(&lightvec, &lightvec, &mat);
	if (m_pShaderLight != nullptr)
	{
		m_pShaderLight->position = GetPosition() + lightpos;
		m_pShaderLight->direction = lightvec;
	}
	if (m_pSpotLight != nullptr)
	{
		m_pSpotLight->SetPositon(GetPosition() + lightpos);
		m_pSpotLight->SetDirection(lightvec);
		m_pSpotLight->Update();
	}

	auto list = CPoliceManager::GetInstance()->GetNearList();
	auto it = find(list->begin(), list->end(), this);

	if (m_Info.bChase && list->end() != it)
	{
		if (m_pPatrolLamp == nullptr)
		{
			m_pPatrolLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\patrollamp.efkefc", 
				VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
		}

		if (m_pPatrolLamp != nullptr)
		{
			m_pPatrolLamp->m_pos = GetPosition();
			m_pPatrolLamp->m_rot = GetRotation();
		}
	}
	else
	{
		SAFE_DELETE(m_pPatrolLamp);
	}

	if (list->end() != it)
	{
		TailLamp();

		// 一番近いとシェーダーライト、それ以外はスポットライトを設定
		if (std::distance(list->begin(), it) < 1)
		{
			if (m_pShaderLight == nullptr)
			{
				m_pShaderLight = CShaderLight::Create(GetPosition(), D3DXVECTOR3(1.0f, 0.9f, 0.8f), 0.0f, 5000.0f, D3DXVECTOR3(0.0f, -0.25f, 1.0f), D3DXToRadian(45));
				SAFE_UNINIT(m_pSpotLight);
			}

			if (m_pShaderLight != nullptr)
			{
				if (m_pShaderLight->intensity < 1.0f)
				{
					m_pShaderLight->intensity += 1.0f * 0.1f;
				}
			}

		}
		else
		{
			/*if (m_pSpotLight == nullptr)
			{
				m_pSpotLight = CSpotLight::Create();
				m_pSpotLight->SetDiffuse(D3DXCOLOR(3.0f, 2.7f, 2.4f, 1.0f));
			}*/

			if (m_pShaderLight != nullptr)
			{
				m_pShaderLight->intensity -= 1.0f * 0.1f;
				if (m_pShaderLight->intensity <= 0.0f)
				{
					CShaderLight::Delete(m_pShaderLight);
					SAFE_DELETE(m_pShaderLight);
				}
			}
		}
	}
	else
	{
		DeleteTailLamp();

		if (m_pShaderLight != nullptr)
		{
			m_pShaderLight->intensity -= 2.0f * 0.1f;
			if (m_pShaderLight->intensity <= 0.0f)
			{
				CShaderLight::Delete(m_pShaderLight);
				SAFE_DELETE(m_pShaderLight);
			}
		}

		SAFE_UNINIT(m_pSpotLight);
	}

	// 状態更新
	UpdateState();
}

//==========================================================
// 生成
//==========================================================
CPolice *CPolice::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move, int nId)
{
	CPolice *pCar = nullptr;

	pCar = DEBUG_NEW CPolice(nId, CAR_TYPE::CAR_TYPE_POLICE);

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
// 道移動処理
//==========================================================
void CPolice::MoveRoad()
{
	// 移動地点用変数
	CRoad* pRoadStart = GetRoadStart();
	CRoad* pRoadTarget = GetRoadTarget();

	// 目的地が存在しない時最寄りの道を目的地に設定する
	if (pRoadTarget == nullptr && IsActive())
		SearchRoad();

	// 追跡状態の判定
	if (m_Info.bChase)
	{
		SetRoadStart(GetRoadTarget());

		// プレイヤー追跡処理
		ChasePlayer();

		// 応援要請処理
		m_pPoliceAI->CallBackup();

		float dis = GetDistance(m_Info.pPlayer->GetPosition() , GetPosition());
		float vol = 8000.0f - dis;
		vol /= 8000.0f;
		if (vol < 0.0f)
		{
			vol = 0.0f;
		}

		if (m_pSiren != nullptr)
		{
			m_pSiren->SetVolume(vol*0.5f);
		}

		// カーブ時の速度を設定
		SetSpeedCurve(10.0f);


		// 目的地が存在するかどうか
		if (pRoadTarget != nullptr)
		{
			// 追跡用に加速する
			SetSpeedDest(GetSpeedDest() + m_pPoliceAI->GetChaseSpeed());
			SetRotMulti(ROT_MULTI_CHASE);

			// 目的地の座標を代入
			SetPosTarget(pRoadTarget->GetPosition() + GetOffsetLane());

			// プレイヤーが存在しないなら最寄りの道に戻る
			if (m_Info.pPlayer == nullptr) { return; }

			// 一定距離まで近づいたら減速させる
			D3DXVECTOR3 len = m_Info.pPlayer->GetPosition() - GetPosition();
			if (D3DXVec3Length(&len) > LENGTH_POINT_CHASE) { return; }

			// プレイヤーの座標を目指す
			SetPosTarget(m_Info.pPlayer->GetPosition() + GetOffsetLane());

			// 速度を設定
			SetSpeedDest(SECURE_SPEEDDEST);
			SetSpeed(GetSpeed() * SECURE_SPEED);
		}
		else
		{
			// プレイヤーが存在しないなら最寄りの道に戻る
			if (m_Info.pPlayer == nullptr) { return; }

			// 追跡用に加速する
			SetSpeedDest(GetSpeedDest() + m_pPoliceAI->GetChaseSpeed());

			if (m_pPoliceAI->GetState() == CPoliceAI::STATE_ATTACK)
			{
				SetRotMulti(ROT_MULTI_ATTACK);

				// プレイヤーの座標を目指す
				SetPosTarget(m_Info.pPlayer->GetPosition());
			}
			else if(m_pPoliceAI->GetState() == CPoliceAI::STATE_PREP)
			{
				SetRotMulti(ROT_MULTI_PREP);

				// プレイヤーの座標を目指す
				SetPosTarget(m_Info.pPlayer->GetPosition());

				// 一定距離まで近づいたら減速させる
				D3DXVECTOR3 len = m_Info.pPlayer->GetPosition() - GetPosition();
				if (D3DXVec3Length(&len) > LENGTH_POINT_CHASE) { return; }

				// 速度を設定
				SetSpeedDest(SECURE_SPEEDDEST);
				SetSpeed(GetSpeed() * SECURE_SPEED);
			}
			else if (m_pPoliceAI->GetState() == CPoliceAI::STATE_FINISH)
			{
				SetRotMulti(ROT_MULTI_ATTACK);

				// プレイヤーの座標を目指す
				SetPosTarget(m_Info.pPlayer->GetPosition());

				// 一定距離まで近づいたら減速させる
				D3DXVECTOR3 len = m_Info.pPlayer->GetPosition() - GetPosition();
				if (D3DXVec3Length(&len) > LENGTH_POINT_CHASE) { return; }

				// 速度を設定
				SetSpeedDest(SECURE_SPEEDDEST);
				SetSpeed(GetSpeed() * SECURE_SPEED);
			}
			else
			{
				SetRotMulti(ROT_MULTI_CHASE);

				// プレイヤーの座標を目指す
				SetPosTarget(m_Info.pPlayer->GetPosition() + GetOffsetLane());

				// 一定距離まで近づいたら減速させる
				D3DXVECTOR3 len = m_Info.pPlayer->GetPosition() - GetPosition();
				if (D3DXVec3Length(&len) > LENGTH_POINT_CHASE) { return; }

				// 速度を設定
				SetSpeedDest(SECURE_SPEEDDEST);
				SetSpeed(GetSpeed() * SECURE_SPEED);
			}
		}
	}
	else
	{
		// 目的地が存在する時
		if (pRoadTarget != nullptr || IsActive())
		{
			pRoadStart = GetRoadStart();
			pRoadTarget = GetRoadTarget();

			// 一定距離まで近づいたら到達判定処理を行う
			if (pRoadTarget == nullptr) { return; }
			D3DXVECTOR3 len = (pRoadTarget->GetPosition() - GetPosition());
			float length = D3DXVec3Length(&len);
			if (length < LENGTH_POINT)
				ReachRoad();

			// 目的地の座標を代入
			SetPosTarget(pRoadTarget->GetPosition() + GetOffsetLane());
			SetRotMulti(ROT_MULTI_DEF);
		}
	}

	if (!GetBack()) { return; }

	// 速度を設定
	SetSpeedDest(SECURE_SPEEDDEST * 2.0f);
	SetSpeed(GetSpeed() * SECURE_SPEED);
}

//==========================================================
// プレイヤー発見処理
//==========================================================
void CPolice::SearchPlayer()
{
	if (m_pPoliceAI != nullptr)
	{
		m_pPoliceAI->Search();
	}
}

//==========================================================
// 追跡経路探索処理
//==========================================================
void CPolice::ChasePlayer()
{
	// 追跡する
	if (m_pPoliceAI == nullptr || !IsActive()) { return; }

	m_pPoliceAI->Chase();

	m_pPoliceAI->Attack();

	LanePlayer();

	// 追跡経路が存在するならば目標地点に設定する
	if (m_pPoliceAI->GetSearchRoad() != nullptr)
	{
		SetRoadTarget(m_pPoliceAI->GetSearchRoad()->pConnectRoad);
	}
	else
	{
		SetRoadTarget(nullptr);
	}
}

//==========================================================
// 蛇行運転処理
//==========================================================
void CPolice::LanePlayer()
{
	m_Info.nLaneCount++;

	if (m_Info.nLaneCount > m_Info.nLaneTime)
	{
		// 移動の幅取得
		float length1 = static_cast<float>(((rand() % 6001) + 2000));
		float length2 = static_cast<float>((-(rand() % 6001) - 2000));

		float length = ((rand() % 2 == 0) ? length1 : length2) * 0.1f;

		if (m_pPoliceAI->GetSearchRoad() == nullptr || m_pPoliceAI->GetSearchRoad()->pChild == nullptr) { return; }

		D3DXVECTOR3 posRoad = m_pPoliceAI->GetSearchRoad()->pConnectRoad->GetPosition();
		D3DXVECTOR3 posRoadChild = m_pPoliceAI->GetSearchRoad()->pChild->pConnectRoad->GetPosition();
		D3DXVECTOR3 vecRoad = posRoadChild - posRoad;
		float rotVec = atan2f(vecRoad.z, vecRoad.x);								// 角度計算

		m_Info.offsetLane.x = sinf(rotVec) * length;
		m_Info.offsetLane.y = 0.0f;
		m_Info.offsetLane.z = cosf(rotVec) * length;

		m_Info.nLaneCount = 0;
		m_Info.nLaneTime = rand() % 181 + 120;
	}

	SetOffsetLane(m_Info.offsetLane);
}

//==========================================================
// 当たり判定処理
//==========================================================
bool CPolice::Collision()
{
	CollisionEnemy();

	return CCar::Collision();
}

//==========================================================
// オブジェクトとの当たり判定処理
//==========================================================
bool CPolice::CollisionEnemy(void)
{
	D3DXVECTOR3 posPolice = GetPosition();
	D3DXVECTOR3 rotPolice = GetRotation();
	D3DXVECTOR3 sizeMaxPolice = m_pObj->GetVtxMax();
	D3DXVECTOR3 sizeMinPolice = m_pObj->GetVtxMin();
	D3DXVECTOR3 sizePolice = (sizeMaxPolice - sizeMinPolice) * 0.5f;

	auto mgr = CObjectX::GetList();
	for (int i = 0; i < mgr->GetNum(); i++)
	{// 使用されていない状態まで

		CObjectX* pObjectX = mgr->Get(i);	// 取得

		if (pObjectX->GetType() != CObject::TYPE_ENEMY){ continue; }

		if (!pObjectX->GetEnableCollision()) { continue; }

		// オブジェクトの情報取得
		D3DXVECTOR3 posObjectX = pObjectX->GetPosition();
		D3DXVECTOR3 rotObjectX = pObjectX->GetRotation();
		D3DXVECTOR3 sizeMaxObjectX = pObjectX->GetVtxMax();
		D3DXVECTOR3 sizeMinObjectX = pObjectX->GetVtxMin();
		D3DXVECTOR3 sizeObjectX = (sizeMaxObjectX - sizeMinObjectX) * 0.5f;

		// OBBとの当たり判定を実行
		bool bCollision = collision::CollideOBBToOBBTrigger(posPolice, rotPolice, sizePolice, posObjectX, rotObjectX, sizeObjectX);

		// 衝突していない場合繰り返す
		if (!bCollision) { continue; }

		m_Info.nLaneTime = 0;
		LanePlayer();

		return true;
	}

	return false;
}

//==========================================================
// 接触時処理
//==========================================================
void CPolice::Hit()
{
	if (GetBack()) { return; }

	if (!m_Info.bChase)
	{
		CRoad* pRoadNext = GetRoadTarget();
		SetRoadTarget(GetRoadStart());
		SetRoadStart(pRoadNext);
		SetBack(true);
		SetBackTime(80);
	}

	m_pPoliceAI->StopAttack();
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
// AIタイプ変更処理
//==========================================================
void CPolice::SetTypeAI(CPoliceAI::TYPE type)
{
	// AIを破棄
	if (m_pPoliceAI != nullptr)
	{
		m_pPoliceAI->Uninit();
		delete m_pPoliceAI;
		m_pPoliceAI = nullptr;
	}

	// AIを生成
	m_pPoliceAI = CPoliceAI::Create(this, type);
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

//===============================================
// 警察の座標の送信
//===============================================
void CPolice::SendPosition()
{
	CNetWork* pNet = CNetWork::GetInstance();

	pNet->SendPdPos(GetId(), GetPosition(), GetRotation().y);
}

//===============================================
// 警察の追跡開始送信
//===============================================
void CPolice::SendChase()
{
	if (m_Info.pPlayer == nullptr) { return; }
	CNetWork* pNet = CNetWork::GetInstance();

	pNet->SendPdChase(GetId(), m_Info.pPlayer->GetId());
}

//===============================================
// 警察の追跡終了送信
//===============================================
void CPolice::SendChaseEnd()
{
	CNetWork* pNet = CNetWork::GetInstance();

	pNet->SendPdChaseEnd(GetId());
}

//===============================================
// 受信した種類設定
//===============================================
void CPolice::RecvTypeSet()
{
	// 種類設定
	CCar::RecvTypeSet();

	if (m_stateInfo.chasenext == CHASE::CHASE_MAX) { return; }

	// 次の指定された状態での設定
	switch (m_stateInfo.chasenext)
	{
	case CHASE::CHASE_BEGIN:
	{
		// 追跡開始
		m_pPoliceAI->BeginChase(m_stateInfo.pNextPlayer);
		m_stateInfo.pNextPlayer = nullptr;
	}
	break;

	case CHASE::CHASE_END:
	{
		m_pPoliceAI->EndChase();
	}
	break;

	default:
		break;
	}

	m_stateInfo.chasenext = CHASE::CHASE_MAX;
}