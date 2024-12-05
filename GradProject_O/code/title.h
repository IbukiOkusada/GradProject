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
class CNumber;

//<**********************************************
//名前宣言(ココでしか使わない変数)
//<**********************************************
namespace INITIAL
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
	const enum OBJ2D
	{
		OBJ2D_TeamLogo = 0,		//チームロゴ
		OBJ2D_BLACKCOVER,		//黒カバー
		OBJ2D_TITLELOGO,		//タイトルロゴ
		OBJ2D_PressEnter,		//プレスエンター
		OBJ2D_FRAME,			//選択肢
		OBJ2D_NUMCHAR,			//"何人選択しますか"の文字
		OBJ2D_CHECK,			//確認メッセージ
		OBJ2D_MAX

	};
	// ステート
	const enum STATE
	{
		STATE_TEAMLOGO = 0,	//チームロゴ
		STATE_PRE,			//仮名
		STATE_PRESSENTER,	//プレスエンター
		STATE_CHASING,		//警察がプレイヤーを追跡中
		STATE_ICETHROW,		//アイスを投げ入れるシーン
		STATE_MAX
	};

	//シングルかマルチかの選択肢
	const enum SELECT
	{
		SELECT_SINGLE = 0,		//シングル
		SELECT_MULTI,			//マルチ
		SELECT_MAX
	};

	//選択肢
	const enum SELECT_YN
	{
		SELECT_YN_YES = 0,
		SELECT_YN_NO,
		SELECT_YN_MAX
	};

	// メンバ関数
	CTitle();	//コンストラクタ
	~CTitle();	//デストラクタ

	//メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//<************************************
	//Get系関数
	//<************************************
	//シングルとマルチどっちが選択されているかの取得関数
	int GetSelectSingleMulti(void) { return m_nSelect; }

	//警察取得関数
	static CPoliceTitle* GetPoliTitle(int nNum)
	{
		//中身なければnullptrを返す
		if (!m_apPolice[nNum]){return nullptr;}

		//中身あればその警察情報を返す
		else{return m_apPolice[nNum];}
	}

private:

	//サイズに関係する列挙型
	const enum SIZING
	{
		SIZING_WIDTH = 0,	//横
		SIZING_HEIGHT,		//縦
		SIZING_MAX
	};

	//チームロゴの際に使う関数
	void StateLogo(void);

	//仮名の際に使う関数
	void StatePre(void);

	//プレスエンターステートの時に使う関数
	void MoveP_E(void);
	void StateP_E(void);
	void InitingP_E(void);
	void ColChange(CObject2D *pObj2D);
	void MovingLogo(void);
	void BlackCoverM(void);
	void SkipMovement(void);
	void PreMove(void);
	void LightOff(void);

	//追跡状態の時の動き
	void ChaseMovement(void);
	void Chasing(void);
	void ChaseCamera(void);

	//選択
	void InitingSelect(void);
	void Sizing(void);
	void Selecting(void);
	void SelectSingleMulti(void);
	void SelectYesNO(void);
	void SelectCol(void);

	//アイスステート
	void IceMovement(void);
	void InitingIce(void);

	//デバッグ用
	void DebugCam(void);

	//オブジェクト2Dの初期化
	CObject2D* InitObj2D(const D3DXVECTOR3 rPos,		
		const D3DXVECTOR3 rRot,								
		const int nPri,										
		const float fWidth,									
		const float fHeight,								
		const bool bDraw,									
		const char* pTexName,								
		const D3DXCOLOR rCol = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f));

	STATE m_eState;								//ステート
	//<************************************
	//ポインタ型
	//<************************************
	CPlayerTitle* m_pPlayer;					//プレイヤーのポインタ
	static CPoliceTitle* m_apPolice[INITIAL::POLICE_MAX];		//警察のポインタ
	CGole* m_pGoal;								//ゴールのポインタ

	CObject2D* m_pObject2D[OBJ2D::OBJ2D_MAX];			//オブジェクト2Dのポインタ
	CObject2D* m_apSelect[SELECT::SELECT_MAX];	//シングルとマルチの選択肢のポインタ
	CObject2D* m_apYesNoObj[SELECT_YN::SELECT_YN_MAX];		//YESとNOの選択肢のポインタ
	CCamera* m_pCam;

	//<************************************
	//int型
	//<************************************
	int m_nCounterRanking;						//ランキング自動遷移タイマー
	int m_nLogoAlpgha;							//タイトルロゴの色変化タイマー
	int m_nLogoCou;								//ライトオフするまでのタイマー
	int m_nCounter;								//汎用カウンター
	int m_nSelect;								//シングルとマルチどちらが選択されているか
	int m_nSelectYN;							//YESとNOどっちが選択されているか

	float m_fDis;								//距離変数

	//<************************************
	//bool型
	//<************************************
	bool m_bPush;								//チュートリアル遷移に必要なボタンが押されているか
	bool m_bDisplay;							//画面に映すかどうか
	bool m_bIniting;							//オブジェクトの初期化が終わったかどうかのチェック
	bool m_bCol;								//色変更しているかどうか		
	bool m_bNext;								//次に行けるかの是非
	bool m_bSkipped;							//スキップしたかどうか
	bool m_bSizing;								//サイズ調整が完了したかどうか
	bool m_bSelected;							//セレクトされたかどうか
};

#endif