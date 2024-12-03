//==========================================================
//
// 検問処理 [inspection.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "inspection.h"
#include "add_police.h"
#include "objectX.h"
#include "debugproc.h"
#include "player.h"
#include "player_manager.h"
#include "deltatime.h"
#include "gimmick_policestation.h"
#include "a_star.h"
#include "particle3D.h"
#include "inspection_manager.h"

// 無名名前空間を定義
namespace
{
	const float POLICE_SETLENGTH = 700.0f;	// 警察設置距離
	const float TIMER = 10.0f;				// タイマー
	const float START_SPEEDDEST = 40.0f;	// スタート
	const float STOP_TIMER = 3.0f;			// ストップ
	const float ROTATE_TIMER = 1.0f;		// 回転
	const float LAGER_TIMER = 3.0f;			// 検問線出るまでの速度
	const float LAGER_LENGTH = 700.0f;
}

//==========================================================
// コンストラクタ
//==========================================================
CInspection::CInspection(int nId)
{
	// 値のクリア
	m_Info = SInfo();
	m_Info.nId = nId;
	m_pNearStation = nullptr;
	m_pRoad = nullptr;
	m_LagerInfo = SLagerInfo();

	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		m_aPoliceInfo[i] = SPoliceInfo();
	}

	// リストに挿入
	CInspectionManager::GetInstance()->ListIn(this);
}

//==========================================================
// デストラクタ
//==========================================================
CInspection::~CInspection()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CInspection::Init(void)
{
	// 座標取得
	D3DXVECTOR3 pos = m_Info.pos;

	// 生成
	m_pNearStation = CGimmickPoliceStation::GetNear(GetPosition());

	// 向き取得
	D3DXVECTOR3 rot = m_Info.rot;
	rot.y = m_Info.rot.y;
	Adjust(rot);

	// 出動するパトカーの生成
	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		if (m_pNearStation != nullptr) { pos = m_pNearStation->GetPos(); }

		// 座標取得
		D3DXVECTOR3 goalpos = m_Info.pos;
		goalpos.x += sinf(rot.y) * POLICE_SETLENGTH;
		goalpos.z += cosf(rot.y) * POLICE_SETLENGTH;

		m_aPoliceInfo[i].pPolice = CAddPolice::Create(pos, VECTOR3_ZERO, VECTOR3_ZERO);
		m_aPoliceInfo[i].goalpos = goalpos;

		// 経路を設定
		if (m_pRoad != nullptr)
		{
			m_aPoliceInfo[i].pPolice->SetNavi(AStar::AStar(m_pNearStation->GetRoad()->GetSearchSelf(), m_pRoad->GetSearchSelf()));
			m_aPoliceInfo[i].pPolice->SetState(CPolice::STATE::STATE_NORMAL);
		}

		rot.y += D3DX_PI;
		Adjust(rot.y);
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CInspection::Uninit(void)
{
	// 警察廃棄
	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		if (m_aPoliceInfo[i].pPolice != nullptr)
		{
			m_aPoliceInfo[i].pPolice->SetState(CPolice::STATE::STATE_SEARCH);
			m_aPoliceInfo[i].pPolice = nullptr;
			m_aPoliceInfo[i] = SPoliceInfo();
		}
	}

	for (int i = 0; i < InstpectionData::NUM_EFFECT; i++)
	{
		SAFE_DELETE(m_LagerInfo.apEffect[i]);
	}

	// リストから外す
	CInspectionManager::GetInstance()->ListOut(this);

	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CInspection::Update(void)
{
	// デバッグ表示
	CDebugProc::GetInstance()->Print("検問の座標 : [ %f, %f, %f] : 向き : [ %f, %f, %f] \n", 
		m_Info.pos.x, m_Info.pos.y, m_Info.pos.z, m_Info.rot.x, m_Info.rot.y, m_Info.rot.z);

	// 移動
	Away();

	// 検問線配置
	LagerSet();

	// 衝突判定
	Collision();
}

//==========================================================
// 生成
//==========================================================
CInspection* CInspection::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, CRoad* pRoad, int nId)
{
	CInspection* pInsp = nullptr;

	pInsp = DEBUG_NEW CInspection(nId);

	if (pInsp != nullptr)
	{
		// 値設定
		pInsp->SetPosition(pos);
		pInsp->SetRotation(rot);
		pInsp->m_pRoad = pRoad;

		// 初期化処理
		pInsp->Init();
	}

	return pInsp;
}

