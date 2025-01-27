//===============================================
//
// �}�l�[�W���[�̏��� [manager.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "sound.h"
#include "camera.h"
#include "light.h"
#include "Xfile.h"
#include "texture.h"
#include "game.h"
#include "title.h"
#include "result.h"
#include "tutorial.h"
#include "multi_result.h"
#include "fade.h"
#include "object.h"
#include "ranking.h"
#include "slow.h"
#include "task_manager.h"
#include "object_manager.h"
#include "camera_manager.h"
#include "player_manager.h"
#include "player.h"
#include "road_manager.h"
#include "car_manager.h"
#include "effekseerControl.h"
#include "objectsound.h"
#include "font.h"
#include "deltatime.h"
#include "entry.h"
#include "network.h"
#include "fxmanager.h"
//===============================================
// �ÓI�����o�ϐ�
//===============================================
CManager *CManager::m_pManager = nullptr;

//===================================================
// �R���X�g���N�^
//===================================================
CManager::CManager()
{
	m_pRenderer = nullptr;		// �����_���[�̃|�C���^
	m_pInput = nullptr;
	m_pDebugProc = nullptr;		// �f�o�b�O�\���̃|�C���^
	m_pSound = nullptr;			// �T�E���h�̃|�C���^
	m_pCamera = nullptr;			// �J�����̃|�C���^
	m_pLight = nullptr;			// ���C�g�̃|�C���^
	m_pTexture = nullptr;		// �e�N�X�`���̃|�C���^
	m_pModelFile = nullptr;		// X�t�@�C�����̃|�C���^
	m_pSlow = nullptr;			// �X���[��Ԃւ̃|�C���^
	m_pScene = nullptr;			// �V�[���̃|�C���^
	m_pFade = nullptr;			// �t�F�[�h�ւ̃|�C���^
	m_pDeltaTime = nullptr;     // �^�C�}�[�ւ̃|�C���^
	m_pFont = nullptr;
	m_pNetWork = nullptr;

	m_nDeliveryStatus = 0;
	m_fLife = 0.0f;
	m_bSuccess = false;
}

//===================================================
// �f�X�g���N�^
//===================================================
CManager::~CManager()
{

}

