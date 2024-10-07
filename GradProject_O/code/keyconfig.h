//=============================================================================
//
// キーコンフィグヘッダー [keyconfig.h]
// Author : Ibuki Okusada
//
//=============================================================================
#ifndef _KEYCONFIG_H_
#define _KEYCONFIG_H_	// 二重インクルード防止

// インゲーム
namespace INGAME
{
	enum ACTION
	{
		ACT_OK = 0,		// 決定
		ACT_BACK,		// 戻る
		ACT_UPDOWN,		// 浮上
		ACT_AIR,		// 空気
		ACT_CHECKPOINT,	// チェックポイント
		ACT_RETRY,		// リトライ
		ACT_PAUSE,		// ポーズ
		ACT_MAX
	};
}

// アウトゲーム
namespace OUTGAME
{
	enum ACTION
	{
		ACT_OK = 0,		// 決定
		ACT_BACK,		// 戻る
		ACT_MAX
	};
}

//==========================================================================
// クラス定義
//==========================================================================
// キーコンフィグ
class CKeyConfig
{
public:

	// コンストラクタ
	CKeyConfig() {}
	virtual ~CKeyConfig(){}

	//=============================
	// メンバ関数
	//============================= 
	virtual void Uninit() = 0;
	virtual bool GetPress(const int type, const int nId = 0) = 0;
	virtual bool GetTrigger(const int type, const int nId = 0) = 0;
	virtual bool GetRelease(const int type, const int nId = 0) = 0;
	virtual bool GetRepeat(const int type, const int nId = 0) = 0;
	virtual void Load(const std::string& file) = 0;
	virtual void Setting(const int type, const int nId = 0) = 0;
	virtual void Join(const int type, const int nKey, const int nId = 0) = 0;
	virtual int GetKey(const int type, const int nId = 0) = 0;
};

//==========================================================================
// キーコンフィグマネージャー
//==========================================================================
class CKeyConfigManager
{
public:
	// 操作種類列挙
	enum Control
	{
		CONTROL_INPAD = 0,	// インゲームパッド
		CONTROL_OUTPAD,		// アウトゲームパッド
		CONTROL_INKEY,		// インゲームキーボード
		CONTROL_OUTKEY,		// アウトゲームキーボード
		CONTROL_MAX
	};

private:

	// コンストラクタ
	CKeyConfigManager();
	virtual ~CKeyConfigManager() {}

public:

	// メンバ関数
	void Uninit();

	// 静的メンバ関数
	static CKeyConfigManager* GetInstance() { return m_pInstance; }
	static CKeyConfigManager* Create();
	static void ConfigTextureLoad();
	bool Bind(CKeyConfig* pConfig, const int type);
	CKeyConfig* GetConfig(const Control type) { return m_apKeyConfig[type]; }

private:

	// メンバ関数
	void ConfigCreate();

	// 静的メンバ変数
	static CKeyConfigManager* m_pInstance;

	// メンバ変数
	CKeyConfig* m_apKeyConfig[Control::CONTROL_MAX];	// 情報格納場所
};

#endif