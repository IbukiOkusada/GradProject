//===============================================
//
// マルチリザルト画面の管理処理 [multi_result.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "multi_result.h"
#include "multi_result_manager.h"
#include "map_manager.h"
#include "manager.h"
#include "sound.h"
#include "camera.h"

//==========================================================
// 定数定義
//==========================================================

//===============================================
// コンストラクタ
//===============================================
CMultiResult::CMultiResult()
{
	m_pMgr = nullptr;
}

//===============================================
// デストラクタ
//===============================================
CMultiResult::~CMultiResult()
{

}

//===============================================
// 初期化処理
//===============================================
HRESULT CMultiResult::Init(void)
{
	// マップ読み込み
	CMapManager::GetInstance()->Load();

	// マネージャー取得
	m_pMgr = CMultiResultManager::GetInstance();

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CMultiResult::Uninit(void)
{
	// マネージャー廃棄
	if(m_pMgr != nullptr)
	{
		m_pMgr->Release();
		m_pMgr = nullptr;
	}

	// マップ情報廃棄
	CMapManager::Release();
	CManager::GetInstance()->GetCamera()->SetActive(true);
	CManager::GetInstance()->GetSound()->Stop();
}

//===============================================
// 更新処理
//===============================================
void CMultiResult::Update(void)
{
	CScene::Update();
}

//===============================================
// 描画処理
//===============================================
void CMultiResult::Draw(void)
{
	CScene::Draw();
}
