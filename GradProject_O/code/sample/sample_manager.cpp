//==========================================================
//
// サンプルマネージャー [sample_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "sample_manager.h"
#include "sample.h"

// 静的メンバ変数宣言
CSampleManager* CSampleManager::m_pInstance = nullptr;	// インスタンス

//==========================================================
// コンストラクタ
//==========================================================
CSampleManager::CSampleManager()
{
	// 値のクリア
	m_pCur = nullptr;
	m_pTop = nullptr;
	m_nNum = 0;
}

//==========================================================
// デストラクタ
//==========================================================
CSampleManager::~CSampleManager()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CSampleManager::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CSampleManager::Uninit(void)
{

}

//==========================================================
// 更新処理
//==========================================================
void CSampleManager::Update(void)
{

}

//==========================================================
// インスタンスを確保
//==========================================================
CSampleManager* CSampleManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// 使われていない
		m_pInstance = DEBUG_NEW CSampleManager;
	}

	return m_pInstance;
}

//==========================================================
// インスタンスをリリース
//==========================================================
void CSampleManager::Release(void)
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
void CSampleManager::ListIn(CSample* pSample)
{
	if (m_pTop != nullptr)
	{// 先頭が存在している場合
		m_pCur->SetNext(pSample);	// 現在最後尾のオブジェクトのポインタにつなげる
		pSample->SetPrev(m_pCur);
		m_pCur = pSample;	// 自分自身が最後尾になる
	}
	else
	{// 存在しない場合
		m_pTop = pSample;	// 自分自身が先頭になる
		m_pCur = pSample;	// 自分自身が最後尾になる
	}

	m_nNum++;
}

//==========================================================
// リストから外す
//==========================================================
void CSampleManager::ListOut(CSample* pSample)
{
	// リストから自分自身を削除する
	if (m_pTop == pSample)
	{// 自身が先頭
		if (pSample->GetNext() != nullptr)
		{// 次が存在している
			m_pTop = pSample->GetNext();	// 次を先頭にする
			m_pTop->SetPrev(nullptr);	// 次の前のポインタを覚えていないようにする
		}
		else
		{// 存在していない
			m_pTop = nullptr;	// 先頭がない状態にする
			m_pCur = nullptr;	// 最後尾がない状態にする
		}
	}
	else if (m_pCur == pSample)
	{// 自身が最後尾
		if (pSample->GetPrev() != nullptr)
		{// 次が存在している
			m_pCur = pSample->GetPrev();		// 前を最後尾にする
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
		if (pSample->GetNext() != nullptr)
		{
			pSample->GetNext()->SetPrev(pSample->GetPrev());	// 自身の次に前のポインタを覚えさせる
		}
		if (pSample->GetPrev() != nullptr)
		{
			pSample->GetPrev()->SetNext(pSample->GetNext());	// 自身の前に次のポインタを覚えさせる
		}
	}

	m_nNum--;
}
