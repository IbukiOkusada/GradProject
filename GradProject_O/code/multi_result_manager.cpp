//==========================================================
//
// マルチリザルトマネージャー [multi_result_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "multi_result_manager.h"
#include "player_manager.h"
#include "player.h"

// 静的メンバ変数宣言
CMultiResultManager* CMultiResultManager::m_pInstance = nullptr;	// インスタンス

//==========================================================
// コンストラクタ
//==========================================================
CMultiResultManager::CMultiResultManager()
{
	// 値のクリア
	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		m_aInfo[i] = SInfo();
		m_aInfo[i].bActive = false;
		m_aInfo[i].nId = i;
		m_aInfo[i].nNumDelv = 0;
	}

	m_nNumPlayer = 0;
	m_nMyId = -1;
}

//==========================================================
// デストラクタ
//==========================================================
CMultiResultManager::~CMultiResultManager()
{

}

//==========================================================
// インスタンスを確保
//==========================================================
CMultiResultManager* CMultiResultManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// 使われていない
		m_pInstance = DEBUG_NEW CMultiResultManager;
	}

	return m_pInstance;
}

//==========================================================
// インスタンスをリリース
//==========================================================
void CMultiResultManager::Release(void)
{
	if (m_pInstance != nullptr) {	// インスタンスを確保されている
		m_pInstance->Uninit();
	}
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CMultiResultManager::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CMultiResultManager::Uninit(void)
{
	// 終了処理
	CListManager::Uninit();

	// インスタンスの廃棄
	if (m_pInstance != nullptr) {	// インスタンスを確保されている
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// 更新処理
//==========================================================
void CMultiResultManager::Update(void)
{

}

//===============================================
// データ設定
//===============================================
void CMultiResultManager::DataSet(void)
{
	// ネットワーク取得
	CNetWork* pNet = CNetWork::GetInstance();
	if (pNet == nullptr) { return; }

	// 自身のID保存
	m_nMyId = pNet->GetIdx();

	// プレイヤーマネージャーの情報取得
	CPlayerManager* pMgr = CPlayerManager::GetInstance();
	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		// ポインタに変換
		SInfo* pInfo = &m_aInfo[i];

		// プレイヤーを取得
		CPlayer* pPlayer = pMgr->GetPlayer(i);

		// 使用されていたら配達数保存
		if (pNet->GetConnect(i) && pPlayer != nullptr)
		{
			pInfo->bActive = true;
			pInfo->nNumDelv = pPlayer->GetNumDeliverStatus();
			pInfo->nId = pPlayer->GetId();
			m_nNumPlayer++;
		}
		else	// 使用されていない
		{
			pInfo->bActive = false;
		}
	}
}