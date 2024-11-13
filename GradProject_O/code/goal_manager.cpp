//==========================================================
//
// �S�[���Ǘ��̏��� [goal_manager.cpp]
// Author : Ryosuke Oohara
//
//==========================================================
#include "goal_manager.h"
#include "goal.h"

//==========================================================
// �ÓI�����o�ϐ�
//==========================================================
CGoalManager *CGoalManager::m_pInstance = nullptr;

//==========================================================
// �R���X�g���N�^
//==========================================================
CGoalManager::CGoalManager()
{
	m_pGoal = nullptr;

	m_pInstance = this;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CGoalManager::~CGoalManager()
{

}

//==========================================================
// ��������
//==========================================================
CGoalManager* CGoalManager::Create(void)
{
	CGoalManager* pGoalManager = DEBUG_NEW CGoalManager;

	if (pGoalManager != nullptr)
	{
		pGoalManager->Init();
	}

	return pGoalManager;
}

//==========================================================
// ����������
//==========================================================
HRESULT CGoalManager::Init(void)
{
	//m_pGoal = CGole::Create(D3DXVECTOR3(10000.0f, 0.0f, 12500.0f), 600.0f, 20.0f);

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CGoalManager::Uninit(void)
{
	// �S�[���̔j��
	if (m_pGoal != nullptr)
	{
		m_pGoal->Uninit();
		m_pGoal = nullptr;
	}

	// ���g�̔j��
	if (m_pInstance != nullptr)
	{
		m_pInstance = nullptr;
	}
}

//==========================================================
// �X�V����
//==========================================================
void CGoalManager::Update(void)
{

}