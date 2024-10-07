//==========================================================
//
// カメラマネージャー [camera_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "camera_manager.h"
#include "camera.h"

// 静的メンバ変数宣言
CCameraManager *CCameraManager::m_pInstance = nullptr;	// インスタンス

//==========================================================
// コンストラクタ
//==========================================================
CCameraManager::CCameraManager()
{
	// 値のクリア
	m_pCur = nullptr;
	m_pTop = nullptr;
	m_nNum = 0;
}

//==========================================================
// デストラクタ
//==========================================================
CCameraManager::~CCameraManager()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CCameraManager::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CCameraManager::Uninit(void)
{

}

//==========================================================
// 更新処理
//==========================================================
void CCameraManager::Update(void)
{

}

//==========================================================
// インスタンスを確保
//==========================================================
CCameraManager *CCameraManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// 使われていない
		m_pInstance = new CCameraManager;
	}

	return m_pInstance;
}

//==========================================================
// インスタンスをリリース
//==========================================================
void CCameraManager::Release(void)
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
void CCameraManager::ListIn(CCamera *pCamera)
{
	if (m_pTop != nullptr)
	{// 先頭が存在している場合
		m_pCur->SetNext(pCamera);	// 現在最後尾のオブジェクトのポインタにつなげる
		pCamera->SetPrev(m_pCur);
		m_pCur = pCamera;	// 自分自身が最後尾になる
	}
	else
	{// 存在しない場合
		m_pTop = pCamera;	// 自分自身が先頭になる
		m_pCur = pCamera;	// 自分自身が最後尾になる
	}

	m_nNum++;
}

//==========================================================
// リストから外す
//==========================================================
void CCameraManager::ListOut(CCamera *pCamera)
{
	// リストから自分自身を削除する
	if (m_pTop == pCamera)
	{// 自身が先頭
		if (pCamera->GetNext() != nullptr)
		{// 次が存在している
			m_pTop = pCamera->GetNext();	// 次を先頭にする
			m_pTop->SetPrev(nullptr);	// 次の前のポインタを覚えていないようにする
		}
		else
		{// 存在していない
			m_pTop = nullptr;	// 先頭がない状態にする
			m_pCur = nullptr;	// 最後尾がない状態にする
		}
	}
	else if (m_pCur == pCamera)
	{// 自身が最後尾
		if (pCamera->GetPrev() != nullptr)
		{// 次が存在している
			m_pCur = pCamera->GetPrev();		// 前を最後尾にする
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
		if (pCamera->GetNext() != nullptr)
		{
			pCamera->GetNext()->SetPrev(pCamera->GetPrev());	// 自身の次に前のポインタを覚えさせる
		}
		if (pCamera->GetPrev() != nullptr)
		{
			pCamera->GetPrev()->SetNext(pCamera->GetNext());	// 自身の前に次のポインタを覚えさせる
		}
	}

	m_nNum--;
}