//===================================================
// ����������
//===================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// �����_���[�̐���
	if (m_pRenderer == nullptr)
	{// �g�p���Ă��Ȃ��ꍇ
		m_pRenderer = DEBUG_NEW CRenderer;
	}

	//����������
	if (m_pRenderer != nullptr)
	{// �g�p���Ă���ꍇ
		if (FAILED(m_pRenderer->Init(hWnd, TRUE)))
		{//�����������s�����ꍇ
			return E_FAIL;
		}
	}

	// ���͂̐���
	if (m_pInput == nullptr)
	{// �g�p���Ă��Ȃ��ꍇ
		m_pInput = CInput::Create(hInstance, hWnd);
	}

	// �f�o�b�O�\���̐���
	if (m_pDebugProc == nullptr)
	{// �g�p���Ă��Ȃ��ꍇ
		m_pDebugProc = CDebugProc::Create();
	}

	// �T�E���h�̐���
	if (m_pSound == nullptr)
	{// �g�p���Ă��Ȃ��ꍇ
		m_pSound = DEBUG_NEW CSound;

		// ������
		if (m_pSound != nullptr)
		{
			m_pSound->Init(hWnd);
		}
	}
	CMasterSound::GetInstance()->Init(hWnd);

	// �J�����̐���
	if (m_pCamera == nullptr)
	{// �g�p���Ă��Ȃ��ꍇ
		m_pCamera = DEBUG_NEW CMultiCamera;

		// ������
		if (m_pCamera != nullptr)
		{
			m_pCamera->Init();

			D3DVIEWPORT9 viewport;

			//�v���C���[�Ǐ]�J�����̉�ʈʒu�ݒ�
			viewport.X = 0;
			viewport.Y = 0;
			viewport.Width = (DWORD)(SCREEN_WIDTH * 1.0f);
			viewport.Height = (DWORD)(SCREEN_HEIGHT * 1.0f);
			viewport.MinZ = 0.0f;
			viewport.MaxZ = 1.0f;

			m_pCamera->SetViewPort(viewport);
		}
	}

	// ���C�g�̐���
	if (m_pLight == nullptr)
	{// �g�p���Ă��Ȃ��ꍇ
		m_pLight = DEBUG_NEW CLight;

		// ������
		if (m_pLight != nullptr)
		{
			m_pLight->Init();
		}
	}

	//�e�N�X�`���̐���
	if (m_pTexture == nullptr)
	{// �g�p���Ă��Ȃ��ꍇ
		m_pTexture = DEBUG_NEW CTexture;

		// �����ǂݍ���
		if (m_pTexture != nullptr)
		{
			m_pTexture->Load();
		}
	}

	// X�t�@�C�����̐���
	if (m_pModelFile == nullptr)
	{// �g�p���Ă��Ȃ��ꍇ
		m_pModelFile = DEBUG_NEW CXFile;
	}

	// �X���[���̐���
	if (m_pSlow == nullptr)
	{
		m_pSlow = DEBUG_NEW CSlow;
		m_pSlow->Init();
	}

	if (m_pDeltaTime == nullptr)
	{
		m_pDeltaTime = DEBUG_NEW CDeltaTime;
		m_pDeltaTime->Init();
	}
	if (m_pFont == nullptr)
	{
		m_pFont = CFont::Create();
	}

	// �l�b�g���[�N�̐���
	if (m_pNetWork == nullptr)
	{
		m_pNetWork = CNetWork::Create();
	}

	// �G�t�F�N�V�A������
	CEffekseer::GetInstance()->Init();
	
	// ���[�h�̐���
	SetMode(CScene::MODE::MODE_TITLE);

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CManager::Uninit(void)
{
	// �T�E���h�̒�~
	m_pSound->Stop();

	if (m_pFade != nullptr)
	{
		m_pFade->Uninit();
		delete m_pFade;
		m_pFade = nullptr;
	}

	if (m_pScene != nullptr)
	{
		m_pScene->Uninit();
		delete m_pScene;
		m_pScene = nullptr;
	}

	if (m_pSlow != nullptr)
	{
		m_pSlow->Uninit();
		delete m_pSlow;
		m_pSlow = nullptr;
	}

	if (m_pLight != nullptr)
	{
		m_pLight->Uninit();
		delete m_pLight;
		m_pLight = nullptr;
	}

	if (m_pCamera != nullptr)
	{
		m_pCamera->Uninit();
		delete m_pCamera;
		m_pCamera = nullptr;
	}

	if (m_pSound != nullptr)
	{
		m_pSound->Uninit();
		delete m_pSound;
		m_pSound = nullptr;
	}

	if (m_pInput != nullptr)
	{// �g�p���Ă���ꍇ

		m_pInput->Release();
		m_pInput = nullptr;	// �g�p���Ă��Ȃ���Ԃɂ���
	}

	if (m_pDebugProc != nullptr)
	{// �g�p���Ă���ꍇ
		// �I������
		m_pDebugProc->Uninit();
		m_pDebugProc = nullptr;	// �g�p���Ă��Ȃ���Ԃɂ���
	}

	if (m_pRenderer != nullptr)
	{// �g�p���Ă���ꍇ
		// �I������
		m_pRenderer->Uninit();

		delete m_pRenderer;	// �������̊J��

		m_pRenderer = nullptr;	// �g�p���Ă��Ȃ���Ԃɂ���
	}

	if (m_pSound != nullptr)
	{// �g�p���Ă���ꍇ
		// �I������
		m_pSound->Uninit();

		delete m_pSound;	// �������̊J��

		m_pSound = nullptr;	// �g�p���Ă��Ȃ���Ԃɂ���
	}

	// �e�N�X�`���̔p��
	if (m_pTexture != nullptr)
	{// �g�p���Ă���ꍇ
		// �I������
		m_pTexture->Unload();

		delete m_pTexture;	// �������̊J��

		m_pTexture = nullptr;	// �g�p���Ă��Ȃ���Ԃɂ���
	}

	// X�t�@�C�����̔p��
	if (m_pModelFile != nullptr)
	{// �g�p���Ă���ꍇ
		// �I������
		m_pModelFile->Unload();

		delete m_pModelFile;	// �������̊J��

		m_pModelFile = nullptr;	// �g�p���Ă��Ȃ���Ԃɂ���
	}

	// �^�C�}�[�̔j��
	if (m_pDeltaTime != nullptr)
	{// �g�p���Ă���ꍇ

		// �I������
		m_pDeltaTime->Uninit();

		// �������̉��
		delete m_pDeltaTime;

		// �g�p���Ă��Ȃ���Ԃɂ���
		m_pDeltaTime = nullptr;
	}

	// �l�b�g���[�N�̐���
	if (m_pNetWork != nullptr)
	{
		m_pNetWork->Release();
		m_pNetWork = nullptr;
	}
	CFXManager::Release();
	SAFE_UNINIT_DELETE(m_pFont);
	// �e��}�l�[�W���̔j��
	CListManager::Release();
	CMasterSound::GetInstance()->Uninit();
	CMasterSound::Release();
	//�V�F�[�_�[���C�g�̔j��
	CShaderLight::Release();
	// �G�t�F�N�V�A�j��
	CEffekseer::GetInstance()->Uninit();
}

