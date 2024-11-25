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
#include "map_manager.h"
#include "meshfield.h"
#include "PlayerTitle.h"
#include "PoliceTitle.h"
#include "goal.h"

//===============================================
// �������O���
//===============================================
namespace 
{
	const D3DXVECTOR3 PRESSENTER_POS = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.9f, 0.0f };		//�v���X�G���^�[�̍��W�ʒu
	const D3DXVECTOR3 TITLELOGO_POS = { SCREEN_WIDTH, SCREEN_HEIGHT * 0.1f, 0.0f };				//�^�C�g�����S�̍��W�ʒu
	const D3DXVECTOR3 TEAMLOGO_POS = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f ,0.0f };		//�`�[�����S�̍��W�ʒu

	const float TITLELOGO_DEST = SCREEN_WIDTH * 0.5f;											//�^�C�g�����S�̖ڕW�ʒu

	const int AUTOMOVE_RANKING = 1000;	// �����L���O�����J�ڎ���
	const int MOVE_TUTORIAL = 120;		// �`���[�g���A���ɑJ�ڂ���܂ł̎���
	const int ENEMY_NUM = 3;					//���o�p�G(�p�g�J�[)�o����

	const float MAX_ALPHA = 1.0f;		//�����x�̍ő�l
	const float MIN_ALPHA = 0.3f;		//�����x�̍ŏ��l
	const float ALPHA_ZERO = 0.0f;		//�����̎��̃��l


	//const char* FILEPASS = "data\\TXT\\player";	// �t�@�C���̃p�X
	//const char* FILEEXT = ".txt";				// �t�@�C���̊g���q
	//const int FILEPASS_SIZE = (200);			// �t�@�C���̃p�X�T�C�Y
}

//<===============================================
//�R���X�g���N�^
//<===============================================
CTitle::CTitle()
{
	m_nCounterRanking = 0;
	m_nCounter = 0;
	m_nLogoAlpgha = 0;
	m_TitlePos = VECTOR3_ZERO;

	m_eState = STATE::STATE_TEAMLOGO;

	//bool�n
	m_bPush = false;
	m_bNext = false;
	m_bDisplay = false;
	m_bSkipped = false;
	m_bCol = false;
	m_bIniting = false;

	//�|�C���^�n
	m_pPlayer = nullptr;
	m_pCam = nullptr;

	//�z��̏�����
	for (int nCnt = 0; nCnt < OBJ2D_MAX; nCnt++)
	{
		m_pObject2D[nCnt] = nullptr;
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
	// �J�ڃ^�C�}�[�ݒ�
	m_nCounter = MOVE_TUTORIAL;
	m_nCounterRanking = AUTOMOVE_RANKING;

	// �T�E���h�Đ�
	CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_TITLE);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo] = CObject2D::Create(TEAMLOGO_POS, VECTOR3_ZERO);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetSize(320, 160.0f);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\logo_thaw.jpg"));

	return S_OK;
}

