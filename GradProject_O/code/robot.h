//==========================================================
//
// サンプルタスク(リスト管理タスク) [sample.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _SAMPLE_H_		// このマクロが定義されていない場合
#define _SAMPLE_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
//#include "task.h"	// これでファイルインクルードできます

//==========================================================
// サンプルのクラス定義
//==========================================================
class CRobot : public CTask
{

public:	// 誰でもアクセス可能

	CRobot();	// コンストラクタ(オーバーロード)
	~CRobot();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CRobot* Create(void);

	// メンバ関数(取得)
	CRobot* GetNext(void) { return m_pNext; }
	CRobot* GetPrev(void) { return m_pPrev; }

	// メンバ関数(設定)
	void SetNext(CRobot* pNext) { m_pNext = pNext; }
	void SetPrev(CRobot* pPrev) { m_pPrev = pPrev; }

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	CRobot* m_pPrev;			// 前のオブジェクトへのポインタ
	CRobot* m_pNext;			// 次のオブジェクトへのポインタ
};

#endif

