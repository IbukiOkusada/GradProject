//===============================================
//
// ゲーム画面の管理処理 [game.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "game.h"

// system
#include "manager.h"
#include "renderer.h"
#include "magic_enum/magic_enum.hpp"
#include "debugproc.h"
#include "deltatime.h"
#include "objectsound.h"
#include "scrollText2D.h"
#include "fog.h"

// input
#include "input.h"
#include "input_keyboard.h"
#include "input_gamepad.h"

// edit
#include "edit_manager.h"

// camera
#include "camera.h"
#include "camera_action.h"
#include "camera_manager.h"

// map
#include "robot_manager.h"
#include "robot.h"
#include "road_manager.h"
#include "road.h"
#include "goal_manager.h"
#include "river.h"
#include "doll.h"
#include "map_character.h"

// enemy
#include "car_manager.h"
#include "car.h"
#include "police_manager.h"
#include "police_AI_manager.h"
#include "police.h"

// scene
#include "result.h"
#include "multi_result_manager.h"

// ui
#include "pause.h"
#include "timer.h"
#include "navi.h"
#include "meter.h"
#include "speedmeter.h"
#include "deliverystatus.h"

// object
#include "fade.h"
#include "time.h"
#include "meshfield.h"
#include "meshdome.h"
#include "meshwall.h"

// player
#include "player.h"
#include "radio.h"

#include "goal.h"
#include "map_obstacle.h"
#include "map_manager.h"
#include "player_manager.h"

// gimmick
#include "inspection_manager.h"
#include "gimmick_firehydrant.h"
#include "gimmick_policestation.h"
#include "gimmick_guardrail.h"
#include "bridge.h"

// ネットワーク
#include "network.h"

// 無名名前空間を定義
namespace {
    const int MAX_STRING = (2048);
    const int TOTAL_POINT = 3;  // 配達する総数
    const char* ADDRESSFILE	= "data\\TXT\\address.txt";
    const D3DXVECTOR3 CAMERA_ROT[4] =
    {
        D3DXVECTOR3(0.0f, D3DX_PI * 1.0f, D3DX_PI * 0.4f),
        D3DXVECTOR3(0.0f, D3DX_PI* 1.0f, D3DX_PI * 0.45f),
        D3DXVECTOR3(0.0f, -D3DX_PI* 1.0f, D3DX_PI * 0.2f),
        D3DXVECTOR3(0.0f, -D3DX_PI * 0.6f, 0.33f),
    };

    const float CAMERA_LENGTH[4] =
    {
        10000.0f,
        500.0f,
        7000.0f,
        3000.0f,
    };

    const float MOVE = 10.0f;
    const D3DXVECTOR3 SET_PLAYER_POS = D3DXVECTOR3(-10000.0f, 0.0f, -200.0f);
}

//===============================================
// 静的メンバ変数
//===============================================
int CGame::m_nNumPlayer = 0;
CGame* CGame::m_pInstance = nullptr;

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
    m_pEndSound = nullptr;
    m_pEndText = nullptr;
    m_GameState = STATE::STATE_NONE;

    for (int i = 0; i < 7; i++)
    {
        m_pDoll[i] = nullptr;
    }
}

