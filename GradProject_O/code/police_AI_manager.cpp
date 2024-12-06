//==========================================================
//
// 警察AIマネージャー [police_AI_manager.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "police_AI_manager.h"
#include "police.h"
#include "police_AI.h"
#include "player_manager.h"
#include "deltatime.h"
#include "debugproc.h"
#include "car_manager.h"

// 名前空間
namespace
{
	const float INTERVAL = 5.0f;	// インターバル
	const int MAX_POLICE = (15);	// 警察の最大値
}

// 静的メンバ変数宣言
CPoliceAIManager* CPoliceAIManager::m_pInstance = nullptr;	// インスタンス

//==========================================================
// コンストラクタ
//==========================================================
CPoliceAIManager::CPoliceAIManager()
{
	// 値のクリア
	m_maplist.Clear();
}

//==========================================================
// デストラクタ
//==========================================================
CPoliceAIManager::~CPoliceAIManager()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CPoliceAIManager::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CPoliceAIManager::Uninit(void)
{
	// 親クラスの終了処理
	CListManager::Uninit();

	// インスタンスの廃棄
	if (m_pInstance != nullptr)
	{	// インスタンスを確保されている
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// 更新処理
//==========================================================
void CPoliceAIManager::Update(void)
{
	// マルチスレッド
	std::thread th(&CPoliceAIManager::Search, this);
	th.detach();
}

//==========================================================
// インスタンスを確保
//==========================================================
CPoliceAIManager* CPoliceAIManager::GetInstance(void)
{
	if (m_pInstance == nullptr)
	{	// 使われていない
		m_pInstance = DEBUG_NEW CPoliceAIManager;
		m_pInstance->Init();
	}

	return m_pInstance;
}

//==========================================================
// インスタンスをリリース
//==========================================================
void CPoliceAIManager::Release(void)
{
	if (m_pInstance != nullptr)
	{	// インスタンスを確保されている
		m_pInstance->Uninit();
	}
}

//==========================================================
// リストに挿入
//==========================================================
void CPoliceAIManager::ListIn(CPoliceAI* pPoliceAI)
{
	m_maplist.Regist(pPoliceAI->GetPolice()->GetId(), pPoliceAI);
}

//==========================================================
// リストから外す
//==========================================================
void CPoliceAIManager::ListOut(CPoliceAI* pPoliceAI)
{
	// リストから自分自身を削除する
	m_maplist.Delete(pPoliceAI->GetPolice()->GetId(), pPoliceAI);
}

//==========================================================
// AIの経路探索処理
//==========================================================
void  CPoliceAIManager::Search(void)
{
	for (const auto& pair : *m_maplist.GetList())
	{
		// プレイヤー捜索
		pair.second->Search();

		// プレイヤーへの経路探索
		pair.second->ChaseAStar();
	}
}