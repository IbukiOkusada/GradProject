//===============================================
//
// �G���g���[��ʂ̊Ǘ����� [entry.cpp]
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

//===============================================
// �萔��`
//===============================================
namespace
{
    const int WIDTH_NUM = 2;   // ���̕�����
    const int HEIGHT_NUM = 2;  // �c�̕�����
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
    pObj->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\entry.png"));

	//m_ppCamera = DEBUG_NEW CMultiCamera * [4];

 //   for (int i = 0; i < 4; i++)
 //   {
 //       m_ppCamera[i] = new CMultiCamera;
 //       m_ppCamera[i]->Init();
 //       m_ppCamera[i]->SetPositionV(D3DXVECTOR3(-874.3f, 1124.15f, 1717.2f));
 //       m_ppCamera[i]->SetPositionR(D3DXVECTOR3(-320.3f, 1.0f, -91.6f));
 //       m_ppCamera[i]->SetLength(400.0f);
 //       m_ppCamera[i]->SetRotation(D3DXVECTOR3(0.0f, D3DX_PI * 1.0f, D3DX_PI * 0.41f));

 //       D3DVIEWPORT9 viewport;
 //       //�v���C���[�Ǐ]�J�����̉�ʈʒu�ݒ�
 //       viewport.X = (DWORD)((SCREEN_WIDTH * 0.5f) * (i % WIDTH_NUM));
 //       viewport.Y = (DWORD)((SCREEN_HEIGHT * 0.5f) * (i / WIDTH_NUM));

 //       if (i < WIDTH_NUM) {
 //           viewport.Width = (DWORD)(SCREEN_WIDTH * 1.0f);
 //       }
 //       else
 //       {
 //           viewport.Width = (DWORD)(SCREEN_WIDTH * 0.5f);
 //       }

 //       if (i <= WIDTH_NUM) {
 //           viewport.Height = (DWORD)(SCREEN_HEIGHT * 1.0f);
 //       }
 //       else
 //       {
 //           viewport.Height = (DWORD)(SCREEN_HEIGHT * 0.5f);
 //       }

 //       viewport.MinZ = 0.0f;
 //       viewport.MaxZ = 1.0f;
 //       m_ppCamera[i]->SetViewPort(viewport);
 //   }
  
	return S_OK;
}

//===============================================
// �I������
//===============================================
void CEntry::Uninit(void)
{

}

//===============================================
// �X�V����
//===============================================
void CEntry::Update(void)
{
    CInputKeyboard* pKey = CInputKeyboard::GetInstance();
    CInputPad* pPad = CInputPad::GetInstance();

    if (pPad->GetTrigger(CInputPad::BUTTON_A, 0) ||
        pPad->GetTrigger(CInputPad::BUTTON_START, 0) ||
        pKey->GetTrigger(DIK_RETURN))
    {
        CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);
    }
}

//===============================================
// �`�揈��
//===============================================
void CEntry::Draw(void)
{
    // �`�揈��(�R���K�v�����!)
    CScene::Draw();
}