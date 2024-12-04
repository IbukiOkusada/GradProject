//===============================================
//
// エントリー画面の管理処理 [entry.cpp]
// Author : Ryosuke Oohara
//
//===============================================
#include "entry.h"
#include "camera.h"
#include "camera_manager.h"
#include "input.h"
#include "input_gamepad.h"
#include "input_keyboard.h"
#include "manager.h"
#include "fade.h"
#include "object2D.h"
#include "texture.h"
#include "player.h"
#include "player_manager.h"
#include "debugproc.h"
#include "map_manager.h"
#include "objectX.h"
#include "edit_manager.h"
#include "meshfield.h"

//===============================================
// 定数定義
//===============================================
namespace
{
    const int WIDTH_NUM = 4;   // 横の分割数
    const int HEIGHT_NUM = 2;  // 縦の分割数
    const int MAX_PLAYER = 4;  // プレイヤーの最大数

    const D3DXVECTOR3 CAMERA_ROT[4] =
    {
        D3DXVECTOR3(0.0f,  0.0f,           D3DX_PI * 0.3f),
        D3DXVECTOR3(0.0f,  D3DX_PI,        D3DX_PI * 0.3f),
        D3DXVECTOR3(0.0f,  D3DX_PI * 0.5f, D3DX_PI * 0.3f),
        D3DXVECTOR3(0.0f, -D3DX_PI * 0.5f, D3DX_PI * 0.3f),
    };

    const D3DXVECTOR3 CAMERA_POS_R[4] =
    {
        D3DXVECTOR3(800.0f, 50.0f, 600.0f),
        D3DXVECTOR3(800.0f,  50.0f, -600.0f),
        D3DXVECTOR3(800.0f,  50.0f, -600.0f),
        D3DXVECTOR3(800.0f,  50.0f, 600.0f),
    };

    const D3DXVECTOR3 CAMERA_POS_V = D3DXVECTOR3(320.3f, -300.0f, 91.6f);
    const float LENGTH = 500.0f;
    const float ROTATION_Y = 0.005f;

    const char* MODEL_PATH = "data\\MODEL\\bike.x";  // プレイヤーのモデル
}

//===============================================
// コンストラクタ
//===============================================
CEntry::CEntry()
{
    // 値をクリア
	m_ppCamera = nullptr;
    m_ppObj = nullptr;
    m_IsFinish = false;
}

//===============================================
// デストラクタ
//===============================================
CEntry::~CEntry()
{

}

