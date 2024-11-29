//===============================================
//
// �^�C�g����ʂ̊Ǘ����� [title.cpp]
// Author : Ibuki Okusada
//
//===============================================
//���ώ�
//Kazuki Watanabe
//<==============================================
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
#include "TitleMap.h"
#include "meshfield.h"
#include "PlayerTitle.h"
#include "PoliceTitle.h"
#include "goal.h"
#include "camera_manager.h"
#include "number.h"
#include "TitleBaggage.h"
#include "debugproc.h"

//===============================================
// �������O���
//===============================================
namespace 
{
	//<************************************************
	//D3DXVECTOR3�^
	//<************************************************ 
	const D3DXVECTOR3 PRESSENTER_POS = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.9f, 0.0f };		//�v���X�G���^�[�̍��W�ʒu
	const D3DXVECTOR3 TITLELOGO_POS = { SCREEN_WIDTH, SCREEN_HEIGHT * 0.1f, 0.0f };				//�^�C�g�����S�̍��W�ʒu
	const D3DXVECTOR3 TEAMLOGO_POS = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f ,0.0f };		//�`�[�����S�̍��W�ʒu
	const D3DXVECTOR3 FRAME_DEST = { 500.0f,320.0f,0.0f };										//�t���[���̖ڕW�l

	//<************************************************
	//int�^
	//<************************************************ 
	const int AUTOMOVE_RANKING = 1000;															//�����L���O�����J�ڎ���
	const int MOVE_LOGO = 120;																	//���̃X�e�[�g�ɑJ�ڂ���܂ł̎���

	//<************************************************
	//float�^
	//<************************************************ 
	const float MAX_ALPHA = 1.0f;																//�����x�̍ő�l
	const float MIN_ALPHA = 0.3f;																//�����x�̍ŏ��l
	const float ALPHA_ZERO = 0.0f;																//�����̎��̃��l
	const float TITLELOGO_DEST = SCREEN_WIDTH * 0.5f;											//�^�C�g�����S�̖ڕW�ʒu

	//<************************************************
	//�e�N�X�`���̖��O�֘A
	//<************************************************ 
	//�I�u�W�F�N�g2D�Ɏg���e�N�X�`���̖��O
	const char* TEX_NAME[CTitle::OBJ2D_MAX] = 
	{
		"data\\TEXTURE\\logo_thaw.jpg",			//�`�[�����S
		"",										//���J�o�[
		"data\\TEXTURE\\Pre_char000.png",		//�^�C�g�����S
		"data\\TEXTURE\\T_PressEnter000.png",	//�v���X�G���^�[
		"",										//�I����
		"data\\TEXTURE\\T_PressEnter001.png",	//"���l�I�����܂���"�̕���
		"data\\TEXTURE\\T_PressEnter002.png",	//�m�F���b�Z�[�W
	};				

	//�I�����Ɏg���e�N�X�`���̖��O
	const char* SELECT_NAME[CTitle::SELECT_MAX] =
	{
		"data\\TEXTURE\\T_PressEnter003.png",	//
		"data\\TEXTURE\\T_PressEnter004.png",	//�}���`�v���C
	};

	//�I�����Ɏg���e�N�X�`���̖��O
	const char* SELECT_YN_NAME[CTitle::SELECT_YN_MAX] =
	{
		"data\\TEXTURE\\GameOver-Yes.png",	//�͂�
		"data\\TEXTURE\\GameOver-No.png",	//������
	};
}
//<===============================================
//�Z���N�g��ʂ̏������̍ۂɎg�����O�錾
//<===============================================
namespace INIT_SELECT
{
	const D3DXVECTOR3 NUMCHAR_POS = D3DXVECTOR3(625.0f, 100.0f, 0.0f);					//�ǂ�����
	const D3DXVECTOR3 CHECK_POS = D3DXVECTOR3(625.0f, 475.0f, 0.0f);					//�m�F
}

//<===============================================
//�R���X�g���N�^
//<===============================================
CTitle::CTitle()
{
	//int�֌W
	m_nCounter = MOVE_LOGO;
	m_nCounterRanking = AUTOMOVE_RANKING;
	m_nLogoAlpgha = 0;

	//�I�����֘A
	m_nSelect = SELECT::SELECT_SINGLE;
	m_nSelectYN = SELECT_YN::SELECT_YN_YES;

	m_fDis = 0.0f;

	m_eState = STATE::STATE_TEAMLOGO;

	//bool�n
	m_bPush = false;
	m_bNext = false;
	m_bDisplay = false;
	m_bSkipped = false;
	m_bCol = false;
	m_bIniting = false;
	m_bSizing = false;
	m_bSelected = false;

	//�|�C���^�n
	m_pPlayer = nullptr;
	m_pCam = nullptr;
	m_pGoal = nullptr;

	//�z��̏�����
	for (int nCnt = 0; nCnt < OBJ2D_MAX; nCnt++)
	{
		m_pObject2D[nCnt] = nullptr;
	}

	//�z��̏�����
	for (int nCnt = 0; nCnt < SELECT::SELECT_MAX; nCnt++)
	{
		m_apSelect[nCnt] = nullptr;
	}

	//�z��̏�����
	for (int nCnt = 0; nCnt < SELECT_YN::SELECT_YN_MAX; nCnt++)
	{
		m_apYesNoObj[nCnt] = nullptr;
	}

	//�z��̏�����
	for (int nCnt = 0; nCnt < POLICE_MAX; nCnt++)
	{
		m_apPolice[nCnt] = nullptr;
	}

}

