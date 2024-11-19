//==========================================================
//
// �v���C���[�}�l�[�W���[ [car_manager.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "car_manager.h"
#include "car.h"

// �ÓI�����o�ϐ��錾
CCarManager* CCarManager::m_pInstance = nullptr;	// �C���X�^���X

//==========================================================
// �R���X�g���N�^
//==========================================================
CCarManager::CCarManager()
{
	// �l�̃N���A
	m_pList = nullptr;
	m_nNum = 0;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CCarManager::~CCarManager()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CCarManager::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CCarManager::Uninit(void)
{
	// �e�N���X�̏I������
	CListManager::Uninit();

	// ���X�g�p��
	ListRelease();

	// �C���X�^���X�̔p��
	if (m_pInstance != nullptr) {	// �C���X�^���X���m�ۂ���Ă���
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// �X�V����
//==========================================================
void CCarManager::Update(void)
{

}

//==========================================================
// �C���X�^���X���m��
//==========================================================
CCarManager* CCarManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// �g���Ă��Ȃ�
		m_pInstance = DEBUG_NEW CCarManager;
	}

	return m_pInstance;
}

//==========================================================
// �C���X�^���X�������[�X
//==========================================================
void CCarManager::Release(void)
{
	if (m_pInstance != nullptr) {	// �C���X�^���X���m�ۂ���Ă���
		m_pInstance->Uninit();
	}
}

//==========================================================
// ���X�g�ɑ}��
//==========================================================
void CCarManager::ListIn(CCar* pCar)
{
	GetList()->Regist(pCar);
}

//==========================================================
// ���X�g����O��
//==========================================================
void CCarManager::ListOut(CCar* pCar)
{
	// ���X�g���玩�����g���폜����
	GetList()->Delete(pCar);
}

//==========================================================
// �U���q�b�g�m�F
//==========================================================
bool CCarManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	bool bUse = false;

	//�ʔ���
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		CCar* pCar = GetList()->Get(i);
		//if (pCar->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
	}

	return bUse;
}