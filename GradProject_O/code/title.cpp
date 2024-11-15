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
	const int AMO_VALUE = 20;			//�F�ύX���̒l

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
	m_bPush = false;
	m_bNext = false;
	m_bDisplay = false;
	m_eState = STATE::STATE_TEAMLOGO;
	m_bCol = false;
	m_pPlayer = nullptr;
	m_pCam = nullptr;

	//�z��̏�����
	for (int nCnt = 0; nCnt < OBJ2D_MAX; nCnt++)
	{
		m_pObject2D[nCnt] = nullptr;
	}

	m_bIniting = false;

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
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo] = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), VECTOR3_ZERO);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetSize(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);
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

		//
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
	const int COUNT_MAX = 10;											// �J�E���^�[�̌Œ�l

	//�X�e�[�g���`�[�����S�������ꍇ
	if (m_eState == STATE::STATE_TEAMLOGO)
	{
		if(!m_bCol)
		{
			TeamCol.a -= MAX_ALPHA / AMO_VALUE;	//�����ɋ߂Â��Ă���

			if (TeamCol.a <= 0.0f)
			{//���S�ɓ����ɂȂ����ꍇ
				TeamCol.a = 0.0f;	//�����x�𓧖���

				//�����Ă�����J�E���g�̏������Ɠ����I�����}�𑗂�
				if (m_nCounter >= COUNT_MAX)
				{
					//���̃X�e�[�g�Ɉڍs����
					m_eState = STATE::STATE_PRESSENTER;
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
	const int COUNT_MAX = 10;	

	//���̏ꏊ�ɂ��Ă��ăv���X�G���^�[�̕������\������Ă��Ȃ����
	if (m_pPlayer->GetReached()&&!m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetDraw())
	{
		//�����Ă�����
		if (m_nCounter >= COUNT_MAX)
		{
			MovingLogo();
		}
		//�����Ă��Ȃ�������J�E���g����
		else{m_nCounter++;}

	}
	//�v���X�G���^�[�̕������\������Ă�����
	else if (m_pObject2D[OBJ2D::OBJ2D_PressEnter]->GetDraw())
	{
		TitleLogo();

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
}
//<===============================================
//�^�C�g�����S�̓���
//<===============================================
void CTitle::MovingLogo(void)
{
	const float SPEED = 0.09f;							//�^�C�g�����S�������X�s�[�h

	//�`��ݒ���I���ɂ��A�ړ��p�̕ϐ��Ƀ^�C�g�����S�̈ʒu���擾������
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetDraw(true);
	m_TitlePos = m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->GetPosition();

	//�ړI�n�܂ňړ�������
	m_TitlePos.x += (TITLELOGO_DEST - m_TitlePos.x - 0.0f) * 0.09f;//X��

	//�ړI�n�ɒ�������
	if (Function::MoveToDest(m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->GetPosition(),
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
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetPosition(m_TitlePos);
	m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetVtx();
}
//<===============================================
//�v���X�G���^�[�X�e�[�g�ł̓���
//<===============================================
void CTitle::InitingP_E(void)
{
	D3DXCOLOR TeamCol = m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->GetCol();	// �`�[�����S�̐F�����擾

	//�������ς݂ł͂Ȃ����
	if (!m_bIniting)
	{
		//�`�[�����S��`�悵�Ȃ�
		m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetDraw(false);

		//2�̏���������
		m_bPush = false;
		m_bNext = false;

		//�J�����������
		m_pCam = CManager::GetInstance()->GetCamera();
		m_pCam->SetPositionR(D3DXVECTOR3(-4000.0f, 95.0f, 260.0f));
		m_pCam->SetLength(100.0f);
		m_pCam->SetRotation(D3DXVECTOR3(0.0f, -0.0f, 1.79f));
		//m_pCam->SetRotation(D3DXVECTOR3(0.0f, -2.1f, 1.79f));
		m_pCam->SetActive(false);

		//�^�C�g�����S����
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO] = CObject2D::Create(TITLELOGO_POS, VECTOR3_ZERO, 5);
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetLength(150.0f, 150.0f);
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->SetDraw(false);
		m_pObject2D[OBJ2D::OBJ2D_TITLELOGO]->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\Pre_char000.png"));

		//�v���X�G���^�[����
		m_pObject2D[OBJ2D::OBJ2D_PressEnter] = CObject2D::Create(PRESSENTER_POS, VECTOR3_ZERO,5);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetLength(150.0f, 150.0f);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(false);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\T_PressEnter000.png"));

		//�K�v�ȃI�u�W�F�N�g�̐���
		CMapManager::GetInstance()->Load();
		CMeshField::Create(D3DXVECTOR3(0.0f, -10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1000.0f, 1000.0f, "data\\TEXTURE\\field000.jpg", 30, 30);
		m_pPlayer = CPlayerTitle::Create(D3DXVECTOR3(-4734.0f, 50.0f, -1988.0f), D3DXVECTOR3(0.0f, 3.14f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f),nullptr,nullptr);

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
	const int COUNT_MAX = 50;													//�J�E���^�[�̌Œ�l

	//�F�ύX�������Ă��Ȃ����
	if (!m_bCol) 
	{
		//�����ɋ߂Â��Ă���
		TitleLogoCol.a += MAX_ALPHA / -AMO_VALUE;

		//���̒l�܂ōs������
		if (TitleLogoCol.a <= MIN_ALPHA)
		{
			TitleLogoCol.a = MIN_ALPHA;	//�����x�����̒l�ɂ���

			//�����Ă�����J�E���g�̏������Ɠ����I�����}�𑗂�
			if (m_nLogoAlpgha >= COUNT_MAX)
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
		TitleLogoCol.a += MAX_ALPHA / AMO_VALUE;

		//�ő�l�܂ŋ߂Â�����
		if (TitleLogoCol.a >= MAX_ALPHA)
		{
			//�����x�����̒l�ɂ���
			TitleLogoCol.a = MAX_ALPHA;	

			//�����Ă�����J�E���g�̏������ƏI�����}�𑗂�
			if (m_nLogoAlpgha >= COUNT_MAX)
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