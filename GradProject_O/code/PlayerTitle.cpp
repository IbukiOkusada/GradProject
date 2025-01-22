//<=================================================
//�^�C�g���ł̃v���C���\�̓������� [PlayerTitle.cpp]
//
//Author : Kazuki Watanabe
//<=================================================
#include "PlayerTitle.h"
#include "debugproc.h"
#include "objectX.h"
#include "TitleBaggage.h"
#include "character.h"
#include "model.h"
#include "motion.h"
#include "title.h"

//<************************************************
//���O�錾
//<************************************************
namespace
{
	//�ړI�n�̈ʒu(�����ł����g���܂���)
	const D3DXVECTOR3 DEST_POS[] =
	{ 
		D3DXVECTOR3(2630.0f, 0.0f, 1054.0f),
		D3DXVECTOR3(2630.0f, 0.0f, -200.0f),
	};
}

//<================================================
//�R���X�g���N�^
//<================================================
CPlayerTitle::CPlayerTitle()
{
	//������
	m_nNumDest = 0;
	m_fBDustValue = 50.0f;
	m_eState = STATE_NONE;
	m_bReached = false;
	m_bNextMove = false;
	m_bFirst = true;
	m_pTitleBaggage = nullptr;
	m_pTitleGoal = nullptr;
}
//<================================================
//�f�X�g���N�^
//<================================================
CPlayerTitle::~CPlayerTitle()
{

}
//<===============================================
//����������(�I�[�o�[���[�h)
//<===============================================
HRESULT CPlayerTitle::Init(const char* pBodyName, const char* pLegName)
{
	constexpr char* MODEL_NAME = "data\\MODEL\\bike.x";				//���f����
	constexpr char* SoundName = "data\\SE\\idol.wav";				//�T�E���h��
	const D3DXCOLOR YellowCol = D3DXCOLOR(1.0f, 1.0f, 0.3f, 1.0f);	//���F
	CModel* pParts = nullptr;										//���f���̃p�[�c�擾�p�ϐ�

	//�R���e�i��������Ȃ��̂ł��̂悤�Ȍ`�ɂ��܂���
	m_pObj = CObjectX::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), MODEL_NAME);
	m_pObj->SetType(CObject::TYPE_PLAYER);
	m_pObj->SetRotateType(CObjectX::TYPE_QUATERNION);
	SetMatrix();
	SetCol();

	//�o�C�J�[����
	m_pCharacter = CCharacter::Create("data\\TXT\\character\\player\\motion_player.txt");
	m_pCharacter->SetParent(m_pObj->GetMtx());
	m_pCharacter->GetMotion()->InitSet(0);
	m_pCharacter->SetScale(D3DXVECTOR3(3.0f, 3.0f, 3.0f));
	m_pCharacter->SetPosition(D3DXVECTOR3(0.0f, -60.0f, 75.0f));
	m_pCharacter->SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//�o�C�J�[�̐F�ݒ�
	for (int nCntParts = 0; nCntParts < m_pCharacter->GetNumParts(); nCntParts++)
	{
		//���f���̃p�[�c���擾
		pParts = m_pCharacter->GetParts(nCntParts);
		pParts->SetColMulti(YellowCol);
	}

	//�T�E���h����
	m_pSound = CMasterSound::CObjectSound::Create(SoundName, -1);
	m_pSound->SetVolume(1.0f);
	m_pSound->Stop();

	//�ו�����
	m_pTitleBaggage = CTitleBaggage::Create(this->GetPosition());
	m_pTitleBaggage->GetObj()->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));

	//�G�t�F�N�g����
	m_pAfterburner = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\afterburner.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\trail.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 10.0f, false, false);
	m_pBackdust = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\backdust.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);

	return S_OK;
}
//<================================================
//�I������
//<================================================
void CPlayerTitle::Uninit(void)
{
	//�I������
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
	//�f�o�b�O�ȊO��������
	if (m_eState == STATE_NONE)
	{
		// �}�g���b�N�X
		SetMatrix();

		//�I�u�W�F�N�g���̂̒��g�������
		if (m_pObj)
		{
			//�ݒ�
			SetPosition(GetPosition());
			SetRotation(GetRotation());
			m_pObj->SetPosition(GetPosition());
			m_pObj->SetRotation(GetRotation());
			m_pObj->SetShadowHeight(GetPosition().y);

			//�o�C�J�[�̍X�V
			m_pCharacter->Update();
		}
		//���̓����Ɉڍs���Ă��Ȃ�������A�A�C�X���v���C���[�̏�ɏ������
		if (!m_bNextMove) 
		{
			m_pTitleBaggage->GetObj()->SetParent(m_pCharacter->GetParts(5)->GetMtx());
		}

		//�ڍs���Ă����珉���ʒu�ɖ߂�
		else 
		{ 
			constexpr float BDustMaxValue = 300.0f;		//��납��o�鉌�̍ő�l
			m_pTitleBaggage->SetPosition(VECTOR3_ZERO); 
			m_fBDustValue = BDustMaxValue;
		}

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

	correction::Adjust(&m_Info.rot);

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
	constexpr float fSpeed = 0.075f;										//����
	D3DXVECTOR3 rPos = GetPosition();										//�ʒu
	D3DXVECTOR3 rMove = GetMove();

	//���K��Ԃ��Ȃ���
	if (m_bReached) { m_bReached = false; }

	//�f�o�b�O�ȊO��������
	if (m_eState == STATE_NONE)
	{
		//�ړI�n�܂ňړ�
		rPos.x += (DEST_POS[nNum].x - rPos.x) * fSpeed;//X��
		rPos.z += (DEST_POS[nNum].z - rPos.z) * fSpeed;//Z��

		//�ړI�n�ɓ��������画���true�ɂ���
		if (Function::BoolDis(rPos, DEST_POS[nNum], DEST_DIFF)) { m_bReached = true; }
	}

	//�ʒu�ݒ�
	SetPosition(rPos);
}
//<================================================
//�ו��̓�������
//<================================================
void CPlayerTitle::BaggageMove(void)
{
	//���̓����Ɉڍs���Ă��Ȃ�������
	if (!m_bNextMove)
	{
		constexpr float fDis = -200.0f;		//����
		m_bNextMove = true;					//���̍s���Ɉړ����Ă��܂��Ƃ������Ƃ��ؖ�

		//�ʒu��ݒ�
		D3DXVECTOR3 GoalPos = D3DXVECTOR3(this->GetPosition().x + fDis, this->GetPosition().y, this->GetPosition().z);

		//�S�[�����v���C���[�̉��ɐݒ肷��
		m_pTitleGoal = CTitleGoal::Create(GoalPos, D3DXVECTOR3(0.0f, -1.56f, 0.0f));

		//�l�ԂƉו��̐ݒ�
		m_pTitleBaggage->GetObj()->SetParent(m_pTitleGoal->GetCHR()->GetParts(6)->GetMtx());
		m_pTitleGoal->GetCHR()->GetMotion()->BlendSet(4);
	}
}
//<================================================
//�I����ʂ̍ۂ̓���
//<================================================
void CPlayerTitle::MovingSelect(void)
{
	constexpr float fMove = 0.12f;					//�ړ����x�Ɖ�]���x
	const float fRad1 = 500.0f,fRad2 = 1000.0f;		//�͈�

	//�ړI�n�ɓ��������画���true�ɂ���
	if (Function::BoolDis(GetPosition(), DEST_POS_SELECT[m_nNumDest], fRad1))
	{
		//�ړI�n�ύX
		m_nNumDest = (m_nNumDest + 1) % DEST_MAX;
	}
	
	PlayerRotSet();

	//�f�o�b�O�\��
	CDebugProc::GetInstance()->Print("[���݂̔ԍ�]�F%d\n", m_nNumDest);
}
//<================================================
//�v���C���[�̈ʒu�����ݒ�
//<================================================
void CPlayerTitle::PlayerRotSet(void)
{
	//�ʒu���Ȃ�
	constexpr float fMove = 50.0f;									//�ړ����x
	D3DXVECTOR3 rDestRot = VECTOR3_ZERO;							//�ړI����
	D3DXVECTOR3 rRot = GetRotation();								//����
	D3DXVECTOR3 rPos = GetPosition();								//�ʒu

	//�ړI�����ݒ�
	rDestRot = DEST_ROT_SELECT[m_nNumDest];

	//�ԍ��ɂ���ĕύX������
	switch (m_nNumDest)
	{
	case DEST::DEST_FIRST:							rPos.z += fMove; break;			//�ŏ��̔ԍ�
	case DEST::DEST_SECOND:case DEST::DEST_FOUTH:	rPos.x -= fMove; break;			//��Ԗڂ��l�Ԗ�
	case DEST::DEST_THIRD: case DEST::DEST_FIFTH:	rPos.z -= fMove; break;			//�O�Ԗڂ��ܔԖ�
	case DEST::DEST_SIXTH:							rPos.x += fMove; break;			//�Ō�̔ԍ�
	}

	//�ʒu��ݒ�
	SetPosition(rPos);

	//<**********************************************
	//��]�֘A
	float fDiff = rDestRot.y - rRot.y;	//����
	float fIner = 0.3f;							//����

	correction::Adjust(&fDiff);

	rRot.y += fDiff * fIner;

	//���������Ɛݒ�
	SetRotation(rRot);
	//<**********************************************
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