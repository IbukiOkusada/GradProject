//==========================================================
//
// ゴール管理の処理 [goal_manager.h]
// Author : Ryosuke Ohara
//
//==========================================================
#ifndef _GOAL_MANAGER_H_		// このマクロが定義されていない場合
#define _GOAL_MANAGER_H_		// 二重インクルード防止用マクロを定義

// 前方宣言
class CGoal;

//==========================================================
// ゴール管理のクラス定義
//==========================================================
class CGoalManager
{
public:
	CGoalManager();      // コンストラクタ
	~CGoalManager();     // デストラクタ

	HRESULT Init(void);  // 初期化処理
	void Uninit(void);   // 終了処理
	void Update(void);   // 更新処理

	static CGoalManager *Create(void);  // 生成処理

	// 取得系
	static CGoalManager *GetInstance(void) { return m_pInstance; }

private:

	CGoal*m_pGoal;  // ゴールのポインタ
	static CGoalManager *m_pInstance;  // 自身のポインタ
};

#endif