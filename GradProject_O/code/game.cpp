//===============================================
//
// ゲーム画面の管理処理 [game.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "game.h"
#include "manager.h"
#include "renderer.h"
#include "sound.h"
#include "camera.h"
#include "light.h"
#include "texture.h"
#include "Xfile.h"
#include "input.h"
#include "fade.h"
#include "result.h"
#include "debugproc.h"
#include "time.h"
#include "tcp_client.h"
#include <thread>
#include "protocol_online.h"
#include "object2D.h"
#include <assert.h>
#include "ranking.h"
#include "input_gamepad.h"
#include "input_keyboard.h"
#include "player.h"
#include "meshfield.h"
#include "road.h"
#include "road_manager.h"
#include "car.h"
#include "police.h"
#include "car_manager.h"
#include "goal.h"
#include "edit_manager.h"
#include "map_obstacle.h"
#include "map_manager.h"
#include "speedmeter.h"
#include "meter.h"
#include "camera_manager.h"
#include "deliverystatus.h"
#include "player_manager.h"
#include "camera_action.h"
#include "camera_manager.h"
#include "timer.h"
#include "gimmick_firehydrant.h"
#include "navi.h"
#include "bridge.h"
#include "gimmick_policestation.h"
#include "gimmick_guardrail.h"
#include "goal_manager.h"
#include "police_manager.h"
#include "objectsound.h"
#include "scrollText2D.h"
#include "radio.h"
// ネットワーク
#include "network.h"

// 無名名前空間を定義
namespace {
    const int MAX_STRING = (2048);
    const int TOTAL_POINT = 3;  // 配達する総数
    const char* ADDRESSFILE	= "data\\TXT\\address.txt";
    const D3DXVECTOR3 CAMERA_ROT[4] =
    {
        D3DXVECTOR3(0.0f, -2.37f, 1.0f),
        D3DXVECTOR3(0.0f, 2.37f, 1.0f),
        D3DXVECTOR3(0.0f, 0.46f, 0.7f),
        D3DXVECTOR3(0.0f, -0.6f, 1.0f),
    };

    const float CAMERA_LENGHT[4] =
    {
        7000.0f,
        10000.0f,
        15000.0f,
        1000.0f,
    };
}

//===============================================
// 静的メンバ変数
//===============================================
int CGame::m_nNumPlayer = 0;

//===============================================
// 関数ポインタ
//===============================================
// 状態管理
CGame::CREATE_PL_FUNC CGame::m_CreatePlayerFunc[] =
{
    &CGame::CreateSinglePlayer,	// シングルプレイ
    &CGame::CreateMultiPlayer,	// マルチプレイ
    &CGame::CreateSinglePlayer,	// シングルプレイ(例外
};

//===============================================
// コンストラクタ
//===============================================
CGame::CGame()
{
    // 値のクリア
    m_ppCamera = nullptr;
    m_ppPlayer = nullptr;
    m_pFileLoad = nullptr;
    m_pMeshDome = nullptr;
    m_pGoalManager = nullptr;

    m_pDeliveryStatus = nullptr;
    m_pGameTimer = nullptr;
    m_nSledCnt = 0;
    m_bEnd = false;
	m_fOpenDoorUISin = 0.0f;
    m_bPause = false;
    m_pPause = nullptr;
    m_nTotalDeliveryStatus = 0;
    m_nStartCameraCount = 0;
}

//===============================================
// コンストラクタ(人数設定)
//===============================================
CGame::CGame(int nNumPlayer)
{
    // 値のクリア
    m_ppCamera = nullptr;
    m_ppPlayer = nullptr;
    m_pFileLoad = nullptr;
    m_pMeshDome = nullptr;
    m_pGoalManager = nullptr;
  
    m_pDeliveryStatus = nullptr;
    m_pGameTimer = nullptr;
    m_nSledCnt = 0;
    m_bEnd = false;
	m_fOpenDoorUISin = 0.0f;
    m_bPause = false;
    m_pPause = nullptr;
    m_nTotalDeliveryStatus = 0;
    m_nStartCameraCount = 0;
    m_GameState = STATE::STATE_NONE;
    m_pEndSound = nullptr;
    m_pEndText = nullptr;
    // 人数設定
    m_nNumPlayer = nNumPlayer;
}

