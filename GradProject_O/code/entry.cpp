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

//===============================================
// �萔��`
//===============================================
namespace
{
    const int WIDTH_NUM = 4;   // ���̕�����
    const int HEIGHT_NUM = 2;  // �c�̕�����
    const int MAX_PLAYER = 4;  // �v���C���[�̍ő吔

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
}

//===============================================
// �R���X�g���N�^
//===============================================
CEntry::CEntry()
{
    // �l���N���A
	m_ppCamera = nullptr;
    m_IsFinish = false;
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

    if (net->GetState() == CNetWork::STATE::STATE_ONLINE)
    {
        while (1)
        {
            net->SendGetId();

            if (net->GetIdx() != -1)
            {
                break;
            }
        }
    }
    else
    {
        CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);
        return E_FAIL;
    }

    // �}�b�v�ǂݍ���
    CMapManager::GetInstance()->Load();

    CObject2D* pObj = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.8f, 0.0f), VECTOR3_ZERO, 4);
    pObj->SetSize(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.2f);
    pObj->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\concrete002.jpg"));

    CCameraManager* mgr = CCameraManager::GetInstance();
	m_ppCamera = new CMultiCamera*[MAX_PLAYER];

    //mgr->GetTop()->SetDraw(false);

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
        //�v���C���[�Ǐ]�J�����̉�ʈʒu�ݒ�
        viewport.X = (DWORD)((SCREEN_WIDTH / 4.0f) * (i % WIDTH_NUM));
        viewport.Y = (DWORD)(SCREEN_HEIGHT * 0.7f);

        /*if constexpr (MAX_PLAYER < WIDTH_NUM) {
            viewport.Width = (DWORD)(SCREEN_WIDTH * 1.0f);
        }
        else
        {
            viewport.Width = (DWORD)(SCREEN_WIDTH * 0.5f);
        }

        if constexpr (MAX_PLAYER <= WIDTH_NUM) {
            viewport.Height = (DWORD)(SCREEN_HEIGHT * 1.0f);
        }
        else
        {
            viewport.Height = (DWORD)(SCREEN_HEIGHT * 0.5f);
        }*/

        viewport.Width = (DWORD)(SCREEN_WIDTH / 4.0f);
        viewport.Height = (DWORD)(SCREEN_HEIGHT * 0.4f);

        viewport.MinZ = 0.0f;
        viewport.MaxZ = 1.0f;
        m_ppCamera[i]->SetViewPort(viewport);
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
}

//===============================================
// �X�V����
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

    // �v���C���[�Q������
    AddPlayer();

    // �v���C���[�Q������������
    DecreasePlayer();

    CScene::Update();

    CPlayerManager* mgr = CPlayerManager::GetInstance();
    CDebugProc::GetInstance()->Print("�v���C���[�̐�[%d]\n", mgr->GetNum());

    for (int i = 0; i < MAX_PLAYER; i++)
    {
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
// �v���C���[�Q������
//===============================================
void CEntry::AddPlayer(void)
{
   
    auto net = CNetWork::GetInstance();
    auto mgr = CPlayerManager::GetInstance();

   
    // �l���m�F
    if (net->GetState() == CNetWork::STATE::STATE_ONLINE)
    {
        
        for (int i = 0; i < NetWork::MAX_CONNECT; i++)
        {
            auto player = mgr->GetPlayer(i);

           
            // �l��������
            if (player == nullptr && net->GetConnect(i))
            {
                D3DXVECTOR3 pos = m_ppCamera[i]->GetPositionR();
                CPlayer* pPlayer = CPlayer::Create(pos, D3DXVECTOR3(0.0f, CAMERA_ROT[i].y, 0.0f), VECTOR3_ZERO, i);
                pPlayer->SetType(CPlayer::TYPE::TYPE_RECV);
                pPlayer->SetType(CPlayer::TYPE::TYPE_SEND);
                //pPlayer->SetType(CPlayer::TYPE::TYPE_ACTIVE);
                pPlayer->EffectUninit();
            }

            D3DXVECTOR3 pos = m_ppCamera[num]->GetPositionR();
            CObjectX* pObj = CObjectX::Create(pos, D3DXVECTOR3(0.0f, CAMERA_ROT[num].y, 0.0f), "data\\MODEL\\flyingscooter.x");
            pObj->SetType(CObject::TYPE::TYPE_PLAYER);
            pObj->SetRotateType(CObjectX::TYPE_QUATERNION);

            // �`���[�g���A�����I�����Ă��邱�Ƃɂ���
            m_IsFinish = true;
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
}