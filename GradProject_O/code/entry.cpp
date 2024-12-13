//===============================================
//
// �G���g���[��ʂ̊Ǘ����� [entry.cpp]
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
#include "meshfield.h"
#include "goal_manager.h"
#include "edit_manager.h"
#include "scrollText2D.h"

//===============================================
// �萔��`
//===============================================
namespace
{
    const int WIDTH_NUM = 4;   // ���̕�����
    const int HEIGHT_NUM = 2;  // �c�̕�����

    const float LENGTH = 500.0f;
    const float ROTATION_Y = 0.005f;

    const char* MODEL_PATH = "data\\MODEL\\bike.x";  // �v���C���[�̃��f���p�X

    const D3DXVECTOR3 TITLE_POS = D3DXVECTOR3(SCREEN_CENTER.x, 100.0f, 0.0f);	// �^�C�g�����W

    const float TITLE_SIZE = 150.0f;	// �^�C�g���̕����T�C�Y

    // ������@UI�̃e�N�X�`���p�X
    const char* TEX_PATH[NUM_CONTROL_UI] =
    {
        "data\\TEXTURE\\controlUI\\01_controlUI_Turn.png",
        "data\\TEXTURE\\controlUI\\02_controlUI_Accel.png",
        "data\\TEXTURE\\controlUI\\03_controlUI_Brake.png",
        "data\\TEXTURE\\controlUI\\04_controlUI_Boost.png",
    };

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

    const D3DXCOLOR TEX_COL_ALPHA = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);   // ������
    const D3DXCOLOR TEX_COL_OPAQUE = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);  // �s����
}

namespace READY
{
    const char* TEX_PATH_OK = "data\\TEXTURE\\GameOver-Yes.png";  // �����ł���
    const char* TEX_PATH_NO = "data\\TEXTURE\\GameOver-No.png";   // �����ł��ĂȂ�
}

//===============================================
// �R���X�g���N�^
//===============================================
CEntry::CEntry()
{
    // �l���N���A
	m_ppCamera = nullptr;
    m_ppObjX = nullptr;
    m_pGoalManager = nullptr;
    m_pString = nullptr;
    m_IsFinish = false;
    m_bSetReady = false;
    m_nID = -1;

    for (int i = 0; i < NUM_CONTROL_UI; i++)
    {
        m_pControlsUI[i] = nullptr;
    }

    for (int i = 0; i < MAX_PLAYER; i++)
    {
        m_pReady[i] = nullptr;
    }
}

//===============================================
// �f�X�g���N�^
//===============================================
CEntry::~CEntry()
{

}

//===============================================
// ����������
//===============================================
HRESULT CEntry::Init(void)
{
    auto net = CNetWork::GetInstance();
    net->ReConnect();

    // ID�擾��҂�
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

    // �}�b�v�ǂݍ���
    CMapManager::GetInstance()->Load();

    // ���b�V���t�B�[���h����
    CMeshField::Create(D3DXVECTOR3(0.0f, -10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1000.0f, 1000.0f, "data\\TEXTURE\\field001.jpg", 30, 30);

    if (m_pGoalManager == nullptr)
    {
        m_pGoalManager = CGoalManager::Create();
        if (m_pGoalManager->GetCreateIdx() <= 0)
        {
            m_pGoalManager->Init();
        }
    }

    CObject2D *pObj = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.85f, SCREEN_HEIGHT * 0.2f, 0.0f), VECTOR3_ZERO, 7);
    pObj->SetSize(100.0f, 40.0f);
    pObj->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\controlUI\\00_controlUI_Header.png"));

    // ������@UI�̐���
    for (int i = 0; i < NUM_CONTROL_UI; i++)
    {
        m_pControlsUI[i] = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.9f, SCREEN_HEIGHT * 0.3f + i * 60.0f, 0.0f), VECTOR3_ZERO, 7);
        m_pControlsUI[i]->SetSize(125.0f, 30.0f);
        m_pControlsUI[i]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(TEX_PATH[i]));
        m_pControlsUI[i]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));
    }

    CCameraManager* mgr = CCameraManager::GetInstance();
	m_ppCamera = DEBUG_NEW CMultiCamera*[MAX_PLAYER];
    m_ppObjX = DEBUG_NEW CObjectX * [MAX_PLAYER];

    for (int i = 0; i < MAX_PLAYER; i++)
    {
        m_ppCamera[i] = nullptr;
        m_ppObjX[i] = nullptr;
    }

    CCamera *pCamera = mgr->GetTop();

    if (pCamera != nullptr)
    {
        pCamera->SetPositionV(D3DXVECTOR3(0.0f, 4000.0f, 0.0f));
    }

    // �����J�����̐���
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
        
        // �v���C���[�Ǐ]�J�����̉�ʈʒu�ݒ�
        viewport.X = (DWORD)((SCREEN_WIDTH / 4.0f) * (i % WIDTH_NUM));
        viewport.Y = (DWORD)(SCREEN_HEIGHT * 0.7f);

        viewport.Width = (DWORD)(SCREEN_WIDTH / 4.0f);
        viewport.Height = (DWORD)(SCREEN_HEIGHT * 0.4f);

        viewport.MinZ = 0.0f;
        viewport.MaxZ = 1.0f;

        m_ppCamera[i]->SetViewPort(viewport);
    }

    // �z�B�J�n�̕�������
    m_pString = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, TITLE_POS,
        0.4f, TITLE_SIZE, TITLE_SIZE, XALIGN_CENTER, YALIGN_CENTER, VECTOR3_ZERO, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

    if (m_pString != nullptr)
    {
        // �����ݒ�
        m_pString->PushBackString("�z�B�J�n!!");
        m_pString->SetEnableScroll(false);
    }
  
	return S_OK;
}