//===============================================
// デストラクタ
//===============================================
CGame::~CGame()
{
   
}

//===============================================
// 初期化処理
//===============================================
HRESULT CGame::Init(void)
{
    memset(&m_aAddress[0], '\0', sizeof(m_aAddress));

    // 外部ファイル読み込みの生成
    if (nullptr == m_pFileLoad)
    {// 使用していない場合

    }

    // 配達する総数
    m_nTotalDeliveryStatus = TOTAL_POINT;

    CMeshField::Create(D3DXVECTOR3(0.0f, -10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1000.0f, 1000.0f, "data\\TEXTURE\\field000.jpg", 30, 30);

    // マップ読み込み
    CMapManager::GetInstance()->Load();

    auto net = CNetWork::GetInstance();

    // プレイヤー生成
    (this->*(m_CreatePlayerFunc[net->GetState()]))();

    CMeter::Create();
    //CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_GAME);

    int myid = net->GetIdx();

    for (int i = 0; i < NetWork::MAX_CONNECT; i++)
    {
        if (!net->GetConnect(i)) { continue; }

        if (i <= myid) { myid = i; }
    }

    // 自分が先頭の場合
    if (myid == net->GetIdx())
    {
        // 車の生成
        CreateCar();
        // 警察の生成
        CreatePolice();
    }

    if (m_pGoalManager == nullptr)
    {
        m_pGoalManager = CGoalManager::Create();
        if (m_pGoalManager->GetCreateIdx() <= 0)
        {
            m_pGoalManager->Init();
        }
    }

    CCameraManager::GetInstance()->GetTop()->SetRotation(D3DXVECTOR3(0.0f, -D3DX_PI * 0.5f, 0.0f));

    if (m_pDeliveryStatus == nullptr)
    {
        m_pDeliveryStatus = CDeliveryStatus::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.8f, SCREEN_HEIGHT * 0.6f, 0.0f), m_nTotalDeliveryStatus);
    }

    if (m_pGameTimer == nullptr)
    {
        m_pGameTimer = CTimer::Create();
    }

    CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
    CCameraManager::GetInstance()->GetTop()->GetAction()->Set(pCamera, CAMERA_ROT[m_nStartCameraCount], CAMERA_LENGHT[m_nStartCameraCount], 3.0f, 2.0f, CCameraAction::MOVE_POSV, true);

    return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CGame::Uninit(void)
{
   CManager::GetInstance()->GetSound()->Stop();

    m_bEnd = true;

    while (1)
    {
        //OnlineEnd();
        if (m_nSledCnt <= 0)
        {
            break;
        }
    }   

    if (m_pDeliveryStatus != nullptr)
    {
        m_pDeliveryStatus->Uninit();
        m_pDeliveryStatus = nullptr;
    }

    if (m_pGameTimer != nullptr)
    {
        m_pGameTimer->Uninit();
        m_pGameTimer = nullptr;
    }

    if (m_pGoalManager != nullptr)
    {
        m_pGoalManager->Release();
        m_pGoalManager = nullptr;
    }

    // ネットワーク切断
    auto net = CNetWork::GetInstance();
    net->DisConnect();

    // defaultカメラオン
    CManager::GetInstance()->GetCamera()->SetDraw(true);

    //Winsock終了処理
    WSACleanup();	// WSACleanup関数 : winsockの終了処理

    // エディット設定
    CEditManager::Release();

    // マップマネージャー廃棄
    CMapManager::Release();
}

