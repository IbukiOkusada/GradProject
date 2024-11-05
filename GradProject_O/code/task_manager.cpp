//==========================================================
//
// �^�X�N�Ǘ��̏��� [task_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "task_manager.h"
#include "edit_manager.h"
#include "task.h"

CTaskManager *CTaskManager::m_pInstance = nullptr;

//==========================================================
// �R���X�g���N�^
//==========================================================
CTaskManager::CTaskManager()
{
	m_plist = nullptr;
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

	ListRelease();

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
	for (int i = 0; i < GetList()->GetNum(); i++)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		CTask* pTask = GetList()->Get(i);	// �擪���擾

		if (!pTask->GetDeath())
		{
			// �I������
			pTask->Uninit();
		}
	}

	// ���S�t���O���`�F�b�N
	DeathCheck();
}

//===============================================
// �S�Ẵ^�X�N�̍X�V
//===============================================
void CTaskManager::UpdateAll(void)
{
	CEditManager* pEdit = CEditManager::GetInstance();

	for (int i = 0; i < GetList()->GetNum(); i++)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		CTask* pTask = GetList()->Get(i);	// �擪���擾

		// �G�f�B�b�g��
		if (pEdit != nullptr && pTask->GetType() != CTask::TYPE_MODEL) { 
			continue;
		}

		if (!pTask->GetDeath())
		{
			// �X�V����
			pTask->Update();
		}
	}

	// ���S�t���O���`�F�b�N
	DeathCheck();
}

//===============================================
// ���S�t���O���`�F�b�N
//===============================================
void CTaskManager::DeathCheck(void)
{
	for (int i = 0; i < GetList()->GetNum(); i++)
	{// �g�p����Ă��Ȃ���Ԃ܂�
		CTask* pTask = GetList()->Get(i);	// �擪���擾

		if (pTask->GetDeath())
		{
			i--;
			GetList()->Delete(pTask);
			delete pTask;	// �������̊J��
			pTask = nullptr;

			if (GetList()->Empty())
			{
				ListRelease();
			}
		}
	}
}

//===============================================
// ���X�g�ɑ}������
//===============================================
void CTaskManager::ListIn(CTask *pTask)
{
	//���g�����X�g�ɓo�^
	GetList()->Regist(pTask);
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