//<===============================================
//�I������
//<===============================================
void CTitle::Uninit(void)
{
	CManager::GetInstance()->GetSound()->Stop();
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

		MoveP_E();

		break;

		//�v���X�G���^�[����������̃X�e�[�g�������ꍇ
	case STATE::STATE_CHASING:

		ChaseMovement();

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
		//���̃X�e�[�g�Ɉڍs����
		m_eState = STATE::STATE_CHASING;

		////�����L���O��ʂɈڍs
		//CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);
	}
	//�����������Ȃ�������
	else
	{
		m_nCounterRanking--;
		if (m_nCounterRanking <= 0)
		{
			//�����L���O��ʂɈڍs
			CManager::GetInstance()->GetFade()->Set(CScene::MODE_RANKING);

		}

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

	TitleLogo();

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
	const float fLogoLength = (150.0f, 150.0f);								//���S�̒���(�T�C�Y)
	const float fP_ELength = (350.0f, 350.0f);								//�v���X�G���^�[�̒���(�T�C�Y)

	//�������ς݂ł͂Ȃ����
	if (!m_bIniting)
	{
		//2��bool����������
		m_bPush = false;
		m_bNext = false;

		//�J�����������
		m_pCam = CManager::GetInstance()->GetCamera();
		m_pCam->SetPositionR(D3DXVECTOR3(-4000.0f, 95.0f, 260.0f));
		m_pCam->SetLength(100.0f);
		m_pCam->SetRotation(D3DXVECTOR3(0.0f, -0.0f, 1.79f));
		m_pCam->SetActive(false);

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
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\Pre_char000.png"));

		//�E�v���X�G���^�[
		m_pObject2D[OBJ2D::OBJ2D_PressEnter] = CObject2D::Create(PRESSENTER_POS, VECTOR3_ZERO,5);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetSize(100.0f,100.0f);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(false);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\T_PressEnter000.png"));

		//�K�v�ȃI�u�W�F�N�g�̐���
		CMapManager::GetInstance()->Load();
		CMeshField::Create(D3DXVECTOR3(0.0f, -10.0f, 0.0f), VECTOR3_ZERO, 1000.0f, 1000.0f, "data\\TEXTURE\\field000.jpg", 30, 30);
		m_pPlayer = CPlayerTitle::Create(D3DXVECTOR3(-4734.0f, 50.0f, -1988.0f), D3DXVECTOR3(0.0f, 3.14f, 0.0f), VECTOR3_ZERO,nullptr,nullptr);

		for (int nCnt = 0; nCnt < POLICE_MAX; nCnt++)
		{
			m_apPolice[nCnt] = CPoliceTitle::Create(D3DXVECTOR3(-4734.0f + 150.0f * nCnt, 0.0f, -1300.0f), D3DXVECTOR3(0.0f, 3.14f, 0.0f), VECTOR3_ZERO);
		}

		//�����������̍��}��true�ɂ���
		m_bIniting = true;
	}
}
//<===============================================
//�^�C�g�����S�̓���
//<===============================================
void CTitle::TitleLogo(void)
{
	D3DXCOLOR TitleLogoCol = m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetCol();	//�`�[�����S�̐F�����擾
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
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetCol(TitleLogoCol);
}
//<===============================================
//�ǐՃX�e�[�g�Ɉڍs�����ۂ̓���
//<===============================================
void CTitle::ChaseMovement(void)
{
	D3DXVECTOR3 CameraRot = m_pCam->GetRotation();		//�J��������
	D3DXVECTOR3 PlayerPos = m_pPlayer->GetPosition();	//�v���C���[�ʒu
	D3DXVECTOR3 aPolicePos[POLICE_MAX] = {};			//�x�@�̈ʒu
	const float PlayerMove = 25.0f;						//�v���C���[�̓����l
	const float fRotMove = 0.01f, fDestRot = -1.11f;	//�����ړ��̍ۂ̈ړ��l�ƖړI����

	m_pCam->Pursue(m_pPlayer->GetPosition(), D3DXVECTOR3(0.0f, -0.0f, 1.79f), 1500.0f);
	//�v���C���[�ƌx�@���ړ�������
	PlayerPos.z += PlayerMove;
	
	//�J�����̌����ƃv���C���[�̈ʒu�̐ݒ�
	m_pPlayer->SetPosition(PlayerPos);

	//<******************************************
	//�x�@�֘A�̏���
	for (int nCnt = 0; nCnt < POLICE_MAX; nCnt++)
	{
		//�x�@�̈ʒu���擾
		aPolicePos[nCnt].x = m_apPolice[nCnt]->GetPosition().x;
		aPolicePos[nCnt].y = m_apPolice[nCnt]->GetPosition().y;
		aPolicePos[nCnt].z = m_apPolice[nCnt]->GetPosition().z;

		///�x�@�̈ʒu���ړ������A�ʒu��ݒ肷��
		aPolicePos[nCnt].z += PlayerMove;
		m_apPolice[nCnt]->SetPosition(aPolicePos[nCnt]);
	}
}
//<===============================================
//�X�L�b�v�����ۂ̍ۂ̓���
//<===============================================
void CTitle::SkipMovement(void)
{
	const float DEST_ROT = 0.40f;						//�ړI�̌���

	//�E�^�C�g�����S
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetPosition(D3DXVECTOR3(TITLELOGO_DEST,TITLELOGO_POS.y, TITLELOGO_POS.z));
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetVtx();
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetDraw(true);

	//�E�v���X�G���^�[
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(true);

	//�E�v���C���[
	m_pPlayer->SetPosition(D3DXVECTOR3(-4734.0f, 50.0f, -250.0f));
	m_pPlayer->SetRotation(D3DXVECTOR3(0.0f, -3.14f, 0.0f));
	m_pPlayer->SetReached(true);

	//�X�e�[�g��ύX����
	m_eState = STATE::STATE_PRESSENTER;
}