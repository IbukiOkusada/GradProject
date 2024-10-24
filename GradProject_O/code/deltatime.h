//==========================================================
//
// デルタタイム [deltatime.cpp]
// Author : Ryosuke Ohara
//
//==========================================================
#ifndef _DELTATIME_H_		// このマクロが定義されていない場合
#define _DELTATIME_H_		// 二重インクルード防止用マクロを定義

#include <iostream>
#include <chrono>

//==========================================================
// デルタタイムのクラス定義
//==========================================================
class CDeltaTime
{

public:	// 誰でもアクセス可能

	CDeltaTime();	// コンストラクタ(オーバーロード)
	~CDeltaTime();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CDeltaTime* Create(void);

	// メンバ関数(取得)
	float GetDeltaTime(void);

	// メンバ関数(設定)

private:	// 自分だけがアクセス可能

	// タイマーの情報
	struct Timer
	{
		std::chrono::high_resolution_clock::time_point CurrentTime;  // 現在の時間
		std::chrono::high_resolution_clock::time_point LastTime;     // 前回の時間
		std::chrono::duration<float> DeltaTime;                      // デルタタイム
	};

	Timer m_Timer;  // タイマーの情報

	// メンバ関数

	// メンバ変数
};

#endif
