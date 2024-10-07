//==========================================================
//
//入力デバイス処理 [input_mouse.cpp]
//Author 奥定伊吹
//
//==========================================================
#include "input_mouse.h"
#include "manager.h"
#include "debugproc.h"

#define REPEAT_TIME (15)	//リピートタイマー

// 静的メンバ変数
CInputMouse* CInputMouse::m_pInstance = nullptr;	// オブジェクトへのポインタ

//====================================================================================
//派生クラス(マウス)
//====================================================================================
// コンストラクタ
//==========================================================
CInputMouse::CInputMouse()
{
	m_State = {};
	m_Trigger = {};
	m_Release = {};
	m_Repeat = {};
	m_Point = {};
}

//==========================================================
// デストラクタ
//==========================================================
CInputMouse::~CInputMouse()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CInputMouse* CInputMouse::Create(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInstance != nullptr) {
		return m_pInstance;
	}

	// インスタンス生成
	m_pInstance = DEBUG_NEW CInputMouse;
	if (m_pInstance != nullptr) {

		// 初期化処理
		m_pInstance->Init(hInstance, hWnd);
	}

	return m_pInstance;
}

//==========================================================
//マウスの初期化
//==========================================================
HRESULT CInputMouse::Init(HINSTANCE hInstance, HWND hWnd)
{
	//入力デバイスの生成
	CInput::Init(hInstance, hWnd);

	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pDevice, nullptr)))
	{
		return E_FAIL;
	}

	//データフォーマットの生成
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		return E_FAIL;
	}

	//データフォーマットを設定
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	//デバイスの設定
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;	//相対値モードで設定

	if (FAILED(m_pDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph)))
	{
		return E_FAIL;
	}

	//マウスへのアクセス権を獲得
	m_pDevice->Acquire();

	return S_OK;
}

//==========================================================
//マウスの終了
//==========================================================
void CInputMouse::Uninit(void)
{
	//入力デバイスの廃棄
	CInput::Uninit();

	delete m_pInstance;
	m_pInstance = nullptr;
}

//==========================================================
//マウスの更新
//==========================================================
void CInputMouse::Update(void)
{
	DIMOUSESTATE2 MouseState;	//マウスの入力情報

	//入力デバイスからデータを取得
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(MouseState), (LPVOID)&MouseState)))
	{
		for (int nCnt = 0; nCnt < 8; nCnt++)
		{
			m_Trigger.rgbButtons[nCnt] = (m_State.rgbButtons[nCnt] ^ MouseState.rgbButtons[nCnt]) & MouseState.rgbButtons[nCnt];	//キーボードからのトリガー情報を保存
		}

		m_State = MouseState;	//プレス情報取得

		//マウスの現在のポインタを取得する
		GetCursorPos(&m_Point);
	}
	else
	{
		m_pDevice->Acquire(); //マウスへのアクセス権を獲得
	}
}

//==========================================================
//マウスのプレス情報取得
//==========================================================
bool CInputMouse::GetPress(int nKey)
{
	return (m_State.rgbButtons[nKey] & 0x80) ? true : false;
}

//==========================================================
//マウスのトリガー情報取得
//==========================================================
bool CInputMouse::GetTrigger(int nKey)
{
	return (m_Trigger.rgbButtons[nKey] & 0x80) ? true : false;
}

//==========================================================
//マウスのリリース情報取得
//==========================================================
bool CInputMouse::GetRelease(int nKey)
{
	return (m_Release.rgbButtons[nKey] & 0x80) ? true : false;
}

//==========================================================
//マウスのリピート情報取得
//==========================================================
bool CInputMouse::GetRepeat(int nKey)
{
	return (m_Repeat.rgbButtons[nKey] & 0x80) ? true : false;
}

//==========================================================
//マウスのカーソル移動量取得
//==========================================================
D3DXVECTOR3 CInputMouse::GetCousorMove(void)
{
	return D3DXVECTOR3((float)m_State.lX, (float)m_State.lY, (float)m_State.lZ);
}