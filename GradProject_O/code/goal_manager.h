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
	CGoalManager();
	~CGoalManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	// 取得系
	static CGoalManager *GetInstance(void) { return m_Instance; }

private:

	CGoal *m_pGoal;  // ゴールのポインタ
	static CGoalManager *m_Instance;  // 自身のポインタ
};

#endif