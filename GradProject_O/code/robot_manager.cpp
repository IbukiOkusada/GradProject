//==========================================================
//
// ���{�b�g�}�l�[�W���[ [robot_manager.cpp]
// Author : Kenta Hashimoto
//
//==========================================================
#include "robot_manager.h"
#include "robot.h"

// �ÓI�����o�ϐ��錾
CRobotManager* CRobotManager::m_pInstance = nullptr;	// �C���X�^���X

//==========================================================
// �R���X�g���N�^
//==========================================================
CRobotManager::CRobotManager()
{
	// �l�̃N���A
	m_pCur = nullptr;
	m_pTop = nullptr;
	m_nNum = 0;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CRobotManager::~CRobotManager()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CRobotManager::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CRobotManager::Uninit(void)
{
	// �e�N���X�I��
	CListManager::Uninit();

	// �C���X�^���X�p��
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// �X�V����
//==========================================================
void CRobotManager::Update(void)
{

}

//==========================================================
// �C���X�^���X���m��
//==========================================================
CRobotManager* CRobotManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// �g���Ă��Ȃ�
		m_pInstance = DEBUG_NEW CRobotManager;
	}

	return m_pInstance;
}

//==========================================================
// �C���X�^���X�������[�X
//==========================================================
void CRobotManager::Release(void)
{
	if (m_pInstance != nullptr) {	// �C���X�^���X���m�ۂ���Ă���
		m_pInstance->Uninit();
	}
}

//==========================================================
// ���X�g�ɑ}��
//==========================================================
void CRobotManager::ListIn(CRobot* pRobot)
{
	if (m_pTop != nullptr)
	{// �擪�����݂��Ă���ꍇ
		m_pCur->SetNext(pRobot);	// ���ݍŌ���̃I�u�W�F�N�g�̃|�C���^�ɂȂ���
		pRobot->SetPrev(m_pCur);
		m_pCur = pRobot;	// �������g���Ō���ɂȂ�
	}
	else
	{// ���݂��Ȃ��ꍇ
		m_pTop = pRobot;	// �������g���擪�ɂȂ�
		m_pCur = pRobot;	// �������g���Ō���ɂȂ�
	}

	m_nNum++;
}

//==========================================================
// ���X�g����O��
//==========================================================
void CRobotManager::ListOut(CRobot* pRobot)
{
	// ���X�g���玩�����g���폜����
	if (m_pTop == pRobot)
	{// ���g���擪
		if (pRobot->GetNext() != nullptr)
		{// �������݂��Ă���
			m_pTop = pRobot->GetNext();	// ����擪�ɂ���
			m_pTop->SetPrev(nullptr);	// ���̑O�̃|�C���^���o���Ă��Ȃ��悤�ɂ���
		}
		else
		{// ���݂��Ă��Ȃ�
			m_pTop = nullptr;	// �擪���Ȃ���Ԃɂ���
			m_pCur = nullptr;	// �Ō�����Ȃ���Ԃɂ���
		}
	}
	else if (m_pCur == pRobot)
	{// ���g���Ō��
		if (pRobot->GetPrev() != nullptr)
		{// �������݂��Ă���
			m_pCur = pRobot->GetPrev();		// �O���Ō���ɂ���
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
		if (pRobot->GetNext() != nullptr)
		{
			pRobot->GetNext()->SetPrev(pRobot->GetPrev());	// ���g�̎��ɑO�̃|�C���^���o��������
		}
		if (pRobot->GetPrev() != nullptr)
		{
			pRobot->GetPrev()->SetNext(pRobot->GetNext());	// ���g�̑O�Ɏ��̃|�C���^���o��������
		}
	}

	m_nNum--;
}
