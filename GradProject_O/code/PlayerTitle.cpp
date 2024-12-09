//<=================================================
//�^�C�g���ł̃v���C���\�̓������� [PlayerTitle.cpp]
//
//Author : Kazuki Watanabe
//<=================================================
#include "PlayerTitle.h"
#include "camera.h"
#include "camera_manager.h"
#include "debugproc.h"
#include "objectX.h"
#include "navi.h"
#include "TitleBaggage.h"
#include "character.h"
#include "model.h"
#include "motion.h"
#include "PoliceTitle.h"
#include "title.h"

//<************************************************
//���O�錾
//<************************************************
namespace
{
	//�ړI�n�̈ʒu
	const D3DXVECTOR3 DEST_POS[CPlayerTitle::DEST::DEST_MAX] =
	{ D3DXVECTOR3(2630.0f, 0.0f, 1054.0f),
		D3DXVECTOR3(2630.0f, 0.0f, -200.0f),
		D3DXVECTOR3(-4734.0f, 0.0f, -800.0f),
		D3DXVECTOR3(-4734.0f, 0.0f, -800.0f),
		D3DXVECTOR3(-4734.0f, 0.0f, -800.0f),
		D3DXVECTOR3(-4734.0f, 0.0f, -800.0f)
	
	};

	//�ړI�n�̈ʒu
	const D3DXVECTOR3 DEST_POS_SELECT[CPlayerTitle::DEST::DEST_MAX]=
	{
		D3DXVECTOR3(2700.0f, 0.0f, 6000.0f),		//�P��
		D3DXVECTOR3(-3160.0f, 0.0f, 6000.0f),		//�Q��
		D3DXVECTOR3(-3160.0f, 0.0f, 4000.0f),		//�R��
		D3DXVECTOR3(-4750.0f, 0.0f, 4000.0f),		//�S��
		D3DXVECTOR3(-4750.0f, 0.0f, -200.0f),		//�T��
		D3DXVECTOR3(2700.0f, 0.0f, -200.0f)			//�U��
	};
	//�ړI�̌���
	const D3DXVECTOR3 DEST_ROT_SELECT[CPlayerTitle::DEST::DEST_MAX] =
	{
		D3DXVECTOR3(0.0f,-3.14f,0.0f),				//�P��
		D3DXVECTOR3(0.0f,1.57f,0.0f),				//�Q��
		D3DXVECTOR3(0.0f,0.0f,0.0f),				//�R��
		D3DXVECTOR3(0.0f,1.57f,0.0f),				//�S��
		D3DXVECTOR3(0.0f,0.0f,0.0f),				//�T��
		D3DXVECTOR3(0.0f,-1.57f,0.0f)				//�U��
	};
}

