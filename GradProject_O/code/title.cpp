//===============================================
//
// �^�C�g����ʂ̊Ǘ����� [title.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "title.h"
#include "manager.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "camera.h"
#include "texture.h"
#include "input_gamepad.h"
#include "input_keyboard.h"
#include "object2D.h"
#include "map_manager.h"
#include "meshfield.h"
#include "player.h"
#include "goal.h"

//===============================================
// �������O���
//===============================================
namespace {
	const D3DXVECTOR3 ENTERPOS = { 0.0f, 0.0f, 0.0f };	// ENTER ���W
	const D3DXVECTOR3 ENTERROT = { 0.0f, 0.0f, 0.0f };	// ENTER ����
	const int AUTOMOVE_RANKING = 640;	// �����L���O�����J�ڎ���
	const int MOVE_TUTORIAL = 120;		// �`���[�g���A���ɑJ�ڂ���܂ł̎���
	const int PLAYER_WALLKICKTIMER = 40;
	const int ENEMY_NUM = 3;			//���o�p�G�o����
	const char* FILEPASS = "data\\TXT\\player";	// �t�@�C���̃p�X
	const char* FILEEXT = ".txt";				// �t�@�C���̊g���q
	const int FILEPASS_SIZE = (200);			// �t�@�C���̃p�X�T�C�Y
}

//===============================================
// �R���X�g���N�^
//===============================================
CTitle::CTitle()
{
	m_nCounterRanking = 0;
	m_nCounterTutorial = 0;
	m_nCounterRanking = 0;
	m_nCounter = 0;
	m_bPush = false;
	m_pFileLoad = nullptr;
	m_pEnter = nullptr;
	m_bDisplay = false;
	m_eState = STATE::STATE_TEAMLOGO;
	m_bCol = false;

	for (int nCnt = 0; nCnt < OBJ2D_MAX; nCnt++)
	{//�z��̏�����
		m_pObject2D[nCnt] = nullptr;
	}

	m_bIniting = false;

}

//===============================================
// �f�X�g���N�^
//===============================================
CTitle::~CTitle()
{

}

//===============================================
// ����������
//===============================================
HRESULT CTitle::Init(void)
{

	// �J�����̏����ʒu�ݒ�
	//CCamera* pCamera = CManager::GetInstance()->GetCamera();
	//pCamera->SetPositionV(D3DXVECTOR3(-874.3f, 0.0f, 1717.2f));
	//pCamera->SetPositionR(D3DXVECTOR3(80.0f, 95.0f, 220.0f));
	//pCamera->SetLength(350.0f);
	//pCamera->SetRotation(D3DXVECTOR3(0.0f, -2.1f, 1.79f));
	//pCamera->SetActive(false);

	//�J�����������
	CCamera* pCam = CManager::GetInstance()->GetCamera();
	pCam->SetPositionV(D3DXVECTOR3(-874.3f, 0.0f, 1717.2f));
	pCam->SetPositionR(D3DXVECTOR3(80.0f, 95.0f, 220.0f));
	pCam->SetLength(350.0f);
	pCam->SetRotation(D3DXVECTOR3(0.0f, -2.1f, 1.79f));
	pCam->SetActive(false);

	// �J�ڃ^�C�}�[�ݒ�
	m_nCounterTutorial = MOVE_TUTORIAL;
	m_nCounterRanking = AUTOMOVE_RANKING;

	// �T�E���h�Đ�
	CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_TITLE);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo] = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), VECTOR3_ZERO);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetSize(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\logo_thaw.jpg"));

	return S_OK;
}

//===============================================
// �I������
//===============================================
void CTitle::Uninit(void)
{
	CManager::GetInstance()->GetSound()->Stop();
}