//==========================================================
// 移動
//==========================================================
void CInspection::Away()
{
	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		SPoliceInfo* pInfo = &m_aPoliceInfo[i];
		// 停止状態の時だけ動く
		if (pInfo->pPolice == nullptr) { continue; }
		if (pInfo->pPolice->GetState() != CPolice::STATE::STATE_STOP) { continue; }
		if (pInfo->fTimer >= STOP_TIMER) { continue; }

		// 初回の地点を保存
		if (pInfo->fTimer == 0.0f) { 
			pInfo->startpos = pInfo->pPolice->GetPosition();
			pInfo->goalrot.y = atan2f(pInfo->goalpos.x - pInfo->startpos.x, pInfo->goalpos.z - pInfo->startpos.z);
			Adjust(pInfo->goalrot.y);
		}

		// タイマーを進行
		pInfo->fTimer += CDeltaTime::GetInstance()->GetDeltaTime();
		float range = pInfo->fTimer / STOP_TIMER;
		if (range >= 1.0f) { range = 1.0f; }

		// 移動
		D3DXVECTOR3 diff = pInfo->goalpos - pInfo->startpos;
		D3DXVECTOR3 pos = pInfo->startpos + diff * range;
		pInfo->pPolice->SetPosition(pos);

		// 向き
		D3DXVECTOR3 rot = pInfo->pPolice->GetRotation();
		float rotdiff = pInfo->goalrot.y - rot.y;
		Adjust(rotdiff);
		rot.y += rotdiff * 0.1f;
		Adjust(rot);
		pInfo->pPolice->SetRotation(rot);
	}
}

//==========================================================
// 検問線を配置
//==========================================================
void CInspection::LagerSet()
{
	// 停止しているときのみ
	if (m_aPoliceInfo[TYPE::TYPE_SEARCH_L].fTimer < STOP_TIMER
		|| m_aPoliceInfo[TYPE::TYPE_SEARCH_R].fTimer < STOP_TIMER)
	{
		return;
	}

	// 回転処理
	LagerSetRotation();

	// タイマーを進める
	if (m_LagerInfo.fTimer < LAGER_TIMER)
	{
		// 初回のみ
		if (m_LagerInfo.fTimer == 0.0f)
		{
			// エフェクトの生成
			for (int i = 0; i < InstpectionData::NUM_EFFECT; i++)
			{
				// 警察が使われていたら
				if (m_aPoliceInfo[i].pPolice == nullptr) { continue; }

				m_LagerInfo.apEffect[i] = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\marker_laser.efkefc", 
					m_aPoliceInfo[i].goalpos, VECTOR3_ZERO, VECTOR3_ZERO, 70.0f, true, false);
			}
		}

		m_LagerInfo.fTimer += CDeltaTime::GetInstance()->GetDeltaTime();

		if (m_LagerInfo.fTimer >= LAGER_TIMER)
		{
			m_LagerInfo.fTimer = LAGER_TIMER;
		}
	}

	// 割合を求める
	m_LagerInfo.scale = m_LagerInfo.fTimer / LAGER_TIMER;

	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		SPoliceInfo* pInfo = &m_aPoliceInfo[i];

		D3DXVECTOR3 posdiff = m_Info.pos - pInfo->goalpos;
		D3DXVECTOR3 nor = posdiff;
		D3DXVec3Normalize(&nor, &nor);

		if (i >= InstpectionData::NUM_EFFECT) { continue; }
		if (m_LagerInfo.apEffect[i] == nullptr) { continue; }

		// 向き
		float diff = atan2f(m_Info.pos.x - pInfo->goalpos.x, m_Info.pos.z - pInfo->goalpos.z);
		Adjust(diff);
		D3DXVECTOR3 rot = VECTOR3_ZERO;
		rot.y = diff;
		m_LagerInfo.apEffect[i]->m_rot = rot;

		// スケール
		m_LagerInfo.apEffect[i]->m_Scale.Z = m_LagerInfo.scale * LAGER_LENGTH;

		// 位置
		D3DXVECTOR3 pos = pInfo->goalpos + ((m_Info.pos - pInfo->goalpos) * m_LagerInfo.scale);
		m_LagerInfo.apEffect[i]->m_pos = pos;
	}
}