//===================================================
// �X�V����
//===================================================
void CManager::Update(void)
{
	if (m_pDeltaTime != nullptr)
	{
		m_pDeltaTime->Update();
	}

	if (m_pFade != nullptr)
	{
		m_pFade->Update();
	}

	// �f�o�b�O�\���̍X�V����
	if (m_pDebugProc != nullptr)
	{// �g�p���Ă���ꍇ
		m_pDebugProc->Update();
		m_pDebugProc->Print("�������g��ID [ %d ]\n", m_pNetWork->GetIdx());

		for (int i = 0; i < NetWork::MAX_CONNECT; i++)
		{
			if (m_pNetWork->GetConnect(i))
			{
				m_pDebugProc->Print("%d�ԃI�����C������\n", i);
			}
		}
	}

	// ���͂̍X�V����
	if (m_pInput != nullptr)
	{// �g�p���Ă���ꍇ
		m_pInput->Update();
	}

	// ���M�^�C�~���O�擾
	if (m_pNetWork != nullptr)
	{
		m_pNetWork->GetTime()->End();
	}

	if (m_pScene != nullptr)
	{
		CEffekseer::GetInstance()->Update();
		m_pScene->Update();
	}

	if (m_pNetWork != nullptr)
	{
		m_pNetWork->Update();
	}
}

//===================================================
// �`�揈��
//===================================================
void CManager::Draw(void)
{
	if (m_pScene != nullptr)
	{
		m_pScene->Draw();
	}
}

//===================================================
// �����_���[�̎擾
//===================================================
CRenderer *CManager::GetRenderer(void)
{
	return m_pRenderer;
}

//===================================================
// �f�o�b�O�\���̎擾
//===================================================
CDebugProc *CManager::GetDebugProc(void)
{
	return m_pDebugProc;
}

//===================================================
// �T�E���h�̎擾
//===================================================
CSound *CManager::GetSound(void)
{
	return m_pSound;
}

//===================================================
// �J�����̎擾
//===================================================
CMultiCamera *CManager::GetCamera(void)
{
	return m_pCamera;
}

//===================================================
// ���C�g�̎擾
//===================================================
CLight *CManager::GetLight(void)
{
	return m_pLight;
}

//===================================================
// �e�N�X�`���̎擾
//===================================================
CTexture *CManager::GetTexture(void)
{
	return m_pTexture;
}

//===================================================
// X�t�@�C�����̎擾
//===================================================
CXFile *CManager::GetModelFile(void)
{
	return m_pModelFile;
}

//===================================================
// �X���[���̎擾
//===================================================
CSlow *CManager::GetSlow(void)
{
	return m_pSlow;
}

//===================================================
// �t�F�[�h���̎擾
//===================================================
CFade *CManager::GetFade(void)
{
	return m_pFade;
}
//===================================================
// �t�H���g���̎擾
//===================================================
CFont* CManager::GetFont(void)
{
	return m_pFont;
}
//===================================================
// �^�C�}�[���̎擾
//===================================================
CDeltaTime* CManager::GetDeltaTime(void)
{
	return m_pDeltaTime;
}

//===================================================
// �f�[�^�S������
//===================================================
CManager *CManager::GetInstance(void)
{
	if (m_pManager == nullptr)
	{
		m_pManager = DEBUG_NEW CManager;
	}

	return m_pManager;
}

