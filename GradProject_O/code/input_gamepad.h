//==========================================================
//
// 入力デバイス処理(ゲームパッド) [input_gamepad.h]
// Author Ibuki Okusada
//
//==========================================================
#ifndef _INPUT_PAD_H_		// このマクロが定義されていない場合
#define _INPUT_PAD_H_		// 二重インクルード防止用マクロを定義

#include "input.h"
#include "Xinput.h"

#pragma comment(lib, "xinput.lib")	//入力処理

//==========================================================
//マクロ定義
//==========================================================
namespace
{
	const int PLAYER_MAX = (4);	//プレイヤーの最大数
}

//==========================================================
//派生クラスの定義(パッド)
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
	BYTE GetLeftTriggerPress(int nPlayer);
	float GetStickAdd(int nPlayer, int nKey, float DeadZone, Stick PlusStick);
	D3DXVECTOR3 GetLStick(int nPlayer, float DeadZone);
	D3DXVECTOR3 GetRStick(int nPlayer, float DeadZone);
	static CInputPad* Create(HINSTANCE hInstance, HWND hWnd);
	static CInputPad* GetInstance() { return m_pInstance; }

private:	// 自分だけアクセス可能

	// メンバ変数
	XINPUT_STATE m_State[PLAYER_MAX];		// プレス
	XINPUT_STATE m_Trigger[PLAYER_MAX];		// トリガー
	XINPUT_STATE m_Release[PLAYER_MAX];		// リリース
	XINPUT_STATE m_Repeat[PLAYER_MAX];	// リピート
	int m_aRepeatCnt[PLAYER_MAX];			// パッドのリピートカウント
	static CInputPad* m_pInstance;
};

#endif
