//<=================================================
//�^�C�g���ł̌x�@�̓������� [PlayerTitle.h]
//
//Author : Kazuki Watanabe
//<=================================================
#include "PoliceTitle.h"
#include "police_manager.h"
#include "debugproc.h"
#include "manager.h"
#include "title.h"
#include "car_manager.h"

//�T�E���h�Đ�
CMasterSound::CObjectSound* CPoliceTitle::m_pSound = nullptr;
// 
//<======================================
//�R���X�g���N�^
//<======================================
CPoliceTitle::CPoliceTitle() : CPolice(-1)
{
	//������
	m_pPatrolLamp = nullptr;
	m_pTailLamp = nullptr;
}

//<======================================
//�f�X�g���N�^
//<======================================
CPoliceTitle::~CPoliceTitle()
{

}
//<======================================
//����������(�ʒu��񂠂�)
//<======================================
HRESULT CPoliceTitle::Init(const D3DXVECTOR3 pos)
{
	constexpr char* MODEL_NAME = "data\\MODEL\\police.x";	//���f����
	constexpr char* SoundName = "data\\SE\\siren.wav";		//�T�E���h��

	//�ʒu����̃I�u�W�F�N�g����
	m_pObj = CObjectX::Create(pos, VECTOR3_ZERO, MODEL_NAME);

	//������
	if (!m_pSound)
	{
		//�����J�n
		m_pSound = CMasterSound::CObjectSound::Create(SoundName, -1);
		m_pSound->SetVolume(0.2f);
		m_pSound->Stop();
	}

	return S_OK;
}
//<======================================
//�I������
//<======================================
void CPoliceTitle::Uninit(void)
{
	CPolice::Uninit();
	SAFE_UNINIT_DELETE(m_pSound);

	SAFE_DELETE(m_pPatrolLamp);
	SAFE_DELETE(m_pTailLamp);
}
//<======================================
//�X�V����
//<======================================
void CPoliceTitle::Update(void)
{
	//�B�̃f�o�b�O�p
	CDebugProc::GetInstance()->Print("���W: [ %f, %f, %f ]",
		this->GetPosition().x, this->GetPosition().y, this->GetPosition().z);
}
//<======================================
//�ǂ������Ă��鏈��
//<======================================
void CPoliceTitle::Chasing(const float fMoveZ)
{
	D3DXVECTOR3 PolicePos = this->GetPosition();		//�x�@�̈ʒu�擾

	//<*******************************************
	//�p�g�����v����
	if (!m_pPatrolLamp)
	{
		m_pPatrolLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\patrollamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	}
	//�Ă�����Ր���
	if (!m_pTailLamp)
	{
		m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\taillamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	}
	//�ʒu�ƌ�����ݒ�
	m_pPatrolLamp->m_pos = this->GetPosition();
	m_pPatrolLamp->m_rot = this->GetRotation();

	//�ʒu�ƌ�����ݒ�
	m_pTailLamp->m_pos = this->GetPosition();
	m_pTailLamp->m_rot = this->GetRotation();
	//
	//<*******************************************

	///�x�@�̈ʒu���ړ������A�ʒu��ݒ肷��
	PolicePos.z += fMoveZ;
	SetPosition(PolicePos);
}
//<======================================
//��������
//<======================================
CPoliceTitle* CPoliceTitle::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move)
{
	//�����p�̃I�u�W�F�N�g
	CPoliceTitle* pPoliceTitle = DEBUG_NEW CPoliceTitle;

	//�m�ۂł��Ă�����
	if (pPoliceTitle != nullptr)
	{
		//����������
		pPoliceTitle->Init(pos);

		//���W�ݒ�
		pPoliceTitle->SetPosition(pos);

		//�����ݒ�
		pPoliceTitle->SetRotation(rot);

		//�ړ��ʐݒ�
		pPoliceTitle->SetMove(move);
	}
	//�m�ۂł��Ă��Ȃ�������
	else { return nullptr; }

	return pPoliceTitle;
}