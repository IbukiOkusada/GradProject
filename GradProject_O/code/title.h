//===============================================
//
// タイトル画面の管理処理 [title.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _TITLE_H_		// このマクロが定義されていない場合
#define _TITLE_H_		// 二重インクルード防止用マクロを定義

#include "manager.h"

class CFileLoad;
class CTitleEnter;
class CPlayer;
class CObject2D;

//===============================================
// タイトルクラスの定義(派生クラス)
//===============================================
class CTitle : public CScene
{
public:

	// オブジェクト2Dの列挙型
	enum OBJ2D
	{
		OBJ2D_TeamLogo = 0,		//チームロゴ
		OBJ2D_PressEnter,		//プレスエンター
		OBJ2D_MAX

	};
	// ステート
	enum STATE
	{
		STATE_TEAMLOGO = 0,	//チームロゴ
		STATE_PRESSENTER,	//プレスエンター
		STATE_CHASING,		//警察がプレイヤーを追跡中
		STATE_ICETHROW,		//アイスを投げ入れるシーン
		STATE_MAX
	};


	// メンバ関数
	CTitle();	// コンストラクタ
	~CTitle();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:

	void StateLogo(void);
	void StateP_E(void);
	void InitingP_E(void);
	void ColChange(void);


	CFileLoad *m_pFileLoad;		// ファイル読み込みのポインタ
	CTitleEnter *m_pEnter;
	CPlayer* m_pPlayer;						// プレイヤーのポインタ
	CObject2D* m_pObject2D[OBJ2D_MAX];		// チームロゴのポインタ
	STATE m_eState;							// ステート
	int m_nCounterTutorial;					// チュートリアル及び人数選択画面遷移タイマー
	int m_nCounterRanking;					// ランキング自動遷移タイマー

	int m_nCounter;							//汎用カウンター

	bool m_bPush;							// チュートリアル遷移に必要なボタンが押されているか
	bool m_bDisplay;						// 画面に映すかどうか
	bool m_bIniting;						// オブジェクトの初期化が終わったかどうかのチェック
	bool m_bCol;
};

#endif