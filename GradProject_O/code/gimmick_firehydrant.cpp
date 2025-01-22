//==========================================================
//
// 消火栓ギミック処理 [gimmick_firehydrant.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "manager.h"
#include "gimmick_firehydrant.h"
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
	const char* FILENAME = "data\\MODEL\\map\\fire_hydrant_00.x";
}

//==========================================================
// コンストラクタ
//==========================================================
CGimmickFireHydrant::CGimmickFireHydrant()
{
	// 値のクリア
	m_pObj = nullptr;
	m_bHit = false;
	m_TargetPos = VECTOR3_ZERO;
	m_TargetRot = VECTOR3_ZERO;
	m_pEffect = nullptr;
}

//==========================================================
// デストラクタ
//==========================================================
CGimmickFireHydrant::~CGimmickFireHydrant()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CGimmickFireHydrant::Init(void)
{
	m_pObj = CObjectX::Create(GetPos(), GetRot(), FILENAME);
	m_pObj->SetScale(VECTOR3_ONE);
	SetVtxMax(m_pObj->GetVtxMax());
	SetVtxMin(m_pObj->GetVtxMin());
	m_pObj->SetScale(GetScale());
	SetType(TYPE::TYPE_FIREHYDRANT);

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CGimmickFireHydrant::Uninit(void)
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
void CGimmickFireHydrant::Update(void)
{
	if (m_pObj == nullptr) { return; }

	// 衝突した
	if (m_pObj->GetHit() || m_pObj->GetHitOld())
	{
		D3DXVECTOR3 pos = CPlayerManager::GetInstance()->GetPlayer()->GetPosition();
		float speed = CPlayerManager::GetInstance()->GetPlayer()->GetEngine();
		Hit(pos, speed);

		// ネットワークでの衝突送信
		CNetWork::GetInstance()->SendGmHit(GetId(), pos, speed);
	}

	// 吹っ飛び
	Away();

	// エフェクト設定
	SetEffect();
}

//==========================================================
// 生成
//==========================================================
CGimmickFireHydrant* CGimmickFireHydrant::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale)
{
	CGimmickFireHydrant* pFireHydrant = nullptr;

	pFireHydrant = DEBUG_NEW CGimmickFireHydrant;

	if (pFireHydrant != nullptr)
	{
		pFireHydrant->SetPos(pos);
		pFireHydrant->SetRot(rot);
		pFireHydrant->SetScale(scale);

		// 初期化処理
		pFireHydrant->Init();
	}

	return pFireHydrant;
}

//==========================================================
// 吹っ飛び
//==========================================================
void CGimmickFireHydrant::Away()
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
	correction::Adjust(rotdiff);
	D3DXVECTOR3 setrot = m_pObj->GetRotation() + rotdiff * 0.1f;
	correction::Adjust(setrot);
	m_pObj->SetRotation(setrot);
}

//==========================================================
// エフェクト生成
//==========================================================
void CGimmickFireHydrant::SetEffect()
{
	// 消火栓が移動した
	if (m_pObj == nullptr) { return; }
	if (!m_bHit) { return; }

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();
	D3DXMATRIX mtxProjection, mtxView, mtxWorld;
	D3DVIEWPORT9 Viewport;
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	D3DXVECTOR3 mypos = GetPos();

	// 必要な情報取得
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetViewport(&Viewport);

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// スクリーン座標取得
	D3DXVec3Project(&pos, &mypos, &Viewport, &mtxProjection, &mtxView, &mtxWorld);

	// 画面外なら出さない
	if (pos.x < 0.0f - RANGE_WIDTH || pos.x > SCREEN_WIDTH + RANGE_WIDTH ||
		pos.y < 0.0f - RANGE_HEIGHT || pos.y > SCREEN_HEIGHT + RANGE_HEIGHT) {
		SAFE_DELETE(m_pEffect);
		return;
	}

	D3DXVECTOR3 objpos = GetPos();
	
	if (m_pEffect == nullptr)
	{
		m_pEffect = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\splash.efkefc", 
			GetPos(), VECTOR3_ZERO, VECTOR3_ZERO, 120.0f, true, false);

	}
}

//==========================================================
// 色倍率変更
//==========================================================
void CGimmickFireHydrant::SetColMulti(const D3DXCOLOR& col)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetColMulti(col);
}

//==========================================================
// 座標反映
//==========================================================
void CGimmickFireHydrant::SetObjPos(const D3DXVECTOR3& pos)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetPosition(pos);
}

//==========================================================
// 向き反映
//==========================================================
void CGimmickFireHydrant::SetObjRot(const D3DXVECTOR3& rot)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetRotation(rot);
}

//==========================================================
// スケール反映
//==========================================================
void CGimmickFireHydrant::SetObjScale(const D3DXVECTOR3& scale)
{
	if (m_pObj == nullptr) { return; }

	m_pObj->SetScale(scale);
}

//==========================================================
// 吹っ飛び
//==========================================================
void CGimmickFireHydrant::Hit(const D3DXVECTOR3& HitPos, const float fSpeed)
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
		0.0f,
		rot,
		D3DX_PI * 0.5f
	};

	if (m_pObj->GetHit() || m_pObj->GetHitOld()) { return; }
	//CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\spark.efkefc", GetPos(), VECTOR3_ZERO, VECTOR3_ZERO, 300.0f);
}