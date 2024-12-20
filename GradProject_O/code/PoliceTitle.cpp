//<=================================================
//�^�C�g���ł̌x�@�̓������� [PlayerTitle.h]
//
//Author : Kazuki Watanabe
//<=================================================
#include "PoliceTitle.h"
#include "debugproc.h"
#include "title.h"
#include "PlayerTitle.h"

//�T�E���h�Đ�
CMasterSound::CObjectSound* CPoliceTitle::m_pSound = nullptr;

//����
int CPoliceTitle::m_nNumAll = 0;
 
//<======================================
//�R���X�g���N�^
//<======================================
CPoliceTitle::CPoliceTitle() : CPolice(-1)
{
	//������
	m_pPatrolLamp = nullptr;
	m_pTailLamp = nullptr;
	m_nNumDest = 0;
	m_nNumAll++;
}

//<======================================
//�f�X�g���N�^
//<======================================
CPoliceTitle::~CPoliceTitle()
{
	m_nNumAll--;
}
//<======================================
//����������(�ʒu��񂠂�)
//<======================================
HRESULT CPoliceTitle::Init(void)
{
	constexpr char* MODEL_NAME = "data\\MODEL\\police.x";	//���f����
	constexpr char* SoundName = "data\\SE\\siren.wav";		//�T�E���h��
	constexpr float fDis = 1000.0f;							//�v���C���[����̋���

	//�ʒu�ݒ�
	D3DXVECTOR3 rPos = D3DXVECTOR3(
		CTitle::GetPlayer()->GetPosition().x,
		CTitle::GetPlayer()->GetPosition().y,
		CTitle::GetPlayer()->GetPosition().z - fDis * m_nNumAll);

	//������
	if (!m_pSound)
	{
		//�����J�n
		m_pSound = CMasterSound::CObjectSound::Create(SoundName, -1);
		m_pSound->SetVolume(0.2f);
		m_pSound->Stop();
	}

	//�ʒu����̃I�u�W�F�N�g����
	m_pObj = CObjectX::Create(rPos, VECTOR3_ZERO, MODEL_NAME);

	//�ʒu�ݒ�
	SetPosition(rPos);

	return S_OK;
}
//<======================================
//�I������
//<======================================
void CPoliceTitle::Uninit(void)
{
	//�I������
	CPolice::Uninit();
	SAFE_UNINIT_DELETE(m_pSound);
	SAFE_DELETE(m_pPatrolLamp);
	SAFE_DELETE(m_pTailLamp);
}
//<======================================
//�ǂ������Ă��鏈��
//<======================================
void CPoliceTitle::Chasing(const float fMoveZ)
{
	D3DXVECTOR3 PolicePos = this->GetPosition();		//�x�@�̈ʒu�擾

	//�G�t�F�N�g����
	SettingPatLamp();

	///�x�@�̈ʒu���ړ������A�ʒu��ݒ肷��
	PolicePos.z += fMoveZ;
	SetPosition(PolicePos);
}
//<======================================
//�^�C�g���̓�������
//<======================================
void CPoliceTitle::TitleMove(void)
{
	//�͈�
	const float fRad1 = 500.0f;

	//�G�t�F�N�g����
	SettingPatLamp();

	//�ړI�n�ɓ��������画���true�ɂ���
	if (Function::BoolDis(GetPosition(), DEST_POS_SELECT[m_nNumDest], fRad1))
	{
		//�ړI�n�ύX
		m_nNumDest = (m_nNumDest + 1) % DEST_MAX;
	}
	Move();
}
//<======================================
//��������
//<======================================
void CPoliceTitle::Move(void)
{
	D3DXVECTOR3 rDestRot = VECTOR3_ZERO;		//�ړI����
	D3DXVECTOR3 rRot = GetRotation();			//����
	D3DXVECTOR3 rPos = GetPosition();			//�ʒu
	constexpr float fMove = 50.0f;				//�ړ��l

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
	//<****************************************
	//������]�֘A
	float fDiff = rDestRot.y - rRot.y;	//����
	float fIner = 0.3f;					//�����̓���

	//����
	Adjust(fDiff);

	//��]������
	rRot.y += fDiff * fIner;

	//���������Ɛݒ�
	SetRotation(rRot);
	//<****************************************

}
//<======================================
//��������
//<======================================
CPoliceTitle* CPoliceTitle::Create(const D3DXVECTOR3 rot, const D3DXVECTOR3 move)
{
	//�����p�̃I�u�W�F�N�g
	CPoliceTitle* pPoliceTitle = DEBUG_NEW CPoliceTitle;

	//�m�ۂł��Ă�����
	if (pPoliceTitle)
	{
		//����������
		pPoliceTitle->Init();

		//�ʒu�ݒ�
		pPoliceTitle->SetPosition(pPoliceTitle->GetPosition());

		//�����ݒ�
		pPoliceTitle->SetRotation(rot);

		//�ړ��ʐݒ�
		pPoliceTitle->SetMove(move);
	}
	//�m�ۂł��Ă��Ȃ�������
	else { return nullptr; }

	return pPoliceTitle;
}