//===============================================
// �X�V����
//===============================================
void CTitle::Update(void)
{
	//�X�e�[�g���Ƃɏ�������
	switch (m_eState)
	{
		//�`�[�����S�X�e�[�g�������ꍇ
	case STATE::STATE_TEAMLOGO:

		//�L�[�{�[�h���͂��p�b�h���͂������
		if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) ||
			CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) ||
			CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0)) {
			m_bPush = true;
		}

		//�����ꂽ�ꍇ
		if (m_bPush)
		{
			StateLogo();
		}
		else
		{
			m_nCounterTutorial--;
			if (m_nCounterTutorial <= 0)
			{
				StateLogo();
			}
		}

		break;

		//�v���X�G���^�[�X�e�[�g�������ꍇ
	case STATE::STATE_PRESSENTER:

		InitingP_E();
		StateP_E();

		break;

		//�v���X�G���^�[����������̃X�e�[�g�������ꍇ
	case STATE::STATE_CHASING:

		break;

		//�A�C�X�𓊂������X�e�[�g�������ꍇ
	case STATE::STATE_ICETHROW:

		break;

		//����ȊO�������ꍇ
	default:

		break;
		
	}

	CScene::Update();
}

//===============================================
// �`�揈��
//===============================================
void CTitle::Draw(void)
{
	CScene::Draw();
}
//===============================================
// �`�揈��
//===============================================
void CTitle::ColChange(void)
{
	// �\������Ă��Ȃ����
	if (!m_bDisplay)
	{

		// �`�[�����S�̐F�����擾
		D3DXCOLOR TeamCol = m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->GetCol();

		TeamCol.a -= 1.0f / 30;	//�����ɋ߂Â��Ă���

		if (TeamCol.a <= 0.0f)
		{//���S�ɓ����ɂȂ����ꍇ
			TeamCol.a = 0.0f;	//�����x�𓧖���

			if (m_pObject2D[OBJ2D::OBJ2D_TeamLogo] != NULL)
			{
				m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetDraw(false);
			}
		}
		m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetCol(TeamCol);
	}
}
//===============================================
// �`�[�����S�X�e�[�g�ł̓���
//===============================================
void CTitle::StateLogo(void)
{
	
	// �`�[�����S�̐F�����擾
	D3DXCOLOR TeamCol = m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->GetCol();

	//�X�e�[�g���`�[�����S�������ꍇ
	if (m_eState == STATE::STATE_TEAMLOGO)
	{
		if(!m_bCol)
		{
			TeamCol.a -= 1.0f / 30;	//�����ɋ߂Â��Ă���

			if (TeamCol.a <= 0.0f)
			{//���S�ɓ����ɂȂ����ꍇ
				TeamCol.a = 0.0f;	//�����x�𓧖���

				m_nCounter++;

				if (m_nCounter >= 100)
				{
					m_bCol = true;

					m_nCounter = 0;
				}
				
			}
		}
		else
		{
			TeamCol.a += 1.0f / 30;	//�����ɋ߂Â��Ă���

			if (TeamCol.a >= 1.0f)
			{//���S�ɓ����ɂȂ����ꍇ
				TeamCol.a = 1.0f;	//�����x�𓧖���

				m_eState = STATE::STATE_PRESSENTER;
				m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetDraw(false);

			}
		}
		m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetCol(TeamCol);
	}

}
//===============================================
//�v���X�G���^�[�X�e�[�g�ł̓���
//===============================================
void CTitle::StateP_E(void)
{
	CCamera* pCam = CManager::GetInstance()->GetCamera();


	pCam->SetActive(true);


	//�L�[�{�[�h���͂��p�b�h���͂������
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) || 
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) || 
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0)) {
		m_bPush = true;
	}

	if (m_bPush)
	{
		CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);
	}
}
//===============================================
//�v���X�G���^�[�X�e�[�g�ł̓���
//===============================================
void CTitle::InitingP_E(void)
{
	//�������ς݂ł͂Ȃ����
	if (!m_bIniting)
	{
		//��������������
		m_bPush = false;

		//�v���X�G���^�[����
		m_pObject2D[OBJ2D::OBJ2D_PressEnter] = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), VECTOR3_ZERO,5);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetSize(640.0f, 320.0f);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(true);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\T_PressEnter000.png"));


		//�}�b�v�ƒn�ʂ������̓ǂݍ���
		CMapManager::GetInstance()->Load();
		CMeshField::Create(D3DXVECTOR3(0.0f, -10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1000.0f, 1000.0f, "data\\TEXTURE\\field000.jpg", 30, 30);


		m_bIniting = true;
	}
}