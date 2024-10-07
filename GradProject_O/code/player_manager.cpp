//==========================================================
//
// 敵マネージャー [Player_manager.cpp]
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
	m_pCur = nullptr;
	m_pTop = nullptr;
	m_nNum = 0;
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
		m_pInstance = new CPlayerManager;
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
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// リストに挿入
//==========================================================
void CPlayerManager::ListIn(CPlayer* pPlayer)
{
	if (m_pTop != nullptr)
	{// 先頭が存在している場合
		m_pCur->SetNext(pPlayer);	// 現在最後尾のオブジェクトのポインタにつなげる
		pPlayer->SetPrev(m_pCur);
		m_pCur = pPlayer;	// 自分自身が最後尾になる
	}
	else
	{// 存在しない場合
		m_pTop = pPlayer;	// 自分自身が先頭になる
		m_pCur = pPlayer;	// 自分自身が最後尾になる
	}

	m_nNum++;
}

//==========================================================
// リストから外す
//==========================================================
void CPlayerManager::ListOut(CPlayer* pPlayer)
{
	// リストから自分自身を削除する
	if (m_pTop == pPlayer)
	{// 自身が先頭
		if (pPlayer->GetNext() != nullptr)
		{// 次が存在している
			m_pTop = pPlayer->GetNext();	// 次を先頭にする
			m_pTop->SetPrev(nullptr);	// 次の前のポインタを覚えていないようにする
		}
		else
		{// 存在していない
			m_pTop = nullptr;	// 先頭がない状態にする
			m_pCur = nullptr;	// 最後尾がない状態にする
		}
	}
	else if (m_pCur == pPlayer)
	{// 自身が最後尾
		if (pPlayer->GetPrev() != nullptr)
		{// 次が存在している
			m_pCur = pPlayer->GetPrev();		// 前を最後尾にする
			m_pCur->SetNext(nullptr);			// 前の次のポインタを覚えていないようにする
		}
		else
		{// 存在していない
			m_pTop = nullptr;	// 先頭がない状態にする
			m_pCur = nullptr;	// 最後尾がない状態にする
		}
	}
	else
	{
		if (pPlayer->GetNext() != nullptr)
		{
			pPlayer->GetNext()->SetPrev(pPlayer->GetPrev());	// 自身の次に前のポインタを覚えさせる
		}
		if (pPlayer->GetPrev() != nullptr)
		{
			pPlayer->GetPrev()->SetNext(pPlayer->GetNext());	// 自身の前に次のポインタを覚えさせる
		}
	}

	m_nNum--;
}

//==========================================================
// 攻撃ヒット確認
//==========================================================
bool CPlayerManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	CPlayer* pPlayer = m_pTop;
	bool bUse = false;

	//個別判定
	while (pPlayer != nullptr) {
		CPlayer* pPlayerNext = pPlayer->GetNext();
		//if (pPlayer->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
		pPlayer = pPlayerNext;
	}

	return bUse;
}