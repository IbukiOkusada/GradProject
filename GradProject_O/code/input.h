//==========================================================
//
//入力デバイス処理 [input.h]
//Author 奥定伊吹
//
//==========================================================
#ifndef _INPUT_H_		// このマクロが定義されていない場合
#define _INPUT_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "Xinput.h"

#pragma comment(lib, "xinput.lib")	//入力処理

//==========================================================
//マクロ定義
//==========================================================
#define NUM_KEY_MAX	(256)	//キーの最大数
#define PLAYER_MAX	(4)		//プレイヤーの最大数

//==========================================================
//基底クラスの定義(入力デバイス)
//==========================================================
class CInput
{
public:	// 誰でもアクセス可能
	
	CInput();	// コンストラクタ
	virtual ~CInput();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void) = 0;

protected:	// 派生クラスからもアクセス可能
	static LPDIRECTINPUT8 m_pInput;	//DirectInputオブジェクトへのポインタ
	LPDIRECTINPUTDEVICE8 m_pDevice;	//入力デバイスへのポインタ

private:	// 自分だけアクセス可能

};

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

private:	// 自分だけアクセス可能

	// メンバ変数
	BYTE m_aKeyState[NUM_KEY_MAX];	//プレス情報
	BYTE m_aKeyStateTrigger[NUM_KEY_MAX];	//トリガー情報
	BYTE m_aKeyStateRelease[NUM_KEY_MAX];	//リリース情報
	BYTE m_aKeyStateRepeat[NUM_KEY_MAX];	//リピート情報
	int m_aRepeatCnt[NUM_KEY_MAX];	// リピートタイマー
};

//==========================================================
//派生クラスの定義(マウス)
//==========================================================
class CInputMouse : public CInput
{
public:	// 誰でもアクセス可能

	//マウス
	enum BUTTON
	{
		BUTTON_LBUTTON = 0,	// 左クリック
		BUTTON_RBUTTON,		// 右クリック
		BUTTON_WHEEL,			// ホイール
		BUTTON_MAX
	};

	CInputMouse();	// コンストラクタ
	~CInputMouse();	// デストラクタ

	// メンバ関数
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nKey);
	bool GetTrigger(int nKey);
	bool GetRelease(int nKey);
	bool GetRepeat(int nKey);
	D3DXVECTOR3 GetCousorMove();

private:	// 自分だけアクセス可能

	// メンバ変数
	DIMOUSESTATE2 m_State;	//プレス情報
	DIMOUSESTATE2 m_Trigger;	//トリガー情報
	DIMOUSESTATE2 m_Release;	//リリース情報
	DIMOUSESTATE2 m_Repeat;	//リピート情報
	POINT m_Point;			//カーソルポインター
};

//==========================================================
//派生クラスの定義(マウス)
//==========================================================
class CInputPad : public CInput
{
public:	// 誰でもアクセス可能

	//ゲームパッドボタン
	enum BUTTON
	{
		BUTTON_UP = 0,		// 上ボタン
		BUTTON_DOWN,			// 下ボタン
		BUTTON_LEFT,			// 左ボタン
		BUTTON_RIGHT,			// 右ボタン
		BUTTON_START,			// start
		BUTTON_BACK,			// back
		BUTTON_LEFTSTCPUSH,	// 左スティック押し込み
		BUTTON_RIGHTSTCPUSH,	// 右スティック押し込み
		BUTTON_LEFTBUTTON,	// LB(L)ボタン
		BUTTON_RIGHTBUTTON,	// RB(R)ボタン
		BUTTON_11,			// Lトリガー
		BUTTON_12,			// Rトリガー
		BUTTON_A,			// Aボタン
		BUTTON_B,			// Bボタン
		BUTTON_X,			// Xボタン
		BUTTON_Y,			// Yボタン
		BUTTON_LEFT_Y,		// 左(Y)
		BUTTON_LEFT_X,		// 左(X)
		BUTTON_RIGHT_Y,		// 右(Y)
		BUTTON_RIGHT_X,		// 右(X)
		BUTTON_MAX
	};

	//スティック
	typedef enum
	{
		STICK_PLUS = 0,		// 値がプラス
		STICK_MINUS,			// 値がマイナス
		STICK_MAX
	}Stick;

	CInputPad();	// コンストラクタ
	~CInputPad();	// デストラクタ

	// メンバ関数
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nKey, int nPlayer);
	bool GetTrigger(int nKey, int nPlayer);
	bool GetRelease(int nKey, int nPlayer);
	bool GetRepeat(int nKey, int nPlayer);
	bool GetStickPress(int nPlayer, int nKey, float DeadZone, Stick PlusStick);
	BYTE GetRightTriggerPress(int nPlayer);
	float GetStickAdd(int nPlayer, int nKey, float DeadZone, Stick PlusStick);

private:	// 自分だけアクセス可能

	// メンバ変数
	XINPUT_STATE m_State[PLAYER_MAX];		// プレス
	XINPUT_STATE m_Trigger[PLAYER_MAX];		// トリガー
	XINPUT_STATE m_Release[PLAYER_MAX];		// リリース
	XINPUT_STATE m_Repeat[PLAYER_MAX];	// リピート
	int m_aRepeatCnt[NUM_KEY_MAX];			// パッドのリピートカウント
};

// キネクト
#endif
