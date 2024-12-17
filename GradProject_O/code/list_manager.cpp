//==========================================================
//
// �}�l�[�W���[�Ǘ�(���X�g�Ǘ�) [list_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "list_manager.h"

// �ÓI�����o�ϐ��錾
Clist<CListManager*>* CListManager::m_pList = nullptr;

//==========================================================
// �R���X�g���N�^
//==========================================================
CListManager::CListManager()
{
	if (m_pList == nullptr)
	{
		m_pList = m_pList->Create();
	}

	// �l�̃N���A
	m_pList->Regist(this);
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CListManager::~CListManager()
{
	
}

//==========================================================
// �I������
//==========================================================
void CListManager::Uninit(void)
{
	m_pList->Delete(this);

	if (m_pList->Empty())
	{
		delete m_pList;
		m_pList = nullptr;
	}
}

//==========================================================
// �J��
//==========================================================
void CListManager::Release()
{
	if (m_pList == nullptr) { return; }

	while (m_pList->GetNum() != 0)
	{
		std::vector<CListManager*> list;

		for (int i = 0; i < m_pList->GetNum(); i++)
		{
			list.push_back(m_pList->Get(i));
		}

		for (auto& it : list)
		{
			it->Uninit();
		}

		if (m_pList == nullptr)
		{
			break;
		}
	}
}