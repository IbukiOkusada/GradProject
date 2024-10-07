//==========================================================
//
// スローの処理管理 [slow.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _SLOW_H_		// このマクロが定義されていない場合
#define _SLOW_H_		// 二重インクルード防止用マクロを定義

//**********************************************************
// スロークラスの定義
//**********************************************************
class CSlow
{
public:		// 誰でもアクセス可能

	CSlow();	// コンストラクタ
	~CSlow();	// デストラクタ

	// メンバ関数
	void Init(void);
	void Uninit(void);
	void Update(void);
	float Get(void);
	float GetOld(void);
	void SetSlow(bool bUse);
	void SetSlow(float fValue) { m_fAdd = fValue; }

private:	// 自分だけがアクセス可能

	bool m_b;		// カウントが回ったか否か
	bool m_bUse;		// スロー入力されたか
	int m_nBoostCnt;	// 強さ上昇カウント
	int m_nTimeCnt;	// タイマー
	int m_nTimer;		// カウント規定値
	float m_fAdd;		// スロー倍率
	float m_fMulOld;	// 前回の倍率
};

#endif