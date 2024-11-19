//==========================================================
//
// �S�[���Ǘ��̏��� [goal_manager.cpp]
// Author : Ryosuke Oohara
//
//==========================================================
#include "goal_manager.h"
#include "goal.h"
#include "camera.h"
#include "camera_action.h"
#include "camera_manager.h"
#include "player.h"
#include "player_manager.h"

//==========================================================
// �ÓI�����o�ϐ�
//==========================================================
CGoalManager *CGoalManager::m_pInstance = nullptr;

namespace
{
	const D3DXVECTOR3 POS[2] =
	{
		D3DXVECTOR3(-8600.0f, 0.0f, -10600.0f),
		D3DXVECTOR3(0.0f, 0.0f, -4000.0f),
	};
}

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
	m_pGoal = CGole::Create(D3DXVECTOR3(10000.0f, 0.0f, 12500.0f), 600.0f, 20.0f);

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
	CCamera* pCamera = CCameraManager::GetInstance()->GetTop();

	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	int nNum = pPlayer->GetNumDeliverStatus();

	if (m_pGoal == nullptr) { return; }

	if (m_pGoal->GetEnd() && pCamera->GetAction()->IsFinish())
	{
		m_pGoal->Uninit();
		m_pGoal = nullptr;

		if (nNum >= 3) { return; }

		m_pGoal = CGole::Create(POS[nNum - 1], 600.0f, 20.0f);
	}
}