//==========================================================
// 検問時の向き設定
//==========================================================
void CInspection::LagerSetRotation()
{
	// タイマーを進行
	if (m_LagerInfo.fRotateTimer <= ROTATE_TIMER) {
		m_LagerInfo.fRotateTimer += CDeltaTime::GetInstance()->GetDeltaTime();
	}

	float range = m_LagerInfo.fRotateTimer / ROTATE_TIMER;
	if (range > 1.0f) { range = 1.0f; }

	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		SPoliceInfo* pInfo = &m_aPoliceInfo[i];

		// 停止状態の時だけ動く
		if (pInfo->pPolice == nullptr) { continue; }
		if (pInfo->pPolice->GetState() != CPolice::STATE::STATE_STOP) { continue; }

		// 差分を求める
		float diff = atan2f(m_Info.pos.x - pInfo->goalpos.x, m_Info.pos.z - pInfo->goalpos.z);
		Adjust(diff);
		float dest = diff - pInfo->goalrot.y;
		Adjust(dest);

		// 向きを設定
		D3DXVECTOR3 rot = pInfo->goalrot;
		rot.y += dest * range;
		pInfo->pPolice->SetRotation(rot);

		// 座標も少し調整
		diff += D3DX_PI;
		Adjust(diff);
		D3DXVECTOR3 pos = pInfo->goalpos;
		pos.x += sinf(diff) * (100.0f * range);
		pos.z += cosf(diff) * (100.0f * range);
		pInfo->pPolice->SetPosition(pos);
	}
}

//==========================================================
// 出撃
//==========================================================
void CInspection::Start()
{
	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{
		SPoliceInfo* pInfo = &m_aPoliceInfo[i];

		// 警察を切り離す
		if (pInfo->pPolice != nullptr)
		{
			std::vector<CRoad::SSearch*> navi = {};
			navi.clear();
			
			pInfo->pPolice->SetNavi(navi);
			pInfo->pPolice->SetState(CPolice::STATE::STATE_SEARCH);
			pInfo->pPolice = nullptr;
		}

		if (i < InstpectionData::NUM_EFFECT)
		{
			SAFE_DELETE(m_LagerInfo.apEffect[i]);
		}
	}

	// 終了
	Uninit();
}

//==========================================================
// 判定
//==========================================================
void CInspection::Collision()
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
	if (pPlayer == nullptr) { return; }

	for (int i = 0; i < InstpectionData::NUM_EFFECT; i++)
	{
		if (m_LagerInfo.apEffect[i] == nullptr) { continue; }

		// 線分
		D3DXVECTOR3 vtx1 = m_aPoliceInfo[i].goalpos;
		D3DXVECTOR3 vtx2 = m_aPoliceInfo[i].goalpos + (m_Info.pos - m_aPoliceInfo[i].goalpos) * m_LagerInfo.scale;

		// プレイヤーの位置
		D3DXVECTOR3 pos = pPlayer->GetPosition();
		D3DXVECTOR3 posOld = pPlayer->GetOldPosition();

		// 通り過ぎた
		if (collision::LineCrossProduct(vtx1, vtx2, &pos, posOld))
		{
			// 追跡
			Start();
			return;
		}
		// 警察が既に出発済み
		else if (m_aPoliceInfo[i].pPolice->GetState() != CPolice::STATE::STATE_STOP &&
			m_aPoliceInfo[i].pPolice->GetState() != CPolice::STATE::STATE_NORMAL)
		{
			// 追跡
			Start();
			return;
		}
	}

	// 時間を計測
	for (int i = 0; i < TYPE::TYPE_MAX; i++)
	{ 
		if (m_aPoliceInfo[i].pPolice == nullptr) { return; }
		//if(m_aPoliceInfo[i].pPolice->GetState() != )
	}
}