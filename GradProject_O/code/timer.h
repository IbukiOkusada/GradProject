//===============================================
//
// フェードの処理全般 [fade.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _TIMER_H_
#define _TIMER_H_

#include "main.h"	//main.hで定義しているものが必要なためinclude
#include "manager.h"
#include "Number.h"
#include "task.h"

//===============================================
// タイマークラスの定義(派生クラス)
//===============================================
class CTimer
{
public:	// 誰でもアクセス可能な定義


public:	// 誰でもアクセス可能

	CTimer();	// コンストラクタ
	~CTimer();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CTimer* Create();

private:	// 自分だけがアクセス可能な定義

	void CalTime();

	// メンバ変数
	CNumber* m_pObject[5];		// 描画オブジェクト
	CObject2D* m_pDecPoint;		// 小数点のオブジェクト

	float m_LimitTime;
	int m_Time[5];
};

#endif
