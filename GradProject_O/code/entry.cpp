//===============================================
//
// エントリー画面の管理処理 [entry.cpp]
// Author : Ryosuke Oohara
//
//===============================================
#include "entry.h"
#include "camera.h"
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

//===============================================
// 定数定義
//===============================================
namespace
{
    const int WIDTH_NUM = 2;   // 横の分割数
    const int HEIGHT_NUM = 2;  // 縦の分割数

    const float CAMERA_ROT_Y[4] =
    {
        D3DX_PI,
        D3DX_PI * 0.5f,
        -D3DX_PI,
        -D3DX_PI * 0.5f,
    };

    const D3DXVECTOR3 CAMERA_POS_V[4] =
    {
        D3DXVECTOR3(-874.3f, 1124.15f, 717.2f),
        D3DXVECTOR3(-1874.3f, 2124.15f, 2000.2f),
        D3DXVECTOR3(874.3f, 50.15f, -200.2f),
        D3DXVECTOR3(3000.3f, 1124.15f, -1717.2f),
    };

    const D3DXVECTOR3 CAMERA_POS_R[4] =
    {
        D3DXVECTOR3(-320.3f, 1.0f, -91.6f),
        D3DXVECTOR3(-320.3f, 1.0f, -91.6f),
        D3DXVECTOR3(-320.3f, 1.0f, -91.6f),
        D3DXVECTOR3(-320.3f, 1.0f, -91.6f),
    };
}

//===============================================
// コンストラクタ
//===============================================
CEntry::CEntry()
{
	m_ppCamera = nullptr;
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
   /* CObject2D* pObj = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), VECTOR3_ZERO);
    pObj->SetSize(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);
    pObj->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\entry.png"));*/

    // マップ読み込み
    CMapManager::GetInstance()->Load();

    // defaultカメラオフ
    CManager::GetInstance()->GetCamera()->SetDraw(false);

	m_ppCamera = DEBUG_NEW CMultiCamera * [4];

    for (int i = 0; i < 4; i++)
    {
        m_ppCamera[i] = new CMultiCamera;
        m_ppCamera[i]->Init();
        m_ppCamera[i]->SetPositionV(CAMERA_POS_V[i]);
        m_ppCamera[i]->SetPositionR(CAMERA_POS_R[i]);
       /* m_ppCamera[i]->SetLength(400.0f);
        m_ppCamera[i]->SetRotation(D3DXVECTOR3(0.0f, CAMERA_ROT_Y[i], D3DX_PI * 0.5f));*/

        D3DVIEWPORT9 viewport;
        //プレイヤー追従カメラの画面位置設定
        viewport.X = (DWORD)((SCREEN_WIDTH * 0.5f) * (i % WIDTH_NUM));
        viewport.Y = (DWORD)((SCREEN_HEIGHT * 0.5f) * (i / WIDTH_NUM));

        if (i < WIDTH_NUM) {
            viewport.Width = (DWORD)(SCREEN_WIDTH * 1.0f);
        }
        else
        {
            viewport.Width = (DWORD)(SCREEN_WIDTH * 0.5f);
        }

        if (i <= WIDTH_NUM) {
            viewport.Height = (DWORD)(SCREEN_HEIGHT * 1.0f);
        }
        else
        {
            viewport.Height = (DWORD)(SCREEN_HEIGHT * 0.5f);
        }

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
        for (int i = 0; i < 4; i++)
        {
            if (m_ppCamera[i] == nullptr) { continue; }
           
            m_ppCamera[i]->Uninit();
            delete m_ppCamera[i];
            m_ppCamera[i] = nullptr;
        }

        delete[] m_ppCamera;
        m_ppCamera = nullptr;
    }
}

//===============================================
// 更新処理
//===============================================
void CEntry::Update(void)
{
    CInputKeyboard* pKey = CInputKeyboard::GetInstance();
    CInputPad* pPad = CInputPad::GetInstance();

    if (pPad->GetTrigger(CInputPad::BUTTON_START, 0) ||
        pKey->GetTrigger(DIK_RETURN))
    {
        CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);
    }

    AddPlayer();

    CPlayerManager* mgr = CPlayerManager::GetInstance();
    CDebugProc::GetInstance()->Print("プレイヤーの数[%d]\n", mgr->GetNum());
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
    CPlayerManager* mgr = CPlayerManager::GetInstance();

    if (pPad->GetTrigger(CInputPad::BUTTON_A, 0))
    {
        if (mgr->GetNum() < PLAYER_MAX) { // 人数が最大ではない場合

            int id = mgr->GetNum();

            CPlayer* pPlayer = CPlayer::Create(D3DXVECTOR3(-320.3f, 1.0f, -91.6f), VECTOR3_ZERO, VECTOR3_ZERO, id);
            mgr->ListIn(pPlayer);

        }
    }
}