//===============================================
// コンストラクタ(人数設定)
//===============================================
CGame::CGame(int nNumPlayer)
{
    // 値のクリア
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
    m_pInstance = this;

    // 配達する総数
    m_nTotalDeliveryStatus = TOTAL_POINT;

    // 右側
    CMeshField::Create(D3DXVECTOR3(27250.0f, -10.0f, 3000.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1000.0f, 1000.0f, "data\\TEXTURE\\field000.jpg", 13, 16);

    // 左側
    CMeshField::Create(D3DXVECTOR3(-750.0f, -10.0f, 3000.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 500.0f, 500.0f, "data\\TEXTURE\\field001.jpg", 26, 32);

    // 川
    CRiver::Create(D3DXVECTOR3(13250.0f, -600.0f, 3000.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(500.0f, 1000.0f), D3DXVECTOR2(-0.0001f, -0.00125f), 2, 16);

    // 川淵の壁
    CMeshWall::Create(D3DXVECTOR3(12250.0f, -600.0f, 3000.0f), D3DXVECTOR3(0.0f, -D3DX_PI * 0.5f, 0.0f), 1000.0f, 300.0f, "data\\TEXTURE\\field001.jpg", 16, 1);
    CMeshWall::Create(D3DXVECTOR3(14250.0f, -600.0f, 3000.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.5f, 0.0f), 1000.0f, 300.0f, "data\\TEXTURE\\field001.jpg", 16, 1);

    // 空生成
    //m_pMeshDome = CMeshDome::Create(VECTOR3_ZERO, VECTOR3_ZERO, Game::DOME_LENGTH, 2000.0f, 3, 20, 20);

    auto net = CNetWork::GetInstance();

    // マップ読み込み
    CMapManager::GetInstance()->Load();

    // プレイヤー生成
    (this->*(m_CreatePlayerFunc[net->GetState()]))();

    CMeter::Create();

    int myid = net->GetIdx();

    for (int i = 0; i < NetWork::MAX_CONNECT; i++)
    {
        if (!net->GetConnect(i)) { continue; }

        if (i < myid) { myid = i; }
    }

    // 自分が先頭の場合
    if (myid == net->GetIdx())
    {
        // 車の生成
        CreateCar();

        // 警察の生成
        //CreatePolice();
    }

    // キャラクターの生成
    CreateCharacter();

    if (m_pGoalManager == nullptr)
    {
        m_pGoalManager = CGoalManager::Create();
        if (m_pGoalManager->GetCreateIdx() <= 0)
        {
            m_pGoalManager->Init();
        }
    }

    CCameraManager::GetInstance()->GetTop()->Init();

    if (m_pDeliveryStatus == nullptr)
    {
        m_pDeliveryStatus = CDeliveryStatus::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.8f, SCREEN_HEIGHT * 0.6f, 0.0f), m_nTotalDeliveryStatus);
    }

    if (m_pGameTimer == nullptr)
    {
        m_pGameTimer = CTimer::Create();
    }

    m_pPause = CPause::Create();

  /*  pFog = DEBUG_NEW CFog;
    pFog->Set(D3DFOG_LINEAR, D3DXCOLOR(0.2f, 0.2f, 0.3f, 0.5f), 100.0f, 15000.0f, 1.0f);*/
    return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CGame::Uninit(void)
{
    m_pInstance = nullptr;

    CManager::GetInstance()->GetSound()->Stop();

    CManager::GetInstance()->GetDeltaTime()->SetSlow(1.0f);

    m_bEnd = true;

    while (1)
    {
        //OnlineEnd();
        if (m_nSledCnt <= 0)
        {
            break;
        }

    }

    // 終了
    SAFE_UNINIT(m_pMeshDome);
    SAFE_UNINIT(m_pDeliveryStatus);
    SAFE_UNINIT(m_pGameTimer);
    SAFE_UNINIT(m_pPause);

    for (int i = 0; i < 7; i++)
    {
        SAFE_UNINIT(m_pDoll[i]);
    }
    
    // 解放
    SAFE_RELEASE(m_pGoalManager);
  
    // ネットワーク切断
    auto net = CNetWork::GetInstance();
    net->DisConnect();

    // defaultカメラオン
    CManager::GetInstance()->GetCamera()->SetDraw(true);
    CManager::GetInstance()->GetCamera()->GetAction()->SetFinish(true);
    CManager::GetInstance()->GetCamera()->GetAction()->SetPause(true);

    // エディット廃棄
    CEditManager::Release();

    // マップマネージャー廃棄
    CMapManager::Release();

    // 各種マネージャー廃棄
    CCarManager::Release();
    CGoalManager::Release();
    CPoliceManager::Release();
    CPoliceAIManager::Release();
    CInspectionManager::Release();
}

//===============================================
// 更新処理
//===============================================
void CGame::Update(void)
{
	CInputKeyboard *pInputKey = CInputKeyboard::GetInstance();

    if (CNetWork::GetInstance()->GetState() == CNetWork::STATE::STATE_SINGLE)
    {
        m_pPause->ChangePause();
    }

    if (m_pPause->GetPause()) 
    { 
        m_pPause->Update();
        return; 
    }

    // タイマーの更新
    if (m_pGameTimer != nullptr && CEditManager::GetInstance() == nullptr)
    {
        CPlayer* player = CPlayerManager::GetInstance()->GetPlayer();
        if (player != nullptr)
        {
            if (player->GetType() == CPlayer::TYPE::TYPE_ACTIVE)
            {
                m_pGameTimer->Update();
            }
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
#if 1

    CEditManager* pMgr = CEditManager::GetInstance();
    // エディター生成
    if (pInputKey->GetTrigger(DIK_F4) && CEditManager::GetInstance() == nullptr)
    {
        pMgr = CEditManager::Create();
    }

    // エディター更新
    if (pMgr != nullptr) { pMgr->Update(); }

    if (pInputKey->GetTrigger(DIK_J))
    {
        CPoliceManager::GetInstance()->SetInspection();
    }

#endif

    auto net = CNetWork::GetInstance();
    auto mgr = CPlayerManager::GetInstance();

    CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
    if (pPlayer != nullptr)
    {
        int nNum = pPlayer->GetNumDeliverStatus();
        CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
        if (m_nTotalDeliveryStatus <= nNum && pCamera->GetAction()->IsFinish() &&
            CNetWork::GetInstance()->GetState() == CNetWork::STATE::STATE_SINGLE)
        {// 配達する総数以上かつカメラの演出が終了している
            End_Success();
        }
        else if (m_pGameTimer != nullptr)
        {
            // 時間切れ
            if (m_pGameTimer->GetTime() <= 0.0f)
            {
                // 通信状態で終了変更
                if (net->GetState() == CNetWork::STATE::STATE_SINGLE)
                {
                    // 配達失敗
                    End_Fail();
                }
                else
                {   // 複数人終了
                    End_MultiEnd();
                }

            }
        }
    }

    // 人数確認
    if (net->GetState() == CNetWork::STATE::STATE_ONLINE)
    {
        for (int i = 0; i < NetWork::MAX_CONNECT; i++)
        {
            auto player = mgr->GetPlayer(i);

            // 人数が多い
            if (player != nullptr && !net->GetConnect(i) && i != net->GetIdx())
            {
                player->Uninit();
            }
        }
    }

    // シングルモードで体力がキレた
    if (net->GetState() == CNetWork::STATE::STATE_SINGLE)
    {
        if (mgr->GetPlayer() != nullptr)
        {
            if (mgr->GetPlayer()->GetLife() <= 0.0f)
            {
                End_Fail();
            }
        }
    }

    // 空の位置更新
    if (m_pMeshDome != nullptr && pPlayer != nullptr)
    {
        m_pMeshDome->SetPosition(pPlayer->GetPosition());
    }

    // 各マネージャー更新
    CPoliceManager::GetInstance()->Update();
    CPoliceAIManager::GetInstance()->Update();
    CInspectionManager::GetInstance()->Update();


#if NDEBUG
    CScene::Update();
#else

    CScene::Update();

#endif

    // 状態による音量設定
    switch (m_GameState)
    {
    case CGame::STATE_NONE:
        break;
    case CGame::STATE_PROG:
        break;
    case CGame::STATE_SUCCESS:
        pPlayer->GetRadio()->SetVol(pPlayer->GetRadio()->GetVol() * 0.9f);
        pPlayer->Damage(pPlayer->GetLife() - pPlayer->GetLifeOrigin());
        if (!m_pEndSound->GetPlay())
        {
            CManager::GetInstance()->GetFade()->Set(CScene::MODE_RESULT);
            CManager::GetInstance()->SetSuccess(true);
        }
        break;
    case CGame::STATE_FAIL:
        pPlayer->GetRadio()->SetVol(pPlayer->GetRadio()->GetVol()*0.9f);
        if (!m_pEndSound->GetPlay())
        {
            CManager::GetInstance()->GetFade()->Set(CScene::MODE_RESULT);
            CManager::GetInstance()->SetSuccess(false);
        }
        break;

    case CGame::STATE_MULTIEND:
        pPlayer->GetRadio()->SetVol(pPlayer->GetRadio()->GetVol() * 0.9f);
        if (!m_pEndSound->GetPlay())
        {
            CManager::GetInstance()->GetFade()->Set(CScene::MODE_MULTI_RESULT);
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
// 開始時の演出
//===================================================
void CGame::StartIntro(void)
{
    CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
    if (pPlayer == nullptr) { return; }
    if (pPlayer->GetType() != CPlayer::TYPE::TYPE_GAMESTARTOK) { return; }

    CCamera* pCamera = CCameraManager::GetInstance()->GetTop();

    if (m_nStartCameraCount >= 4)
    {
        if (!pCamera->GetAction()->IsFinish())
        {
            D3DXVECTOR3 pos = pPlayer->GetPosition();
            pos.x += MOVE * 0.5f;
            pPlayer->SetPosition(pos);
        }
        else
        {
            if (m_nStartCameraCount == 4)
            {
                m_nStartCameraCount++;
                pPlayer->SetType(CPlayer::TYPE::TYPE_ACTIVE);
            }
        }
        return;
    }

    if ((pCamera->GetAction()->IsNext() && pCamera->GetAction()->IsPause() && m_nStartCameraCount < 3))
    {
        CCameraManager::GetInstance()->GetTop()->GetAction()->Set(pCamera, pPlayer->GetPosition(), CAMERA_ROT[m_nStartCameraCount], CAMERA_LENGTH[m_nStartCameraCount], 2.0f, 1.0f, CCameraAction::MOVE_POSV, true);
        m_nStartCameraCount++;
    }
    else if ((pCamera->GetAction()->IsNext() && pCamera->GetAction()->IsPause() && m_nStartCameraCount >= 3))
    {
        CCameraManager::GetInstance()->GetTop()->GetAction()->Set(pCamera, pPlayer->GetPosition(), CAMERA_ROT[m_nStartCameraCount], CAMERA_LENGTH[m_nStartCameraCount], 2.0f, 1.0f, CCameraAction::MOVE_POSV, false);
        m_nStartCameraCount++;
    }
    else if (m_nStartCameraCount == 0)
    {
        CCameraManager::GetInstance()->GetTop()->GetAction()->Set(pCamera, pCamera->GetPositionR(), CAMERA_ROT[m_nStartCameraCount], CAMERA_LENGTH[m_nStartCameraCount], 1.0f, 1.0f, CCameraAction::MOVE_POSV, true); 
        m_nStartCameraCount++;
    }

    D3DXVECTOR3 pos = pPlayer->GetPosition();
    pos.x += MOVE;
    pPlayer->SetPosition(pos);
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
    CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
    
    if (m_GameState != STATE_SUCCESS)
    {
        SetGameState(STATE::STATE_SUCCESS);
        m_pEndText = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, SCREEN_CENTER, 1.0f, 200.0f, 200.0f, XALIGN_CENTER, YALIGN_CENTER, VECTOR3_ZERO, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
        m_pEndText->PushBackString("配達完了");
        m_pEndText->SetEnableScroll(true);
        m_pEndSound = CMasterSound::CObjectSound::Create("data\\SE\\OPED35.wav", 0);
        CManager::GetInstance()->SetDeliveryStatus(pPlayer->GetNumDeliverStatus());
        CManager::GetInstance()->SetLife(pPlayer->GetLife());
    }
 
}
void CGame::End_Fail()
{
    CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
    
    if (m_GameState != STATE_FAIL)
    {
        SetGameState(STATE::STATE_FAIL);
        m_pEndText = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, SCREEN_CENTER, 0.7f, 200.0f, 200.0f, XALIGN_CENTER, YALIGN_CENTER, VECTOR3_ZERO, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
        m_pEndText->PushBackString("配達失敗");
        m_pEndText->SetEnableScroll(true);
        m_pEndSound = CMasterSound::CObjectSound::Create("data\\SE\\f_jingle.wav", 0);
        CManager::GetInstance()->SetDeliveryStatus(pPlayer->GetNumDeliverStatus());
        CManager::GetInstance()->SetLife(pPlayer->GetLife());
    }
}

void CGame::End_MultiEnd()
{
    if (m_GameState != STATE::STATE_MULTIEND)
    {
        SetGameState(STATE::STATE_MULTIEND);
        m_pEndText = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, SCREEN_CENTER, 0.7f, 200.0f, 200.0f, XALIGN_CENTER, YALIGN_CENTER, VECTOR3_ZERO, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
        m_pEndText->PushBackString("配達終了");
        m_pEndText->SetEnableScroll(true);
        m_pEndSound = CMasterSound::CObjectSound::Create("data\\SE\\f_jingle.wav", 0);

        // データを保存
        CMultiResultManager::GetInstance()->DataSet();
    }
}
//===================================================
// プレイヤーの生成(シングル)
//===================================================
void CGame::CreateSinglePlayer(void)
{

    CPlayer* pPlayer = CPlayer::Create(SET_PLAYER_POS,
        VECTOR3_ZERO, VECTOR3_ZERO, CNetWork::GetInstance()->GetIdx());
    pPlayer->SetType(CPlayer::TYPE::TYPE_ACTIVE);
    pPlayer->SetType(CPlayer::TYPE::TYPE_GAMESTARTOK);
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

        D3DXVECTOR3 pos = SET_PLAYER_POS;
        pos.z += 200.0f * i;
        CPlayer* pPlayer = CPlayer::Create(pos,
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
    for (int i = 0; i < 3; i++)
    {
        CCar* pCar = CPolice::Create(D3DXVECTOR3(6000.0f + 1000.0f * i, 0.0f, 1000.0f * i), 
            D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), CCarManager::GetInstance()->GetMapList()->GetInCnt());
        pCar->SetType(CCar::TYPE::TYPE_ACTIVE);
    }
}

//===================================================
// 車の生成
//===================================================
void CGame::CreateCar()
{
    for (int i = 0; i < 3; i++)
    {
        CCar* pCar = CCar::Create(D3DXVECTOR3(3000.0f + 750.0f * i, 0.0f, 1000.0f * i), 
            D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), CCarManager::GetInstance()->GetMapList()->GetInCnt());
        pCar->SetType(CCar::TYPE::TYPE_ACTIVE);
    }
}

//===================================================
// 車の生成
//===================================================
int CGame::GetRestDeliveryStatus()
{
    int num = m_nTotalDeliveryStatus;
    CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
    if (pPlayer != nullptr)
    {
        num -= pPlayer->GetNumDeliverStatus();
    }

    return num;
}

//===================================================
// キャラクターの生成
//===================================================
void CGame::CreateCharacter()
{
    // 人形の生成
    m_pDoll[0] = CDoll::Create(D3DXVECTOR3(31900.0f, 0.0f, -2200.0f), VECTOR3_ZERO, 0);
    m_pDoll[1] = CDoll::Create(D3DXVECTOR3(32100.0f, 0.0f, -2200.0f), VECTOR3_ZERO, 1);
    m_pDoll[2] = CDoll::Create(D3DXVECTOR3(32300.0f, 0.0f, -2200.0f), VECTOR3_ZERO, 2);
    m_pDoll[3] = CDoll::Create(D3DXVECTOR3(32400.0f, 0.0f, -2500.0f), VECTOR3_ZERO, 3);
    m_pDoll[4] = CDoll::Create(D3DXVECTOR3(32200.0f, 0.0f, -2500.0f), VECTOR3_ZERO, 4);
    m_pDoll[5] = CDoll::Create(D3DXVECTOR3(32000.0f, 0.0f, -2500.0f), VECTOR3_ZERO, 5);
    m_pDoll[6] = CDoll::Create(D3DXVECTOR3(31800.0f, 0.0f, -2500.0f), VECTOR3_ZERO, 6);

    // 魚生成
    CMapCharacter::Create(D3DXVECTOR3(13250.0f, -600.0f, 3000.0f), VECTOR3_ZERO, VECTOR3_ONE, "data\\TXT\\character\\fish\\motion_fish.txt", 0);
    CMapCharacter::Create(D3DXVECTOR3(13250.0f, -600.0f, 5000.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.5f, 0.0f), VECTOR3_ONE, "data\\TXT\\character\\fish\\motion_fish.txt", 1);
    CMapCharacter::Create(D3DXVECTOR3(13250.0f, -600.0f, 1000.0f), D3DXVECTOR3(0.0f, D3DX_PI * 1.0f, 0.0f), VECTOR3_ONE, "data\\TXT\\character\\fish\\motion_fish.txt", 2);
}