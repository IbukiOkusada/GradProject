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

	const D3DXVECTOR3 DEST_ROT_SELECT[CPlayerTitle::DEST::DEST_MAX] =
	{
		D3DXVECTOR3(0.0f,3.14f,0.0f),				//�P��
		D3DXVECTOR3(0.0f,1.56f,0.0f),				//�Q��
		D3DXVECTOR3(0.0f,0.0f,0.0f),				//�R��
		D3DXVECTOR3(0.0f,1.56f,0.0f),				//�S��
		D3DXVECTOR3(0.0f,0.0f,0.0f),				//�T��
		D3DXVECTOR3(0.0f,-1.56f,0.0f)				//�U��
	};

	const float DEST_DIFF = 5.0f;										//�����̍�
	const float BDustMaxValue = 300.0f;									//��납��o�鉌�̍ő�l
}

//<================================================
//�R���X�g���N�^
//<================================================
CPlayerTitle::CPlayerTitle()
{
	m_nNumDest = 0;
	m_eState = STATE_NONE;
	m_fBDustValue = 50.0f;
	m_bReached = false;
	m_bNextMove = false;
	m_bMoved = false;
	m_pTitleBaggage = nullptr;
	m_pTitleGoal = nullptr;
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
	//�R���e�i��������Ȃ��̂ł��̂悤�Ȍ`�ɂ��܂���
	m_pObj = CObjectX::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), "data\\MODEL\\flyingscooter.x");
	m_pObj->SetType(CObject::TYPE_PLAYER);
	m_pObj->SetRotateType(CObjectX::TYPE_QUATERNION);
	SetMatrix();
	SetMatrix();
	m_pNavi = CNavi::Create();

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
	SAFE_UNINIT(m_pTitleBaggage);
	SAFE_UNINIT(m_pTitleGoal);
}
//<================================================
//�X�V����
//<================================================
void CPlayerTitle::Update(void)
{
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
//
//<================================================
void CPlayerTitle::MovingSelect(void)
{
	const float PlayerMove = 25.0f;						//�v���C���[�̓����l

	////�ړI�n�܂ňړ�
	m_Info.pos.x += (DEST_POS_SELECT[m_nNumDest].x - m_Info.pos.x - m_Info.move.x) * 0.065f;//X��
	m_Info.pos.z += (DEST_POS_SELECT[m_nNumDest].z - m_Info.pos.z - m_Info.move.z) * 0.065f;//Z��

	//�ړI�n�ɓ��������画���true�ɂ���
	if (Function::BoolDis(GetPosition(), DEST_POS_SELECT[m_nNumDest], 50.0f))
	{
		m_nNumDest +=1;
	}

	//�ړI�n�̍ő�l�܂ōs���Ă�����A������
	if (m_nNumDest >= DEST_MAX)
	{m_nNumDest = DEST::DEST_FIRST;}

	PoliceRotSet();

	////���̕����Ƀv���C���[����������
	//m_Info.rot.y += (DEST_ROT_SELECT[m_nNumDest].y - m_Info.rot.y) * 0.065f;
	//Adjust(m_Info.rot);

	//���̌����ɐݒ�
	SetRotation(DEST_ROT_SELECT[m_nNumDest]);

	//�f�o�b�O�\��
	CDebugProc::GetInstance()->Print("[���݂̔ԍ�]�F%d\n", m_nNumDest);
}
//<================================================
//�ו��̓�������
//<================================================
void CPlayerTitle::BaggageMove(void)
{
	const float fDis = -200.0f;
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
	CPoliceTitle* apPolice[INITIAL::POLICE_MAX] = { nullptr };
	D3DXVECTOR3 rRot = VECTOR3_ZERO;

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

		break;

		//��Ԗڂ��l�Ԗ�
	case DEST::DEST_SECOND:
	case DEST::DEST_FOUTH:

		//��Ԗڂ̌����ɂ���
		rRot = DEST_ROT_SELECT[DEST::DEST_SECOND];

		break;

		//�O�Ԗڂ��ܔԖ�
	case DEST::DEST_THIRD:
	case DEST::DEST_FIFTH:

		//�O�Ԗڂ̌����ɂ���
		rRot = DEST_ROT_SELECT[DEST::DEST_THIRD];

		break;

		//�Ō�̔ԍ�
	case DEST::DEST_SIXTH:

		//�ŏI�n�_�̌����ɂ���
		rRot = DEST_ROT_SELECT[DEST::DEST_SIXTH];

		break;
	}
	//�x�@�̌�����ݒ�
	for (int nCnt = 0; nCnt < INITIAL::POLICE_MAX; nCnt++)
	{apPolice[nCnt]->SetRotation(rRot);}

	PolicePosSet();
}
//<================================================
//�x�@�̈ʒu�ݒ�
//<================================================
void CPlayerTitle::PolicePosSet(void)
{
	CPoliceTitle* apPolice[INITIAL::POLICE_MAX] = { nullptr };
	D3DXVECTOR3 arPos[INITIAL::POLICE_MAX] = { VECTOR3_ZERO };
	float fDis = 500.0f;

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
			arPos[nCnt] = D3DXVECTOR3(GetPosition().x + 150.0f*nCnt, GetPosition().y, GetPosition().z - fDis);

			break;

			//��Ԗڂ��l�Ԗ�
		case DEST::DEST_SECOND:
		case DEST::DEST_FOUTH:

			//���̈ʒu�ɐݒ�
			arPos[nCnt] = D3DXVECTOR3(GetPosition().x + fDis, GetPosition().y, GetPosition().z + 150.0f * nCnt);

			break;

			//�O�Ԗڂ��ܔԖ�
		case DEST::DEST_THIRD:
		case DEST::DEST_FIFTH:

			//���̈ʒu�ɐݒ�
			arPos[nCnt] = D3DXVECTOR3(GetPosition().x + 150.0f * nCnt, GetPosition().y, GetPosition().z + fDis);

			break;

			//�Ō�̔ԍ�
		case DEST::DEST_SIXTH:

			//���̈ʒu�ɐݒ�
			arPos[nCnt] = D3DXVECTOR3(GetPosition().x - fDis, GetPosition().y, GetPosition().z + 150.0f * nCnt);

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
	D3DXVECTOR3 rDis = D3DXVECTOR3(Pos.x - DestPos.x, Pos.y - DestPos.y, Pos.z - DestPos.z);

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