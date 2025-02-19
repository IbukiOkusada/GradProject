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
#include "shaderlight.h"

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
class CFog;
class CScrollText2D;
class CDoll;

namespace Game
{
	const float DOME_LENGTH = 15000.0f;
}

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
		STATE_MULTIEND,
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
	STATE GetState() { return m_GameState; }
	// メンバ関数(ポインタ)
	int GetTotalDeliveryStatus() { return m_nTotalDeliveryStatus; }
	int GetRestDeliveryStatus();

	void End_Success();
	void End_Fail();
	void End_MultiEnd();
	static CGame* GetInstance() { return m_pInstance; }
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
	void CreateCharacter();

	CMeshDome *m_pMeshDome;		// メッシュドームのポインタ
	CGoalManager *m_pGoalManager;  // ゴールマネージャーのポインタ
	CDeliveryStatus* m_pDeliveryStatus;  // 配達状況のUIのポインタ
	CTimer* m_pGameTimer;		// タイマーのポインタ
	int m_nSledCnt;				// 現在動作しているスレッド数
	static int m_nNumPlayer;	// プレイ人数
	CPause *m_pPause;			// ポーズ画面
	bool m_bEnd;
	float m_fOpenDoorUISin;
	bool m_bPause;              // ポーズ
	int m_nTotalDeliveryStatus;  // プレイヤーが配達する数
	int m_nStartCameraCount;
	CDoll* m_pDoll[7];

	STATE m_GameState;
	CScrollText2D* m_pEndText;
	CMasterSound::CObjectSound* m_pEndSound;
	static CGame* m_pInstance;
};

#endif