//<===============================================
//�f�X�g���N�^
//<===============================================
CTitle::~CTitle()
{

}

//<===============================================
//����������
//<===============================================
HRESULT CTitle::Init(void)
{
	const D3DXVECTOR3 CAMERA_POS = { 3350.0f, 95.0f, 260.0f };				//�J�����̏����ʒu

	//�^�C�g��BGM�Đ��ƃ`�[�����S�I�u�W�F�N�g�̐���
	CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_TITLE);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo] = CObject2D::Create(TEAMLOGO_POS, VECTOR3_ZERO);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetSize(320, 160.0f);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(TEX_NAME[OBJ2D::OBJ2D_TeamLogo]));

	//�J�����������
	m_pCam = CCameraManager::GetInstance()->GetTop();
	m_pCam->SetPositionR(CAMERA_POS);
	m_pCam->SetLength(100.0f);
	m_pCam->SetRotation(D3DXVECTOR3(0.0f, -0.0f, 1.79f));
	m_pCam->SetActive(true);

	return S_OK;
}

//<===============================================
//�I������
//<===============================================
void CTitle::Uninit(void)
{
	CManager::GetInstance()->GetSound()->Stop();

	//�J�����̎��_�̈ʒu��������
	m_pCam->SetPositionR(VECTOR3_ZERO);

	//�I�u�W�F�N�g�j��
	SAFE_UNINIT(m_pPlayer);
}
//<===============================================
//�X�V����
//<===============================================
void CTitle::Update(void)
{
	//�X�e�[�g���Ƃɏ�������
	switch (m_eState)
	{
		//�`�[�����S�X�e�[�g�������ꍇ
	case STATE::STATE_TEAMLOGO:

		//���ւ̍��}���Ȃ����
		if (!m_bNext)
		{
			//�L�[�{�[�h���͂��p�b�h���͂������
			if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) ||
				CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) ||
				CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0)) {
				m_bPush = true;
			}

			//�����ꂽ�ꍇ
			if (m_bPush)
			{
				m_bNext = true;
			}
			//������Ă��Ȃ�������
			else
			{
				m_nCounter--;
				if (m_nCounter <= 0)
				{
					m_bNext = true;
					m_nCounter = 0;
				}
			}
		}
		//���ɍs���������ł��Ă�����
		else { StateLogo(); }

		break;

		//�v���X�G���^�[�X�e�[�g�������ꍇ
	case STATE::STATE_PRE:

		StatePre();

		break;

		//�v���X�G���^�[�X�e�[�g�������ꍇ
	case STATE::STATE_PRESSENTER:

		DebugCam();

		MoveP_E();

		break;

		//�v���X�G���^�[����������̃X�e�[�g�������ꍇ
	case STATE::STATE_CHASING:

		ChaseMovement();

		break;

		//�A�C�X�𓊂������X�e�[�g�������ꍇ
	case STATE::STATE_ICETHROW:

		DebugCam();
		IceMovement();

		break;

		//����ȊO�������ꍇ
	default:

		break;
		
	}

	CScene::Update();
}

