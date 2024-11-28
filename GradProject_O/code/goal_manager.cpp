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
#include "debugproc.h"

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
	m_pGoalOld = nullptr;
	m_pInstance = this;
	m_InfoList.clear();
	m_List.clear();
	m_nOldIdx = 0;
	m_nNumCreate = 0;
	m_nNextIdx = 0;
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
	if (m_pInstance == nullptr)
	{
		m_pInstance = DEBUG_NEW CGoalManager;
		m_pInstance->Init();
	}

	return m_pInstance;
}

//==========================================================
// ���
//==========================================================
void CGoalManager::Release()
{
	if (m_pInstance != nullptr)
	{
		m_pInstance->Uninit();
		m_pInstance = nullptr;
	}
}

//==========================================================
// ���X�g�Ǘ��ɓ����
//==========================================================
bool CGoalManager::ListIn(CGoal* pGoal)
{
	if (pGoal == nullptr) { return false; }

	auto it = m_List.find(pGoal->GetId());

	// ���݂��Ȃ�
	if (it == m_List.end())
	{
		m_List[pGoal->GetId()] = pGoal;
		return true;
	}

	return false;
}

//==========================================================
// ���X�g�Ǘ��Ɋ܂߂�
//==========================================================
bool CGoalManager::ListOut(CGoal* pGoal)
{
	if (pGoal == nullptr) { return false; }

	auto it = m_List.find(pGoal->GetId());

	// ��������
	if (it != m_List.end())
	{
		if (it->second == pGoal)
		{
			m_List.erase(pGoal->GetId());
			return true;
		}
	}

	return false;
}

//==========================================================
// �S�[�����擾
//==========================================================
CGoal* CGoalManager::GetGoal(int nIdx)
{
	auto it = m_List.find(nIdx);

	// ��������
	if (it != m_List.end())
	{
		return it->second;
	}

	return nullptr;
}

//==========================================================
// ����������
//==========================================================
HRESULT CGoalManager::Init(void)
{
	if (m_InfoList.size() <= 0) { return E_FAIL; }

	GoalCreate(m_nOldIdx);

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

	m_InfoList.clear();

	// ���g�̔j��
	if (m_pInstance != nullptr)
	{
		m_pInstance = nullptr;
	}

	delete this;
}

//==========================================================
// �X�V����
//==========================================================
void CGoalManager::Update(void)
{
	CCamera* pCamera = CCameraManager::GetInstance()->GetTop();

	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
	int nNum = pPlayer->GetNumDeliverStatus();

	CDebugProc::GetInstance()->Print("�S�[���̐����� [ %d ] : ���݂̔z�u�� [ %d ]\n",m_nNumCreate, m_List.size());

	if (m_pGoal == nullptr) { return; }
}

//==========================================================
// �S�[���̐���
//==========================================================
void CGoalManager::GoalCreate(int nId)
{
	// �O�񂪑��݂���
	if (m_pGoalOld != nullptr)
	{
		m_pGoalOld->Uninit();
		m_pGoalOld = nullptr;
	}

	// �O��o���Ȃ���
	m_pGoalOld = m_pGoal;

	// �V��������
	auto it = m_InfoList[nId];
	m_pGoal = CGoal::Create(it.pos, it.fRange, it.fLimit, m_nNumCreate);
	m_nOldIdx = nId;
	m_nNumCreate++;

	// ���̔ԍ���ݒ�
	while(1)
	{
		m_nNextIdx = rand() % m_InfoList.size();

		if (m_InfoList.size() <= 1 || m_nNextIdx != m_nOldIdx)
		{
			break;
		}
	}
}