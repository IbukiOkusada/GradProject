//=============================================================================
//
// ゲームパッドキーコンフィグヘッダー [keyconfig_gamepad.h]
// Author : Ibuki Okusada
//
//=============================================================================
#ifndef _KEYCONFIG_GAMEPAD_H_
#define _KEYCONFIG_GAMEPAD_H_	// 二重インクルード防止

#include "keyconfig.h"
#include "input_gamepad.h"

//==========================================================================
// クラス定義
//==========================================================================
// キーコンフィグ
class CkeyConfigPad: public CKeyConfig
{
public:

	// コンストラクタ
	CkeyConfigPad();
	~CkeyConfigPad(){}

	// メンバ関数
	void Uninit() override;
	bool GetPress(const int type, const int nId = 0) override;
	bool GetTrigger(const int type, const int nId = 0) override;
	bool GetRelease(const int type, const int nId = 0) override;
	bool GetRepeat(const int type, const int nId = 0) override;
	void Load(const std::string& file) override;
	void Setting(const int type, const int nId = 0) override;
	int GetKey(const int type, const int nId = 0) override;
	void Join(const int type, const int nKey, const int nId = 0) override { Join(type, static_cast<CInputGamepad::BUTTON>(nKey)); }

public:
	void Join(const int action, const CInputGamepad::BUTTON key);

private:

	// メンバ変数
	std::map<int, CInputGamepad::BUTTON> m_Info;

};

#endif