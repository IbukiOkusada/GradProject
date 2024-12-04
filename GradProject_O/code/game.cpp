//===============================================
//
// �Q�[����ʂ̊Ǘ����� [game.cpp]
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
// �l�b�g���[�N
#include "network.h"

// �������O��Ԃ��`
namespace {
    const int MAX_STRING = (2048);
    const int TOTAL_POINT = 3;  // �z�B���鑍��
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
// �ÓI�����o�ϐ�
//===============================================
int CGame::m_nNumPlayer = 0;

//===============================================
// �֐��|�C���^
//===============================================
// ��ԊǗ�
CGame::CREATE_PL_FUNC CGame::m_CreatePlayerFunc[] =
{
    &CGame::CreateSinglePlayer,	// �V���O���v���C
    &CGame::CreateMultiPlayer,	// �}���`�v���C
    &CGame::CreateSinglePlayer,	// �V���O���v���C(��O
};

//===============================================
// �R���X�g���N�^
//===============================================
CGame::CGame()
{
    // �l�̃N���A
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
// �R���X�g���N�^(�l���ݒ�)
//===============================================
CGame::CGame(int nNumPlayer)
{
    // �l�̃N���A
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
    // �l���ݒ�
    m_nNumPlayer = nNumPlayer;
}

//===============================================
// �f�X�g���N�^
//===============================================
CGame::~CGame()
{
   
}

//===============================================
// ����������
//===============================================
HRESULT CGame::Init(void)
{
    memset(&m_aAddress[0], '\0', sizeof(m_aAddress));

    // �O���t�@�C���ǂݍ��݂̐���
    if (nullptr == m_pFileLoad)
    {// �g�p���Ă��Ȃ��ꍇ

    }

    // �z�B���鑍��
    m_nTotalDeliveryStatus = TOTAL_POINT;

    CMeshField::Create(D3DXVECTOR3(0.0f, -10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1000.0f, 1000.0f, "data\\TEXTURE\\field000.jpg", 30, 30);

    // �}�b�v�ǂݍ���
    CMapManager::GetInstance()->Load();

    auto net = CNetWork::GetInstance();

    // �v���C���[����
    (this->*(m_CreatePlayerFunc[net->GetState()]))();

    CMeter::Create();
    //CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_GAME);

    int myid = net->GetIdx();

    for (int i = 0; i < NetWork::MAX_CONNECT; i++)
    {
        if (!net->GetConnect(i)) { continue; }

        if (i <= myid) { myid = i; }
    }

    // �������擪�̏ꍇ
    if (myid == net->GetIdx())
    {
        // �Ԃ̐���
        CreateCar();
        // �x�@�̐���
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
// �I������
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

    // �l�b�g���[�N�ؒf
    auto net = CNetWork::GetInstance();
    net->DisConnect();

    // default�J�����I��
    CManager::GetInstance()->GetCamera()->SetDraw(true);

    //Winsock�I������
    WSACleanup();	// WSACleanup�֐� : winsock�̏I������

    // �G�f�B�b�g�ݒ�
    CEditManager::Release();

    // �}�b�v�}�l�[�W���[�p��
    CMapManager::Release();
}

//===============================================
// �X�V����
//===============================================
void CGame::Update(void)
{
	CInputPad *pInputPad = CInputPad::GetInstance();
	CInputKeyboard *pInputKey = CInputKeyboard::GetInstance();

	if (pInputKey->GetTrigger(DIK_P) == true || pInputPad->GetTrigger(CInputPad::BUTTON_START, 0))
	{//�|�[�Y�L�[(P�L�[)�������ꂽ
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

    // �S�[���}�l�[�W���[�̍X�V
    if (m_pGoalManager != nullptr)
    {
        m_pGoalManager->Update();
    }

    // �J�n���̉��o
    StartIntro();

    // �G�f�B�^�[�֘A
#if _DEBUG

    CEditManager* pMgr = CEditManager::GetInstance();
    // �G�f�B�^�[����
    if (pInputKey->GetTrigger(DIK_F4) && CEditManager::GetInstance() == nullptr)
    {
        pMgr = CEditManager::Create();
    }

    // �G�f�B�^�[�X�V
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
    {// �z�B���鑍���ȏォ�J�����̉��o���I�����Ă���

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

    // �l���m�F
    if (net->GetState() == CNetWork::STATE::STATE_ONLINE)
    {
        for (int i = 0; i < NetWork::MAX_CONNECT; i++)
        {
            auto player = mgr->GetPlayer(i);

            // �l��������
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
// �`�揈��
//===============================================
void CGame::Draw(void)
{
    CScene::Draw();
}

//===================================================
// �v���C���[�̎擾
//===================================================
CPlayer *CGame::GetPlayer(void)
{
    return *m_ppPlayer;
}

//===================================================
// �t�@�C���ǂݍ��݂̎擾
//===================================================
CFileLoad *CGame::GetFileLoad(void)
{
    return m_pFileLoad;
}

//===================================================
// �J�n���̉��o
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
// �J�n���o
//===================================================
bool CGame::StartDirection(void)
{
    return false;
}

//===================================================
// �I�����o
//===================================================
void CGame::End_Success()
{
    if (m_GameState != STATE_SUCCESS)
    {
        SetGameState(STATE::STATE_SUCCESS);
        m_pEndText = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, SCREEN_CENTER, 1.0f, 200.0f, 200.0f, XALIGN_CENTER, YALIGN_CENTER, VECTOR3_ZERO, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
        m_pEndText->PushBackString("�z�B����");
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
        m_pEndText->PushBackString("�z�B���s");
        m_pEndText->SetEnableScroll(true);
        m_pEndSound = CMasterSound::CObjectSound::Create("data\\SE\\f_jingle.wav", 0);
    }
}
//===================================================
// �v���C���[�̐���(�V���O��)
//===================================================
void CGame::CreateSinglePlayer(void)
{
    auto net = CNetWork::GetInstance();

    CPlayer* pPlayer = CPlayer::Create(D3DXVECTOR3(-3034.65f, 1.0f, 1.0f), 
        VECTOR3_ZERO, VECTOR3_ZERO, CNetWork::GetInstance()->GetIdx());
    pPlayer->SetType(CPlayer::TYPE::TYPE_ACTIVE);
}

//===================================================
// �v���C���[�̐���(�}���`)
//===================================================
void CGame::CreateMultiPlayer(void)
{
    auto net = CNetWork::GetInstance();

    for (int i = 0; i < NetWork::MAX_CONNECT; i++)
    {
        if (!net->GetConnect(i)) { continue; }

        CPlayer* pPlayer = CPlayer::Create(D3DXVECTOR3(-3034.65f, 1.0f, 1.0f + 20.0f * i),
            VECTOR3_ZERO, VECTOR3_ZERO, i);

        // �v���C���[���g
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
// �x�@�̐���
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
// �Ԃ̐���
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