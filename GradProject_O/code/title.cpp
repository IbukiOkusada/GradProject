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
#include "camera_manager.h"
#include "number.h"
#include "TitleBaggage.h"
#include "debugproc.h"
#include "objectsound.h"

//�ÓI�����o�ϐ�
CPoliceTitle* CTitle::m_apPolice[INITIAL::POLICE_MAX] = {nullptr};

//<===============================================
//�������O���
//<===============================================
namespace 
{
	//<************************************************
	//D3DXVECTOR3�^
	//<************************************************ 
	const D3DXVECTOR3 PRESSENTER_POS = { SCREEN_WIDTH, SCREEN_HEIGHT * 0.9f, 0.0f };			//�v���X�G���^�[�̍��W�ʒu
	const D3DXVECTOR3 TITLELOGO_POS = { SCREEN_WIDTH, SCREEN_HEIGHT * 0.23f, 0.0f };			//�^�C�g�����S�̍��W�ʒu
	const D3DXVECTOR3 TEAMLOGO_POS = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f ,0.0f };		//�`�[�����S�̍��W�ʒu
	const D3DXVECTOR3 PolicePos = { 2530.0f, 50.0f, -800.0f };									//�x�@�ʒu
	const D3DXVECTOR3 DEST_ROT = { 0.0f,-3.14f,0.0f };											//�ړI����

	constexpr int nNatPriority = 5;																//���ʂ��Ďg���D��x�ϐ�
	const D3DXCOLOR INV_COL = { 0.0f,0.0f,0.0f,0.0f };											//�����̎��̐F
	const D3DXCOLOR VIS_COL = { 0.0f,0.0f,0.0f,1.0f };											//�񓧖��̎��̐F


	//<************************************************
	//float�^
	//<************************************************ 
	constexpr float MAX_ALPHA = 1.0f;															//�����x�̍ő�l
	constexpr float MIN_ALPHA = 0.3f;															//�����x�̍ŏ��l
	constexpr float ALPHA_ZERO = 0.0f;															//�����̎��̃��l
	constexpr float TITLELOGO_DEST = SCREEN_WIDTH * 0.5f;										//�^�C�g�����S�̖ڕW�ʒu

	const D3DXCOLOR ZERO_COL = D3DXCOLOR(1.0f, 1.0f, 1.0f, ALPHA_ZERO);							//�����F�̒l
	const D3DXCOLOR ONE_COL = D3DXCOLOR(1.0f, 1.0f, 1.0f, MAX_ALPHA);							//�񓧖��̒l
	
}
//<===============================================
//�R���X�g���N�^
//<===============================================
CTitle::CTitle()
{
	//int�֌W
	m_nCounter = 0;
	m_nCounterRanking = 0;
	m_nLogoAlpgha = 0;
	m_nLogoCou = 0;

	//�I�����֘A
	m_nSelect = SELECT::SELECT_SINGLE;
	m_nSelectYN = SELECT_YN::SELECT_YN_YES;
	m_eState = STATE::STATE_TEAMLOGO;

	m_fDis = 0.0f;

	//bool�n
	m_bPush = false;
	m_bNext = false;
	m_bSkipped = false;
	m_bCol = false;
	m_bIniting = false;
	m_bSizing = false;
	m_bSelected = false;

	//�|�C���^�n
	m_pPlayer = nullptr;
	m_pCam = nullptr;

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
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
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
	constexpr char* TEX_TEAMLOGO = "data\\TEXTURE\\team_logo.png";				//�`�[�����S
	const D3DXVECTOR3 CAMERA_POS = { 3350.0f, 95.0f, 260.0f };					//�J�����̏����ʒu
	constexpr int MOVE_LOGO = 135;												//���̃X�e�[�g�ɑJ�ڂ���܂ł̎���
	constexpr int AUTOMOVE_RANKING = 10000;										//�����L���O�����J�ڎ���
	constexpr float fSize[SIZING::SIZING_MAX] = { 250.0f, 125.0f };				//�T�C�Y(�`�[�����S)

	//int�^�ϐ��̐ݒ�
	m_nCounterRanking = AUTOMOVE_RANKING;
	m_nCounter = MOVE_LOGO;

	//�`�[�����S
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo] = InitObj2D(TEAMLOGO_POS, 
		VECTOR3_ZERO,3, fSize[SIZING_WIDTH], fSize[SIZING_HEIGHT],true, TEX_TEAMLOGO);

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

	// �}�b�v���p��
	CMapManager::Release();
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
			if (TriggerEnter()) {m_bPush = true;}

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
	constexpr int nCountMax = 10;											// �J�E���^�[�̌Œ�l
	constexpr int nAmoValue = 20;											//�F�ύX���̒l

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
	constexpr int nCountMax = 10;	

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
	if (TriggerEnter()){m_bPush = true;}

	//��������������
	if (m_bPush)
	{
		//�K�v�Ȃ��̂ŏI������������(���邩�͕s�������ǔO�̂��߂ł�)
		SAFE_UNINIT(m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]);
		SAFE_UNINIT(m_pObject2D[OBJ2D::OBJ2D_PressEnter]);

		//���̃X�e�[�g�Ɉڍs����A�u���b�N�J�o�[�ŉB��
		m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetCol(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
		m_eState = STATE::STATE_CHASING;
	}
	else { return; }
}
//<===============================================
//���J�o�[�̓���
//<===============================================
void CTitle::BlackCoverM(void)
{
	D3DXCOLOR BCoverCol = m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->GetCol();	// �`�[�����S�̐F�����擾
	constexpr int nAmoValue = 20;												//�F�ύX���̒l
	BCoverCol.a -= MAX_ALPHA / nAmoValue;									//�����ɋ߂Â��Ă���

	if (BCoverCol.a <= 0.0f){BCoverCol.a = ALPHA_ZERO;}

	m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetCol(BCoverCol);
}
//<===============================================
//�^�C�g�����S�̓���
//<===============================================
void CTitle::MovingLogo(void)
{
	constexpr float fSpeed = 0.09f;													//�^�C�g�����S�������X�s�[�h
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

	//�^�C�g�����S�̐ݒ�
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetPosition(TitlePos);
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetVtx();
}
//<===============================================
//�����X�e�[�g�ł̓���
//<===============================================
void CTitle::PreMove(void)
{
	D3DXVECTOR3 P_EPos = m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetPosition();	//�v���X�G���^�[�̈ʒu

	float PlayerRot = m_pPlayer->GetRotation().y;								//�v���C���[�̌�����ς���
	constexpr float fRotMove = 0.05f;											//�����̈ړ��l
	constexpr float fSpeed = 0.09f;												//���x

	//�ړI�n�܂ňړ�������
	P_EPos.x += (TITLELOGO_DEST - P_EPos.x - 0.0f) * fSpeed;//X��

	//�ړI�n�ɒ�������
	if (Function::BoolToDest(m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetPosition(),
		D3DXVECTOR3(TITLELOGO_DEST, 0.0f, 0.0f), 3.0f, false))
	{
		//�v���X�G���^�[�ƃ^�C�g�����S�Ɋւ��鏈��
		ColChange(m_pObject2D[OBJ2D_PressEnter],20);
		LightOff();

		//�������Ȃ�������
		if (!m_bPush)
		{
			//�����L���O��ʂɈڍs
			if (m_nCounterRanking <= 0) { CManager::GetInstance()->GetFade()->Set(CScene::MODE_RESULT); }
			else { m_nCounterRanking--; }
		}
	}

	//��Ԗڂ̖ړI�n�Ƀv���C���[���ړ�������
	m_pPlayer->Moving(CPlayerTitle::DEST_SECOND);

	//���̏ꏊ�ɂ��Ă��邩�v���X�G���^�[�̕������\������Ă��Ȃ����
	if (m_pPlayer->GetReached())
	{
		//
		if (PlayerRot <= DEST_ROT.y) { PlayerRot = DEST_ROT.y; }
		else { PlayerRot += -fRotMove; }

		//�v���C���[�̌����ɔ��f
		m_pPlayer->SetRotation(D3DXVECTOR3(0.0f, PlayerRot, 0.0f));

		//�T�E���h�Đ�
		m_pPlayer->SetS(true);
	}
	//
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetPosition(P_EPos);
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetVtx();
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
	//�������ς݂ł���Δ�����
	if (m_bIniting) {return;}

	const D3DXVECTOR3 PLAYER_POS = { 2630.0f, 50.0f, -1988.0f };							//�v���C���[�̈ʒu
	constexpr float fLogoLength = 150.0f;													//���S�̒���(�T�C�Y)
	constexpr float fP_ELength = (350.0f, 350.0f);											//�v���X�G���^�[�̒���(�T�C�Y)

	constexpr char* TEX_TITLELOGO = "data\\TEXTURE\\Title\\Title_logo.png";					//�^�C�g�����S�̃e�N�X�`���l�[��
	constexpr char* TEX_PRESSENTER = "data\\TEXTURE\\Title\\-PRESS ENTER-.png";				//�v���X�G���^�[�̃e�N�X�`���l�[��

	constexpr int nMaxPri = 7;																//�D��x�ő�l

	//�T�C�Y�֘A
	constexpr float fSizeBlack[SIZING_MAX] = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };	//�T�C�Y(�u���b�N�J�o�[)
	constexpr float fSizeTitleLogo[SIZING_MAX] = { 650.0f,400.0f };							//�T�C�Y(�^�C�g�����S)
	constexpr float fSizePressEnter[SIZING_MAX] = { 550.0f,80.0f };						//�T�C�Y(�v���X�G���^�[)

	//2��bool����������
	m_bIniting = true;
	m_bPush = false;
	m_bNext = false;

	//�K�v�Ȃ��̂ŏI������������(���邩�͕s�������ǔO�̂��߂ł�)
	SAFE_UNINIT(m_pObject2D[OBJ2D::OBJ2D_TeamLogo]);
	//<******************************************
	// 2D�I�u�W�F�N�g�̐�������
	//<****************************************** 
	//�u���b�N�J�o�[
	m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER] = InitObj2D(TEAMLOGO_POS, VECTOR3_ZERO, nMaxPri,
		fSizeBlack[SIZING_WIDTH], fSizeBlack[SIZING_HEIGHT], true, nullptr, VIS_COL);

	//�u���b�N�J�o�[
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO] = InitObj2D(TITLELOGO_POS, VECTOR3_ZERO, nNatPriority,
		fSizeTitleLogo[SIZING_WIDTH], fSizeTitleLogo[SIZING_HEIGHT], false, TEX_TITLELOGO);

	//�u���b�N�J�o�[
	m_pObject2D[OBJ2D::OBJ2D_PressEnter] = InitObj2D(PRESSENTER_POS, VECTOR3_ZERO, nNatPriority,
		fSizePressEnter[SIZING_WIDTH], fSizePressEnter[SIZING_HEIGHT], false, TEX_PRESSENTER);
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetVtx();

	//�K�v�ȃI�u�W�F�N�g�̐���
	CMapManager::GetInstance()->Load();
	m_pPlayer = CPlayerTitle::Create(PLAYER_POS, DEST_ROT, VECTOR3_ZERO,nullptr,nullptr);

	// �E��
	CMeshField::Create(D3DXVECTOR3(27250.0f, -10.0f, 3000.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1000.0f, 1000.0f, "data\\TEXTURE\\field000.jpg", 13, 16);

	// ����
	CMeshField::Create(D3DXVECTOR3(-750.0f, -10.0f, 3000.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 500.0f, 500.0f, "data\\TEXTURE\\field001.jpg", 26, 32);

	//�x�@�̐���
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{
		m_apPolice[nCnt] = CPoliceTitle::Create(
			D3DXVECTOR3(PolicePos.x + 150.0f *nCnt, PolicePos.y, PolicePos.z),
			DEST_ROT, VECTOR3_ZERO);
	}

	//�^�C�g��BGM�Đ�
	CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_TITLE);
}
//<===============================================
//�F�ύX����
//<===============================================
void CTitle::ColChange(CObject2D* pObj2D,const int nCntMax)
{
	D3DXCOLOR TitleLogoCol = pObj2D->GetCol();	//���̃I�u�W�F�N�g�̐F�����擾
	constexpr int nAmoValue = 10;				//�F�ω��l

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
			if (m_nLogoAlpgha >= nCntMax)
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
			if (m_nLogoAlpgha >= nCntMax)
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
//���C�g�I�t����
//<===============================================
void CTitle::LightOff(void)
{
	constexpr int nCountMax = 200;													//�J�E���^�[�̌Œ�l
	constexpr int nRandMax = 7;														//�����_���ϐ��̍ő�l
	constexpr char* TEX_LIGHTON = "data\\TEXTURE\\Title\\Title_logo.png";			//�^�C�g�����S(���C�g�I��)
	constexpr char* TEX_LIGHTOFF = "data\\TEXTURE\\Title\\Title_logo_lightoff.png";	//�^�C�g�����S(���C�g�I�tVer);

	//�ő�l�܂ōs���Ă�����
	if (m_nLogoCou >= nCountMax)
	{
		//�e�N�X�`�������C�g�I�t����Ă��Ȃ���Ԃ�������
		if (m_pObject2D[OBJ2D_TITLELOGO]->GetIdxTex() ==
			CManager::GetInstance()->GetTexture()->Regist(TEX_LIGHTON))
		{
			//���C�g�I�t����Ă���e�N�X�`���ɕύX����
			m_pObject2D[OBJ2D_TITLELOGO]->BindTexture
			(CManager::GetInstance()->GetTexture()->Regist(TEX_LIGHTOFF));
		}
		//�e�N�X�`�������C�g�I�t����Ă�����
		else if (m_pObject2D[OBJ2D_TITLELOGO]->GetIdxTex() ==
			CManager::GetInstance()->GetTexture()->Regist(TEX_LIGHTOFF))
		{
			//���C�g�I�t����Ă��Ȃ��e�N�X�`���ɂ���
			m_pObject2D[OBJ2D_TITLELOGO]->BindTexture
			(CManager::GetInstance()->GetTexture()->Regist(TEX_LIGHTON));
		}

		//������
		m_nLogoCou = 0;
	}
	else{m_nLogoCou += rand() % nRandMax;}
}
//<===============================================
//�ǐՃX�e�[�g�Ɉڍs�����ۂ̓���
//<===============================================
void CTitle::ChaseMovement(void)
{
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

		//�u���b�N�J�o�[��t����(�񓧖��ɂ���)
		m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetCol(VIS_COL);

		//�A�C�X�X�e�[�g�Ɉڍs���A�ϐ��̐ݒ������
		m_eState = STATE::STATE_ICETHROW; 
		m_bIniting = false;
		m_nCounter = 0; 
	}
	//����������
	else
	{
		Selecting();
		Chasing();
	}
}
//<===============================================
//�ǐՃX�e�[�g�̒ǐՏ���
//<===============================================
void CTitle::Chasing(void)
{
	//�v���C���[�ƌx�@�𓮂���
	m_pPlayer->MovingSelect();
	ChaseCamera();
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
	constexpr float fRotMoveY = 0.02f,
		fRotMoveZ = 0.005f,
		fDestRotY = -0.66f,
		fDestRotZ = 1.15f;

	float rotY = m_pPlayer->GetRotation().y - D3DX_PI * 0.5f;

	float CameraPosDif[3] = {};	//�J�����̕␳����
	CameraPosDif[0] = sinf(rotY) * 200.0f;
	CameraPosDif[1] = 210.0f;
	CameraPosDif[2] = cosf(rotY) * 200.0f;
		 
	float CameraDis = 2500.0f;							//�J�����̋���

	//�J�����̌����̒���
	if (CameraRot.y <= fDestRotY) { CameraRot.y = fDestRotY; }
	else { CameraRot.y -= fRotMoveY; }

	//�J�����̌����̒���
	if (CameraRot.z >= fDestRotZ) { CameraRot.z = fDestRotZ; }
	else { CameraRot.z += fRotMoveZ; }
	
	//�J�����̐ݒ�
	{
		float posX = CameraPos.x + ((m_pPlayer->GetPosition().x + CameraPosDif[0]) - CameraPos.x) * 0.075f;
		float posY = CameraPos.y + ((m_pPlayer->GetPosition().y + CameraPosDif[1]) - CameraPos.y) * 0.075f;
		float posZ = CameraPos.z + ((m_pPlayer->GetPosition().z + CameraPosDif[2]) - CameraPos.z) * 0.075f;
		m_pCam->SetPositionR(D3DXVECTOR3(posX,
			posY,
			posZ));
	}

	m_pCam->SetLength(CameraDis);
	m_pCam->SetRotation(CameraRot);
}
//<===============================================
//�X�L�b�v�����ۂ̍ۂ̓���
//<===============================================
void CTitle::SkipMovement(void)
{
	const D3DXVECTOR3 PLAYER_POS = { 2630.0f, 50.0f, -250.0f };	//�v���C���[�̈ʒu
	
	//�E�^�C�g�����S
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetPosition(D3DXVECTOR3(TITLELOGO_DEST,TITLELOGO_POS.y, TITLELOGO_POS.z));
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetVtx();
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetDraw(true);

	//�E�v���X�G���^�[
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetPosition(D3DXVECTOR3(TITLELOGO_DEST, PRESSENTER_POS.y, PRESSENTER_POS.z));
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetVtx();
	m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(true);

	//�E�v���C���[
	m_pPlayer->SetPosition(PLAYER_POS);
	m_pPlayer->SetRotation(DEST_ROT);
	m_pPlayer->SetReached(true);

	//�X�e�[�g��ύX����
	m_eState = STATE::STATE_PRESSENTER;
}
//<===============================================
//�l���I���Ɏg���ϐ��̏�����
//<===============================================
void CTitle::InitingSelect(void)
{
	//�������ς݂ł���Δ�����
	if (m_bIniting) { return; }

	//�e�N�X�`���l�[��
	constexpr char* TEX_MODESELECT = "data\\TEXTURE\\Title\\select.png";					//"MODE SELECT"�̕���
	constexpr char* TEX_CHECK = "data\\TEXTURE\\Title\\start.png";							//�m�F����

	//�I����(�V���O���ƃ}���`)
	constexpr char* SELECT_NAME[CTitle::SELECT_MAX] =
	{
		"data\\TEXTURE\\Title\\silen.png",	//�V���O���v���C
		"data\\TEXTURE\\Title\\multi.png",	//�}���`�v���C
	};

	//�I����(YESNO)
	constexpr char* SELECT_YN_NAME[CTitle::SELECT_YN_MAX] =
	{
		"data\\TEXTURE\\Title\\YES.png",	//�͂�
		"data\\TEXTURE\\Title\\NO.png",	//������
	};

	//�T�C�Y�֘A
	constexpr float fSizeModeSelect[SIZING_MAX] = { 300.0f, 40.0f };					//�T�C�Y(MODE SELECT)
	constexpr float fSizeSINGLEMULTI[SIZING_MAX] = { 200.0f, 50.0f };					//�T�C�Y(�V���O���ƃ}���`)
	constexpr float fSizeCHECK[SIZING_MAX] = { 400.0f, 50.0f };							//�T�C�Y(�m�F����)
	constexpr float fSizeYESNO[SIZING_MAX] = { 52.0f, 40.0f };							//�T�C�Y(�I����YESNO)

	//�I�u�W�F�N�g2D�֘A
	const D3DXCOLOR InitFrameCol = D3DXCOLOR(0.0f,0.0f,0.0f,0.6f);					//�t���[���̏����F
	const D3DXVECTOR3 NUMCHAR_POS = D3DXVECTOR3(625.0f, 100.0f, 0.0f);					//�ǂ�����
	const D3DXVECTOR3 CHECK_POS = D3DXVECTOR3(625.0f, 475.0f, 0.0f);					//�m�F
		 
	const D3DXVECTOR3 SELECT_POS = D3DXVECTOR3(400.0f, 275.0f, 0.0f);					//�I�����̈ʒu
	constexpr float fDis_SELECT = 475.0f;												//����1	

	const D3DXVECTOR3 YES_POS = D3DXVECTOR3(485.0f, CHECK_POS.y + 150.0f, 0.0f);		//�͂��Ƃ������̈ʒu
	constexpr float fDis_YESNO = 250.0f;												//����2

	//bool�^�̏���`
	m_bIniting = true;
	m_bPush = false;

	//�T�E���h���~�߂�
	m_pPlayer->SetS(false);
	m_pPlayer->SetRotation(D3DXVECTOR3(0.0f, -3.14f, 0.0f));

	//�t���[��
	m_pObject2D[OBJ2D::OBJ2D_FRAME] = InitObj2D(TEAMLOGO_POS, VECTOR3_ZERO, nNatPriority,
		0.0f, 0.0f, true, nullptr, InitFrameCol);

	//�m�F����
	m_pObject2D[OBJ2D::OBJ2D_NUMCHAR] = InitObj2D(NUMCHAR_POS, VECTOR3_ZERO, nNatPriority,
		fSizeModeSelect[SIZING_WIDTH], fSizeModeSelect[SIZING_HEIGHT], false, TEX_MODESELECT);

	//�V���O�����}���`���̑I����
	for (int nCnt = 0; nCnt < SELECT_MAX; nCnt++)
	{

		//�V���O���ƃ}���`
		m_apSelect[nCnt] = InitObj2D(D3DXVECTOR3(
			SELECT_POS.x + fDis_SELECT * nCnt,
			SELECT_POS.y,
			SELECT_POS.z), 
			VECTOR3_ZERO, 
			nNatPriority,
			fSizeSINGLEMULTI[SIZING_WIDTH],
			fSizeSINGLEMULTI[SIZING_HEIGHT], 
			false, SELECT_NAME[nCnt]);
	}

	//�m�F���b�Z�[�W
	m_pObject2D[OBJ2D::OBJ2D_CHECK] = InitObj2D(CHECK_POS, VECTOR3_ZERO, nNatPriority,
		fSizeCHECK[SIZING_WIDTH], fSizeCHECK[SIZING_HEIGHT], false, TEX_CHECK);

	//�͂��Ƃ������̑I����
	for (int nCnt = 0; nCnt < SELECT_YN_MAX; nCnt++)
	{
		m_apYesNoObj[nCnt] = InitObj2D(D3DXVECTOR3(
			YES_POS.x + fDis_YESNO * nCnt,
			YES_POS.y,
			YES_POS.z),
			VECTOR3_ZERO,
			nNatPriority,
			fSizeYESNO[SIZING_WIDTH],
			fSizeYESNO[SIZING_HEIGHT],
			false, SELECT_YN_NAME[nCnt]);
	}

	//�u���b�N�J�o�[���O��(�����ɂ���)
	m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetCol(INV_COL);

	//�T�C�������Ɠ�������ݒ�
	m_pPlayer->SetReached(false);
	CPoliceTitle::SetSiren(true);
	
}
//<===============================================
//�t���[���̃T�C�Y����
//<===============================================
void CTitle::Sizing(void)
{
	D3DXVECTOR3 FrameSize = m_pObject2D[OBJ2D::OBJ2D_FRAME]->GetSize();		//�t���[���T�C�Y
	constexpr float fSpeedX = 21.0f,fSpeedY = 15.0f;						//�X�s�[�h

	const D3DXVECTOR3 FRAME_DEST = { 500.0f,320.0f,0.0f };					//�t���[���̖ڕW�l

	//X��
	if (FrameSize.x >= FRAME_DEST.x) { FrameSize.x = FRAME_DEST.x; }
	else { FrameSize.x += fSpeedX;}

	//Y��
	if (FrameSize.y >= FRAME_DEST.y) { FrameSize.y = FRAME_DEST.y; }
	else { FrameSize.y += fSpeedY;}

	//�T�C�Y�������I�������A���}��true�ɂ���
	if (FrameSize.y >= FRAME_DEST.y&& FrameSize.x >= FRAME_DEST.x) { m_bSizing = true; }

	//�T�C�Y��ݒ�
	m_pObject2D[OBJ2D::OBJ2D_FRAME]->SetSize(FrameSize.x,FrameSize.y);
}
//<===============================================
//�l���I���̍ۂ̏���
//<===============================================
void CTitle::Selecting(void)
{
	const D3DXVECTOR3 NUMBER_POS = D3DXVECTOR3(625.0f, 325.0f, 0.0f);		//�����̈ʒu
	constexpr int ONE_PLAYER = 1,MAX_PLAYER = 4;								//�v���C���[�̐�

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

		//��������������
		if (m_bPush)
		{
			//�V���O���������ɂȂ��Ă����猳�̐F�ɖ߂�
			if (m_apSelect[SELECT::SELECT_SINGLE]->GetCol().a == ALPHA_ZERO)
			{m_apSelect[SELECT::SELECT_SINGLE]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));}

			//�}���`�������ɂȂ��Ă����猳�̐F�ɖ߂�
			else if (m_apSelect[SELECT::SELECT_MULTI]->GetCol().a == ALPHA_ZERO)
			{m_apSelect[SELECT::SELECT_MULTI]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));}
			
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
	constexpr int nChangeColTime = 10;										//�F��ύX����ۂ̃^�C�}�[

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

	//�������ƑI������YES�ɐݒ�
	if (TriggerEnter()) {m_bPush = true; m_nSelectYN = SELECT_YN::SELECT_YN_YES;}

	//�E�{�^��
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RIGHTARROW) || 
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_RIGHT, 0))
	{ m_nSelect = (m_nSelect + 1) % SELECT_MAX; }

	//���{�^��
	else if (CInputKeyboard::GetInstance()->GetTrigger(DIK_LEFTARROW) || 
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_LEFT, 0))
	{ m_nSelect = (m_nSelect + (SELECT_MAX - 1)) % SELECT_MAX; }

	//"YES"��I�����Ă��鎞
	if (m_nSelect == SELECT::SELECT_SINGLE)
	{
		//"YES"��I����ԁA"NO"���I�����
		ColChange(m_apSelect[SELECT::SELECT_SINGLE], nChangeColTime);
		m_apSelect[SELECT::SELECT_MULTI]->SetCol(ONE_COL);
	}
	//"NO"��I�����Ă��鎞
	else if (m_nSelect == SELECT::SELECT_MULTI)
	{
		//"YES"���I����ԁA"NO"��I�����
		ColChange(m_apSelect[SELECT::SELECT_MULTI], nChangeColTime);
		m_apSelect[SELECT::SELECT_SINGLE]->SetCol(ONE_COL);
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
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RIGHTARROW) 
		|| CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_RIGHT, 0)) 
	{ m_nSelectYN = (m_nSelectYN + 1) % SELECT_YN_MAX; }

	else if (CInputKeyboard::GetInstance()->GetTrigger(DIK_LEFTARROW) 
		|| CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_LEFT, 0))
	{ m_nSelectYN = (m_nSelectYN + (SELECT_YN_MAX - 1)) % SELECT_YN_MAX; }

	//���͂������
	if (TriggerEnter())
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
	constexpr int nChangeColTime = 7;										//�F��ύX����ۂ̃^�C�}�[

	//"YES"��I�����Ă��鎞
	if (m_nSelect == SELECT::SELECT_SINGLE) 
	{
		//�I������Ă��Ȃ��}���`�𓧖��ɂ���
		m_apSelect[SELECT::SELECT_SINGLE]->SetCol(ONE_COL);
		m_apSelect[SELECT::SELECT_MULTI]->SetCol(ZERO_COL);
	
	}

	//"NO"��I�����Ă��鎞
	else if (m_nSelect == SELECT::SELECT_MULTI)
	{
		//�I������Ă��Ȃ��V���O���𓧖��ɂ���
		m_apSelect[SELECT::SELECT_SINGLE]->SetCol(ZERO_COL);
		m_apSelect[SELECT::SELECT_MULTI]->SetCol(ONE_COL);
	}

	//"YES"��I�����Ă��鎞
	if (m_nSelectYN == SELECT_YN::SELECT_YN_YES)
	{
		//"YES"��I����ԁA"NO"���I�����
		ColChange(m_apYesNoObj[SELECT_YN::SELECT_YN_YES], nChangeColTime);
		m_apYesNoObj[SELECT_YN::SELECT_YN_NO]->SetCol(ONE_COL);
	}
	//"NO"��I�����Ă��鎞
	else if (m_nSelectYN == SELECT_YN::SELECT_YN_NO)
	{
		//"YES"���I����ԁA"NO"��I�����
		ColChange(m_apYesNoObj[SELECT_YN::SELECT_YN_NO], nChangeColTime);
		m_apYesNoObj[SELECT_YN::SELECT_YN_YES]->SetCol(ONE_COL);
	}

}
//<===============================================
//�f�o�b�O���̃J�����̓���
//<===============================================
void CTitle::DebugCam(void)
{
#ifdef _DEBUG
	D3DXVECTOR3 CamRot = m_pCam->GetRotation();	//�J��������
	constexpr float fRotMove = 0.01f;				//�J���������̈ړ��l

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
//�A�C�X�X�e�[�g���̏�����
//<===============================================
void CTitle::InitingIce(void)
{
	//����������Ă����珈���𔲂���
	if (m_bIniting) { return; }

	const D3DXVECTOR3 PLAYER_POS = { 2630.0f, 50.0f, -200.0f };			//�v���C���[�̈ʒu
	constexpr float fDis = 150.0f;										//�x�@�̋���
	constexpr float fLength = 1350.0f;									//�J�����̒���

	//�������������Ƃɂ���
	m_bIniting = true;
	m_bPush = false;

	//�ʒu��񏉊���
	m_pPlayer->SetPosition(PLAYER_POS);
	m_pPlayer->SetRotation(DEST_ROT);

	//�x�@�̐���
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{
		//�ʒu��񏉊���
		m_apPolice[nCnt]->SetPosition(D3DXVECTOR3(PolicePos.x + fDis * nCnt, PolicePos.y, PolicePos.z));
		m_apPolice[nCnt]->SetRotation(DEST_ROT);
	}

	//�J�����̐ݒ�
	m_pCam->SetRotation(D3DXVECTOR3(0.0f, 0.0f, 1.57f));
	m_pCam->SetLength(fLength);

	//�u���b�N�J�o�[���O��(�����ɂ���)
	m_pObject2D[OBJ2D::OBJ2D_BLACKCOVER]->SetCol(INV_COL);
}
//<===============================================
//�A�C�X�X�e�[�g���̓���
//<===============================================
void CTitle::IceMovement(void)
{
	D3DXVECTOR3 PlayerPos = VECTOR3_ZERO;							//�v���C���[�ʒu
	constexpr int FADE_TIME = 200;									//�Q�[����ʂɈڍs����܂ł̎���
	constexpr int FADE_TIME_HARF = 65;								//�Q�[����ʂɈڍs����܂ł̎��Ԃ̔����l

	//<*************************************************************
	//float�^
	//<*************************************************************
	constexpr float PlayerMove = 50.0f;								//�v���C���[�̓����l
	constexpr float CamHeightDis = 150.0f;							//�����̍�
	constexpr float fDisMax = 900.0f;								//�����̍ő�l
	constexpr float fMoveValue = 7.5f;								//�����ړ��̍ۂ̒l

	InitingIce();

	m_pPlayer->BaggageMove();

	//�v���C���[�ʒu�ݒ�
	PlayerPos = m_pPlayer->GetPosition();
	PlayerPos.z += PlayerMove;
	m_pPlayer->SetPosition(PlayerPos);

	//�x�@�̐���
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{
		//�ǐ�
		m_apPolice[nCnt]->Chasing(PlayerMove);
	}

	//�J�����̈ʒu��ݒ�
	m_pCam->SetPositionR(D3DXVECTOR3
	(m_pPlayer->GetTitleGoal()->GetPos().x - m_fDis,
		m_pPlayer->GetTitleGoal()->GetPos().y+ CamHeightDis,
		m_pPlayer->GetTitleGoal()->GetPos().z));

	//�t�F�[�h�X�e�[�g�������Ȃ���������
	if (CManager::GetInstance()->GetFade()->GetState() == CFade::STATE_NONE)
	{
		//�����Ă�����Q�[����ʂɑJ�ڂ���
		if (m_nCounter >= FADE_TIME)
		{
			//�V���O���v���C���I������Ă�����A���̂܂܃Q�[����ʂɑJ��
			if (GetSelectSingleMulti() == SELECT::SELECT_SINGLE)
			{CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);}

			//�}���`�v���C���I������Ă�����A�G���g���[��ʂɑJ��
			else if (GetSelectSingleMulti() == SELECT::SELECT_MULTI)
			{CManager::GetInstance()->GetFade()->Set(CScene::MODE_ENTRY);}
		}

		//�����Ă��Ȃ�������J�E���g����
		else { m_nCounter++; }
	}

	//�������炢�̒l�ɂȂ��Ă�����
	if (m_nCounter >= FADE_TIME_HARF)
	{
		//���������l�ɂȂ����炻�̒l�ɂ��A�Ȃ��Ă��Ȃ�������߂Â���
		if (m_fDis >= fDisMax) { m_fDis = fDisMax; }
		else { m_fDis += fMoveValue;}

		//�T�C���������~�߂�
		CPoliceTitle::SetSiren(false);
	}

	//�X�L�b�v�p�̓��͏���
	if (TriggerEnter())
	{
		//�V���O���v���C���I������Ă�����A���̂܂܃Q�[����ʂɑJ��
		if (GetSelectSingleMulti() == SELECT::SELECT_SINGLE)
		{CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);}

		//�}���`�v���C���I������Ă�����A�G���g���[��ʂɑJ��
		else if (GetSelectSingleMulti() == SELECT::SELECT_MULTI)
		{CManager::GetInstance()->GetFade()->Set(CScene::MODE_ENTRY);}
	}
}
//<===============================================
//�G���^�[�����������ǂ���
//<===============================================
bool CTitle::TriggerEnter(void) 
{
	//�G���^�[��������
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) ||
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) ||
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0))
	{
		return true;
	}

	return false;
}
//<===============================================
//�I�u�W�F�N�g2D�̏�����
//<===============================================
CObject2D* CTitle::InitObj2D(const D3DXVECTOR3 rPos,
	const D3DXVECTOR3 rRot,
	const int nPri,
	const float fWidth,
	const float fHeight,
	const bool bDraw,
	const char* pTexName,
	const D3DXCOLOR rCol)
{
	//�����p�̃I�u�W�F�N�g
	CObject2D* pObj2D = nullptr;

	//�I�u�W�F�N�g2D�̏��������s��
	pObj2D = CObject2D::Create(rPos, rRot, nPri);
	pObj2D->SetSize(fWidth, fHeight);
	pObj2D->SetDraw(bDraw);

	//�e�N�X�`���̖��O����������
	if (pTexName)
	{
		pObj2D->BindTexture(CManager::GetInstance()->GetTexture()->Regist(pTexName));
	}

	//�F�̐ݒ�
	pObj2D->SetCol(rCol);	

	return pObj2D;
}
