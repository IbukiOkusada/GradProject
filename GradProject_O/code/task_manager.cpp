//==========================================================
//
// タスク管理の処理 [task_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "task_manager.h"
#include "task.h"

CTaskManager *CTaskManager::m_pInstance = NULL;

//==========================================================
// コンストラクタ
//==========================================================
CTaskManager::CTaskManager()
{

}

//==========================================================
// デストラクタ
//==========================================================
CTaskManager::~CTaskManager()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CTaskManager::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CTaskManager::Uninit(void)
{
	// リストの全廃棄
	ReleaseAll();
}

//==========================================================
// 更新処理
//==========================================================
void CTaskManager::Update(void)
{
	// リストの全更新
	UpdateAll();
}

//===============================================
// 全てのタスクの廃棄
//===============================================
void CTaskManager::ReleaseAll(void)
{
	CTask *pTask = m_pTop;	// 先頭を取得

	while (pTask != NULL)
	{// 使用されていない状態まで

		CTask *pTaskNext = pTask->GetNext();	// 次のオブジェクトへのポインタを取得

		if (!pTask->GetDeath())
		{
			// 終了処理
			pTask->Uninit();
		}

		pTask = pTaskNext;	// 次のオブジェクトに移動
	}

	// 死亡フラグをチェック
	DeathCheck();
}

//===============================================
// 全てのタスクの更新
//===============================================
void CTaskManager::UpdateAll(void)
{
	CTask *pTask = m_pTop;	// 先頭を取得

	while (pTask != NULL)
	{// 使用されていない状態まで

		CTask *pTaskNext = pTask->GetNext();	// 次のオブジェクトへのポインタを取得

		if (!pTask->GetDeath())
		{
			// 更新処理
			pTask->Update();
		}

		pTask = pTaskNext;	// 次のオブジェクトに移動
	}

	// 死亡フラグをチェック
	DeathCheck();
}

//===============================================
// 死亡フラグをチェック
//===============================================
void CTaskManager::DeathCheck(void)
{
	CTask *pTask = m_pTop;	// 先頭を取得

	while (pTask != NULL)
	{// 使用されていない状態まで
		CTask *pTaskNext = pTask->GetNext();	// 次のオブジェクトへのポインタを取得

		if (pTask->GetDeath() == true)
		{
			// リストから自分自身を削除する
			if (m_pTop == pTask)
			{// 自身が先頭
				if (pTask->GetNext() != NULL)
				{// 次が存在している
					m_pTop = pTask->GetNext();	// 次を先頭にする
					m_pTop->SetPrev(NULL);	// 次の前のポインタを覚えていないようにする
				}
				else
				{// 存在していない
					m_pTop = NULL;	// 先頭がない状態にする
					m_pCur = NULL;	// 最後尾がない状態にする
				}
			}
			else if (m_pCur == pTask)
			{// 自身が最後尾
				if (pTask->GetPrev() != NULL)
				{// 次が存在している
					m_pCur = pTask->GetPrev();		// 前を最後尾にする
					m_pCur->SetNext(NULL);			// 前の次のポインタを覚えていないようにする
				}
				else
				{// 存在していない
					m_pTop = NULL;	// 先頭がない状態にする
					m_pCur = NULL;	// 最後尾がない状態にする
				}
			}
			else
			{
				if (pTask->GetNext() != NULL)
				{
					pTask->GetNext()->SetPrev(pTask->GetPrev());	// 自身の次に前のポインタを覚えさせる
				}
				if (pTask->GetPrev() != NULL)
				{
					pTask->GetPrev()->SetNext(pTask->GetNext());	// 自身の前に次のポインタを覚えさせる
				}
			}

			delete pTask;	// メモリの開放
			pTask = NULL;
		}

		pTask = pTaskNext;	// 次のオブジェクトに移動
	}
}

//===============================================
// リストに挿入する
//===============================================
void CTaskManager::ListIn(CTask *pTask)
{
	if (m_pTop != NULL)
	{// 先頭が存在している場合
		m_pCur->SetNext(pTask);	// 現在最後尾のオブジェクトのポインタにつなげる
		pTask->SetPrev(m_pCur);
		m_pCur = pTask;	// 自分自身が最後尾になる
	}
	else
	{// 存在しない場合
		m_pTop = pTask;	// 自分自身が先頭になる
		m_pCur = pTask;	// 自分自身が最後尾になる
	}
}

//===============================================
// インスタンスの取得
//===============================================
CTaskManager* CTaskManager::GetInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CTaskManager;
	}

	return m_pInstance;
}

//===============================================
// インスタンスの廃棄
//===============================================
void CTaskManager::Release(void)
{
	if (m_pInstance != NULL)
	{
		m_pInstance->Uninit();
		delete m_pInstance;
		m_pInstance = NULL;
	}
}