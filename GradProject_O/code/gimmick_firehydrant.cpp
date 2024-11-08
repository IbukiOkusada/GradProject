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
// エフェクト生成
//==========================================================
void CGimmickFireHydrant::SetEffect()
{
	// 消火栓が移動した
	if (m_pObj == nullptr) { return; }
	if (m_bHit) { return; }

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

	CParticle3D::Create(GetPos(), CEffect3D::TYPE_SMAKE);
}