//===================================================
// �J��
//===================================================
void CManager::Release(void)
{
	if (m_pManager != nullptr)
	{
		m_pManager->Uninit();
		delete m_pManager;
		m_pManager = nullptr;
	}
}

//===================================================
// �f�[�^�S������
//===================================================
void CManager::DataReset(void)
{
	// �^�X�N�}�l�[�W���[�̏I��
	CTaskManager::GetInstance()->Uninit();

	// �I�u�W�F�N�g�}�l�[�W���[�̏I��
	CObjectManager::GetInstance()->Uninit();

	if (GetRenderer() != nullptr)
	{
		// �t�B�[�h�o�b�N�G�t�F�N�g���Z�b�g
		CManager::GetInstance()->GetRenderer()->SetEnableDrawMultiScreen(
			0.0f,
			1.0f,
			0.01f);
	}

	//�t�F�[�h�̍폜
	m_pFade = nullptr;

	// �^�X�N�}�l�[�W���[�̏�����
	CTaskManager::GetInstance()->Init();

	// �I�u�W�F�N�g�}�l�[�W���[�̏�����
	CObjectManager::GetInstance()->Init();
}

//===================================================
// ���[�h�ݒ�
//===================================================
void CManager::SetMode(CScene::MODE mode)
{
	// �T�E���h�̒�~
	if (m_pSound != nullptr)
	{
		m_pSound->Stop();
	}

	// ���݂̃��[�h�̒�~
	if (m_pScene != nullptr)
	{
		m_pScene->Uninit();
		delete m_pScene;
		m_pScene = nullptr;
	}

	if (m_pFade != nullptr)
	{
		m_pFade->Uninit();
		delete m_pFade;
		m_pFade = nullptr;
	}

	// �f�[�^���Z�b�g
	DataReset();

	// �V�������[�h�̐���
	m_pScene = CScene::Create(mode);

	if (m_pScene != nullptr)
	{
		m_pScene->SetMode(mode);
		m_pScene->Init();
	}

	// �t�F�[�h�̐���
	if (m_pFade == nullptr)
	{
		m_pFade = CFade::Create(mode);
	}
}

//===================================================
//
// �V�[���N���X
//
//===================================================
// �R���X�g���N�^
//===================================================
CScene::CScene()
{

}

//===================================================
// �f�X�g���N�^
//===================================================
CScene::~CScene()
{

}

//===================================================
// ����
//===================================================
CScene *CScene::Create(MODE mode)
{
	CScene *pScene = nullptr;

	// ���[�h�ʂɐ���
	switch (mode)
	{
	case MODE_TITLE:
		pScene = DEBUG_NEW CTitle;
		break;

	case MODE_ENTRY:
		pScene = DEBUG_NEW CEntry;
		break;

	case MODE_GAME:
		pScene = DEBUG_NEW CGame;
		break;

	case MODE_RESULT:
		pScene = DEBUG_NEW CResult;
		break;

	case MODE_MULTI_RESULT:
		pScene = DEBUG_NEW CMultiResult;
		break;
	}

	return pScene;
}

//===================================================
// ������
//===================================================
HRESULT CScene::Init(void)
{
	return S_OK;
}

//===================================================
// �I��
//===================================================
void CScene::Uninit(void)
{

}

//===================================================
// �X�V
//===================================================
void CScene::Update(void)
{
	// �J�����̍X�V����
	if (CManager::GetInstance()->GetCamera() != nullptr)
	{
		CManager::GetInstance()->GetCamera()->Update();
	}

	// �X���[�̍X�V����
	if (CManager::GetInstance()->GetSlow() != nullptr)
	{
		CManager::GetInstance()->GetSlow()->Update();
	}

	// �����_���[�̍X�V����
	if (CManager::GetInstance()->GetRenderer() != nullptr)
	{// �g�p���Ă���ꍇ
		CManager::GetInstance()->GetRenderer()->Update();
	}

	// �^�X�N�̑S�X�V
	CTaskManager::GetInstance()->Update();
}

//===================================================
// �`��
//===================================================
void CScene::Draw(void)
{
	// �`�揈��
	if (CManager::GetInstance()->GetRenderer() != nullptr)
	{// �g�p���Ă���ꍇ
		CManager::GetInstance()->GetRenderer()->Draw();
	}
}

