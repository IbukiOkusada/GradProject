//==========================================================
//
// タスク管理の処理 [task_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _TASK_MANAGER_H_	// このマクロが定義されていない場合
#define _TASK_MANAGER_H_	// 二重インクルード防止用マクロを定義

// 前方宣言
class CTask;

//==========================================================
// サンプルのクラス定義
//==========================================================
class CTaskManager
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
	void ReleaseAll(void);
	void UpdateAll(void);
	void DeathCheck(void);

	// メンバ変数
	CTask *m_pTop;	// 先頭のオブジェクトへのポインタ
	CTask *m_pCur;	// 最後尾のオブジェクトへのポインタ
	static CTaskManager *m_pInstance;
};

#endif