//===============================================
// 更新処理
//===============================================
void CGame::Update(void)
{
	CInputPad *pInputPad = CInputPad::GetInstance();
	CInputKeyboard *pInputKey = CInputKeyboard::GetInstance();

	if (pInputKey->GetTrigger(DIK_P) == true || pInputPad->GetTrigger(CInputPad::BUTTON_START, 0))
	{//ポーズキー(Pキー)が押された
		m_bPause = m_bPause ? false : true;
	}

    if (m_pGameTimer != nullptr)
    {
        CPlayer* player = CPlayerManager::GetInstance()->GetPlayer();
        if (player->GetType() == CPlayer::TYPE::TYPE_ACTIVE)
        {
            m_pGameTimer->Update();
        }
    }

    // ゴールマネージャーの更新
    if (m_pGoalManager != nullptr)
    {
        m_pGoalManager->Update();
    }

    // 開始時の演出
    StartIntro();

    // エディター関連
#if _DEBUG

    CEditManager* pMgr = CEditManager::GetInstance();
    // エディター生成
    if (pInputKey->GetTrigger(DIK_F4) && CEditManager::GetInstance() == nullptr)
    {
        pMgr = CEditManager::Create();
    }

    // エディター更新
    if (pMgr != nullptr) { pMgr->Update(); }

#endif

    if (pInputKey->GetTrigger(DIK_J))
    {
        CPoliceManager::GetInstance()->SetInspection();
}

    CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
    int nNum = pPlayer->GetNumDeliverStatus();
    CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
    if (m_nTotalDeliveryStatus <= nNum && pCamera->GetAction()->IsFinish() && CNetWork::GetInstance()->GetState() == CNetWork::STATE::STATE_SINGLE)
    {// 配達する総数以上かつカメラの演出が終了している

        End_Success();
    }
    else if (m_pGameTimer != nullptr)
    {
        if (m_pGameTimer->GetTime() <= 0.0f)
        {
            End_Fail();
        }
    }
   
    auto net = CNetWork::GetInstance();
    auto mgr = CPlayerManager::GetInstance();

    // 人数確認
    if (net->GetState() == CNetWork::STATE::STATE_ONLINE)
    {
        for (int i = 0; i < NetWork::MAX_CONNECT; i++)
        {
            auto player = mgr->GetPlayer(i);

            // 人数が多い
            if (player != nullptr && !net->GetConnect(i))
            {
                player->Uninit();
            }
        }
    }
    if (mgr->GetPlayer()->GetLife()<= 0.0f && net->GetState() == CNetWork::STATE::STATE_ONLINE)
    {
        End_Fail();
    }
    CPoliceManager::GetInstance()->Update();
    CScene::Update();
    switch (m_GameState)
    {
    case CGame::STATE_NONE:
        break;
    case CGame::STATE_PROG:
        break;
    case CGame::STATE_SUCCESS:
        pPlayer->GetRadio()->SetVol(pPlayer->GetRadio()->GetVol() * 0.9f);
        if (!m_pEndSound->GetPlay())
        {
            CManager::GetInstance()->GetFade()->Set(CScene::MODE_RESULT);
        }
        break;
    case CGame::STATE_FAIL:
        pPlayer->GetRadio()->SetVol(pPlayer->GetRadio()->GetVol()*0.9f);
        if (!m_pEndSound->GetPlay())
        {
            CManager::GetInstance()->GetFade()->Set(CScene::MODE_RESULT);
        }
        break;
    case CGame::STATE_MAX:
        break;
    default:
        break;
    }
  
}

//===============================================
// 描画処理
//===============================================
void CGame::Draw(void)
{
    CScene::Draw();
}

//===================================================
// プレイヤーの取得
//===================================================
CPlayer *CGame::GetPlayer(void)
{
    return *m_ppPlayer;
}

//===================================================
// ファイル読み込みの取得
//===================================================
CFileLoad *CGame::GetFileLoad(void)
{
    return m_pFileLoad;
}

//===================================================
// 開始時の演出
//===================================================
void CGame::StartIntro(void)
{
    if (m_nStartCameraCount >= 4)
        return;

    CCamera* pCamera = CCameraManager::GetInstance()->GetTop();

    if (pCamera->GetAction()->IsNext() && pCamera->GetAction()->IsPause() && m_nStartCameraCount < 3)
    {
        m_nStartCameraCount++;
        CCameraManager::GetInstance()->GetTop()->GetAction()->Set(pCamera, CAMERA_ROT[m_nStartCameraCount], CAMERA_LENGHT[m_nStartCameraCount], 2.0f, 2.0f, CCameraAction::MOVE_POSV, true);
    }
    else if (m_nStartCameraCount >= 3)
    {
        CCameraManager::GetInstance()->GetTop()->GetAction()->Set(pCamera, CAMERA_ROT[m_nStartCameraCount], CAMERA_LENGHT[m_nStartCameraCount], 2.0f, 2.0f, CCameraAction::MOVE_POSV, false);
        m_nStartCameraCount++;
    }
}