//<================================================
//�R���X�g���N�^
//<================================================
CPlayerTitle::CPlayerTitle()
{
	//������
	//<*************************************
	//int�^
	m_nNumDest = 0;
	//<*************************************
	//float�^
	m_fDestrot = 0.0f;
	m_fMoveRot = 0.0f;
	m_fBDustValue = 50.0f;
	//<*************************************
	//���̑�
	m_eState = STATE_NONE;
	m_bReached = false;
	m_bNextMove = false;
	m_bMoved = false;
	m_bFirst = true;
	m_pTitleBaggage = nullptr;
	m_pTitleGoal = nullptr;
	//<*************************************
}
//<================================================
//�f�X�g���N�^
//<================================================
CPlayerTitle::~CPlayerTitle()
{

}
//<================================================
//����������
//<================================================
HRESULT CPlayerTitle::Init(void)
{
	CPlayer::Init();

	return S_OK;
}
//<===============================================
//����������(�I�[�o�[���[�h)
//<===============================================
HRESULT CPlayerTitle::Init(const char* pBodyName, const char* pLegName)
{
	constexpr char* MODEL_NAME = "data\\MODEL\\bike.x";		//���f����
	constexpr char* SoundName = "data\\SE\\idol.wav";		//�T�E���h��

	//�R���e�i��������Ȃ��̂ł��̂悤�Ȍ`�ɂ��܂���
	m_pObj = CObjectX::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), MODEL_NAME);
	m_pObj->SetType(CObject::TYPE_PLAYER);
	m_pObj->SetRotateType(CObjectX::TYPE_QUATERNION);
	SetMatrix();
	SetMatrix();
	m_pNavi = CNavi::Create();

	//�T�E���h����
	m_pSound = CMasterSound::CObjectSound::Create(SoundName, -1);
	m_pSound->SetVolume(1.0f);
	m_pSound->Stop();

	//�ו�����
	m_pTitleBaggage = CTitleBaggage::Create(this->GetPosition());

	//�G�t�F�N�g����
	m_pAfterburner = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\afterburner.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\taillamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	m_pBackdust = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\backdust.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);

	return S_OK;
}
//<================================================
//�I������
//<================================================
void CPlayerTitle::Uninit(void)
{
	CPlayer::Uninit();
	SAFE_UNINIT_DELETE(m_pSound);
	SAFE_UNINIT(m_pTitleBaggage);
	SAFE_UNINIT(m_pTitleGoal);
}
//<================================================
//�X�V����
//<================================================
void CPlayerTitle::Update(void)
{
	constexpr float BDustMaxValue = 300.0f;									//��납��o�鉌�̍ő�l

	//�f�o�b�O�ȊO��������
	if (m_eState == STATE_NONE)
	{
		// �O��̍��W���擾
		m_Info.posOld = GetPosition();

		StateSet();

		// �}�g���b�N�X
		SetMatrix();

		// �����蔻��
		Collision();

		//�I�u�W�F�N�g���̂̒��g�������
		if (m_pObj)
		{
			//�ݒ�
			SetPosition(GetPosition());
			SetRotation(GetRotation());
			m_pObj->SetPosition(GetPosition());
			m_pObj->SetRotation(GetRotation());
			m_pObj->SetShadowHeight(GetPosition().y);
		}
		//���̓����Ɉڍs���Ă��Ȃ�������A�A�C�X���v���C���[�̏�ɏ������
		if (!m_bNextMove) { m_pTitleBaggage->SetPosition(GetPosition()); }

		//�ڍs���Ă����珉���ʒu�ɖ߂�
		else { m_pTitleBaggage->SetPosition(VECTOR3_ZERO); m_fBDustValue = BDustMaxValue; }

		//<********************************
		//�G�t�F�N�g���o��
		//<********************************
		m_pTailLamp->m_pos = GetPosition();
		m_pTailLamp->m_rot = GetRotation();
		m_pBackdust->m_pos = GetPosition();
		m_pBackdust->m_rot = GetRotation();
		m_pBackdust->m_Scale = VECTOR3_ONE * m_fBDustValue;
		m_pAfterburner->m_pos = GetPosition();
		m_pAfterburner->m_Scale = VECTOR3_ONE * m_fBrake * m_fBDustValue;
	}
	//�f�o�b�O��������
	else if (m_eState == STATE_DEBUG)
	{
		//�s�����[�h
		SetType(CPlayer::TYPE::TYPE_ACTIVE);
		CPlayer::Update();
	}

	//�f�o�b�O�\��
	CDebugProc::GetInstance()->Print("�v���C���[���W: [ %f, %f, %f ]\n", m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);
	CDebugProc::GetInstance()->Print("�v���C���[����: [ %f, %f, %f ]\n", m_Info.rot.x, m_Info.rot.y, m_Info.rot.z);
}
//<================================================
//�����Ɋւ��鏈��
//<================================================
void CPlayerTitle::Moving(const int nNum)
{
	constexpr float DEST_DIFF = 5.0f;										//�����̍�

	//���K��Ԃ��Ȃ���
	if (m_bReached) { m_bReached = false; }

	//�f�o�b�O�ȊO��������
	if (m_eState == STATE_NONE)
	{
		//�ړI�n�܂ňړ�
		m_Info.pos.x += (DEST_POS[nNum].x - m_Info.pos.x - m_Info.move.x) * 0.075f;//X��
		m_Info.pos.z += (DEST_POS[nNum].z - m_Info.pos.z - m_Info.move.z) * 0.075f;//Z��

		//�ړI�n�ɓ��������画���true�ɂ���
		if (Function::BoolDis(m_Info.pos, DEST_POS[nNum], DEST_DIFF)) { m_bReached = true; }
	}
}
//<================================================
//�I����ʂ̍ۂ̓���
//<================================================
void CPlayerTitle::MovingSelect(void)
{
	constexpr float fMove = 0.12f;	//�ړ����x�Ɖ�]���x
	const float fRad1 = 95.0f,fRad2 = 500.0f;		//�͈�

	//�ړI�n�ɓ��������画���true�ɂ���
	if (Function::BoolDis(GetPosition(), DEST_POS_SELECT[m_nNumDest], fRad1))
	{
		//�ړI�n�ύX
		m_nNumDest = (m_nNumDest + 1) % DEST_MAX;
	}
	//���菉�����p
	if (m_nNumDest >= DEST_SIXTH){m_bFirst = false;}
	
	PoliceRotSet();

	//�f�o�b�O�\��
	CDebugProc::GetInstance()->Print("[���݂̔ԍ�]�F%d\n", m_nNumDest);
}
//<================================================
//�ו��̓�������
//<================================================
void CPlayerTitle::BaggageMove(void)
{
	constexpr float fDis = -200.0f;
	D3DXVECTOR3 GoalPos = VECTOR3_ZERO;

	//���̓����Ɉڍs���Ă��Ȃ�������
	if (!m_bNextMove)
	{
		//�ݒ�
		m_bNextMove = true;

		//�ʒu��ݒ�
		GoalPos = D3DXVECTOR3(this->GetPosition().x + fDis, this->GetPosition().y, this->GetPosition().z);

		//�S�[�����v���C���[�̉��ɐݒ肷��
		m_pTitleGoal = CTitleGoal::Create(GoalPos,D3DXVECTOR3(0.0f,-1.56f,0.0f));

		//�l�ԂƉו��̐ݒ�
		m_pTitleBaggage->GetObj()->SetParent(m_pTitleGoal->GetCHR()->GetParts(6)->GetMtx());
		m_pTitleGoal->GetCHR()->GetMotion()->BlendSet(4);
	}
}
//<================================================
//�x�@�̈ʒu�ݒ�
//<================================================
void CPlayerTitle::PoliceRotSet(void)
{
	CPoliceTitle* apPolice[INITIAL::POLICE_MAX] = { nullptr };	//�x�@�̃|�C���^
	D3DXVECTOR3 rRot = VECTOR3_ZERO;							//����
	float fMove = 125.0f;										//�ړ����x
	constexpr float fRotation = 0.3f;							//��]���x

	//�x�@�̏����擾���Ă���
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{
		apPolice[nCnt] = CTitle::GetPoliTitle(nCnt);
	}

	//�ԍ��ɂ���ĕύX������
	switch (m_nNumDest)
	{
		//�ŏ��̖ړI�n
	case DEST::DEST_FIRST:

		//�ŏ��̌����ɂ���
		rRot = DEST_ROT_SELECT[DEST::DEST_FIRST];

		//�ړ�
		m_Info.pos.z += fMove;

		break;

		//��Ԗڂ��l�Ԗ�
	case DEST::DEST_SECOND:
	case DEST::DEST_FOUTH:

		//��Ԗڂ̌����ɂ���
		rRot = DEST_ROT_SELECT[DEST::DEST_SECOND];

		//�ړ�
		m_Info.pos.x -= fMove;

		break;

		//�O�Ԗڂ��ܔԖ�
	case DEST::DEST_THIRD:
	case DEST::DEST_FIFTH:

		//�O�Ԗڂ̌����ɂ���
		rRot = DEST_ROT_SELECT[DEST::DEST_THIRD];

		//�ړ�
		m_Info.pos.z -= fMove;

		break;

		//�Ō�̔ԍ�
	case DEST::DEST_SIXTH:

		//�ŏI�n�_�̌����ɂ���
		rRot = DEST_ROT_SELECT[DEST::DEST_SIXTH];

		//�ړ�
		m_Info.pos.x += fMove;


		break;
	}

	//�x�@�̌�����ݒ�
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{
		apPolice[nCnt]->SetRotation(rRot);
	}

	//��]������ݒ�
	m_fDestrot = rRot.y;

	//�l�ԖڈȊO��������
	if (m_nNumDest != DEST::DEST_FOUTH)
	{
		//��Ԗڂ̖ړI�n���܂����肳��ĂȂ�������A����ƌ�����ݒ�
		if (m_nNumDest == DEST::DEST_SECOND && !m_bFirst) { m_Info.rot.y = 3.14f; m_bFirst = true; }

		//�ړ����x�ݒ�
		m_fMoveRot = -fRotation;

		//�v���C���[��]
		m_Info.rot.y += m_fMoveRot;

		//������������
		if (m_Info.rot.y <= m_fDestrot) { m_Info.rot.y = m_fDestrot; }
	}
	//�l�Ԗڂ�������
	else
	{
		//�ړ����x�ݒ�
		m_fMoveRot = fRotation;

		//�v���C���[��]
		m_Info.rot.y += m_fMoveRot;

		//������������
		if (m_Info.rot.y >= m_fDestrot) { m_Info.rot.y = m_fDestrot; }
	}

	//����
	Adjust(m_Info.rot.y);

	//���̌����ɐݒ�
	SetRotation(m_Info.rot);

	PolicePosSet();
}
//<================================================
//�x�@�̈ʒu�ݒ�
//<================================================
void CPlayerTitle::PolicePosSet(void)
{
	CPoliceTitle* apPolice[INITIAL::POLICE_MAX] = { nullptr };	//�x�@�|�C���^
	D3DXVECTOR3 arPos[INITIAL::POLICE_MAX] = { VECTOR3_ZERO };	//�x�@�̈ʒu
	constexpr float fDis = 650.0f;								//�v���C���[����̋���
	constexpr float fDis_Police = 350.0f;						//�x�@�Ԃ̋���
	constexpr float fDiff = 250.0f;								//�␳(�x�@�p)

	const D3DXVECTOR3 aPos = GetPosition();						//�v���C���[�ʒu

	//�x�@�̏����擾���Ă���
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{
		//���擾+�ʒu���擾
		apPolice[nCnt] = CTitle::GetPoliTitle(nCnt);
		arPos[nCnt] = apPolice[nCnt]->GetPosition();
	}

	//�x�@�̏����擾���Ă���
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{
		//�p�g�����v
		apPolice[nCnt]->SettingPatLamp();

		//�ԍ��ɂ���ĕύX������
		switch (m_nNumDest)
		{
			//�ŏ��̖ړI�n
		case DEST::DEST_FIRST:

			//���̈ʒu�ɐݒ�
			arPos[nCnt] = D3DXVECTOR3(aPos.x - fDiff + fDis_Police *nCnt, aPos.y, aPos.z - fDis);

			break;

			//��Ԗڂ��l�Ԗ�
		case DEST::DEST_SECOND:
		case DEST::DEST_FOUTH:

			//���̈ʒu�ɐݒ�
			arPos[nCnt] = D3DXVECTOR3(aPos.x + fDis, aPos.y, aPos.z - fDiff + fDis_Police * nCnt);

			break;

			//�O�Ԗڂ��ܔԖ�
		case DEST::DEST_THIRD:
		case DEST::DEST_FIFTH:

			//���̈ʒu�ɐݒ�
			arPos[nCnt] = D3DXVECTOR3(aPos.x - fDiff + fDis_Police * nCnt, aPos.y, GetPosition().z + fDis);

			break;

			//�Ō�̔ԍ�
		case DEST::DEST_SIXTH:

			//���̈ʒu�ɐݒ�
			arPos[nCnt] = D3DXVECTOR3(aPos.x - fDis, aPos.y, aPos.z - fDiff + fDis_Police * nCnt);

			break;
		}
		//�ʒu�ƌ�����ݒ�
		apPolice[nCnt]->SetPosition(arPos[nCnt]);
	}
}
//<================================================
//��������
//<================================================
CPlayerTitle* CPlayerTitle::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move
, const char* pBodyName, const char* pLegName)
{
	//�����̂��߂̃I�u�W�F�N�g
	CPlayerTitle* pPlayertitle = DEBUG_NEW CPlayerTitle;

	//���g����������
	if (pPlayertitle)
	{
		// ����������
		pPlayertitle->Init(pBodyName, pLegName);

		// ���W�ݒ�
		pPlayertitle->SetPosition(pos);

		// �����ݒ�
		pPlayertitle->SetRotation(rot);

		//���邩�͕�����Ȃ�
		pPlayertitle->m_fRotDest = rot.y;

		// �ړ��ʐݒ�
		pPlayertitle->SetMove(move);
	}
	//���������ꍇ�A���g�Ȃ���Ԃ�
	else
	{
		return nullptr;
	}

	//���̏���Ԃ�
	return pPlayertitle;
}
//<================================================
//�ړI�n�ɓ����������̔���
//<================================================
bool Function::BoolToDest(const D3DXVECTOR3 Pos, const D3DXVECTOR3 DestPos, const float Distance, bool bZuse)
{
	//Z�����g���Ȃ�
	if (bZuse)
	{
		//�ړI�̈ʒu�ɂ�����
		if (Pos.x >= DestPos.x + Distance && Pos.x <= DestPos.x - Distance
			|| Pos.x <= DestPos.x + Distance && Pos.x >= DestPos.x - Distance
			&& Pos.z >= DestPos.z + Distance && Pos.z <= DestPos.z - Distance
			|| Pos.z <= DestPos.z + Distance && Pos.z >= DestPos.z - Distance)
		{
			return true;
		}

	}
	//Z�����g��Ȃ��Ȃ�
	else
	{
		//�ړI�̈ʒu�ɂ�����
		if (Pos.x >= DestPos.x + Distance && Pos.x <= DestPos.x - Distance
			|| Pos.x <= DestPos.x + Distance && Pos.x >= DestPos.x - Distance)
		{
			return true;
		}

	}

	return false;
}
//<================================================
//�ړI�n�ɓ����������̔���
//<================================================
bool Function::BoolDis(const D3DXVECTOR3 Pos, const D3DXVECTOR3 DestPos, const float Distance)
{
	//�����v�Z
	const D3DXVECTOR3 rDis = D3DXVECTOR3(Pos.x - DestPos.x, Pos.y - DestPos.y, Pos.z - DestPos.z);

	//�����߂Â��Ă�����
	if (rDis.x <= Distance &&
		rDis.z <= Distance &&
		!(-rDis.x >= Distance) &&
		!(-rDis.z >= Distance))
	{
		return true;
	}

	return false;

}