//===============================================
// �I������
//===============================================
void CEntry::Uninit(void)
{
    // �J�����̔j��
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

    // ��ʉ��ɂł�v���C���[���f���̔j��
    if (m_ppObjX != nullptr)
    {
        for (int i = 0; i < MAX_PLAYER; i++)
        {
            if (m_ppObjX[i] == nullptr) { continue; }

            m_ppObjX[i]->Uninit();
            m_ppObjX[i] = nullptr;
        }

        delete[] m_ppObjX;
        m_ppObjX = nullptr;
    }

    // ����UI�̔j��
    if (m_pControlsUI != nullptr)
    {
        for (int i = 0; i < NUM_CONTROL_UI; i++)
        {
            if (m_pControlsUI[i] == nullptr) { continue; }

            m_pControlsUI[i]->Uninit();
            m_pControlsUI[i] = nullptr;
        }
    }

    // ����UI�̔j��
    if (m_pReady != nullptr)
    {
        for (int i = 0; i < MAX_PLAYER; i++)
        {
            if (m_pReady[i] == nullptr) { continue; }

            m_pReady[i]->Uninit();
            m_pReady[i] = nullptr;
        }
    }

    if (m_pString != nullptr)
    {
        m_pString->Uninit();
        m_pString = nullptr;
    }

    // �S�[���}�l�[�W���[�̔j��
    SAFE_RELEASE(m_pGoalManager);
}

//===============================================
// �X�V����
//===============================================
void CEntry::Update(void)
{
    CInputKeyboard* pKey = CInputKeyboard::GetInstance();
    CInputPad* pPad = CInputPad::GetInstance();

    CEditManager* pMgr = CEditManager::GetInstance();
    // �G�f�B�^�[����
    if (pKey->GetTrigger(DIK_F4) && CEditManager::GetInstance() == nullptr)
    {
        pMgr = CEditManager::Create();
    }

    // �G�f�B�^�[�X�V
    if (pMgr != nullptr) { pMgr->Update(); }

    for (int i = 0; i < MAX_PLAYER; i++)
    {
        if (m_ppObjX[i] != nullptr)
        {
            D3DXVECTOR3 rot = m_ppObjX[i]->GetRotation();
            rot.y += ROTATION_Y;
            m_ppObjX[i]->SetRotation(rot);
        }
    }

    // �S�[���}�l�[�W���[�̍X�V
    if (m_pGoalManager != nullptr)
    {
        m_pGoalManager->Update();
    }

    // ������@UI
    ControlsUI();

    // ����UI
    //ReadyUp();

    // �v���C���[�Q������
    AddPlayer();

    // �v���C���[�Q������������
    DecreasePlayer();

    // �V�[���̍X�V
    CScene::Update();

    if (m_pString != nullptr)
    {
        if (m_pString->GetEnd())
        {// �����ǂݍ��݂��I�����

            // ���̉�ʂɑJ��
            if (CManager::GetInstance()->GetMode() == CScene::MODE::MODE_ENTRY)
            {
                CManager::GetInstance()->GetFade()->Set(CScene::MODE::MODE_GAME);
            }
        }
    }

    CPlayerManager* mgr = CPlayerManager::GetInstance();

    // �f�o�b�N�\��
    CDebugProc::GetInstance()->Print("�v���C���[�̐�[%d]\n", mgr->GetNum());

    for (int i = 0; i < MAX_PLAYER; i++)
    {
        // �J����
        D3DXVECTOR3 CamPosV = m_ppCamera[i]->GetPositionV();
        D3DXVECTOR3 CamPosR = m_ppCamera[i]->GetPositionR();
        CDebugProc::GetInstance()->Print("�J���� ���_   : [%f, %f, %f]\n", CamPosV.x, CamPosV.y, CamPosV.z);
        CDebugProc::GetInstance()->Print("�J���� �����_ : [%f, %f, %f]\n", CamPosR.x, CamPosR.y, CamPosR.z);
        CDebugProc::GetInstance()->Print("���� [%f]\n", m_ppCamera[i]->GetLength());
    }
}

