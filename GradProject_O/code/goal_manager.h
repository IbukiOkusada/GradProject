//==========================================================
//
// ゴール管理の処理 [goal_manager.h]
// Author : Ryosuke Ohara
//
//==========================================================
#ifndef _GOAL_MANAGER_H_		// このマクロが定義されていない場合
#define _GOAL_MANAGER_H_		// 二重インクルード防止用マクロを定義

class CGoalManager
{
public:
	CGoalManager();
	~CGoalManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

private:


};

#endif