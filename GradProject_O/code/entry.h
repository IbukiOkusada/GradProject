//===============================================
//
// エントリー画面の管理処理 [entry.h]
// Author : Ryosuke Oohara
//
//===============================================
#ifndef _ENTRY_H_		// このマクロが定義されていない場合
#define _ENTRY_H_		// 二重インクルード防止用マクロを定義

#include "manager.h"

class CPlayer;
class CMultiCamera;
class CObjectX;
class CObject2D;
class CGoalManager;
class CScrollText2D;

namespace
{
	const int MAX_PLAYER = 4;      // プレイヤーの最大数
	const int NUM_CONTROL_UI = 4;  // 操作UIの総数
}

//===============================================
// エントリークラスの定義(派生クラス)
//===============================================
class CEntry : public CScene
{
public:

	// 種類列挙型
	enum TYPE
	{
		TYPE_MULTI_WIN,		// マルチ勝利
		TYPE_MULTI_LOSE,	// マルチ敗北
		TYPE_MAX			// 通常
	};

	// 操作UI種類
	enum CONTROL_UI_TYPE
	{
		UI_TURN,   // 旋回
		UI_ACCEL,  // アクセル
		UI_BRAKE,  // ブレーキ
		UI_BOOST,  // ブースト
		UI_MAX
	};

public:

	// メンバ関数
	CEntry();	// コンストラクタ
	~CEntry();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetID(const int id) override;
	void ChangeFlag(bool value) override;
	void EndTutorial(void) override;
	void ReadyUp(const int id) override;

private:

	// メンバ関数
	void AddPlayer(void);       // プレイヤー参加処理
	void DecreasePlayer(void);  // プレイヤー参加取り消し処理
	void ControlsUI(void);      // 操作方法のUI
	void ReadyUp(void);         // 準備OK処理

	// メンバ変数
	CMultiCamera** m_ppCamera;                 // カメラのポインタ
	CObjectX** m_ppObjX;                       // 画面下にでるプレイヤーのポインタ
	CObject2D* m_pControlsUI[NUM_CONTROL_UI];  // 操作UIのポインタ
	CObject2D* m_pReady[MAX_PLAYER];           // 準備UIのポインタ
	CObject2D* m_pPPP = nullptr;
	CGoalManager* m_pGoalManager;              // ゴールマネージャーのポインタ
	CScrollText2D* m_pString;	               // 文字

	int m_nID;
	bool m_IsFinish;   // チュートリアルが終了しているかどうか
	bool m_bSetReady;  // 準備が完了しているかどうか
};

// チュートリアルステップクラス
class CTutorialStep
{
public:
	CTutorialStep();
	~CTutorialStep();

	virtual void Update(CEntry* pEntry) = 0;

private:

};

#endif