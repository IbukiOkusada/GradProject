//<=================================================
//�^�C�g���ł̌x�@�̓������� [PlayerTitle.h]
//
//Author : Kazuki Watanabe
//<=================================================
#include "PoliceTitle.h"
#include "debugproc.h"
#include "manager.h"
#include "title.h"
#include "car_manager.h"

//<======================================
//�R���X�g���N�^
//<======================================
CPoliceTitle::CPoliceTitle()
{
	m_pPatrolLamp = nullptr;
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
	//�ʒu����̃I�u�W�F�N�g����
	m_pObj = CObjectX::Create(pos, VECTOR3_ZERO, "data\\MODEL\\car003.x");

	return S_OK;
}
//<======================================
//�I������
//<======================================
void CPoliceTitle::Uninit(void)
{
	//Realese�������ƃ������j�󂪋N����
	SAFE_DELETE(m_pPatrolLamp);
	Release();

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
void CPoliceTitle::Chasing(void)
{
	D3DXVECTOR3 PolicePos = this->GetPosition();		//�x�@�̈ʒu�擾
	const float PoliceMoveZ = 25.0f;					//�x�@��Z�l�̈ړ��l

	//<*******************************************
	//�p�g�����v����
	if (!m_pPatrolLamp)
	{
		m_pPatrolLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\patrollamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	}

	//�ʒu�ƌ�����ݒ�
	m_pPatrolLamp->m_pos = this->GetPosition();
	m_pPatrolLamp->m_rot = this->GetRotation();
	//
	//<*******************************************

	///�x�@�̈ʒu���ړ������A�ʒu��ݒ肷��
	PolicePos.z += PoliceMoveZ;
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