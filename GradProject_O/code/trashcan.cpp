//==========================================================
//
// ゴミ箱ギミック処理 [trashcan.cpp]
// Author : Kenta Hashimoto
//
//==========================================================
#include "manager.h"
#include "trashcan.h"
#include "deltatime.h"
#include "objectX.h"
#include "camera.h"
#include "camera_manager.h"
#include "renderer.h"
#include "particle3D.h"
#include "player.h"
#include "player_manager.h"
#include "network.h"
#include "effekseerControl.h"

// 定数定義
namespace
{
	// ファイル名
	const float RANGE_WIDTH = SCREEN_WIDTH * 0.1f;
	const float RANGE_HEIGHT = SCREEN_HEIGHT * 0.1f;
	const char* FILENAME = "data\\MODEL\\map\\sign_speed_limit.x";
}

//==========================================================
// コンストラクタ
//==========================================================
CGimmickTrashCan::CGimmickTrashCan()
{
	// 値のクリア
	m_pObj = nullptr;
	m_bHit = false;
	m_TargetPos = VECTOR3_ZERO;
	m_TargetRot = VECTOR3_ZERO;
}

//==========================================================
// デストラクタ
//==========================================================
CGimmickTrashCan::~CGimmickTrashCan()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CGimmickTrashCan::Init(void)
{
	m_pObj = CObjectX::Create(GetPos(), GetRot(), FILENAME);
	m_pObj->SetScale(VECTOR3_ONE);
	SetVtxMax(m_pObj->GetVtxMax());
	SetVtxMin(m_pObj->GetVtxMin());
	m_pObj->SetScale(GetScale());
	SetType(TYPE::TYPE_TEASHCAN);

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CGimmickTrashCan::Uninit(void)
{
	// オブジェクト廃棄
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	CGimmick::Uninit();
}

//==========================================================
// 更新処理
//==========================================================
void CGimmickTrashCan::Update(void)
{
	if (m_pObj == nullptr) { return; }

	// 衝突した
	if (!m_bHit)
	{
		if (m_pObj->GetHit() || m_pObj->GetHitOld())
		{
			CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
			D3DXVECTOR3 pos = pPlayer->GetPosition();
			float speed = CPlayerManager::GetInstance()->GetPlayer()->GetEngine();

			Hit(pos, speed);

			// ネットワークでの衝突送信
			CNetWork::GetInstance()->SendGmHit(GetId(), pos, speed);
		}
	}

	// 吹っ飛び
	Away();
}

//==========================================================
// 生成
//==========================================================
CGimmickTrashCan* CGimmickTrashCan::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale)
{
	CGimmickTrashCan* pGuardRail = nullptr;

	pGuardRail = DEBUG_NEW CGimmickTrashCan;

	if (pGuardRail != nullptr)
	{
		pGuardRail->SetPos(pos);
		pGuardRail->SetRot(rot);
		pGuardRail->SetScale(scale);

		// 初期化処理
		pGuardRail->Init();
	}

	return pGuardRail;
}

//==========================================================
// 吹っ飛び
//==========================================================
void CGimmickTrashCan::Away()
{
	// 消火栓が移動した
	if (m_pObj == nullptr) { return; }
	if (!m_bHit) { return; }

	// 位置更新
	D3DXVECTOR3 posdiff = m_TargetPos - m_pObj->GetPosition();
	D3DXVECTOR3 setpos = m_pObj->GetPosition() + posdiff * 0.1f;
	m_pObj->SetPosition(setpos);

	// 向き更新
	D3DXVECTOR3 rotdiff = m_TargetRot - m_pObj->GetRotation();
	Adjust(rotdiff);
	D3DXVECTOR3 setrot = m_pObj->GetRotation() + rotdiff * 0.1f;
	Adjust(setrot);
	m_pObj->SetRotation(setrot);
}

//==========================================================
// 衝突
//==========================================================
void CGimmickTrashCan::Hit(const D3DXVECTOR3& HitPos, const float fSpeed)
{
	if (m_bHit) { return; }

	m_bHit = true;
	m_pObj->SetEnableCollision(false);
	float rot = atan2f(GetPos().x - HitPos.x, GetPos().z - HitPos.z);

	// 座標設定
	m_TargetPos = {
		GetPos().x + sinf(rot) * fSpeed * 500.0f,
		GetPos().y,
		GetPos().z + cosf(rot) * fSpeed * 500.0f,
	};

	m_TargetRot = {
		D3DX_PI * 0.5f,
		rot,
		0.0f
	};

	if (m_pObj->GetHit() || m_pObj->GetHitOld()) { return; }
	//CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\spark.efkefc", GetPos(), VECTOR3_ZERO, VECTOR3_ZERO, 300.0f);
}

//==========================================================
// 色倍率変更
//==========================================================
void CGimmickTrashCan::SetColMulti(const D3DXCOLOR& col)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetColMulti(col);
}

//==========================================================
// 座標反映
//==========================================================
void CGimmickTrashCan::SetObjPos(const D3DXVECTOR3& pos)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetPosition(pos);
}

//==========================================================
// 向き反映
//==========================================================
void CGimmickTrashCan::SetObjRot(const D3DXVECTOR3& rot)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetRotation(rot);
}

//==========================================================
// スケール反映
//==========================================================
void CGimmickTrashCan::SetObjScale(const D3DXVECTOR3& scale)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetScale(scale);
}