//===============================================
// �`�揈��
//===============================================
void CEntry::Draw(void)
{
    // �`�揈��(�R���K�v�����!) �� ���ӊ����J�����
    CScene::Draw();
}

//===============================================
// �������������v���C���[�̔ԍ����󂯎��
//===============================================
void CEntry::SetID(const int id)
{
    m_nID = id;
}

//===============================================
// �����������Ă��邩�ǂ����󂯎��
//===============================================
void CEntry::ChangeFlag(bool value)
{
    m_bSetReady = value;
}

//===============================================
// �`���[�g���A���I�����o����
//===============================================
void CEntry::EndTutorial(void)
{
    if (m_pString != nullptr)
    {
        m_pString->SetEnableScroll(true);
    }
}

//===============================================
// ����OK
//===============================================
void CEntry::ReadyUp(const int id)
{
    auto mgr = CPlayerManager::GetInstance();

    // �g�p����Ă��Ȃ������珈���𔲂���
    if (m_pReady[id] == nullptr) { return; }

    if (m_bSetReady)
    {// ���������ł��Ă�

        // �e�N�X�`���ύX
        m_pReady[id]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(READY::TEX_PATH_OK));

        return;
    }

    if (!m_bSetReady)
    {// ���������ł��ĂȂ�

        // �e�N�X�`���ύX
        m_pReady[id]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(READY::TEX_PATH_NO));

        return;
    }
}

//===============================================
// �v���C���[�Q������
//===============================================
void CEntry::AddPlayer(void)
{
    CInputKeyboard* pKey = CInputKeyboard::GetInstance();
    CInputPad* pPad = CInputPad::GetInstance();
    auto net = CNetWork::GetInstance();
    auto mgr = CPlayerManager::GetInstance();
    int id = mgr->GetNum();

    // �l���m�F
    if (net->GetState() == CNetWork::STATE::STATE_ONLINE)
    { 
        for (int i = 0; i < NetWork::MAX_CONNECT; i++)
        {
            auto player = mgr->GetPlayer(i);
           
            // �l��������
            if (player == nullptr && net->GetConnect(i))
            {
                // �v���C���[����
                D3DXVECTOR3 pos = m_ppCamera[i]->GetPositionR();
                CPlayer* pPlayer = CPlayer::Create(D3DXVECTOR3(4000.0f, 0.0f, 1600.0f), D3DXVECTOR3(0.0f, CAMERA_ROT[i].y, 0.0f), VECTOR3_ZERO, i);

                // �f�[�^��M
                pPlayer->SetType(CPlayer::TYPE::TYPE_RECV);
                pPlayer->SetRecvPosition(pos);

                if (i == net->GetIdx())
                {
                    // �f�[�^���M
                    pPlayer->SetType(CPlayer::TYPE::TYPE_SEND);

                    // �`���[�g���A�����̃A�N�e�B�u�ɐݒ�
                    pPlayer->SetType(CPlayer::TYPE::TYPE_TUTOLERIAL_ACTIVE);
                }

                // �G�t�F�N�g�I��
                //pPlayer->EffectUninit();

                // ��ʉ��Ƀv���C���[�̃��f����\��
                pos = m_ppCamera[i]->GetPositionR();
                m_ppObjX[i] = CObjectX::Create(pos, D3DXVECTOR3(0.0f, CAMERA_ROT[i].y, 0.0f), MODEL_PATH, 7);
                m_ppObjX[i]->SetType(CObject::TYPE::TYPE_PLAYER);
                m_ppObjX[i]->SetRotateType(CObjectX::TYPE_QUATERNION);
                m_ppObjX[i]->SetColMulti(pPlayer->GetObj()->GetColMuliti());

                // �����ł��Ă邩�ǂ�����UI�̐���
                D3DXVECTOR3 POS = D3DXVECTOR3((SCREEN_WIDTH * (0.25f * float(i + 1)) - 150.0f), SCREEN_HEIGHT * 0.7f, 0.0f);
                m_pReady[i] = CObject2D::Create(POS, VECTOR3_ZERO, 7);
                m_pReady[i]->SetSize(100.0f, 50.0f);
                m_pReady[i]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(READY::TEX_PATH_NO));
            }
        }
    }

    if (pKey->GetTrigger(DIK_RETURN) || pPad->GetTrigger(CInputPad::BUTTON_A, 0))
    {
        if (m_pString->IsScroll() || m_pString->GetEnd()) { return; }

        m_IsFinish ^= true;

        if (m_IsFinish)
        {// �`���[�g���A�����I�����Ă���

            net->SendTutorialOk();
        }
        else
        {
            net->SendTutorialNo();
        }
    }
}

