//===============================================
//
// ゲーム画面の管理処理 [game.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _GAME_H_		// このマクロが定義されていない場合
#define _GAME_H_		// 二重インクルード防止用マクロを定義

#include "manager.h"
#include <mutex>
#include "objectsound.h"
// 前方宣言
class CScore;
class CTime;
class CPlayer;
class CFileLoad;
class CClient;
class CMeshDome;
class CPause;
class CSpeedMeter;
class CDeliveryStatus;
class CTimer;
class CMultiCamera;
class CGoalManager;

class CScrollText2D;
// マクロ定義
#define NUM_FILTER	(2)

//===============================================
// ゲームクラスの定義(派生クラス)
//===============================================
class CGame : public CScene
{
public:

	
	enum STATE
	{
		STATE_NONE = 0,
		STATE_PROG,
		STATE_SUCCESS,
		STATE_FAIL,
		STATE_MAX
	};
public:

	// メンバ関数
	CGame();	// コンストラクタ
	CGame(int nNumPlayer);	// コンストラクタ
	~CGame();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static void SetNumPlayer(int nNum) { m_nNumPlayer = nNum; }
	static int GetNumPlayer(void) { return m_nNumPlayer; }
	void SetGameState(STATE state) { m_GameState = state; }
	// メンバ関数(ポインタ)
	CPlayer *GetPlayer(void);
	CFileLoad *GetFileLoad(void);


	void End_Success();
	void End_Fail();
private:

	//=============================
	// 関数リスト
	//=============================
	typedef void(CGame::* CREATE_PL_FUNC)();
	static CREATE_PL_FUNC m_CreatePlayerFunc[];

	// 状態設定関数
	void CreateSinglePlayer();
	void CreateMultiPlayer();

	// メンバ関数
	void StartIntro();  // ゲームスタート時の演出
	bool StartDirection();
	void CreatePolice();
	void CreateCar();

	CFileLoad *m_pFileLoad;		// ファイル読み込みのポインタ
	CPlayer** m_ppPlayer;		// プレイヤーのポインタ
	CMultiCamera **m_ppCamera;	// カメラのポインタ
	CMeshDome *m_pMeshDome;		// メッシュドームのポインタ
	CGoalManager *m_pGoalManager;  // ゴールマネージャーのポインタ

	CDeliveryStatus* m_pDeliveryStatus;  // 配達状況のUIのポインタ
	CTimer* m_pGameTimer;		// タイマーのポインタ
	char m_aAddress[30];		// 接続先サーバーのアドレス
	int m_nSledCnt;				// 現在動作しているスレッド数
	static int m_nNumPlayer;	// プレイ人数
	CPause *m_pPause;			// ポーズ画面
	WSADATA m_wsaData;
	std::mutex m_mutex;
	bool m_bEnd;
	float m_fOpenDoorUISin;
	bool m_bPause;              // ポーズ
	int m_nTotalDeliveryStatus;  // プレイヤーが配達する数
	int m_nStartCameraCount;

	STATE m_GameState;
	CScrollText2D* m_pEndText;
	CMasterSound::CObjectSound* m_pEndSound;
};

#endif