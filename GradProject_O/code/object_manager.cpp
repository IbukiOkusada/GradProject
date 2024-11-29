//==========================================================
//
// オブジェクト管理の処理 [object_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "object_manager.h"
#include "camera_manager.h"
#include "camera.h"
#include "object.h"
#include "manager.h"
#include <assert.h>
#include "debugproc.h"

CObjectManager *CObjectManager::m_pInstance = nullptr;

//==========================================================
// コンストラクタ
//==========================================================
CObjectManager::CObjectManager()
{
	// 値のクリア
	for (int nCntPri = 0; nCntPri < NUM_PRIORITY; nCntPri++)
	{
		m_apTop[nCntPri] = nullptr;	// 先頭がない状態にする
		m_apCur[nCntPri] = nullptr;	// 最後尾がない状態にする
		m_aPriNumAll[nCntPri] = 0;
	}

	m_nNumAll = 0;
}

//==========================================================
// デストラクタ
//==========================================================
CObjectManager::~CObjectManager()
{
	for (int nCntPri = 0; nCntPri < NUM_PRIORITY; nCntPri++)
	{
		if (m_apTop[nCntPri] != nullptr) {
			assert(false);
		}
		if (m_apCur[nCntPri] != nullptr) {
			assert(false);
		}
		if (m_aPriNumAll[nCntPri] > 0) {
			assert(false);
		}
		
	}
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CObjectManager::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CObjectManager::Uninit(void)
{
	// 親クラスの終了処理
	CListManager::Uninit();

	// リストの全廃棄
	ReleaseAll();

	// インスタンスの廃棄
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// 描画処理
//==========================================================
void CObjectManager::Draw(void)
{
	CCamera *pCamera = CCameraManager::GetInstance()->GetTop();
	CDebugProc::GetInstance()->Print("オブジェクト数 [ %d ]\n", m_nNumAll);

	while (pCamera != nullptr) {

		CCamera *pCameraNext = pCamera->GetNext();

		// 設定
		pCamera->SetCamera();

		if (pCamera->GetDraw()) {	// 描画する場合

			if (pCamera->GetDrawState() == CCamera::DRAWSTATE::PLAYER_ONLY)
			{
				// プレイヤーだけ描画
				DrawOnlyPlayer();
			}
			else
			{
				// リストの全描画
				DrawAll();
			}
			
			//DrawOneDimension(TYPE_3D);
		}

		pCamera = pCameraNext;
	}
}

//===============================================
// 全てのタスクの廃棄
//===============================================
void CObjectManager::ReleaseAll(void)
{
	for (int nCntPri = 0; nCntPri < NUM_PRIORITY; nCntPri++)
	{
		CObject *pObject = m_apTop[nCntPri];	// 先頭を取得

		while (pObject != nullptr)
		{// 使用されていない状態まで

			CObject *pObjectNext = pObject->GetNext();	// 次のオブジェクトへのポインタを取得

			// 終了処理
			pObject->Uninit();

			pObject = pObjectNext;	// 次のオブジェクトに移動
		}
	}

	// 死亡フラグをチェック
	DeathCheck();
}

//===============================================
// 全てのオブジェクトの描画
//===============================================
void CObjectManager::DrawAll(void)
{
	// 死亡フラグをチェック
	DeathCheck();

	for (int nCntPri = 0; nCntPri < NUM_PRIORITY; nCntPri++)
	{
		CObject *pObject = m_apTop[nCntPri];	// 先頭を取得

		while (pObject != nullptr)
		{// 使用されていない状態まで

			CObject *pObjectNext = pObject->GetNext();	// 次のオブジェクトへのポインタを取得

			// 描画処理
			if (pObject->GetDraw())
			{
				pObject->Draw();
			}

			pObject = pObjectNext;	// 次のオブジェクトに移動
		}
	}
}

//===============================================
// プレイヤーのみ描画
//===============================================
void CObjectManager::DrawOnlyPlayer(void)
{
	// 死亡フラグをチェック
	DeathCheck();

	for (int nCntPri = 0; nCntPri < NUM_PRIORITY; nCntPri++)
	{
		CObject* pObject = m_apTop[nCntPri];	// 先頭を取得

		while (pObject != nullptr)
		{// 使用されていない状態まで

			CObject* pObjectNext = pObject->GetNext();	// 次のオブジェクトへのポインタを取得

			// 描画処理
			if (pObject->GetDraw() && pObject->GetType() == CObject::TYPE::TYPE_PLAYER)
			{
				pObject->Draw();
			}

			pObject = pObjectNext;	// 次のオブジェクトに移動
		}
	}
}

//===============================================
// 死亡フラグをチェック
//===============================================
void CObjectManager::DeathCheck(void)
{
	for (int nCntPri = 0; nCntPri < NUM_PRIORITY; nCntPri++)
	{
		CObject *pObject = m_apTop[nCntPri];	// 先頭を取得

		while (pObject != nullptr)
		{// 使用されていない状態まで
			CObject *pObjectNext = pObject->GetNext();	// 次のオブジェクトへのポインタを取得

			if (pObject->GetDeath() == true)
			{
				// リストから自分自身を削除する
				if (m_apTop[nCntPri] == pObject)
				{// 自身が先頭
					if (pObject->GetNext() != nullptr)
					{// 次が存在している
						m_apTop[nCntPri] = pObject->GetNext();	// 次を先頭にする
						m_apTop[nCntPri]->SetPrev(nullptr);	// 次の前のポインタを覚えていないようにする
					}
					else
					{// 存在していない
						m_apTop[nCntPri] = nullptr;	// 先頭がない状態にする
						m_apCur[nCntPri] = nullptr;	// 最後尾がない状態にする
					}
				}
				else if (m_apCur[nCntPri] == pObject)
				{// 自身が最後尾
					if (pObject->GetPrev() != nullptr)
					{// 次が存在している
						m_apCur[nCntPri] = pObject->GetPrev();		// 前を最後尾にする
						m_apCur[nCntPri]->SetNext(nullptr);			// 前の次のポインタを覚えていないようにする
					}
					else
					{// 存在していない
						m_apTop[nCntPri] = nullptr;	// 先頭がない状態にする
						m_apCur[nCntPri] = nullptr;	// 最後尾がない状態にする
					}
				}
				else
				{
					if (pObject->GetNext() != nullptr)
					{
						pObject->GetNext()->SetPrev(pObject->GetPrev());	// 自身の次に前のポインタを覚えさせる
					}
					if (pObject->GetPrev() != nullptr)
					{
						pObject->GetPrev()->SetNext(pObject->GetNext());	// 自身の前に次のポインタを覚えさせる
					}
				}

				delete pObject;	// メモリの開放
				pObject = nullptr;
			}

			if (pObject == nullptr)
			{
				m_aPriNumAll[nCntPri]--;
				m_nNumAll--;
			}

			pObject = pObjectNext;	// 次のオブジェクトに移動
		}
	}
}

//===============================================
// リストに挿入する
//===============================================
void CObjectManager::ListIn(CObject *pObject)
{
	int nPri = pObject->GetPri();

	if (m_apTop[nPri] != nullptr)
	{// 先頭が存在している場合
		m_apCur[nPri]->SetNext(pObject);	// 現在最後尾のオブジェクトのポインタにつなげる
		pObject->SetPrev(m_apCur[nPri]);
		m_apCur[nPri] = pObject;	// 自分自身が最後尾になる
	}
	else
	{// 存在しない場合
		m_apTop[nPri] = pObject;	// 自分自身が先頭になる
		m_apCur[nPri] = pObject;	// 自分自身が最後尾になる
	}

	m_aPriNumAll[nPri]++;
	m_nNumAll++;
}

//===============================================
// インスタンスの取得
//===============================================
CObjectManager* CObjectManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = DEBUG_NEW CObjectManager;
		m_pInstance->Init();
	}

	return m_pInstance;
}

//===============================================
// インスタンスの廃棄
//===============================================
void CObjectManager::Release(void)
{
	if (m_pInstance != nullptr)
	{
		m_pInstance->Uninit();
	}
}
