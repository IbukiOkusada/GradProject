//==========================================================
//
// 消火栓ギミック処理 [gimmick_firehydrant.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "manager.h"
#include "gimmick_policestation.h"
#include "deltatime.h"
#include "objectX.h"
#include "camera.h"
#include "camera_manager.h"
#include "renderer.h"
#include "particle3D.h"
#include "player.h"
#include "player_manager.h"
#include "add_police.h"
#include "debugproc.h"
#include "road_manager.h"
#include "car_manager.h"

// 定数定義
namespace
{
	// ファイル名
	const char* FILENAME = "data\\MODEL\\map\\policestation.x";
	const float OUT_ENGINE = 0.6f;
	const float INTERVAL = 5.0f;
	const float SEARCH_DISTANCE = 3000.0f;
	const float SEARCH_RANGE = D3DX_PI * 0.1f;
}

Clist<CGimmickPoliceStation*> CGimmickPoliceStation::m_List = {};

//==========================================================
// コンストラクタ
//==========================================================
CGimmickPoliceStation::CGimmickPoliceStation()
{
	// 値のクリア
	m_pObj = nullptr;
	m_Info = SInfo();
	m_pRoad = nullptr;
	m_List.Regist(this);
}

//==========================================================
// デストラクタ
//==========================================================
CGimmickPoliceStation::~CGimmickPoliceStation()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CGimmickPoliceStation::Init(void)
{
	m_pObj = CObjectX::Create(GetPos(), GetRot(), FILENAME);
	m_pObj->SetScale(VECTOR3_ONE);
	SetVtxMax(m_pObj->GetVtxMax());
	SetVtxMin(m_pObj->GetVtxMin());
	m_pObj->SetScale(GetScale());
	m_Info.fInterVal = INTERVAL;
	SetType(TYPE::TYPE_POLICESTATION);

	// 最も近い道を取得
	m_pRoad = CRoadManager::GetInstance()->GetNearRoad(GetPos());

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CGimmickPoliceStation::Uninit(void)
{
	// オブジェクト廃棄
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	m_pRoad = nullptr;
	m_List.Delete(this);

	CGimmick::Uninit();
}

//==========================================================
// 更新処理
//==========================================================
void CGimmickPoliceStation::Update(void)
{
	if (CManager::GetInstance()->GetMode() == CScene::MODE::MODE_TITLE) { return; }
	if (m_pObj == nullptr) { return; }

	// インターバルが終わっていない
	if (m_Info.fSpawnTime < m_Info.fInterVal) {
		m_Info.fSpawnTime += CDeltaTime::GetInstance()->GetDeltaTime();

		if (m_Info.fSpawnTime > m_Info.fInterVal)
		{
			m_Info.fSpawnTime = m_Info.fInterVal;
		}

		return;
	}

	if (CNetWork::GetInstance()->GetState() == CNetWork::STATE::STATE_SINGLE)
	{
		// プレイヤーを確認
		CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
		if (pPlayer == nullptr) { return; }
		if (pPlayer->GetEngine() <= OUT_ENGINE) { return; }	// セーフ

		// 距離判定
		D3DXVECTOR3 pos = GetPos() - pPlayer->GetPosition();
		float distance = D3DXVec3Length(&pos);
		CDebugProc::GetInstance()->Print("距離 [ %f ]", distance);

		if (distance > SEARCH_DISTANCE) { return; }

		// 向き判定
		float rot = atan2f(pos.x, pos.z);
		Adjust(rot);
		float dest = rot - GetRot().y;
		Adjust(dest);
		dest = fabsf(dest);
		CDebugProc::GetInstance()->Print("差分 [ %f ]", dest);

		// 範囲内のみ警察生成
		if (dest < -SEARCH_RANGE || dest > SEARCH_RANGE) { return; }
		m_Info.fSpawnTime = 0.0f;

		CAddPolice* pP = CAddPolice::Create(GetPos(), GetRot(), VECTOR3_ZERO, CCarManager::GetInstance()->GetMapList()->GetInCnt());

		if (pP != nullptr)
		{
			// 応援の警察のタイプを設定
			pP->SetTypeAI(CPoliceAI::TYPE_NONE);
			pP->SetType(CCar::TYPE::TYPE_ACTIVE);

			// 目的地設定
			pP->SetRoadTarget(CRoadManager::GetInstance()->GetNearRoad(GetPos()));

			// 追跡状態に変更
			pP->SetChase(true);
			pP->GetAi()->BeginChase(pPlayer);

			// 応援の警察は応援を呼ばないようにする
			pP->GetAi()->SetCall(true);
		}
	}
	else
	{
		for (int i = 0; i < NetWork::MAX_CONNECT; i++)
		{
			if (!CNetWork::GetInstance()->GetConnect(i)) { continue; }

			// プレイヤーを確認
			CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(i);
			if (pPlayer == nullptr) { return; }
			if (pPlayer->GetEngine() <= OUT_ENGINE) { return; }	// セーフ

			// 距離判定
			D3DXVECTOR3 pos = GetPos() - pPlayer->GetPosition();
			float distance = D3DXVec3Length(&pos);
			CDebugProc::GetInstance()->Print("距離 [ %f ]", distance);

			if (distance > SEARCH_DISTANCE) { return; }

			// 向き判定
			float rot = atan2f(pos.x, pos.z);
			Adjust(rot);
			float dest = rot - GetRot().y;
			Adjust(dest);
			dest = fabsf(dest);
			CDebugProc::GetInstance()->Print("差分 [ %f ]", dest);

			// 範囲内のみ警察生成
			if (dest < -SEARCH_RANGE || dest > SEARCH_RANGE) { return; }
			m_Info.fSpawnTime = 0.0f;

			if (i == CNetWork::GetInstance()->GetIdx())
			{

				//CAddPolice* pP = CAddPolice::Create(GetPos(), GetRot(), VECTOR3_ZERO, CCarManager::GetInstance()->GetMapList()->GetInCnt());

				//if (pP != nullptr)
				//{
				//	// 応援の警察のタイプを設定
				//	pP->SetTypeAI(CPoliceAI::TYPE_NONE);
				//	pP->SetType(CCar::TYPE::TYPE_ACTIVE);

				//	// 目的地設定
				//	pP->SetRoadTarget(CRoadManager::GetInstance()->GetNearRoad(GetPos()));

				//	// 追跡状態に変更
				//	pP->SetChase(true);
				//	pP->GetAi()->BeginChase(pPlayer);

				//	// 応援の警察は応援を呼ばないようにする
				//	pP->GetAi()->SetCall(true);
				//}
			}
		}
	}
}

//==========================================================
// 生成
//==========================================================
CGimmickPoliceStation* CGimmickPoliceStation::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale)
{
	CGimmickPoliceStation* pPoliceStation = nullptr;

	pPoliceStation = DEBUG_NEW CGimmickPoliceStation;

	if (pPoliceStation != nullptr)
	{
		pPoliceStation->SetPos(pos);
		pPoliceStation->SetRot(rot);
		pPoliceStation->SetScale(scale);

		// 初期化処理
		pPoliceStation->Init();
	}

	return pPoliceStation;
}

