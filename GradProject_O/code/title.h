//===============================================
//
// タイトル画面の管理処理 [title.h]
// Author : Ibuki Okusada
//
//===============================================
//改変者
//Kazuki Watanabe
//<==============================================
#ifndef _TITLE_H_		// このマクロが定義されていない場合
#define _TITLE_H_		// 二重インクルード防止用マクロを定義

#include "manager.h"

//<**********************************************
//前方宣言
//<**********************************************
class CFileLoad;
class CObject2D;
class CPlayerTitle;
class CCamera;
class CPoliceTitle;
class CGole;

//<**********************************************
//名前宣言
//<**********************************************
namespace
{
	const int POLICE_MAX = 3;					//警察の数
}

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
		OBJ2D_BLACKCOVER,		//黒カバー
		OBJ2D_TITLELOGO,		//タイトルロゴ
		OBJ2D_PressEnter,		//プレスエンター
		OBJ2D_MAX

	};
	// ステート
	enum STATE
	{
		STATE_TEAMLOGO = 0,	//チームロゴ
		STATE_PRE,			//仮名
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

	//チームロゴの際に使う関数
	void StateLogo(void);

	//仮名の際に使う関数
	void StatePre(void);

	//プレスエンターステートの時に使う関数
	void MoveP_E(void);
	void StateP_E(void);
	void InitingP_E(void);
	void TitleLogo(void);
	void MovingLogo(void);
	void BlackCoverM(void);
	void SkipMovement(void);
	void PreMove(void);

	//追跡状態の時の動き
	void ChaseMovement(void);
	void ChaseCamera(void);

	//デバッグ用
	void DebugCam(void);

	//CFileLoad *m_pFileLoad;				//ファイル読み込みのポインタ
	CPlayerTitle* m_pPlayer;				//プレイヤーのポインタ
	CPoliceTitle* m_apPolice[POLICE_MAX];	//警察のポインタ
	CGole* m_pGoal;							//ゴールのポインタ



	CObject2D* m_pObject2D[OBJ2D_MAX];		//チームロゴのポインタ
	STATE m_eState;							//ステート
	CCamera* m_pCam;

	int m_nCounterRanking;					//ランキング自動遷移タイマー
	int m_nLogoAlpgha;						//タイトルロゴの色変化タイマー
	int m_nCounter;							//汎用カウンター

	D3DXVECTOR3 m_TitlePos;					//タイトルロゴの場所

	bool m_bPush;							//チュートリアル遷移に必要なボタンが押されているか
	bool m_bDisplay;						//画面に映すかどうか
	bool m_bIniting;						//オブジェクトの初期化が終わったかどうかのチェック
	bool m_bCol;							
	bool m_bNext;							//次に行けるかの是非
	bool m_bSkipped;						//スキップしたかどうか
};

#endif