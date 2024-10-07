//==========================================================
//
// 入力デバイス処理(キーボード) [input_keyboard.h]
// Author Ibuki Okusada
//
//==========================================================
#ifndef _INPUT_KEYBOARD_H_		// このマクロが定義されていない場合
#define _INPUT_KEYBOARD_H_		// 二重インクルード防止用マクロを定義

#include "input.h"

//==========================================================
//マクロ定義
//==========================================================
namespace
{
	const int NUM_KEY_MAX = (256);	//キーの最大数
}

//==========================================================
//派生クラスの定義(キーボード)
//==========================================================
class CInputKeyboard : public CInput
{
public:	// 誰でもアクセス可能

	CInputKeyboard();	// コンストラクタ
	~CInputKeyboard();	// デストラクタ

	// メンバ関数
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nKey);
	bool GetTrigger(int nKey);
	bool GetRelease(int nKey);
	bool GetRepeat(int nKey);
	static CInputKeyboard* Create(HINSTANCE hInstance, HWND hWnd);
	static CInputKeyboard* GetInstance() { return m_pInstance; }

private:	// 自分だけアクセス可能

	// メンバ変数
	BYTE m_aKeyState[NUM_KEY_MAX];	//プレス情報
	BYTE m_aKeyStateTrigger[NUM_KEY_MAX];	//トリガー情報
	BYTE m_aKeyStateRelease[NUM_KEY_MAX];	//リリース情報
	BYTE m_aKeyStateRepeat[NUM_KEY_MAX];	//リピート情報
	int m_aRepeatCnt[NUM_KEY_MAX];	// リピートタイマー
	static CInputKeyboard* m_pInstance;
};

#endif
