//==========================================================
//
//入力デバイス処理 [input.cpp]
//Author 奥定伊吹
//
//==========================================================
#include "input.h"
#include "input_gamepad.h"
#include "input_keyboard.h"
#include "input_mouse.h"
#include "manager.h"
#include "debugproc.h"

#define REPEAT_TIME (15)	//リピートタイマー

// 静的メンバ変数宣言
LPDIRECTINPUT8 CInput::m_pInput = nullptr;	// オブジェクトへのポインタ
CInput* CInput::m_pInstance = nullptr;	// オブジェクトへのポインタ

//====================================================================================
//基底クラス(入力デバイス)
//====================================================================================
// コンストラクタ
//==========================================================
CInput::CInput()
{
	m_pDevice = nullptr;
}

//==========================================================
// デストラクタ
//==========================================================
CInput::~CInput()
{
	m_pDevice = nullptr;
}

//==========================================================================
// 生成処理
//==========================================================================
CInput* CInput::Create(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInstance != nullptr) {
		return m_pInstance;
	}

	// 自身
	m_pInstance = DEBUG_NEW CInput;
	if (m_pInstance == nullptr) {
		return nullptr;
	}

	// マウス
	m_pInstance->m_pMouse = CInputMouse::Create(hInstance, hWnd);
	if (m_pInstance->m_pMouse == nullptr) {
		return nullptr;
	}

	// キーボード
	m_pInstance->m_pKeyboard = CInputKeyboard::Create(hInstance, hWnd);
	if (m_pInstance->m_pKeyboard == nullptr) {
		return nullptr;
	}

	// ゲームパッド
	m_pInstance->m_pPad = CInputPad::Create(hInstance, hWnd);
	if (m_pInstance->m_pPad == nullptr) {
		return nullptr;
	}

	//// キーコンフィグ
	//m_pInstance->m_pKeyConfig = CKeyConfigManager::Create();
	//if (m_pInstance->m_pKeyConfig == nullptr) {
	//	return nullptr;
	//}

	return m_pInstance;
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CInput::Init(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInput == nullptr)
	{// まだ誰も使用開始していない場合

		//DirectInputオブジェクトの生成
		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, nullptr)))
		{// 生成失敗した場合
			return E_FAIL;
		}
	}

	return S_OK;
}

//==========================================================
//終了処理
//==========================================================
void CInput::Uninit(void)
{
	//デバイスの廃棄(+アクセス権開放)
	if (m_pDevice != nullptr)
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = nullptr;
	}

	//オブジェクトの廃棄
	if (m_pInput != nullptr)
	{
		m_pInput->Release();
		m_pInput = nullptr;
	}
}

#if 1	// 純粋仮想関数なので通らない
//==========================================================
//更新処理
//==========================================================
void CInput::Update(void)
{
	// 各更新
	m_pMouse->Update();
	m_pKeyboard->Update();
	m_pPad->Update();
}
#endif

//==========================================================
// 開放処理
//==========================================================
void CInput::Release()
{
	if (m_pMouse != nullptr) {
		m_pMouse->Uninit();
		m_pMouse = nullptr;
	}

	if (m_pKeyboard != nullptr) {
		m_pKeyboard->Uninit();
		m_pKeyboard = nullptr;
	}

	if (m_pPad != nullptr) {
		m_pPad->Uninit();
		m_pPad = nullptr;
	}

	/*if (m_pKeyConfig != nullptr) {
		m_pKeyConfig->Uninit();
		m_pKeyConfig = nullptr;
	}*/

	// オブジェクトの破棄
	if (m_pInput != nullptr) {
		m_pInput->Release();
		m_pInput = nullptr;
	}

	if (m_pInstance != nullptr) {
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
