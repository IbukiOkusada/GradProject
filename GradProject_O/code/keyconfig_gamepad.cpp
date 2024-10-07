//=============================================================================
//
// ゲームパッドキーコンフィグ処理 [keyconfig_gamepad.cpp]
// Author : Ibuki Okusada
//
//=============================================================================
#include "keyconfig_gamepad.h"

//==========================================================================
// コンストラクタ
//==========================================================================
CkeyConfigPad::CkeyConfigPad()
{
	m_Info.clear();
}

//==========================================================================
// プレス取得
//==========================================================================
bool CkeyConfigPad::GetPress(const int type, const int nId)
{
	if (m_Info.find(type) == m_Info.end()) { return false; }	// 存在しない

	CInputGamepad* pInputPad = CInputGamepad::GetInstance();

	if (m_Info[type] == CInputGamepad::BUTTON_LT) {
		return pInputPad->GetPressLT(0);
	}
	else if (m_Info[type] == CInputGamepad::BUTTON_RT) {
		return pInputPad->GetPressRT(0);
	}

	return pInputPad->GetPress(m_Info[type], nId);
}

//==========================================================================
// トリガー取得
//==========================================================================
bool CkeyConfigPad::GetTrigger(const int type, const int nId)
{
	// 確認
	if (m_Info.find(type) == m_Info.end()) { return false; }	// 存在しない

	CInputGamepad* pInputPad = CInputGamepad::GetInstance();

	if (m_Info[type] == CInputGamepad::BUTTON_LT) {
		return pInputPad->GetTriggerLT(0);
	}
	else if (m_Info[type] == CInputGamepad::BUTTON_RT) {
		return pInputPad->GetTriggerRT(0);
	}

	return pInputPad->GetTrigger(m_Info[type], nId);
}

//==========================================================================
// リリース取得
//==========================================================================
bool CkeyConfigPad::GetRelease(const int type, const int nId)
{
	// 確認
	if (m_Info.find(type) == m_Info.end()) { return false; }	// 存在しない

	CInputGamepad* pInputPad = CInputGamepad::GetInstance();
	return pInputPad->GetRelease(m_Info[type], nId);
}

//==========================================================================
// リピート処理
//==========================================================================
bool CkeyConfigPad::GetRepeat(const int type, const int nId)
{
	// 確認
	if (m_Info.find(type) == m_Info.end()) { return false; }	// 存在しない

	CInputGamepad* pInputPad = CInputGamepad::GetInstance();
	return pInputPad->GetRepeat(m_Info[type], nId);
}

//==========================================================================
// キーを取得
//==========================================================================
int CkeyConfigPad::GetKey(const int type, const int nId)
{
	return m_Info[type];
}

//==========================================================================
// 終了処理
//==========================================================================
void CkeyConfigPad::Uninit() {
	m_Info.clear();
}

//==========================================================================
// キー情報登録
//==========================================================================
void CkeyConfigPad::Join(const int action, const CInputGamepad::BUTTON key)
{
	m_Info[action] = key;
}

//==========================================================================
// 保存情報読み込み
//==========================================================================
void CkeyConfigPad::Load(const std::string& file)
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
				Join(process, static_cast<CInputGamepad::BUTTON>(button));
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
void CkeyConfigPad::Setting(const int type, const int nId)
{
	CInputGamepad* pPad = CInputGamepad::GetInstance();

	while (1)
	{
		for (int i = 0; i < CInputGamepad::BUTTON_MAX; i++) {
			if (pPad->GetTrigger(static_cast<CInputGamepad::BUTTON>(i), 0))
			{
				Join(type, static_cast<CInputGamepad::BUTTON>(i));
				return;
			}
			else if (pPad->GetPressLT(0))
			{
				Join(type, CInputGamepad::BUTTON_LT);
				return;
			}
			else if (pPad->GetPressRT(0))
			{
				Join(type, CInputGamepad::BUTTON_RT);
				return;
			}
		}
	}
}