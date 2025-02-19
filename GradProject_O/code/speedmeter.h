//==========================================================
//
// スピードメーターの処理 [speedmeter.h]
// Author : Ryosuke Ohara
//
//==========================================================
#ifndef _SPEEDMETER_H_		// このマクロが定義されていない場合
#define _SPEEDMETER_H_		// 二重インクルード防止用マクロを定義

#include "task.h"

// 前方宣言
class CObject2D;

//**********************************************************
// スピードメータークラスの定義
//**********************************************************
class CSpeedMeter : CTask
{
public:		// 誰でもアクセス可能

	CSpeedMeter();	// コンストラクタ
	~CSpeedMeter();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CSpeedMeter* Create(void);

	// 取得系
	static CSpeedMeter* GetInstance(void) { return m_pInstance; }  // 自身

private:	// 自分だけがアクセス可能

	void Needle(void);     // 針の制御処理

	CObject2D* m_pMeter;   // メーターのポインタ
	CObject2D* m_pNeedle;  // メーターの針ポインタ

	static CSpeedMeter* m_pInstance;  // 自身ののポインタ
};

#endif