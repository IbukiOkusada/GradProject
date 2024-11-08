//==========================================================
//
// 消火栓ギミック処理 [gimmick_firehydrant.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "manager.h"
#include "gimmick_firehydrant.h"
#include "deltatime.h"
// マクロ定義

//==========================================================
// コンストラクタ
//==========================================================
CFireHydrant::CFireHydrant()
{
	// 値のクリア
	m_pObj = nullptr;
}

//==========================================================
// デストラクタ
//==========================================================
CFireHydrant::~CFireHydrant()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CFireHydrant::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CFireHydrant::Uninit(void)
{
	CGimmick::Uninit();
}

//==========================================================
// 更新処理
//==========================================================
void CFireHydrant::Update(void)
{
	
}

//==========================================================
// 生成
//==========================================================
CFireHydrant* CFireHydrant::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale)
{
	CFireHydrant* pFireHydrant = nullptr;

	pFireHydrant = DEBUG_NEW CFireHydrant;

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
