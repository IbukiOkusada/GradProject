//==========================================================
//
// �x�@AI�}�l�[�W���[ [police_AI_manager.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "police_AI_manager.h"
#include "police.h"
#include "police_AI.h"
#include "player_manager.h"
#include "deltatime.h"
#include "debugproc.h"
#include "car_manager.h"

// ���O���
namespace
{
	const float INTERVAL = 5.0f;	// �C���^�[�o��
	const int MAX_POLICE = (15);	// �x�@�̍ő�l
}

// �ÓI�����o�ϐ��錾
CPoliceAIManager* CPoliceAIManager::m_pInstance = nullptr;	// �C���X�^���X

//==========================================================
// �R���X�g���N�^
//==========================================================
CPoliceAIManager::CPoliceAIManager()
{
	// �l�̃N���A
	m_maplist.Clear();
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CPoliceAIManager::~CPoliceAIManager()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CPoliceAIManager::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CPoliceAIManager::Uninit(void)
{
	// �e�N���X�̏I������
	CListManager::Uninit();

	// �C���X�^���X�̔p��
	if (m_pInstance != nullptr)
	{	// �C���X�^���X���m�ۂ���Ă���
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// �X�V����
//==========================================================
void CPoliceAIManager::Update(void)
{
	// �}���`�X���b�h
	std::thread th(&CPoliceAIManager::Search, this);
	th.detach();
}

//==========================================================
// �C���X�^���X���m��
//==========================================================
CPoliceAIManager* CPoliceAIManager::GetInstance(void)
{
	if (m_pInstance == nullptr)
	{	// �g���Ă��Ȃ�
		m_pInstance = DEBUG_NEW CPoliceAIManager;
		m_pInstance->Init();
	}

	return m_pInstance;
}

//==========================================================
// �C���X�^���X�������[�X
//==========================================================
void CPoliceAIManager::Release(void)
{
	if (m_pInstance != nullptr)
	{	// �C���X�^���X���m�ۂ���Ă���
		m_pInstance->Uninit();
	}
}

//==========================================================
// ���X�g�ɑ}��
//==========================================================
void CPoliceAIManager::ListIn(CPoliceAI* pPoliceAI)
{
	m_maplist.Regist(pPoliceAI->GetPolice()->GetId(), pPoliceAI);
}

//==========================================================
// ���X�g����O��
//==========================================================
void CPoliceAIManager::ListOut(CPoliceAI* pPoliceAI)
{
	// ���X�g���玩�����g���폜����
	m_maplist.Delete(pPoliceAI->GetPolice()->GetId(), pPoliceAI);
}

//==========================================================
// AI�̌o�H�T������
//==========================================================
void  CPoliceAIManager::Search(void)
{
	for (const auto& pair : *m_maplist.GetList())
	{
		// �v���C���[�{��
		pair.second->Search();

		// �v���C���[�ւ̌o�H�T��
		pair.second->ChaseAStar();
	}
}