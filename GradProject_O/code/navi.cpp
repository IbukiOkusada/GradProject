//==========================================================
//
// ナビ [navi.cpp]
// Author : 丹野竜之介
//
//==========================================================
#include "navi.h"
#include "player.h"
#include "player_manager.h"
#include "manager.h"
#include "object.h"
#include "object_manager.h"
// マクロ定義
namespace
{
 	const D3DVIEWPORT9 VIEWPORT = (D3DVIEWPORT9{ (DWORD)(SCREEN_WIDTH * 0.7f), (DWORD)(SCREEN_WIDTH * 0.7f), 100,100,1.0,1000});
}
//==========================================================
// コンストラクタ
//==========================================================
CNavi::CNavi()
{
	// 値のクリア
}

//==========================================================
// デストラクタ
//==========================================================
CNavi::~CNavi()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CNavi::Init(void)
{
	m_pCamera = DEBUG_NEW CMultiCamera;
	m_pCamera->Init();
	m_pCamera->SetViewPort(VIEWPORT);
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CNavi::Uninit(void)
{
	SAFE_UNINIT_DELETE(m_pCamera);
	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CNavi::Update(void)
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	D3DXVECTOR3 pos = pPlayer->GetPosition();
	m_pCamera->SetPositionR(pos);
	pos.y += 1000.0f;
	m_pCamera->SetPositionV(pos);

}

//==========================================================
// 生成
//==========================================================
CNavi* CNavi::Create(void)
{
	CNavi* pNavi = nullptr;

	pNavi = DEBUG_NEW CNavi;

	if (pNavi != nullptr)
	{
		// 初期化処理
		pNavi->Init();
	}

	return pNavi;
}
