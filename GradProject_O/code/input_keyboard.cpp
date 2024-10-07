//==========================================================
//
//入力デバイス処理(キーボード) [input_keyboard.cpp]
//Author 奥定伊吹
//
//==========================================================
#include "input_keyboard.h"
#include "manager.h"
#include "debugproc.h"

#define REPEAT_TIME (15)	//リピートタイマー

// 静的メンバ変数宣言
CInputKeyboard* CInputKeyboard::m_pInstance = nullptr;	// オブジェクトへのポインタ

//====================================================================================
//派生クラス(キーボード)
//====================================================================================
// コンストラクタ
//==========================================================
CInputKeyboard::CInputKeyboard()
{
	// 値をクリアする
	for (int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
	{
		m_aKeyState[nCntKey] = 0;
		m_aKeyStateTrigger[nCntKey] = 0;
		m_aKeyStateRelease[nCntKey] = 0;
		m_aKeyStateRepeat[nCntKey] = 0;
	}
}

//==========================================================
// デストラクタ
//==========================================================
CInputKeyboard::~CInputKeyboard()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CInputKeyboard* CInputKeyboard::Create(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInstance != nullptr) {
		return m_pInstance;
	}

	// インスタンス生成
	m_pInstance = DEBUG_NEW CInputKeyboard;
	if (m_pInstance != nullptr) {

		// 初期化処理
		m_pInstance->Init(hInstance, hWnd);
	}

	return m_pInstance;
}

//==========================================================
//初期化処理
//==========================================================
HRESULT CInputKeyboard::Init(HINSTANCE hInstance, HWND hWnd)
{
	CInput::Init(hInstance, hWnd);

	//入力デバイス(キーボード)の生成
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevice, nullptr)))
	{
		return E_FAIL;
	}

	//データフォーマットを設定
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	//データフォーマットを設定
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	//キーボードへのアクセス権を取得
	m_pDevice->Acquire();

	return S_OK;
}

//==========================================================
//終了処理
//==========================================================
void CInputKeyboard::Uninit(void)
{
	CInput::Uninit();

	delete m_pInstance;
	m_pInstance = nullptr;
}

//==========================================================
//更新処理
//==========================================================
void CInputKeyboard::Update(void)
{
	BYTE aKeyState[NUM_KEY_MAX];	//キーボードの入力情報
	int nCntKey = 0;

	//入力デバイスからデータを取得
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{
			m_aKeyStateTrigger[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & aKeyState[nCntKey];	//キーボードからのトリガー情報を保存
			m_aKeyStateRelease[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & m_aKeyState[nCntKey];	//キーボードからのリリース情報を保存
			m_aKeyState[nCntKey] = aKeyState[nCntKey];	//キーボードからのプレス情報を保存
			m_aKeyStateRepeat[nCntKey] = m_aKeyStateTrigger[nCntKey];

			//リピート取得
			if (GetPress(nCntKey) == true)
			{//プレスで入力されているとき
				m_aRepeatCnt[nCntKey]++;

				if (m_aRepeatCnt[nCntKey] >= REPEAT_TIME)
				{//リピートカウンターがタイマーを超えたとき
					m_aRepeatCnt[nCntKey] = 0;
					m_aKeyStateRepeat[nCntKey] = m_aKeyState[nCntKey];
				}
				else
				{
					m_aKeyStateRepeat[nCntKey] = m_aKeyStateRelease[nCntKey];
				}
			}

			if (GetRelease(nCntKey) == true)
			{//リリース入力されたとき
				m_aRepeatCnt[nCntKey] = 0;
			}
		}
	}
	else
	{
		m_pDevice->Acquire();	//キーボードへのアクセス権を獲得
	}

}

//==========================================================
//プレス情報を取得
//==========================================================
bool CInputKeyboard::GetPress(int nKey)
{
	return (m_aKeyState[nKey] & 0x80) ? true : false;
}

//==========================================================
//トリガー情報を取得
//==========================================================
bool CInputKeyboard::GetTrigger(int nKey)
{
	return (m_aKeyStateTrigger[nKey] & 0x80) ? true : false;
}

//==========================================================
//リリース情報を取得
//==========================================================
bool CInputKeyboard::GetRelease(int nKey)
{
	return (m_aKeyStateRelease[nKey] & 0x80) ? true : false;
}

//==========================================================
//リピート情報を取得
//==========================================================
bool CInputKeyboard::GetRepeat(int nKey)
{
	return (m_aKeyStateRepeat[nKey] & 0x80) ? true : false;
}