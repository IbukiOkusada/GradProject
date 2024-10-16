//==========================================================
//
// �T���v���}�l�[�W���[ [sample_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "manager_registry.h"
#include "list_manager.h"

// �ÓI�����o�ϐ��錾
CManagerRegistry* CManagerRegistry::m_pInstance = nullptr;	// �C���X�^���X

//==========================================================
// �R���X�g���N�^
//==========================================================
CManagerRegistry::CManagerRegistry()
{
	// �l�̃N���A
	m_pCur = nullptr;
	m_pTop = nullptr;
	m_nNum = 0;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CManagerRegistry::~CManagerRegistry()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CManagerRegistry::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CManagerRegistry::Uninit(void)
{
	CListManager* pManager = m_pTop;

	while (pManager != nullptr)
	{
		CListManager* pNext = pManager->GetNext();
		pManager->Uninit();
		pManager = pNext;
	}
}

//==========================================================
// �X�V����
//==========================================================
void CManagerRegistry::Update(void)
{

}

//==========================================================
// �C���X�^���X���m��
//==========================================================
CManagerRegistry* CManagerRegistry::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// �g���Ă��Ȃ�
		m_pInstance = DEBUG_NEW CManagerRegistry;
	}

	return m_pInstance;
}

//==========================================================
// �C���X�^���X�������[�X
//==========================================================
void CManagerRegistry::Release(void)
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
void CManagerRegistry::ListIn(CListManager* pListManager)
{
	if (m_pTop != nullptr)
	{// �擪�����݂��Ă���ꍇ
		m_pCur->SetNext(pListManager);	// ���ݍŌ���̃I�u�W�F�N�g�̃|�C���^�ɂȂ���
		pListManager->SetPrev(m_pCur);
		m_pCur = pListManager;	// �������g���Ō���ɂȂ�
	}
	else
	{// ���݂��Ȃ��ꍇ
		m_pTop = pListManager;	// �������g���擪�ɂȂ�
		m_pCur = pListManager;	// �������g���Ō���ɂȂ�
	}

	m_nNum++;
}

//==========================================================
// ���X�g����O��
//==========================================================
void CManagerRegistry::ListOut(CListManager* pListManager)
{
	// ���X�g���玩�����g���폜����
	if (m_pTop == pListManager)
	{// ���g���擪
		if (pListManager->GetNext() != nullptr)
		{// �������݂��Ă���
			m_pTop = pListManager->GetNext();	// ����擪�ɂ���
			m_pTop->SetPrev(nullptr);	// ���̑O�̃|�C���^���o���Ă��Ȃ��悤�ɂ���
		}
		else
		{// ���݂��Ă��Ȃ�
			m_pTop = nullptr;	// �擪���Ȃ���Ԃɂ���
			m_pCur = nullptr;	// �Ō�����Ȃ���Ԃɂ���
		}
	}
	else if (m_pCur == pListManager)
	{// ���g���Ō��
		if (pListManager->GetPrev() != nullptr)
		{// �������݂��Ă���
			m_pCur = pListManager->GetPrev();		// �O���Ō���ɂ���
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
		if (pListManager->GetNext() != nullptr)
		{
			pListManager->GetNext()->SetPrev(pListManager->GetPrev());	// ���g�̎��ɑO�̃|�C���^���o��������
		}
		if (pListManager->GetPrev() != nullptr)
		{
			pListManager->GetPrev()->SetNext(pListManager->GetNext());	// ���g�̑O�Ɏ��̃|�C���^���o��������
		}
	}

	m_nNum--;
}
