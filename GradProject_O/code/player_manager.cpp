//==========================================================
//
// プレイヤーマネージャー [Player_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "player_manager.h"
#include "player.h"

// 静的メンバ変数宣言
CPlayerManager* CPlayerManager::m_pInstance = nullptr;	// インスタンス

//==========================================================
// コンストラクタ
//==========================================================
CPlayerManager::CPlayerManager()
{
	// 値のクリア
	m_List.clear();
}

//==========================================================
// デストラクタ
//==========================================================
CPlayerManager::~CPlayerManager()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CPlayerManager::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CPlayerManager::Uninit(void)
{
	// リストマネージャー廃棄
	CListManager::Uninit();

	// インスタンス廃棄
	if (m_pInstance != nullptr) {
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// 更新処理
//==========================================================
void CPlayerManager::Update(void)
{

}

//==========================================================
// インスタンスを確保
//==========================================================
CPlayerManager* CPlayerManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// 使われていない
		m_pInstance = DEBUG_NEW CPlayerManager;
	}

	return m_pInstance;
}

//==========================================================
// インスタンスをリリース
//==========================================================
void CPlayerManager::Release(void)
{
	if (m_pInstance != nullptr) {	// インスタンスを確保されている
		m_pInstance->Uninit();
	}
}

//==========================================================
// リストに挿入
//==========================================================
bool CPlayerManager::ListIn(CPlayer* pPlayer)
{
	if (pPlayer == nullptr) { return false; }

	auto it = m_List.find(pPlayer->GetId());

	// 存在しない
	if (it == m_List.end())
	{
		m_List[pPlayer->GetId()] = pPlayer;
		return true;
	}

	return false;
}

//==========================================================
// リストから外す
//==========================================================
bool CPlayerManager::ListOut(CPlayer* pPlayer)
{
	if (pPlayer == nullptr) { return false; }

	auto it = m_List.find(pPlayer->GetId());

	// 見つかった
	if (it != m_List.end())
	{
		if (it->second == pPlayer)
		{
			m_List.erase(pPlayer->GetId());
			return true;
		}
	}

	return false;
}

//==========================================================
// 指定されたIDのプレイヤー取得
//==========================================================
CPlayer* CPlayerManager::GetPlayer(int nIdx)
{
	auto it = m_List.find(nIdx);

	// 存在している
	if (it != m_List.end())
	{
		CPlayer* pPlayer = it->second;
		return pPlayer;
	}

	return nullptr;
}

//==========================================================
// 攻撃ヒット確認
//==========================================================
bool CPlayerManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	bool bUse = false;

	return bUse;
}