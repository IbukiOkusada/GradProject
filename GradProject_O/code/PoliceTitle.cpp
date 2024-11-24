//<=================================================
//�^�C�g���ł̌x�@�̓������� [PlayerTitle.h]
//
//Author : Kazuki Watanabe
//<=================================================
#include "PoliceTitle.h"
#include "debugproc.h"

//<======================================
//�R���X�g���N�^
//<======================================
CPoliceTitle::CPoliceTitle()
{
	
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
	//�ʒu����ݒ�
	//CPolice::Init(pos);

	return S_OK;
}
//<======================================
//�I������
//<======================================
void CPoliceTitle::Uninit(void)
{
	CPolice::Uninit();
}
//<======================================
//�X�V����
//<======================================
void CPoliceTitle::Update(void)
{
	CDebugProc::GetInstance()->Print("���W: [ %f, %f, %f ]", 
		this->GetPosition().x, this->GetPosition().y, this->GetPosition().z);

	CCar::Set();
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