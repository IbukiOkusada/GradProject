//==========================================================
//
// �^�X�N�Ǘ��̏��� [task_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "task_manager.h"
#include "task.h"

CTaskManager *CTaskManager::m_pInstance = nullptr;

//==========================================================
// �R���X�g���N�^
//==========================================================
CTaskManager::CTaskManager()
{
	m_pTop = nullptr;
	m_pCur = nullptr;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CTaskManager::~CTaskManager()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CTaskManager::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CTaskManager::Uninit(void)
{
	// �e�N���X�̏I������
	CListManager::Uninit();

	// ���X�g�̑S�p��
	ReleaseAll();

	// �C���X�^���X�̔p��
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// �X�V����
//==========================================================
void CTaskManager::Update(void)
{
	// ���X�g�̑S�X�V
	UpdateAll();
}

//===============================================
// �S�Ẵ^�X�N�̔p��
//===============================================
void CTaskManager::ReleaseAll(void)
{
	CTask *pTask = m_pTop;	// �擪���擾

	while (pTask != nullptr)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		CTask *pTaskNext = pTask->GetNext();	// ���̃I�u�W�F�N�g�ւ̃|�C���^���擾

		if (!pTask->GetDeath())
		{
			// �I������
			pTask->Uninit();
		}

		pTask = pTaskNext;	// ���̃I�u�W�F�N�g�Ɉړ�
	}

	// ���S�t���O���`�F�b�N
	DeathCheck();
}

//===============================================
// �S�Ẵ^�X�N�̍X�V
//===============================================
void CTaskManager::UpdateAll(void)
{
	CTask *pTask = m_pTop;	// �擪���擾

	while (pTask != nullptr)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		CTask *pTaskNext = pTask->GetNext();	// ���̃I�u�W�F�N�g�ւ̃|�C���^���擾

		if (!pTask->GetDeath())
		{
			// �X�V����
			pTask->Update();
		}

		pTask = pTaskNext;	// ���̃I�u�W�F�N�g�Ɉړ�
	}

	// ���S�t���O���`�F�b�N
	DeathCheck();
}

//===============================================
// ���S�t���O���`�F�b�N
//===============================================
void CTaskManager::DeathCheck(void)
{
	CTask *pTask = m_pTop;	// �擪���擾

	while (pTask != nullptr)
	{// �g�p����Ă��Ȃ���Ԃ܂�
		CTask *pTaskNext = pTask->GetNext();	// ���̃I�u�W�F�N�g�ւ̃|�C���^���擾

		if (pTask->GetDeath() == true)
		{
			// ���X�g���玩�����g���폜����
			if (m_pTop == pTask)
			{// ���g���擪
				if (pTask->GetNext() != nullptr)
				{// �������݂��Ă���
					m_pTop = pTask->GetNext();	// ����擪�ɂ���
					m_pTop->SetPrev(nullptr);	// ���̑O�̃|�C���^���o���Ă��Ȃ��悤�ɂ���
				}
				else
				{// ���݂��Ă��Ȃ�
					m_pTop = nullptr;	// �擪���Ȃ���Ԃɂ���
					m_pCur = nullptr;	// �Ō�����Ȃ���Ԃɂ���
				}
			}
			else if (m_pCur == pTask)
			{// ���g���Ō��
				if (pTask->GetPrev() != nullptr)
				{// �������݂��Ă���
					m_pCur = pTask->GetPrev();		// �O���Ō���ɂ���
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
				if (pTask->GetNext() != nullptr)
				{
					pTask->GetNext()->SetPrev(pTask->GetPrev());	// ���g�̎��ɑO�̃|�C���^���o��������
				}
				if (pTask->GetPrev() != nullptr)
				{
					pTask->GetPrev()->SetNext(pTask->GetNext());	// ���g�̑O�Ɏ��̃|�C���^���o��������
				}
			}

			delete pTask;	// �������̊J��
			pTask = nullptr;
		}

		pTask = pTaskNext;	// ���̃I�u�W�F�N�g�Ɉړ�
	}
}

//===============================================
// ���X�g�ɑ}������
//===============================================
void CTaskManager::ListIn(CTask *pTask)
{
	if (m_pTop != nullptr)
	{// �擪�����݂��Ă���ꍇ
		m_pCur->SetNext(pTask);	// ���ݍŌ���̃I�u�W�F�N�g�̃|�C���^�ɂȂ���
		pTask->SetPrev(m_pCur);
		m_pCur = pTask;	// �������g���Ō���ɂȂ�
	}
	else
	{// ���݂��Ȃ��ꍇ
		m_pTop = pTask;	// �������g���擪�ɂȂ�
		m_pCur = pTask;	// �������g���Ō���ɂȂ�
	}
}

//===============================================
// �C���X�^���X�̎擾
//===============================================
CTaskManager* CTaskManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = DEBUG_NEW CTaskManager;
	}

	return m_pInstance;
}

//===============================================
// �C���X�^���X�̔p��
//===============================================
void CTaskManager::Release(void)
{
	if (m_pInstance != nullptr)
	{
		m_pInstance->Uninit();
	}
}