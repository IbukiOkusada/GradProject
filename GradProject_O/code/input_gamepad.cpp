//==========================================================
//
//入力デバイス処理 [input.cpp]
//Author 奥定伊吹
//
//==========================================================
#include "input.h"
#include "input_gamepad.h"
#include "manager.h"
#include "debugproc.h"

#define REPEAT_TIME (15)	//リピートタイマー

// 静的メンバ変数宣言
CInputPad* CInputPad::m_pInstance = nullptr;	// オブジェクトへのポインタ

#include "input_gamepad.h"

//==========================================================
//パッドのコンストラクタ
//==========================================================
CInputPad::CInputPad()
{

}

//==========================================================
//デストラクタ
//==========================================================
CInputPad::~CInputPad()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CInputPad* CInputPad::Create(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInstance != nullptr) {
		return m_pInstance;
	}

	// インスタンス生成
	m_pInstance = DEBUG_NEW CInputPad;
	if (m_pInstance != nullptr) {

		// 初期化処理
		m_pInstance->Init(hInstance, hWnd);
	}

	return m_pInstance;
}

//==========================================================
//初期化処理
//==========================================================
HRESULT CInputPad::Init(HINSTANCE hInstance, HWND hWnd)
{
	CInput::Init(hInstance, hWnd);

	//Xinputのステートを設定
	XInputEnable(true);
	int nCnt;

	for (nCnt = 0; nCnt < PLAYER_MAX; nCnt++)
	{
		//メモリーをクリア
		memset(&m_State[nCnt], 0, sizeof(XINPUT_STATE));
		memset(&m_Trigger[nCnt], 0, sizeof(XINPUT_STATE));
	}

	return S_OK;
}

//==========================================================
//終了処理
//==========================================================
void CInputPad::Uninit(void)
{
	//Xinputステートを設定
	XInputEnable(false);

	CInput::Uninit();

	delete m_pInstance;
	m_pInstance = nullptr;
}

//==========================================================
// 更新処理
//==========================================================
void CInputPad::Update(void)
{
	XINPUT_STATE aGamepadState[PLAYER_MAX];	//ゲームパッドの入力情報
	static int nRepeatJoyKeyCnt[PLAYER_MAX] = {};
	int nCntPad = 0;

	for (nCntPad = 0; nCntPad < PLAYER_MAX; nCntPad++)
	{
		//入力デバイスからデータを取得
		if (XInputGetState(nCntPad, &aGamepadState[nCntPad]) == ERROR_SUCCESS)
		{
			m_Trigger[nCntPad].Gamepad.wButtons =
				~m_State[nCntPad].Gamepad.wButtons
				& aGamepadState[nCntPad].Gamepad.wButtons;	//トリガー(ボタン)

			m_Release[nCntPad].Gamepad.wButtons =
				(m_State[nCntPad].Gamepad.wButtons
					^ aGamepadState[nCntPad].Gamepad.wButtons)
				& m_State[nCntPad].Gamepad.wButtons;	//リリース(ボタン)

			m_State[nCntPad] = aGamepadState[nCntPad];	//プレス

			//リピート取得

		}
	}
}

//==========================================================
// プレス処理
//==========================================================
bool CInputPad::GetPress(int nKey, int nPlayer)
{
	return (m_State[nPlayer].Gamepad.wButtons & (0x01 << nKey)) ? true : false;
}

//==========================================================
// トリガー処理
//==========================================================
bool CInputPad::GetTrigger(int nKey, int nPlayer)
{
	return (m_Trigger[nPlayer].Gamepad.wButtons & (0x01 << nKey)) ? true : false;
}

//==========================================================
// リリース処理
//==========================================================
bool CInputPad::GetRelease(int nKey, int nPlayer)
{
	return (m_Release[nPlayer].Gamepad.wButtons & (0x01 << nKey)) ? true : false;
}

//==========================================================
// リピート処理
//==========================================================
bool CInputPad::GetRepeat(int nKey, int nPlayer)
{
	return (m_Repeat[nPlayer].Gamepad.wButtons & (0x01 << nKey)) ? true : false;
}

//==========================================================
// スティック取得
//==========================================================
bool CInputPad::GetStickPress(int nPlayer, int nKey, float DeadZone, Stick PlusStick)
{
	float fAnswer = 0.0f;

	switch (nKey)
	{
	case BUTTON_LEFT_X:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbLX / SHRT_MAX;
		break;
	case BUTTON_LEFT_Y:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbLY / SHRT_MAX;
		break;
	case BUTTON_RIGHT_X:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbRX / SHRT_MAX;
		break;
	case BUTTON_RIGHT_Y:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbRY / SHRT_MAX;
		break;
	default:
		break;
	}

	if ((fAnswer < 0.0f && PlusStick == STICK_MINUS && fAnswer < -DeadZone) || (fAnswer > 0.0f && PlusStick == STICK_PLUS && fAnswer > DeadZone))
	{
		return true;
	}


	return false;
}

//==========================================================
// スティックの値取得
//==========================================================
float CInputPad::GetStickAdd(int nPlayer, int nKey, float DeadZone, Stick PlusStick)
{
	float fAnswer = 0.0f;

	switch (nKey)
	{
	case BUTTON_LEFT_X:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbLX / SHRT_MAX;
		break;
	case BUTTON_LEFT_Y:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbLY / SHRT_MAX;
		break;
	case BUTTON_RIGHT_X:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbRX / SHRT_MAX;
		break;
	case BUTTON_RIGHT_Y:
		fAnswer = (float)m_State[nPlayer].Gamepad.sThumbRY / SHRT_MAX;
		break;
	default:
		break;
	}

	return fAnswer;
}

//==========================================================
// 右トリガー取得
//==========================================================
BYTE CInputPad::GetRightTriggerPress(int nPlayer)
{
	return (m_State[nPlayer].Gamepad.bRightTrigger);
}

//==========================================================
// 左トリガー取得
//==========================================================
BYTE CInputPad::GetLeftTriggerPress(int nPlayer)
{
	return (m_State[nPlayer].Gamepad.bLeftTrigger);
}