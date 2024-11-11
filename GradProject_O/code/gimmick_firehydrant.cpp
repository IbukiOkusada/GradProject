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

// 定数定義
namespace
{
	// ファイル名
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
	m_pObj->SetScale(GetScale());

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
		m_bHit = true;
		m_pObj->SetEnableCollision(false);
		D3DXVECTOR3 pos = CPlayerManager::GetInstance()->GetTop()->GetPosition();
		float rot = atan2f(GetPos().x - pos.x, GetPos().z - pos.z);

		// 座標設定
		m_TargetPos = {
			GetPos().x + sinf(rot) * 500.0f,
			GetPos().y,
			GetPos().z + cosf(rot) * 500.0f,
		};

		m_TargetRot = {
			0.0f,
			rot,
			D3DX_PI * 0.5f
		};
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
	Adjust(rotdiff);
	D3DXVECTOR3 setrot = m_pObj->GetRotation() + rotdiff * 0.1f;
	Adjust(setrot);
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
	if (pos.x < 0.0f || pos.x > SCREEN_WIDTH ||
		pos.y < 0.0f || pos.y > SCREEN_HEIGHT) {
		return;
	}

	D3DXVECTOR3 objpos = GetPos();
	CParticle3D::Create(objpos, CEffect3D::TYPE_SPLASH);
}