//===================================================
// 開始演出
//===================================================
bool CGame::StartDirection(void)
{
    return false;
}

//===================================================
// 終了演出
//===================================================
void CGame::End_Success()
{
    if (m_GameState != STATE_SUCCESS)
    {
        SetGameState(STATE::STATE_SUCCESS);
        m_pEndText = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, SCREEN_CENTER, 1.0f, 200.0f, 200.0f, XALIGN_CENTER, YALIGN_CENTER, VECTOR3_ZERO, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
        m_pEndText->PushBackString("配達完了");
        m_pEndText->SetEnableScroll(true);
        m_pEndSound = CMasterSound::CObjectSound::Create("data\\SE\\OPED35.wav", 0);
    }
 
}
void CGame::End_Fail()
{
    if (m_GameState != STATE_FAIL)
    {
        SetGameState(STATE::STATE_FAIL);
        m_pEndText = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, SCREEN_CENTER, 0.7f, 200.0f, 200.0f, XALIGN_CENTER, YALIGN_CENTER, VECTOR3_ZERO, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
        m_pEndText->PushBackString("配達失敗");
        m_pEndText->SetEnableScroll(true);
        m_pEndSound = CMasterSound::CObjectSound::Create("data\\SE\\f_jingle.wav", 0);
    }
}
//===================================================
// プレイヤーの生成(シングル)
//===================================================
void CGame::CreateSinglePlayer(void)
{
    auto net = CNetWork::GetInstance();

    CPlayer* pPlayer = CPlayer::Create(D3DXVECTOR3(-3034.65f, 1.0f, 1.0f), 
        VECTOR3_ZERO, VECTOR3_ZERO, CNetWork::GetInstance()->GetIdx());
    pPlayer->SetType(CPlayer::TYPE::TYPE_ACTIVE);
}

//===================================================
// プレイヤーの生成(マルチ)
//===================================================
void CGame::CreateMultiPlayer(void)
{
    auto net = CNetWork::GetInstance();

    for (int i = 0; i < NetWork::MAX_CONNECT; i++)
    {
        if (!net->GetConnect(i)) { continue; }

        CPlayer* pPlayer = CPlayer::Create(D3DXVECTOR3(-3034.65f, 1.0f, 1.0f + 20.0f * i),
            VECTOR3_ZERO, VECTOR3_ZERO, i);

        // プレイヤー自身
        if (i == net->GetIdx())
        {
            pPlayer->SetType(CPlayer::TYPE::TYPE_ACTIVE);
            pPlayer->SetType(CPlayer::TYPE::TYPE_SEND);
        }
        else
        {
            pPlayer->SetType(CPlayer::TYPE::TYPE_RECV);
        }
    }
}

//===================================================
// 警察の生成
//===================================================
void CGame::CreatePolice()
{
    for (int i = 0; i < 1; i++)
    {
        CCar* pCar = CPolice::Create(D3DXVECTOR3(3000.0f + 1000.0f * i, 0.0f, 1000.0f * i), 
            D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), CCarManager::GetInstance()->GetMapList()->GetInCnt());
        pCar->SetType(CCar::TYPE::TYPE_ACTIVE);
    }
}

//===================================================
// 車の生成
//===================================================
void CGame::CreateCar()
{
    for (int i = 0; i < 1; i++)
    {
        CCar* pCar = CCar::Create(D3DXVECTOR3(-3000.0f - 1000.0f * i, 0.0f, 1000.0f * i), 
            D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), CCarManager::GetInstance()->GetMapList()->GetInCnt());
        pCar->SetType(CCar::TYPE::TYPE_ACTIVE);
    }
}