//===============================================
// �v���C���[�Q������������
//===============================================
void CEntry::DecreasePlayer(void)
{
    auto net = CNetWork::GetInstance();
    auto mgr = CPlayerManager::GetInstance();

    // �l���m�F
    if (net->GetState() == CNetWork::STATE::STATE_ONLINE) { return; }
    
    for (int i = 0; i < NetWork::MAX_CONNECT; i++)
    {
        auto player = mgr->GetPlayer(i);

        // �l��������
        if (player != nullptr && !net->GetConnect(i))
        {
            // �v���C���[�̏I������
            player->Uninit();

            if (m_ppObjX[i] != nullptr)
            {// ��ʉ��ɏo�Ă�v���C���[���f���̏I��

                m_ppObjX[i]->Uninit();
                m_ppObjX[i] = nullptr;
            }
        }
    }
}

//===============================================
// ������@UI
//===============================================
void CEntry::ControlsUI(void)
{
    // �v���C���[�̑����擾
    int num = CPlayerManager::GetInstance()->GetNum();

    // �v���C���[�̑�����0�ȉ��̂Ƃ������𔲂���
    if (num <= 0) { return; }

    // �Q�[���p�b�h�擾
    CInputPad* pPad = CInputPad::GetInstance();

    // �������ɂ���
    for (int i = 0; i < NUM_CONTROL_UI; i++)
    {
        if (m_pControlsUI[i] == nullptr) { continue; }

        m_pControlsUI[i]->SetCol(TEX_COL_ALPHA);
    }

    // �E�g���K�[�̒l�擾
    float fAccel = (float)pPad->GetRightTriggerPress(0);

    // ���g���K�[�̒l�擾
    float fBrake = (float)pPad->GetLeftTriggerPress(0);

    // ���X�e�B�b�N�̒l�擾
    float fLStick = pPad->GetLStick(0, 0.1f).x;

    // ����
    if (1.0f <= fLStick || -1.0f >= fLStick)
    {
        m_pControlsUI[UI_TURN]->SetCol(TEX_COL_OPAQUE);
    }

    // �A�N�Z��
    if (1.0f <= fAccel)
    {
        m_pControlsUI[UI_ACCEL]->SetCol(TEX_COL_OPAQUE);
    }

    // �u���[�L
    if (1.0f <= fBrake)
    {
        m_pControlsUI[UI_BRAKE]->SetCol(TEX_COL_OPAQUE);
    }

    // �u�[�X�g
    if (pPad->GetPress(CInputPad::BUTTON_B, 0))
    {
        m_pControlsUI[UI_BOOST]->SetCol(TEX_COL_OPAQUE);
    }
}

//===============================================
// ����OK����
//===============================================
void CEntry::ReadyUp(void)
{
    auto mgr = CPlayerManager::GetInstance();

    // �g�p����Ă��Ȃ������珈���𔲂���
    if (m_pPPP == nullptr || m_nID == -1) { return; }

    if (m_bSetReady)
    {// ���������ł��Ă�

        // �e�N�X�`���ύX
        m_pPPP->BindTexture(CManager::GetInstance()->GetTexture()->Regist(READY::TEX_PATH_OK));
    }

    if (!m_bSetReady)
    {// ���������ł��ĂȂ�

        // �e�N�X�`���ύX
        m_pPPP->BindTexture(CManager::GetInstance()->GetTexture()->Regist(READY::TEX_PATH_NO));
    }

    //m_nID = -1;
}

CTutorialStep::CTutorialStep()
{
}

CTutorialStep::~CTutorialStep()
{
}