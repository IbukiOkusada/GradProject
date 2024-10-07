//=============================================================================
//
// キーボードキーコンフィグ処理 [keyconfig_keyboard.cpp]
// Author : Ibuki Okusada
//
//=============================================================================
#include "keyconfig_keyboard.h"
#include "input_keyboard.h"

//==========================================================================
// コンストラクタ
//==========================================================================
CkeyConfigKeyboard::CkeyConfigKeyboard()
{
	m_Info.clear();
}

//==========================================================================
// プレス取得
//==========================================================================
bool CkeyConfigKeyboard::GetPress(const int type, const int nId)
{
	if (m_Info.find(type) == m_Info.end()) { return false; }	// 存在しない

	CInputKeyboard* pInputKeyboard = CInputKeyboard::GetInstance();

	return pInputKeyboard->GetPress(m_Info[type]);
}

//==========================================================================
// トリガー取得
//==========================================================================
bool CkeyConfigKeyboard::GetTrigger(const int type, const int nId)
{
	// 確認
	if (m_Info.find(type) == m_Info.end()) { return false; }	// 存在しない

	CInputKeyboard* pInputKeyboard = CInputKeyboard::GetInstance();

	return pInputKeyboard->GetTrigger(m_Info[type]);
}

//==========================================================================
// リリース取得
//==========================================================================
bool CkeyConfigKeyboard::GetRelease(const int type, const int nId)
{
	// 確認
	if (m_Info.find(type) == m_Info.end()) { return false; }	// 存在しない

	CInputKeyboard* pInputKeyboard = CInputKeyboard::GetInstance();

	return pInputKeyboard->GetRelease(m_Info[type]);
}

//==========================================================================
// リピート処理
//==========================================================================
bool CkeyConfigKeyboard::GetRepeat(const int type, const int nId)
{
	// 確認
	if (m_Info.find(type) == m_Info.end()) { return false; }	// 存在しない

	CInputKeyboard* pInputKeyboard = CInputKeyboard::GetInstance();

	return pInputKeyboard->GetRepeat(m_Info[type], 50);
}

//==========================================================================
// 種類取得
//==========================================================================
int CkeyConfigKeyboard::GetKey(const int type, const int nId)
{
	// 確認
	if (m_Info.find(type) == m_Info.end()) { return 0; }	// 存在しない

	return m_Info[type];
}

//==========================================================================
// 終了処理
//==========================================================================
void CkeyConfigKeyboard::Uninit() {
	m_Info.clear();
}

//==========================================================================
// キー情報登録
//==========================================================================
void CkeyConfigKeyboard::Set(const int action, const int key)
{
	m_Info[action] = key;
}

//==========================================================================
// 保存情報読み込み
//==========================================================================
void CkeyConfigKeyboard::Load(const std::string& file)
{
	// ファイルを開く
	std::ifstream File(file);
	if (!File.is_open()) {
		return;
	}

	// コメント用
	std::string hoge;

	// データ読み込み
	std::string line;
	while (std::getline(File, line))
	{
		// コメントはスキップ
		if (line.empty() ||
			line[0] == '#')
		{
			continue;
		}

		// ストリーム作成
		std::istringstream lineStream(line);

		if (line.find("KEYSET") != std::string::npos)
		{// MODELSETで配置情報読み込み
			int process = -1;
			int button = -1;

			// 読み込み情報

			while (line.find("END_KEYSET") == std::string::npos)
			{
				std::getline(File, line);
				if (line.find("PROCESS") != std::string::npos)
				{// TYPEで配置物の種類

					// ストリーム作成
					std::istringstream lineStream(line);

					// 情報渡す
					lineStream >>
						hoge >>
						hoge >>	// ＝
						process;	// 配置物の種類
					continue;
				}

				if (line.find("BUTTON") != std::string::npos)
				{// POSで位置

					// ストリーム作成
					std::istringstream lineStream(line);

					// 情報渡す
					lineStream >>
						hoge >>
						hoge >>						// ＝
						button;	// 位置
					continue;
				}
			}

			// 生成
			if (process >= 0 && button >= 0)
			{
				Set(process, button);
			}
		}

		if (line.find("END_SCRIPT") != std::string::npos)
		{
			break;
		}
	}

	// ファイルを閉じる
	File.close();
}

//==========================================================================
// 設定
//==========================================================================
void CkeyConfigKeyboard::Setting(const int type, const int nId)
{
	CInputGamepad* pKeyboard = CInputGamepad::GetInstance();

	while (1)
	{
		for (int i = 0; i < CInputGamepad::BUTTON_MAX; i++) {
			if (pKeyboard->GetTrigger(static_cast<CInputGamepad::BUTTON>(i), 0))
			{
				Set(type, static_cast<CInputGamepad::BUTTON>(i));
				return;
			}
			else if (pKeyboard->GetPressLT(0))
			{
				Set(type, CInputGamepad::BUTTON_LT);
				return;
			}
			else if (pKeyboard->GetPressRT(0))
			{
				Set(type, CInputGamepad::BUTTON_RT);
				return;
			}
		}
	}
}