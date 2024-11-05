//==========================================================
//
// タスク管理の処理 [task_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "task_manager.h"
#include "edit_manager.h"
#include "task.h"

CTaskManager *CTaskManager::m_pInstance = nullptr;

//==========================================================
// コンストラクタ
//==========================================================
CTaskManager::CTaskManager()
{
	m_plist = nullptr;
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
	// 親クラスの終了処理
	CListManager::Uninit();

	// リストの全廃棄
	ReleaseAll();

	ListRelease();

	// インスタンスの廃棄
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
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
	for (int i = 0; i < GetList()->GetNum(); i++)
	{// 使用されていない状態まで

		CTask* pTask = GetList()->Get(i);	// 先頭を取得

		if (!pTask->GetDeath())
		{
			// 終了処理
			pTask->Uninit();
		}
	}

	// 死亡フラグをチェック
	DeathCheck();
}

//===============================================
// 全てのタスクの更新
//===============================================
void CTaskManager::UpdateAll(void)
{
	CEditManager* pEdit = CEditManager::GetInstance();

	for (int i = 0; i < GetList()->GetNum(); i++)
	{// 使用されていない状態まで

		CTask* pTask = GetList()->Get(i);	// 先頭を取得

		// エディット中
		if (pEdit != nullptr && pTask->GetType() != CTask::TYPE_MODEL) { 
			continue;
		}

		if (!pTask->GetDeath())
		{
			// 更新処理
			pTask->Update();
		}
	}

	// 死亡フラグをチェック
	DeathCheck();
}

//===============================================
// 死亡フラグをチェック
//===============================================
void CTaskManager::DeathCheck(void)
{
	for (int i = 0; i < GetList()->GetNum(); i++)
	{// 使用されていない状態まで
		CTask* pTask = GetList()->Get(i);	// 先頭を取得

		if (pTask->GetDeath())
		{
			i--;
			GetList()->Delete(pTask);
			delete pTask;	// メモリの開放
			pTask = nullptr;

			if (GetList()->Empty())
			{
				ListRelease();
			}
		}
	}
}

//===============================================
// リストに挿入する
//===============================================
void CTaskManager::ListIn(CTask *pTask)
{
	//自身をリストに登録
	GetList()->Regist(pTask);
}

//===============================================
// インスタンスの取得
//===============================================
CTaskManager* CTaskManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = DEBUG_NEW CTaskManager;
	}

	return m_pInstance;
}

//===============================================
// インスタンスの廃棄
//===============================================
void CTaskManager::Release(void)
{
	if (m_pInstance != nullptr)
	{
		m_pInstance->Uninit();
	}
}