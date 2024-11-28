//==========================================================
//
// ゴール管理の処理 [goal_manager.h]
// Author : Ryosuke Ohara
//
//==========================================================
#ifndef _GOAL_MANAGER_H_		// このマクロが定義されていない場合
#define _GOAL_MANAGER_H_		// 二重インクルード防止用マクロを定義

#include "goal.h"

//==========================================================
// ゴール管理のクラス定義
//==========================================================
class CGoalManager
{
public:
	CGoalManager();      // コンストラクタ
	~CGoalManager();     // デストラクタ

	HRESULT Init(void);  // 初期化処理
	void Update(void);   // 更新処理
	void SetInfoList(std::vector<CGoal::SInfo>& info) { m_InfoList = info; }
	void GoalCreate(int nId);
	bool ListIn(CGoal* pGoal);
	bool ListOut(CGoal* pGoal);

	static CGoalManager *Create(void);  // 生成処理
	static void Release();

	// 取得系
	static CGoalManager *GetInstance(void) { 
		Create();
		return m_pInstance; 
	}
	std::vector<CGoal::SInfo>* GetInfoList() { return &m_InfoList; }
	CGoal* GetGoal(int nIdx);
	std::map<int, CGoal*>* GetList() { return &m_List; }
	int GetCreateIdx() { return m_nNumCreate; }
	int GetNextIdx() { return m_nNextIdx; }

private:

	void Uninit(void);   // 終了処理

	int m_nNextIdx;
	int m_nOldIdx;
	int m_nNumCreate;
	std::vector<CGoal::SInfo> m_InfoList;
	CGoal* m_pGoal;		// ゴールのポインタ
	CGoal* m_pGoalOld;  // 前回のゴールのポインタ
	static CGoalManager *m_pInstance;  // 自身のポインタ
	std::map<int, CGoal*> m_List;	// リスト
};

#endif