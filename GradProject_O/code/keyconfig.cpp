//=============================================================================
//
// キーコンフィグ処理 [keyconfig.cpp]
// Author : Ibuki Okusada
//
//=============================================================================
#include "keyconfig.h"
#include "keyconfig_gamepad.h"
#include "keyconfig_keyboard.h"
#include "controlkeydisp.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const std::string CONFIGFILE[CKeyConfigManager::Control::CONTROL_MAX] =	// ファイル名
	{
		"data\\TEXT\\keyconfig\\gamepad.txt",
		"data\\TEXT\\keyconfig\\keyboard.txt",
	};
}

// ネームSPACE
namespace KEY_PAD
{
	const std::string FOLDER_PATH = "data\\TEXTURE\\ui_button\\";
	std::map<CInputGamepad::BUTTON, std::string> TEXTURE_PATH =
	{
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_UP,			"button_up.png"),
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_DOWN,		"button_down.png"),
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_LEFT,		"button_left.png"),
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_RIGHT,		"button_right.png"),
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_START,		"button_start.png"),
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_BACK,		"button_back.png"),
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_LSTICKPUSH,	"button_ls.png"),
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_RSTICKPUSH,	"button_rs.png"),
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_LB,			"button_lb.png"),
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_RB,			"button_rb.png"),
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_LT,			"button_lt.png"),
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_RT,			"button_rt.png"),
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_A,			"button_a.png"),
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_B,			"button_b.png"),
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_X,			"button_x.png"),
		std::pair<CInputGamepad::BUTTON,std::string>(CInputGamepad::BUTTON::BUTTON_Y,			"button_y.png"),
	};
}


//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CKeyConfigManager* CKeyConfigManager::m_pInstance = nullptr;

//==========================================================================
// コンストラクタ
//==========================================================================
CKeyConfigManager::CKeyConfigManager()
{
	// 値のクリア
	for (int i = 0; i < Control::CONTROL_MAX; i++) {
		m_apKeyConfig[i] = nullptr;
	}
}

//==========================================================================
// インスタンス生成
//==========================================================================
CKeyConfigManager* CKeyConfigManager::Create()
{
	if (m_pInstance != nullptr) { return m_pInstance; }	// インスタンスが存在
	m_pInstance = DEBUG_NEW CKeyConfigManager;
	m_pInstance->ConfigCreate();	// コンフィグ生成

	return m_pInstance;
}

//==========================================================================
// 終了処理
//==========================================================================
void CKeyConfigManager::Uninit()
{
	for (int i = 0; i < CONTROL_MAX; i++)
	{
		if (m_apKeyConfig[i] == nullptr) { continue; }	// 使用されていない

		// 廃棄
		m_apKeyConfig[i]->Uninit();
		delete m_apKeyConfig[i];
		m_apKeyConfig[i] = nullptr;
	}

	if (m_pInstance == nullptr) { return; }	// インスタンスが存在しない

	// 終了処理
	delete m_pInstance;
	m_pInstance = nullptr;
}

//==========================================================================
// 内部に登録
//==========================================================================
bool CKeyConfigManager::Bind(CKeyConfig* pConfig, const int type)
{
	// エラーチェック
	if (type >= Control::CONTROL_MAX || type < Control::CONTROL_INPAD) { return false; }	// 範囲外
	if (m_apKeyConfig[type] != nullptr) { return false; }	// すでに登録済み

	m_apKeyConfig[type] = pConfig;	// 登録

	return true;
}

//==========================================================================
// コンフィグ生成
//==========================================================================
void CKeyConfigManager::ConfigCreate()
{
	if (m_pInstance == nullptr) { return; }	// インスタンス無し

	for (int i = 0; i < Control::CONTROL_MAX; i++) {

		CKeyConfig* p = nullptr;
		switch (i) 
		{
		case Control::CONTROL_INPAD:
			p = DEBUG_NEW CkeyConfigPad;
			break;

		case Control::CONTROL_OUTPAD:
			p = DEBUG_NEW CkeyConfigPad;
			break;

		case Control::CONTROL_INKEY:
			p = DEBUG_NEW CkeyConfigKeyboard;
			break;

		case Control::CONTROL_OUTKEY:
			p = DEBUG_NEW CkeyConfigKeyboard;
			break;

		default:

			break;
		}

		if (p == nullptr) { continue; }	// 生成失敗
		p->Load(CONFIGFILE[i]);
		int h = DIK_SPACE;
		if (!m_pInstance->Bind(p, i))	// 登録失敗
		{
			p->Uninit();
			delete p;
		}
	}
}

//==========================================================================
// コンフィグ用テクスチャ読み込み
//==========================================================================
void CKeyConfigManager::ConfigTextureLoad()
{
	// キーコンフィグボタンテクスチャ読み込み
	for (auto itr = KEY_PAD::TEXTURE_PATH.begin(); itr != KEY_PAD::TEXTURE_PATH.end(); itr++)
	{
		int nIdx = CTexture::GetInstance()->Regist(KEY_PAD::FOLDER_PATH + (*itr).second);
		CControlKeyDisp::Load((*itr).first, nIdx);
	}
}