//==========================================================
// 色倍率変更
//==========================================================
void CGimmickPoliceStation::SetColMulti(const D3DXCOLOR& col)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetColMulti(col);
}

//==========================================================
// 座標反映
//==========================================================
void CGimmickPoliceStation::SetObjPos(const D3DXVECTOR3& pos)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetPosition(pos);
}

//==========================================================
// 向き反映
//==========================================================
void CGimmickPoliceStation::SetObjRot(const D3DXVECTOR3& rot)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetRotation(rot);
}

//==========================================================
// スケール反映
//==========================================================
void CGimmickPoliceStation::SetObjScale(const D3DXVECTOR3& scale)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetScale(scale);
}

//==========================================================
// 最も近い場所受け取る
//==========================================================
CGimmickPoliceStation* CGimmickPoliceStation::GetNear(const D3DXVECTOR3& pos)
{
	float length = 1000000.0f;
	CGimmickPoliceStation* pStation = nullptr;
	// 道数分繰り返す
	for (int i = 0; i < m_List.GetNum(); i++)
	{
		// 確認
		CGimmickPoliceStation* pCheck = m_List.Get(i);
		D3DXVECTOR3 vec = pCheck->GetPos() - pos;
		float temp = D3DXVec3Length(&vec);

		// 距離が近い
		if (temp <= length)
		{
			length = temp;
			pStation = pCheck;
		}
	}

	return pStation;
}