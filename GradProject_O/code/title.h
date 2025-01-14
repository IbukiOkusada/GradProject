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
#include "objectsound.h"

//<**********************************************
//前方宣言
//<**********************************************
class CCamera;
class CObject2D;
class CPlayerTitle;
class CPoliceTitle;
//<**********************************************
//名前宣言(ココでしか使わない変数)
//<**********************************************
namespace
{
	const int POLICE_MAX = 3;					//警察の数


	//効果音名(ココでしか使わないのでstaticにしました)
	const enum TITLE_SE : unsigned int
	{
		TITLE_SE_SELECT = 0,	//選択
		TITLE_SE_CANCEL,		//キャンセル
		TITLE_SE_DECIDE0,		//決定1
		TITLE_SE_DECIDE1,		//決定2
		TITLE_SE_MAX
	};

	//目的地ナンバー
	const enum DEST : unsigned int
	{
		DEST_FIRST = 0,	//一番目
		DEST_SECOND,	//二番目
		DEST_THIRD,		//三番目
		DEST_FOUTH,		//四番目
		DEST_FIFTH,		//五番目
		DEST_SIXTH,		//六番目
		DEST_MAX,

	};

	//目的地の位置
	const D3DXVECTOR3 DEST_POS_SELECT[DEST::DEST_MAX] =
	{
		D3DXVECTOR3(3000.0f, 0.0f, 6600.0f),		//１個目
		D3DXVECTOR3(-3160.0f, 0.0f, 6000.0f),		//２個目
		D3DXVECTOR3(-3160.0f, 0.0f, 3300.0f),		//３個目
		D3DXVECTOR3(-5050.0f, 0.0f, 4000.0f),		//４個目
		D3DXVECTOR3(-4750.0f, 0.0f, -200.0f),		//５個目
		D3DXVECTOR3(3000.0f, 0.0f, -200.0f)			//６個目
	};

	//目的の向き
	const D3DXVECTOR3 DEST_ROT_SELECT[DEST::DEST_MAX] =
	{
		D3DXVECTOR3(0.0f,-3.14f,0.0f),				//１個目
		D3DXVECTOR3(0.0f,1.57f,0.0f),				//２個目
		D3DXVECTOR3(0.0f,0.0f,0.0f),				//３個目
		D3DXVECTOR3(0.0f,1.57f,0.0f),				//４個目
		D3DXVECTOR3(0.0f,0.0f,0.0f),				//５個目
		D3DXVECTOR3(0.0f,-1.57f,0.0f)				//６個目
	};

	//タイトルに使うSEのファイル名(ココでしか使わないのでstaticにしました)
	static const std::string TitleSEName[TITLE_SE_MAX] =
	{
		"data/SE/TITLE/Title-Select.wav",		//選択
		"data/SE/TITLE/Title-Cancel.wav",		//キャンセル
		"data/SE/TITLE/Title-Decide0.wav",		//決定音1
		"data/SE/TITLE/Title-Decide1.wav",		//決定音2
	};
}
//===============================================
// タイトルクラスの定義(派生クラス)
//===============================================
class CTitle : public CScene
{
public:

	// メンバ関数
	CTitle();	//コンストラクタ
	~CTitle();	//デストラクタ

	//メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void) { CScene::Draw(); }

	//<************************************
	//Get系関数
	//<************************************
	//シングルとマルチどっちが選択されているかの取得関数
	inline int GetSelectSingleMulti(void) { return m_nSelect; }

	//プレイヤー取得
	inline static CPlayerTitle* GetPlayer(void)
	{
		//中身なければnullptrを返す
		if (!m_pPlayer) { return nullptr; }

		//中身あればその警察情報を返す
		else { return m_pPlayer; }
	}
private:

	// オブジェクト2Dの列挙型
	const enum OBJ2D : unsigned int
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
	const enum STATE : unsigned int
	{
		STATE_TEAMLOGO = 0,	//チームロゴ
		STATE_PRE,			//仮名
		STATE_PRESSENTER,	//プレスエンター
		STATE_CHASING,		//警察がプレイヤーを追跡中
		STATE_ICETHROW,		//アイスを投げ入れるシーン
		STATE_MAX
	};

	//シングルかマルチかの選択肢
	const enum SELECT : unsigned int
	{
		SELECT_SINGLE = 0,		//シングル
		SELECT_MULTI,			//マルチ
		SELECT_MAX
	};

	//選択肢
	const enum SELECT_YN : unsigned int
	{
		SELECT_YN_YES = 0,
		SELECT_YN_NO,
		SELECT_YN_MAX
	};

	//サイズに関係する列挙型
	const enum SIZING : unsigned int
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
	void ColChange(CObject2D *pObj2D,const int nCntMax);
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

	bool TriggerEnter(void);

	void SelectNum(int *nNum,const int nMax);

	//効果音再生
	inline void SEPlay(const TITLE_SE eTitleSE)
	{
		m_apMSound[eTitleSE]->Play();
	}

	//オブジェクト2Dの初期化
	CObject2D* InitObj2D(const D3DXVECTOR3 rPos,		
		const D3DXVECTOR3 rRot,								
		const int nPri,										
		const float fWidth,									
		const float fHeight,								
		const bool bDraw,									
		const char* pTexName,								
		const D3DXCOLOR rCol = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f));

	STATE m_eState;												//ステート
	//<************************************
	//ポインタ型
	//<************************************
	static CPlayerTitle* m_pPlayer;								//プレイヤーのポインタ
	CPoliceTitle* m_apPolice[POLICE_MAX];						//警察のポインタ

	CObject2D* m_pObject2D[OBJ2D::OBJ2D_MAX];					//オブジェクト2Dのポインタ
	CObject2D* m_apSelect[SELECT::SELECT_MAX];					//シングルとマルチの選択肢のポインタ
	CObject2D* m_apYesNoObj[SELECT_YN::SELECT_YN_MAX];			//YESとNOの選択肢のポインタ
	CCamera* m_pCam;
	CMasterSound::CObjectSound* m_apMSound[TITLE_SE_MAX];

	//<************************************
	//int型
	//<************************************
	int m_nCounterRanking;						//ランキング自動遷移タイマー
	int m_nLogoAlpgha;							//タイトルロゴの色変化タイマー
	int m_nLogoCou;								//ライトオフするまでのタイマー
	int m_nLogoStart;
	int m_nBlinkCount;
	int m_nCounter;								//汎用カウンター
	int m_nSelect;								//シングルとマルチどちらが選択されているか
	int m_nSelectYN;							//YESとNOどっちが選択されているか

	float m_fDis;								//距離変数

	//<************************************
	//bool型
	//<************************************
	bool m_bPush;								//チュートリアル遷移に必要なボタンが押されているか
	bool m_bIniting;							//オブジェクトの初期化が終わったかどうかのチェック
	bool m_bCol;								//色変更しているかどうか		
	bool m_bNext;								//次に行けるかの是非
	bool m_bSkipped;							//スキップしたかどうか
	bool m_bSizing;								//サイズ調整が完了したかどうか
	bool m_bSelected;							//セレクトされたかどうか
};

#endif