//===============================================
// 初期化処理
//===============================================
HRESULT CEntry::Init(void)
{
    auto net = CNetWork::GetInstance();
    //net->ReConnect();

    // ID取得を待つ
    if (net->GetState() == CNetWork::STATE::STATE_ONLINE)
    {
        net->GetTime()->Start();
        while (1)
        {
            net->GetTime()->End();

            if (net->GetTime()->IsOK())
            {
                net->SendGetId();
                net->Update();
            }

            if (net->GetIdx() != -1)
            {
                break;
            }
        }
    }
    else
    {
        
    }

    // マップ読み込み
    CMapManager::GetInstance()->Load();

    CMeshField::Create(D3DXVECTOR3(0.0f, -10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1000.0f, 1000.0f, "data\\TEXTURE\\field000.jpg", 30, 30);

    /*CObject2D* pObj = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.9f, 0.0f), VECTOR3_ZERO, 4);
    pObj->SetSize(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.1f);
    pObj->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\concrete002.jpg"));*/

    CCameraManager* mgr = CCameraManager::GetInstance();
	m_ppCamera = new CMultiCamera*[MAX_PLAYER];
    m_ppObj = new CObjectX * [MAX_PLAYER];

    for (int i = 0; i < MAX_PLAYER; i++)
    {
        m_ppCamera[i] = nullptr;
        m_ppObj[i] = nullptr;
    }

    CCamera *pCamera = mgr->GetTop();

    if (pCamera != nullptr)
    {
        pCamera->SetPositionV(D3DXVECTOR3(0.0f, 4000.0f, 0.0f));
    }

    // 複数カメラの生成
    for (int i = 0; i < MAX_PLAYER; i++)
    {
        m_ppCamera[i] = DEBUG_NEW CMultiCamera;
        m_ppCamera[i]->Init();
        m_ppCamera[i]->SetLength(LENGTH);
        m_ppCamera[i]->SetRotation(CAMERA_ROT[i]);
        m_ppCamera[i]->SetPositionR(D3DXVECTOR3(800.0f, 100.0f, 600.0f));
        m_ppCamera[i]->SetPositionV(CAMERA_POS_V);
        m_ppCamera[i]->SetDrawState(CCamera::DRAWSTATE::PLAYER_ONLY);
        D3DVIEWPORT9 viewport;
        
        // プレイヤー追従カメラの画面位置設定
        viewport.X = (DWORD)((SCREEN_WIDTH / 4.0f) * (i % WIDTH_NUM));
        viewport.Y = (DWORD)(SCREEN_HEIGHT * 0.7f);

        viewport.Width = (DWORD)(SCREEN_WIDTH / 4.0f);
        viewport.Height = (DWORD)(SCREEN_HEIGHT * 0.4f);

        viewport.MinZ = 0.0f;
        viewport.MaxZ = 1.0f;

        m_ppCamera[i]->SetViewPort(viewport);
    }
  
	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CEntry::Uninit(void)
{
    // カメラの破棄
    if (m_ppCamera != nullptr)
    {
        for (int i = 0; i < MAX_PLAYER; i++)
        {
            if (m_ppCamera[i] == nullptr) { continue; }
           
            m_ppCamera[i]->Uninit();
            delete m_ppCamera[i];
            m_ppCamera[i] = nullptr;
        }

        delete[] m_ppCamera;
        m_ppCamera = nullptr;
    }

    // 画面下にでるプレイヤーモデルの破棄
    if (m_ppObj != nullptr)
    {
        for (int i = 0; i < MAX_PLAYER; i++)
        {
            if (m_ppObj[i] == nullptr) { continue; }

            m_ppObj[i]->Uninit();
            m_ppObj[i] = nullptr;
        }

        delete[] m_ppObj;
        m_ppObj = nullptr;
    }
}

//===============================================
// 更新処理
//===============================================
void CEntry::Update(void)
{
    CInputKeyboard* pKey = CInputKeyboard::GetInstance();
    CInputPad* pPad = CInputPad::GetInstance();

    // エディター関連
#if _DEBUG

    CEditManager* pMgr = CEditManager::GetInstance();
    // エディター生成
    if (pKey->GetTrigger(DIK_F4) && CEditManager::GetInstance() == nullptr)
    {
        pMgr = CEditManager::Create();
    }

    // エディター更新
    if (pMgr != nullptr) { pMgr->Update(); }

#endif

    if (pPad->GetTrigger(CInputPad::BUTTON_START, 0) ||
        pKey->GetTrigger(DIK_RETURN))
    {
        //m_IsFinish = true;
        
        //CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);
    }
    for (int i = 0; i < MAX_PLAYER; i++)
    {
        if (m_ppObj[i] != nullptr)
        {
            D3DXVECTOR3 rot = m_ppObj[i]->GetRotation();
            rot.y += ROTATION_Y;
            m_ppObj[i]->SetRotation(rot);
        }
    }

    // プレイヤー参加処理
    AddPlayer();

    // プレイヤー参加取り消し処理
    DecreasePlayer();

    // シーンの更新
    CScene::Update();

    CPlayerManager* mgr = CPlayerManager::GetInstance();

    // デバック表示
    CDebugProc::GetInstance()->Print("プレイヤーの数[%d]\n", mgr->GetNum());

    for (int i = 0; i < MAX_PLAYER; i++)
    {
        // カメラ
        D3DXVECTOR3 CamPosV = m_ppCamera[i]->GetPositionV();
        D3DXVECTOR3 CamPosR = m_ppCamera[i]->GetPositionR();
        CDebugProc::GetInstance()->Print("カメラ 視点   : [%f, %f, %f]\n", CamPosV.x, CamPosV.y, CamPosV.z);
        CDebugProc::GetInstance()->Print("カメラ 注視点 : [%f, %f, %f]\n", CamPosR.x, CamPosR.y, CamPosR.z);
        CDebugProc::GetInstance()->Print("距離 [%f]\n", m_ppCamera[i]->GetLength());
    }
}

//===============================================
// 描画処理
//===============================================
void CEntry::Draw(void)
{
    // 描画処理(コレ必要だよん!) ← 感謝感激雨あられ
    CScene::Draw();
}

//===============================================
// プレイヤー参加処理
//===============================================
void CEntry::AddPlayer(void)
{
    CInputKeyboard* pKey = CInputKeyboard::GetInstance();
    CInputPad* pPad = CInputPad::GetInstance();
    auto net = CNetWork::GetInstance();
    auto mgr = CPlayerManager::GetInstance();
    int id = mgr->GetNum();

    if (pPad->GetTrigger(CInputPad::BUTTON_START, 0) ||
        pKey->GetTrigger(DIK_RETURN))
    {
        D3DXVECTOR3 pos = m_ppCamera[id]->GetPositionR();
        int playid = id;
        if (net->GetState() == CNetWork::STATE::STATE_SINGLE && playid == 0)
        {
            playid = net->GetIdx();
        }

        // プレイヤー生成
        CPlayer* pPlayer = CPlayer::Create(D3DXVECTOR3(4000.0f, 0.0f, 1600.0f), D3DXVECTOR3(0.0f, CAMERA_ROT[id].y, 0.0f), VECTOR3_ZERO, playid);

        // データ受信
        pPlayer->SetType(CPlayer::TYPE::TYPE_RECV);
        pPlayer->SetRecvPosition(pos);

        // チュートリアル時のアクティブに設定
        pPlayer->SetType(CPlayer::TYPE::TYPE_TUTOLERIAL_ACTIVE);

        // エフェクトの終了
        //pPlayer->EffectUninit();

        // 画面下にプレイヤーのモデルを表示
        pos = m_ppCamera[id]->GetPositionR();
        m_ppObj[id] = CObjectX::Create(pos, D3DXVECTOR3(0.0f, CAMERA_ROT[id].y, 0.0f), MODEL_PATH, 7);
        m_ppObj[id]->SetType(CObject::TYPE::TYPE_PLAYER);
        m_ppObj[id]->SetRotateType(CObjectX::TYPE_QUATERNION);
    }

    // 人数確認
    if (net->GetState() == CNetWork::STATE::STATE_ONLINE)
    { 
        for (int i = 0; i < NetWork::MAX_CONNECT; i++)
        {
            auto player = mgr->GetPlayer(i);
           
            // 人数が多い
            if (player == nullptr && net->GetConnect(i))
            {
                // プレイヤー生成
                D3DXVECTOR3 pos = m_ppCamera[i]->GetPositionR();
                CPlayer* pPlayer = CPlayer::Create(D3DXVECTOR3(4000.0f, 0.0f, 1600.0f), D3DXVECTOR3(0.0f, CAMERA_ROT[i].y, 0.0f), VECTOR3_ZERO, i);

                // データ受信
                pPlayer->SetType(CPlayer::TYPE::TYPE_RECV);
                pPlayer->SetRecvPosition(pos);

                if (i == net->GetIdx())
                {
                    // データ送信
                    pPlayer->SetType(CPlayer::TYPE::TYPE_SEND);
                }

                // チュートリアル時のアクティブに設定
                pPlayer->SetType(CPlayer::TYPE::TYPE_TUTOLERIAL_ACTIVE);

                // エフェクト終了
                pPlayer->EffectUninit();

                // 画面下にプレイヤーのモデルを表示
                pos = m_ppCamera[i]->GetPositionR();
                m_ppObj[i] = CObjectX::Create(pos, D3DXVECTOR3(0.0f, CAMERA_ROT[i].y, 0.0f), MODEL_PATH, 7);
                m_ppObj[i]->SetType(CObject::TYPE::TYPE_PLAYER);
                m_ppObj[i]->SetRotateType(CObjectX::TYPE_QUATERNION);
            }
        }
    }

    if (pKey->GetTrigger(DIK_RETURN) || pPad->GetTrigger(CInputPad::BUTTON_A, 0))
    {
        m_IsFinish = true;

        if (m_IsFinish)
        {// チュートリアルが終了している

            net->SendTutorialOk();
        }
        else
        {

        }
    }
}

//===============================================
// プレイヤー参加取り消し処理
//===============================================
void CEntry::DecreasePlayer(void)
{
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
                // プレイヤーの終了処理
                player->Uninit();

                if (m_ppObj[i] != nullptr)
                {// 画面下に出てるプレイヤーモデルの終了

                    m_ppObj[i]->Uninit();
                    m_ppObj[i] = nullptr;   
                }
            }
        }
    }
}