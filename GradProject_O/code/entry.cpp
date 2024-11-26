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

//===============================================
// �萔��`
//===============================================
namespace
{
    const int WIDTH_NUM = 2;   // ���̕�����
    const int HEIGHT_NUM = 2;  // �c�̕�����
    const int MAX_PLAYER = 4;  // �v���C���[�̍ő吔

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
// �R���X�g���N�^
//===============================================
CEntry::CEntry()
{
	m_ppCamera = nullptr;
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
    CObject2D* pObj = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), VECTOR3_ZERO);
    pObj->SetSize(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);
    pObj->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\concrete002.jpg"));

    CCameraManager* mgr = CCameraManager::GetInstance();

	m_ppCamera = new CMultiCamera*[MAX_PLAYER];

    for (int i = 0; i < MAX_PLAYER; i++)
    {
        m_ppCamera[i] = DEBUG_NEW CMultiCamera;
        m_ppCamera[i]->Init();
        m_ppCamera[i]->SetPositionV(CAMERA_POS_V[i]);
        m_ppCamera[i]->SetPositionR(CAMERA_POS_R[i]);

        D3DVIEWPORT9 viewport;
        //�v���C���[�Ǐ]�J�����̉�ʈʒu�ݒ�
        viewport.X = (DWORD)((SCREEN_WIDTH * 0.5f) * (i % WIDTH_NUM));
        viewport.Y = (DWORD)((SCREEN_HEIGHT * 0.5f) * (i / WIDTH_NUM));

        if constexpr (MAX_PLAYER < WIDTH_NUM) {
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
        }

        viewport.MinZ = 0.0f;
        viewport.MaxZ = 1.0f;
        m_ppCamera[i]->SetViewPort(viewport);

        //mgr->ListIn(pCamera);
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

    AddPlayer();

    CScene::Update();

    CPlayerManager* mgr = CPlayerManager::GetInstance();
    CDebugProc::GetInstance()->Print("�v���C���[�̐�[%d]\n", mgr->GetNum());

    for (int i = 0; i < 4; i++)
    {
        CDebugProc::GetInstance()->Print("���߂�̈ʒu[%f, %f, %f]\n", m_ppCamera[i]->GetPositionV());
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
    CInputKeyboard* pKey = CInputKeyboard::GetInstance();
    CInputPad* pPad = CInputPad::GetInstance();
    CPlayerManager* mgr = CPlayerManager::GetInstance();

    if (pPad->GetTrigger(CInputPad::BUTTON_A, 0))
    {
        if (mgr->GetNum() < PLAYER_MAX) { // �l�����ő�ł͂Ȃ��ꍇ

            int id = mgr->GetNum();

            CPlayer* pPlayer = CPlayer::Create(D3DXVECTOR3(-320.3f, 1.0f, -91.6f), VECTOR3_ZERO, VECTOR3_ZERO, id);
            mgr->ListIn(pPlayer);

        }
    }
}