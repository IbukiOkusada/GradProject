//==========================================================
//
// タスク管理の処理 [task_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _TASK_MANAGER_H_	// このマクロが定義されていない場合
#define _TASK_MANAGER_H_	// 二重インクルード防止用マクロを定義

#include "list_manager.h"
#include "list.h"

// 前方宣言
class CTask;

//==========================================================
// タスクマネージャーのクラス定義
//==========================================================
class CTaskManager : public CListManager
{
private:
	CTaskManager();	// コンストラクタ
	~CTaskManager();	// デストラクタ

public:
	// リスト管理メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void ListIn(CTask *pTask);

	// シングルトン
	static CTaskManager* GetInstance(void);
	static void Release(void);

private:	// 自分だけがアクセス可能

	// メンバ関数
	Clist<CTask*>* GetList() { if (m_plist == nullptr) { m_plist = m_plist->Create(); }return m_plist; }	// リスト取得
	void ListRelease() { if (m_plist != nullptr) { delete m_plist; m_plist = nullptr; } }			// リスト解放
	void ReleaseAll(void);
	void UpdateAll(void);
	void DeathCheck(void);

	// メンバ変数
	Clist<CTask*>* m_plist;
	static CTaskManager *m_pInstance;
};

#endif


