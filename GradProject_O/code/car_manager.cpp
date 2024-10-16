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
	m_pCur = nullptr;
	m_pTop = nullptr;
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
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// ���X�g�ɑ}��
//==========================================================
void CCarManager::ListIn(CCar* pCar)
{
	if (m_pTop != nullptr)
	{// �擪�����݂��Ă���ꍇ
		m_pCur->SetNext(pCar);	// ���ݍŌ���̃I�u�W�F�N�g�̃|�C���^�ɂȂ���
		pCar->SetPrev(m_pCur);
		m_pCur = pCar;	// �������g���Ō���ɂȂ�
	}
	else
	{// ���݂��Ȃ��ꍇ
		m_pTop = pCar;	// �������g���擪�ɂȂ�
		m_pCur = pCar;	// �������g���Ō���ɂȂ�
	}

	m_nNum++;
}

//==========================================================
// ���X�g����O��
//==========================================================
void CCarManager::ListOut(CCar* pCar)
{
	// ���X�g���玩�����g���폜����
	if (m_pTop == pCar)
	{// ���g���擪
		if (pCar->GetNext() != nullptr)
		{// �������݂��Ă���
			m_pTop = pCar->GetNext();	// ����擪�ɂ���
			m_pTop->SetPrev(nullptr);	// ���̑O�̃|�C���^���o���Ă��Ȃ��悤�ɂ���
		}
		else
		{// ���݂��Ă��Ȃ�
			m_pTop = nullptr;	// �擪���Ȃ���Ԃɂ���
			m_pCur = nullptr;	// �Ō�����Ȃ���Ԃɂ���
		}
	}
	else if (m_pCur == pCar)
	{// ���g���Ō��
		if (pCar->GetPrev() != nullptr)
		{// �������݂��Ă���
			m_pCur = pCar->GetPrev();		// �O���Ō���ɂ���
			m_pCur->SetNext(nullptr);			// �O�̎��̃|�C���^���o���Ă��Ȃ��悤�ɂ���
		}
		else
		{// ���݂��Ă��Ȃ�
			m_pTop = nullptr;	// �擪���Ȃ���Ԃɂ���
			m_pCur = nullptr;	// �Ō�����Ȃ���Ԃɂ���
		}
	}
	else
	{
		if (pCar->GetNext() != nullptr)
		{
			pCar->GetNext()->SetPrev(pCar->GetPrev());	// ���g�̎��ɑO�̃|�C���^���o��������
		}
		if (pCar->GetPrev() != nullptr)
		{
			pCar->GetPrev()->SetNext(pCar->GetNext());	// ���g�̑O�Ɏ��̃|�C���^���o��������
		}
	}

	m_nNum--;
}

//==========================================================
// �U���q�b�g�m�F
//==========================================================
bool CCarManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	CCar* pCar = m_pTop;
	bool bUse = false;

	//�ʔ���
	while (pCar != nullptr) {
		CCar* pCarNext = pCar->GetNext();
		//if (pCar->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
		pCar = pCarNext;
	}

	return bUse;
}