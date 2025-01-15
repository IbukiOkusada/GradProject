//==========================================================
//
// ロボットマネージャー [robot_manager.h]
// Author : Kenta Hashimoto
//
//==========================================================
#ifndef _ROBOTMANAGER_H_		// このマクロが定義されていない場合
#define _ROBOTMANAGER_H_		// 二重インクルード防止用マクロを定義

// 前方宣言
class CRobot;

#include "list_manager.h"

//==========================================================
// ロボットマネージャーのクラス定義
//==========================================================
class CRobotManager : public CListManager
{
private:

	CRobotManager();		// コンストラクタ
	~CRobotManager();	// デストラクタ

public:	// 誰でもアクセス可能

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CRobotManager* GetInstance(void);
	static void Release(void);
	CRobot* GetTop(void) { return m_pTop; }
	CRobot* GetCur(void) { return m_pCur; }
	void ListIn(CRobot* pSample);
	void ListOut(CRobot* pSample);
	int GetNum(void) { return m_nNum; }

private:	// 自分だけがアクセス可能

	// メンバ関数


	// メンバ変数
	CRobot* m_pTop;	// 先頭
	CRobot* m_pCur;	// 最後尾
	int m_nNum;
	static CRobotManager* m_pInstance;	// インスタンス
};

#endif