//<===============================================
//�`�揈��
//<===============================================
void CTitle::Draw(void)
{
	CScene::Draw();
}
//<===============================================
//�`�[�����S�X�e�[�g�ł̓���
//<===============================================
void CTitle::StateLogo(void)
{
	D3DXCOLOR TeamCol = m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->GetCol();	// �`�[�����S�̐F�����擾
	const int nCountMax = 10;											// �J�E���^�[�̌Œ�l
	const int nAmoValue = 20;											//�F�ύX���̒l

	//�X�e�[�g���`�[�����S�������ꍇ
	if (m_eState == STATE::STATE_TEAMLOGO)
	{
		if(!m_bCol)
		{
			//�����ɋ߂Â��Ă���
			TeamCol.a -= MAX_ALPHA / nAmoValue;

			//���S�ɓ����ɂȂ����ꍇ
			if (TeamCol.a <= ALPHA_ZERO)
			{
				//�����x�𓧖���
				TeamCol.a = ALPHA_ZERO;

				//�����Ă�����J�E���g�̏������Ɠ����I�����}�𑗂�
				if (m_nCounter >= nCountMax)
				{
					//���̃X�e�[�g�Ɉڍs����
					m_eState = STATE::STATE_PRE;
					m_nCounter = 0;
				}
				//�����Ă��Ȃ�������J�E���g����
				else{m_nCounter++;}
				
			}
		}
		m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetCol(TeamCol);
	}

}
//<===============================================
//�v���X�G���^�[�X�e�[�g�ł̓���
//<===============================================
void CTitle::StateP_E(void)
{
	// �J�E���^�[�̌Œ�l
	const int nCountMax = 10;	

	//���̏ꏊ�ɂ��Ă��邩�v���X�G���^�[�̕������\������Ă��Ȃ����
	if (m_pPlayer->GetReached()&&!m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetDraw())
	{
		//�����Ă�����
		if (m_nCounter >= nCountMax)
		{
			MovingLogo();
		}
		//�����Ă��Ȃ�������J�E���g����
		else{m_nCounter++;}

	}
	//���̏ꏊ�ɂ��Ă��Ȃ����v���X�G���^�[�̕������\������Ă��Ȃ���Έ�Ԗڂ̖ړI�n�Ƀv���C���[���ړ�������
	else if (!m_pPlayer->GetReached() && !m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetDraw())
	{m_pPlayer->Moving(CPlayerTitle::DEST_FIRST);}

	//�v���X�G���^�[�̕������\������Ă�����X�e�[�g��ύX
	else if (m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetDraw())
	{
		m_eState = STATE::STATE_PRESSENTER;

		//���Ɍ����悤�ɂ���
		m_pPlayer->SetRotation(VECTOR3_ZERO);
	}
}
//<===============================================
//�v���X�G���^�[�ł̓���
//<===============================================
void CTitle::MoveP_E(void)
{
	//������
	if (m_bIniting) { m_bIniting = false; }

	PreMove();
	//�L�[�{�[�h���͂��p�b�h���͂������
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) ||
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) ||
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0))
	{
		m_bPush = true;
	}

	//��������������
	if (m_bPush)
	{
		//�K�v�Ȃ��̂ŏI������������(���邩�͕s�������ǔO�̂��߂ł�)
		SAFE_UNINIT(m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]);
		SAFE_UNINIT(m_pObject2D[OBJ2D::OBJ2D_PressEnter]);

		//���̃X�e�[�g�Ɉڍs����
		m_eState = STATE::STATE_CHASING;
	}
	//�����������Ȃ�������
	else
	{
		//m_nCounterRanking--;
		//if (m_nCounterRanking <= 0)
		//{
		//	//�����L���O��ʂɈڍs
		//	CManager::GetInstance()->GetFade()->Set(CScene::MODE_RANKING);

		//}

	}
}
//<===============================================
//���J�o�[�̓���
//<===============================================
void CTitle::BlackCoverM(void)
{
	D3DXCOLOR BCoverCol = m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->GetCol();	// �`�[�����S�̐F�����擾
	const int nAmoValue = 20;												//�F�ύX���̒l
	BCoverCol.a -= MAX_ALPHA / nAmoValue;									//�����ɋ߂Â��Ă���

	if (BCoverCol.a <= 0.0f){BCoverCol.a = ALPHA_ZERO;}

	m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetCol(BCoverCol);
}
//<===============================================
//�^�C�g�����S�̓���
//<===============================================
void CTitle::MovingLogo(void)
{
	const float fSpeed = 0.09f;													//�^�C�g�����S�������X�s�[�h
	D3DXVECTOR3 TitlePos = m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->GetPosition();	//�^�C�g�����S�̈ʒu

	//�`��ݒ���I���ɂ��A�ړ��p�̕ϐ��Ƀ^�C�g�����S�̈ʒu���擾������
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetDraw(true);

	//�ړI�n�܂ňړ�������
	TitlePos.x += (TITLELOGO_DEST - TitlePos.x - 0.0f) * fSpeed;//X��

	//�ړI�n�ɒ�������
	if (Function::BoolToDest(m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->GetPosition(),
		D3DXVECTOR3(TITLELOGO_DEST, 0.0f, 0.0f), 5.0f, false))
	{
		//�I���ɂȂ��Ă��Ȃ���Ε`���Ԃ��I���ɂ���
		if (!m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetDraw())
		{
			m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(true);
		}

		//�J�E���g������
		m_nCounter = 0;
	}

	//
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetPosition(TitlePos);
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetVtx();
}
//<===============================================
//�����X�e�[�g�ł̓���
//<===============================================
void CTitle::PreMove(void)
{
	float PlayerRot = m_pPlayer->GetRotation().y;		//�v���C���[�̌�����ς���
	const float fDestRot = -3.14f, fRotMove = 0.05f;		//�ړI�̌����ƌ����̈ړ��l

	ColChange(m_pObject2D[OBJ2D_PressEnter]);

	//��Ԗڂ̖ړI�n�Ƀv���C���[���ړ�������
	m_pPlayer->Moving(CPlayerTitle::DEST_SECOND);

	//���̏ꏊ�ɂ��Ă��邩�v���X�G���^�[�̕������\������Ă��Ȃ����
	if (m_pPlayer->GetReached())
	{
		//�ړI�̌����ɂȂ��Ă�����
		if (PlayerRot <= fDestRot) { PlayerRot = fDestRot; }

		//�Ȃ��Ă��Ȃ��������]��������
		else { PlayerRot -= fRotMove; }

		//�v���C���[�̌����ɔ��f
		m_pPlayer->SetRotation(D3DXVECTOR3(0.0f, PlayerRot, 0.0f));
	}
}
//<===============================================
//�����X�e�[�g�ł̓���
//<===============================================
void CTitle::StatePre(void)
{
	//������
	InitingP_E();

	//�����x�������ɂȂ��Ă��Ȃ�������
	if (!(m_pObject2D[OBJ2D_BLACKCOVER]->GetCol().a <= ALPHA_ZERO)){BlackCoverM();}

	//�����x�������ɂȂ��Ă�����
	else
	{
		//�L�[�{�[�h���͂��p�b�h���͂�����΁A�X�L�b�v����true�ɂ���
		if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) ||
			CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) ||
			CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0)) {m_bSkipped = true;}

		//�X�L�b�v����Ă��Ȃ���΁A�X�L�b�v���Ă��Ȃ��ۂ̓��������s
		if (!m_bSkipped) { StateP_E(); }

		//����Ă�����X�L�b�v�����ۂ̓����̏��������s
		else { SkipMovement(); }
	}
}
//<===============================================
//�v���X�G���^�[�X�e�[�g�ł̓���
//<===============================================
void CTitle::InitingP_E(void)
{
	const D3DXVECTOR3 PLAYER_POS = { 2630.0f, 50.0f, -1988.0f };			//�v���C���[�̈ʒu
	const D3DXVECTOR3 PolicePos = { 2530.0f, 0.0f, -550.0f };				//�x�@�ʒu
	const float fLogoLength = (150.0f, 150.0f);								//���S�̒���(�T�C�Y)
	const float fP_ELength = (350.0f, 350.0f);								//�v���X�G���^�[�̒���(�T�C�Y)

	//�������ς݂ł͂Ȃ����
	if (!m_bIniting)
	{
		//2��bool����������
		m_bIniting = true;
		m_bPush = false;
		m_bNext = false;

		//�K�v�Ȃ��̂ŏI������������(���邩�͕s�������ǔO�̂��߂ł�)
		SAFE_UNINIT(m_pObject2D[OBJ2D::OBJ2D_TeamLogo]);
		//<******************************************
		// 2D�I�u�W�F�N�g�̐�������
		//<******************************************
		//�E�u���b�N�J�o�[
		m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER] = CObject2D::Create(TEAMLOGO_POS, VECTOR3_ZERO,6);
		m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetSize(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);
		m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetCol(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

		//�E�^�C�g�����S
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO] = CObject2D::Create(TITLELOGO_POS, VECTOR3_ZERO, 5);
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetLength(fLogoLength);
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetDraw(false);
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(TEX_NAME[OBJ2D::OBJ2D_TITLELOGO]));

		//�E�v���X�G���^�[
		m_pObject2D[OBJ2D::OBJ2D_PressEnter] = CObject2D::Create(PRESSENTER_POS, VECTOR3_ZERO,5);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetSize(100.0f,100.0f);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(false);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(TEX_NAME[OBJ2D::OBJ2D_PressEnter]));

		//�K�v�ȃI�u�W�F�N�g�̐���
		CTitleMap::GetInstance()->Load();
		CMeshField::Create(D3DXVECTOR3(0.0f, -10.0f, 0.0f), VECTOR3_ZERO, 1000.0f, 1000.0f, "data\\TEXTURE\\field000.jpg", 30, 30);
		m_pPlayer = CPlayerTitle::Create(PLAYER_POS, D3DXVECTOR3(0.0f, 3.14f, 0.0f), VECTOR3_ZERO,nullptr,nullptr);

		//�x�@�̐���
		for (int nCnt = 0; nCnt < POLICE_MAX; nCnt++)
		{
			m_apPolice[nCnt] = CPoliceTitle::Create(D3DXVECTOR3(PolicePos.x + 150.0f *nCnt, PolicePos.y, PolicePos.z),
				D3DXVECTOR3(0.0f, 3.14f, 0.0f), VECTOR3_ZERO);
		}
	}
}
//<===============================================
//�F�ύX����
//<===============================================
void CTitle::ColChange(CObject2D* pObj2D)
{
	D3DXCOLOR TitleLogoCol = pObj2D->GetCol();									//���̃I�u�W�F�N�g�̐F�����擾
	const int nCountMax = 25;													//�J�E���^�[�̌Œ�l
	const int nAmoValue = 10;													//�F�ω��l

	//�F�ύX�������Ă��Ȃ����
	if (!m_bCol) 
	{
		//�����ɋ߂Â��Ă���
		TitleLogoCol.a += MAX_ALPHA / -nAmoValue;

		//���̒l�܂ōs������
		if (TitleLogoCol.a <= MIN_ALPHA)
		{
			TitleLogoCol.a = MIN_ALPHA;	//�����x�����̒l�ɂ���

			//�����Ă�����J�E���g�̏������Ɠ����I�����}�𑗂�
			if (m_nLogoAlpgha >= nCountMax)
			{
				//���̃X�e�[�g�Ɉڍs����
				m_nLogoAlpgha = 0;
				m_bCol = true;
			}
			//�����Ă��Ȃ�������J�E���g����
			else { m_nLogoAlpgha++; }

		}
	}
	//�F�ύX�������Ă�����
	else
	{
		//�񓧖��ɋ߂Â��Ă���
		TitleLogoCol.a += MAX_ALPHA / nAmoValue;

		//�ő�l�܂ŋ߂Â�����
		if (TitleLogoCol.a >= MAX_ALPHA)
		{
			//�����x�����̒l�ɂ���
			TitleLogoCol.a = MAX_ALPHA;	

			//�����Ă�����J�E���g�̏������ƏI�����}�𑗂�
			if (m_nLogoAlpgha >= nCountMax)
			{
				//���̃X�e�[�g�Ɉڍs����
				m_nLogoAlpgha = 0;
				m_bCol = false;
			}
			//�ő�l�ł͂Ȃ�������J�E���g����
			else { m_nLogoAlpgha++; }

		}
	}
	//�F�ݒ�
	pObj2D->SetCol(TitleLogoCol);
}
//<===============================================
//�ǐՃX�e�[�g�Ɉڍs�����ۂ̓���
//<===============================================
void CTitle::ChaseMovement(void)
{
	//<*************************************************************
	//�J�����Ɋւ���
	//<*************************************************************
	D3DXVECTOR3 PlayerPos = m_pPlayer->GetPosition();	//�v���C���[�ʒu
	const float PlayerMove = 25.0f;						//�v���C���[�̓����l
	const int FADE_TIME = 200;							//�Q�[����ʂɈڍs����܂ł̎���

	Selecting();
	ChaseCamera();

	//�v���C���[�ƌx�@���ړ�������
	PlayerPos.z += PlayerMove;

	//�J�����̌����ƃv���C���[�̈ʒu�̐ݒ�
	m_pPlayer->SetPosition(PlayerPos);

	//<******************************************
	//�x�@�֘A�̏���
	for (int nCnt = 0; nCnt < POLICE_MAX; nCnt++)
	{
		m_apPolice[nCnt]->Chasing(PlayerMove);
	}

	//�I�𔻒肪�����
	if (m_bSelected) 
	{ 
		//�s�v�ɂȂ���2D�I�u�W�F�N�g�̔j��
		for (int nCnt = OBJ2D::OBJ2D_FRAME; nCnt < OBJ2D::OBJ2D_MAX; nCnt++)
		{SAFE_UNINIT(m_pObject2D[nCnt]);}

		//�V���O�����}���`���̑I�����̔j��
		for (int nCnt = 0; nCnt < SELECT_MAX; nCnt++)
		{SAFE_UNINIT(m_apSelect[nCnt]);}

		//�͂��Ƃ������̑I�����̔j��
		for (int nCnt = 0; nCnt < SELECT_YN_MAX; nCnt++)
		{SAFE_UNINIT(m_apYesNoObj[nCnt]);}

		//�A�C�X�X�e�[�g�Ɉڍs���A�ϐ��̐ݒ������
		m_eState = STATE::STATE_ICETHROW; 
		m_nCounter = 0; 
	}
}
//<===============================================
//�ǐՃX�e�[�g�̍ۂ̃J�����̓���
//<===============================================
void CTitle::ChaseCamera(void)
{
	//<************************************************************
	//�J�����֌W�̕ϐ�
	//<************************************************************
	D3DXVECTOR3 CameraRot = m_pCam->GetRotation();		//�J��������
	D3DXVECTOR3 CameraPos = m_pCam->GetPositionR();		//�J�����ʒu

	//�����ړ��̍ۂ̈ړ��l�ƖړI����
	const float fRotMoveY = 0.02f,
		fRotMoveZ = 0.005f,
		fDestRotY = -0.66f,
		fDestRotZ = 1.15f;

	const float CameraPosDif[2] = { 0.0f,210.0f };	//�J�����̕␳����
	float CameraDis = 1150.0f;					//�J�����̋���

	//�J�����̌����̒���
	if (CameraRot.y <= fDestRotY) { CameraRot.y = fDestRotY; }
	else { CameraRot.y -= fRotMoveY; }

	//�J�����̌����̒���
	if (CameraRot.z >= fDestRotZ) { CameraRot.z = fDestRotZ; }
	else { CameraRot.z += fRotMoveZ; }

	//�J�����̐ݒ�
	m_pCam->SetPositionR(D3DXVECTOR3(m_pPlayer->GetPosition().x + CameraPosDif[0],
		m_pPlayer->GetPosition().y + CameraPosDif[1],
		m_pPlayer->GetPosition().z));

	m_pCam->SetLength(CameraDis);
	m_pCam->SetRotation(CameraRot);
}
//<===============================================
//�X�L�b�v�����ۂ̍ۂ̓���
//<===============================================
void CTitle::SkipMovement(void)
{
	const D3DXVECTOR3 PLAYER_POS = { 2630.0f, 50.0f, -250.0f };	//�v���C���[�̈ʒu
	const float DEST_ROT = 0.40f;								//�ړI�̌���

	//�E�^�C�g�����S
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetPosition(D3DXVECTOR3(TITLELOGO_DEST,TITLELOGO_POS.y, TITLELOGO_POS.z));
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetVtx();
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetDraw(true);

	//�E�v���X�G���^�[
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(true);

	//�E�v���C���[
	m_pPlayer->SetPosition(PLAYER_POS);
	m_pPlayer->SetRotation(D3DXVECTOR3(0.0f, -3.14f, 0.0f));
	m_pPlayer->SetReached(true);

	//�X�e�[�g��ύX����
	m_eState = STATE::STATE_PRESSENTER;
}
//<===============================================
//�l���I���Ɏg���ϐ��̏�����
//<===============================================
void CTitle::InitingSelect(void)
{
	const D3DXVECTOR3 SELECT_POS = D3DXVECTOR3(400.0f, 300.0f, 0.0f);								//�I�����̈ʒu
	const float fDis = 475.0f;																		//����1	

	const D3DXVECTOR3 YES_POS = D3DXVECTOR3(505.0f, INIT_SELECT::CHECK_POS.y + 100.0f, 0.0f);		//�͂��Ƃ������̈ʒu
	const float fDis2 = 200.0f;																		//����2

	const D3DXCOLOR InitFrameCol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.7f);								//�t���[���̏����F

	//����������Ă��Ȃ�������
	if (!m_bIniting)
	{
		//bool�^�̏���`
		m_bIniting = true;
		m_bPush = false;

		//�t���[��
		m_pObject2D[OBJ2D::OBJ2D_FRAME] = CObject2D::Create(TEAMLOGO_POS, VECTOR3_ZERO, 6);
		m_pObject2D[OBJ2D::OBJ2D_FRAME]->SetSize(0.0f, 0.0f);
		m_pObject2D[OBJ2D::OBJ2D_FRAME]->SetDraw(true);
		m_pObject2D[OBJ2D::OBJ2D_FRAME]->SetCol(InitFrameCol);

		//�ǂ���ɂ��邩
		m_pObject2D[OBJ2D::OBJ2D_NUMCHAR] = CObject2D::Create(INIT_SELECT::NUMCHAR_POS, VECTOR3_ZERO, 6);
		m_pObject2D[OBJ2D::OBJ2D_NUMCHAR]->SetSize(450.0f, 135.0f);
		m_pObject2D[OBJ2D::OBJ2D_NUMCHAR]->SetDraw(false);
		m_pObject2D[OBJ2D::OBJ2D_NUMCHAR]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(TEX_NAME[OBJ2D::OBJ2D_NUMCHAR]));

		//�V���O�����}���`���̑I����
		for (int nCnt = 0; nCnt < SELECT_MAX; nCnt++)
		{
			//�ǂ���ɂ��邩
			m_apSelect[nCnt] = CObject2D::Create(
				D3DXVECTOR3(SELECT_POS.x+ fDis*nCnt,
				SELECT_POS.y,
				SELECT_POS.z),
				VECTOR3_ZERO, 6);

			m_apSelect[nCnt]->SetSize(250.0f, 100.0f);
			m_apSelect[nCnt]->SetDraw(false);
			m_apSelect[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(SELECT_NAME[nCnt]));
		}

		//�m�F���b�Z�[�W
		m_pObject2D[OBJ2D::OBJ2D_CHECK] = CObject2D::Create(INIT_SELECT::CHECK_POS, VECTOR3_ZERO, 6);
		m_pObject2D[OBJ2D::OBJ2D_CHECK]->SetSize(250.0f, 75.0f);
		m_pObject2D[OBJ2D::OBJ2D_CHECK]->SetDraw(false);
		m_pObject2D[OBJ2D::OBJ2D_CHECK]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(TEX_NAME[OBJ2D::OBJ2D_CHECK]));

		//�͂��Ƃ������̑I����
		for (int nCnt = 0; nCnt < SELECT_YN_MAX; nCnt++)
		{
			//�I����(�͂�)
			m_apYesNoObj[nCnt] = CObject2D::Create(D3DXVECTOR3(YES_POS.x + fDis2 * nCnt, YES_POS.y, YES_POS.z), VECTOR3_ZERO, 6);
			m_apYesNoObj[nCnt]->SetSize(200.0f, 50.0f);
			m_apYesNoObj[nCnt]->SetDraw(false);
			m_apYesNoObj[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(SELECT_YN_NAME[nCnt]));
		}
	}
}
//<===============================================
//�t���[���̃T�C�Y����
//<===============================================
void CTitle::Sizing(void)
{
	D3DXVECTOR3 FrameSize = m_pObject2D[OBJ2D::OBJ2D_FRAME]->GetSize();		//�t���[���T�C�Y
	const float fSpeed = 0.09f;												//�X�s�[�h

	//X��
	if (FrameSize.x >= FRAME_DEST.x) { FrameSize.x = FRAME_DEST.x; }
	else { FrameSize.x += (FRAME_DEST.x - FrameSize.x - 0.0f) * fSpeed; }

	//Y��
	if (FrameSize.y >= FRAME_DEST.y) { FrameSize.y = FRAME_DEST.y; }
	else { FrameSize.y += (FRAME_DEST.y - FrameSize.y - 0.0f) * fSpeed; }

	//�ړI�n�ɒ�������
	if (Function::BoolToDest(m_pObject2D[OBJ2D::OBJ2D_FRAME]->GetSize(),
		D3DXVECTOR3(FRAME_DEST), 1.0f, false))
	{
		//�T�C�Y�����������}��ݒ�
		m_bSizing = true;
	}

	//�T�C�Y��ݒ�
	m_pObject2D[OBJ2D::OBJ2D_FRAME]->SetSize(FrameSize.x,FrameSize.y);
}
//<===============================================
//�l���I���̍ۂ̏���
//<===============================================
void CTitle::Selecting(void)
{
	const D3DXVECTOR3 NUMBER_POS = D3DXVECTOR3(625.0f, 325.0f, 0.0f);		//�����̈ʒu
	const int ONE_PLAYER = 1,MAX_PLAYER = 4;								//�v���C���[�̐�

	//������
	InitingSelect();

	//�T�C�Y�������I����Ă�����
	if (m_bSizing)
	{
		//�`����I���ɂ���
		if (!m_pObject2D[OBJ2D::OBJ2D_NUMCHAR]->GetDraw()) { m_pObject2D[OBJ2D::OBJ2D_NUMCHAR]->SetDraw(true); }

		//�V���O�����}���`���̑I����
		for (int nCnt = 0; nCnt < SELECT_MAX; nCnt++)
		{
			//�`����I���ɂ���
			if (!m_apSelect[nCnt]->GetDraw()) { m_apSelect[nCnt]->SetDraw(true); }

			//���ʂ�for��������邽��
			else { break; }
		}

		//�J���[��ݒ�
		m_pObject2D[OBJ2D::OBJ2D_FRAME]->SetCol(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.6f));

		//��������������
		if (m_bPush)
		{
			SelectYesNO();
		}
		//�������
		else
		{
			SelectSingleMulti();
		}
	}
	//�I���Ă��Ȃ�������A�T�C�Y�������s��
	else
	{
		Sizing();
	}
}
//<===============================================
//�I����(�V���O�����}���`)
//<===============================================
void CTitle::SelectSingleMulti(void)
{
	//�`����I�t�ɂ���
	if (m_pObject2D[OBJ2D::OBJ2D_CHECK]->GetDraw()) { m_pObject2D[OBJ2D::OBJ2D_CHECK]->SetDraw(false); }

	//�͂����������̑I����
	for (int nCnt = 0; nCnt < SELECT_YN_MAX; nCnt++)
	{
		//�`����I�t�ɂ���
		if (m_apYesNoObj[nCnt]->GetDraw()) { m_apYesNoObj[nCnt]->SetDraw(false); }

		//���ʂ�for��������邽��
		else { break; }
	}

	//�L�[�{�[�h���͂��p�b�h���͂������
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) ||
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) ||
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0))
	{
		m_bPush = true;
		m_nSelectYN = SELECT_YN::SELECT_YN_YES;
	}

	//�l���I��������
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RIGHTARROW)) { m_nSelect = (m_nSelect + 1) % SELECT_MAX; }
	else if (CInputKeyboard::GetInstance()->GetTrigger(DIK_LEFTARROW)) { m_nSelect = (m_nSelect + (SELECT_MAX - 1)) % SELECT_MAX; }

	//"YES"��I�����Ă��鎞
	if (m_nSelect == SELECT::SELECT_SINGLE)
	{
		//"YES"��I����ԁA"NO"���I�����
		ColChange(m_apSelect[SELECT::SELECT_SINGLE]);
		m_apSelect[SELECT::SELECT_MULTI]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
	//"NO"��I�����Ă��鎞
	else if (m_nSelect == SELECT::SELECT_MULTI)
	{
		//"YES"���I����ԁA"NO"��I�����
		ColChange(m_apSelect[SELECT::SELECT_MULTI]);
		m_apSelect[SELECT::SELECT_SINGLE]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
}
//<===============================================
//�I����(Yes��No)
//<===============================================
void CTitle::SelectYesNO(void)
{
	//�`����I���ɂ���
	if (!m_pObject2D[OBJ2D::OBJ2D_CHECK]->GetDraw()) { m_pObject2D[OBJ2D::OBJ2D_CHECK]->SetDraw(true); }

	//�͂����������̑I����
	for (int nCnt = 0; nCnt < SELECT_YN_MAX; nCnt++)
	{
		//�`����I���ɂ���
		if (!m_apYesNoObj[nCnt]->GetDraw()) { m_apYesNoObj[nCnt]->SetDraw(true); }

		//���ʂ�for��������邽��
		else { break; }
	}

	//�l���I��������
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RIGHTARROW)) { m_nSelectYN = (m_nSelectYN + 1) % SELECT_YN_MAX; }
	else if (CInputKeyboard::GetInstance()->GetTrigger(DIK_LEFTARROW)) { m_nSelectYN = (m_nSelectYN + (SELECT_YN_MAX - 1)) % SELECT_YN_MAX; }

	//�L�[�{�[�h���͂��p�b�h���͂������
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) ||
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) ||
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0))
	{
		
		//"YES"��I�������Ƃ��A�Q�[����ʂɈڍs����
		if (m_nSelectYN == SELECT_YN::SELECT_YN_YES){m_bSelected = true;}

		//"NO"��I�������Ƃ��A�����������ɖ߂�
		else if (m_nSelectYN == SELECT_YN::SELECT_YN_NO){m_bPush = false;}
	}

	SelectCol();
}
//<===============================================
//�I�����Ă���ۂ̐F
//<===============================================
void CTitle::SelectCol(void)
{	
	//"YES"��I�����Ă��鎞
	if (m_nSelect == SELECT::SELECT_SINGLE) {m_apSelect[SELECT::SELECT_SINGLE]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));}

	//"NO"��I�����Ă��鎞
	else if (m_nSelect == SELECT::SELECT_MULTI){m_apSelect[SELECT::SELECT_MULTI]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));}

	//"YES"��I�����Ă��鎞
	if (m_nSelectYN == SELECT_YN::SELECT_YN_YES)
	{
		//"YES"��I����ԁA"NO"���I�����
		ColChange(m_apYesNoObj[SELECT_YN::SELECT_YN_YES]);
		m_apYesNoObj[SELECT_YN::SELECT_YN_NO]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
	//"NO"��I�����Ă��鎞
	else if (m_nSelectYN == SELECT_YN::SELECT_YN_NO)
	{
		//"YES"���I����ԁA"NO"��I�����
		ColChange(m_apYesNoObj[SELECT_YN::SELECT_YN_NO]);
		m_apYesNoObj[SELECT_YN::SELECT_YN_YES]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}

}
//<===============================================
//�f�o�b�O���̃J�����̓���
//<===============================================
void CTitle::DebugCam(void)
{
#ifdef _DEBUG
	D3DXVECTOR3 CamRot = m_pCam->GetRotation();	//�J��������
	const float fRotMove = 0.01f;				//�J���������̈ړ��l

	//<*******************************************************************************
	//�J�����ړ�
	//X��
	if (CInputKeyboard::GetInstance()->GetPress(DIK_H)) { CamRot.x -= fRotMove; }
	if (CInputKeyboard::GetInstance()->GetPress(DIK_B)) { CamRot.x += fRotMove; }
	//Y��
	if (CInputKeyboard::GetInstance()->GetPress(DIK_J)) { CamRot.y -= fRotMove; }
	if (CInputKeyboard::GetInstance()->GetPress(DIK_N)) { CamRot.y += fRotMove; }
	//Z��
	if (CInputKeyboard::GetInstance()->GetPress(DIK_K)) { CamRot.z -= fRotMove; }
	if (CInputKeyboard::GetInstance()->GetPress(DIK_M)) { CamRot.z += fRotMove; }

	//�J�������ݒ�
	m_pCam->SetRotation(CamRot);
#endif

}
//<===============================================
//�A�C�X�X�e�[�g���̓���
//<===============================================
void CTitle::IceMovement(void)
{
	D3DXVECTOR3 PlayerPos = m_pPlayer->GetPosition();				//�v���C���[�ʒu
	const int FADE_TIME = 200;										//�Q�[����ʂɈڍs����܂ł̎���
	const int FADE_TIME_HARF = 65;									//�Q�[����ʂɈڍs����܂ł̎��Ԃ̔����l

	//<*************************************************************
	//float�^
	//<*************************************************************
	const float PlayerMove = 50.0f;								//�v���C���[�̓����l
	const float CamHeightDis = 150.0f;							//�����̍�
	const float fDisMax = 700.0f;								//�����̍ő�l
	const float fMoveValue = 10.0f;								//�����ړ��̍ۂ̒l

	m_pPlayer->BaggageMove();

	//�J�����̐ݒ�
	m_pCam->SetRotation(D3DXVECTOR3(0.0f, 0.0f, 1.57f));

	//�v���C���[�ƌx�@���ړ�������
	PlayerPos.z += PlayerMove;

	//�J�����̌����ƃv���C���[�̈ʒu�̐ݒ�
	m_pPlayer->SetPosition(PlayerPos);

	//<******************************************
	//�x�@�֘A�̏���
	for (int nCnt = 0; nCnt < POLICE_MAX; nCnt++)
	{
		m_apPolice[nCnt]->Chasing(PlayerMove);
	}

	//�J�����̈ʒu��ݒ�
	m_pCam->SetPositionR(D3DXVECTOR3
	(m_pPlayer->GetTitleGoal()->GetPos().x - m_fDis,
		m_pPlayer->GetTitleGoal()->GetPos().y+ CamHeightDis,
		m_pPlayer->GetTitleGoal()->GetPos().z));

	//�����Ă�����Q�[����ʂɑJ�ڂ���
	if (m_nCounter >= FADE_TIME) 
	{ 
		//�V���O���v���C���I������Ă�����A���̂܂܃Q�[����ʂɑJ��
		if (GetSelectSingleMulti() == SELECT::SELECT_SINGLE)
		{
			CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);
		}

		//�V���O���v���C���I������Ă�����A�G���g���[��ʂɑJ��
		else if (GetSelectSingleMulti() == SELECT::SELECT_MULTI)
		{
			CManager::GetInstance()->GetFade()->Set(CScene::MODE_ENTRY);
		}
	
	}

	//�����Ă��Ȃ�������J�E���g����
	else { m_nCounter++; }

	//�������炢�̒l�ɂȂ��Ă�����
	if (m_nCounter >= FADE_TIME_HARF)
	{
		//���������l�ɂȂ����炻�̒l�ɂ��A�Ȃ��Ă��Ȃ�������߂Â���
		if (m_fDis >= fDisMax) { m_fDis = fDisMax; }
		else { m_fDis += fMoveValue; }
	}
}