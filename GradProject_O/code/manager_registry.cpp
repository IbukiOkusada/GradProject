//==========================================================
//
// サンプルマネージャー [sample_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "manager_registry.h"
#include "list_manager.h"

// 静的メンバ変数宣言
CManagerRegistry* CManagerRegistry::m_pInstance = nullptr;	// インスタンス

//==========================================================
// コンストラクタ
//==========================================================
CManagerRegistry::CManagerRegistry()
{
	// 値のクリア
	m_pCur = nullptr;
	m_pTop = nullptr;
	m_nNum = 0;
}

//==========================================================
// デストラクタ
//==========================================================
CManagerRegistry::~CManagerRegistry()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CManagerRegistry::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CManagerRegistry::Uninit(void)
{
	CListManager* pManager = m_pTop;

	while (pManager != nullptr)
	{
		CListManager* pNext = pManager->GetNext();
		pManager->Uninit();
		pManager = pNext;
	}
}

//==========================================================
// 更新処理
//==========================================================
void CManagerRegistry::Update(void)
{

}

//==========================================================
// インスタンスを確保
//==========================================================
CManagerRegistry* CManagerRegistry::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// 使われていない
		m_pInstance = DEBUG_NEW CManagerRegistry;
	}

	return m_pInstance;
}

//==========================================================
// インスタンスをリリース
//==========================================================
void CManagerRegistry::Release(void)
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
void CManagerRegistry::ListIn(CListManager* pListManager)
{
	if (m_pTop != nullptr)
	{// 先頭が存在している場合
		m_pCur->SetNext(pListManager);	// 現在最後尾のオブジェクトのポインタにつなげる
		pListManager->SetPrev(m_pCur);
		m_pCur = pListManager;	// 自分自身が最後尾になる
	}
	else
	{// 存在しない場合
		m_pTop = pListManager;	// 自分自身が先頭になる
		m_pCur = pListManager;	// 自分自身が最後尾になる
	}

	m_nNum++;
}

//==========================================================
// リストから外す
//==========================================================
void CManagerRegistry::ListOut(CListManager* pListManager)
{
	// リストから自分自身を削除する
	if (m_pTop == pListManager)
	{// 自身が先頭
		if (pListManager->GetNext() != nullptr)
		{// 次が存在している
			m_pTop = pListManager->GetNext();	// 次を先頭にする
			m_pTop->SetPrev(nullptr);	// 次の前のポインタを覚えていないようにする
		}
		else
		{// 存在していない
			m_pTop = nullptr;	// 先頭がない状態にする
			m_pCur = nullptr;	// 最後尾がない状態にする
		}
	}
	else if (m_pCur == pListManager)
	{// 自身が最後尾
		if (pListManager->GetPrev() != nullptr)
		{// 次が存在している
			m_pCur = pListManager->GetPrev();		// 前を最後尾にする
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
		if (pListManager->GetNext() != nullptr)
		{
			pListManager->GetNext()->SetPrev(pListManager->GetPrev());	// 自身の次に前のポインタを覚えさせる
		}
		if (pListManager->GetPrev() != nullptr)
		{
			pListManager->GetPrev()->SetNext(pListManager->GetNext());	// 自身の前に次のポインタを覚えさせる
		}
	}

	m_nNum--;
}
