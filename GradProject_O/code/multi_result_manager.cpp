//==========================================================
//
// �}���`���U���g�}�l�[�W���[ [multi_result_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "multi_result_manager.h"
#include "player_manager.h"
#include "player.h"

// �ÓI�����o�ϐ��錾
CMultiResultManager* CMultiResultManager::m_pInstance = nullptr;	// �C���X�^���X

//==========================================================
// �R���X�g���N�^
//==========================================================
CMultiResultManager::CMultiResultManager()
{
	// �l�̃N���A
	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		m_aInfo[i] = SInfo();
		m_aInfo[i].bActive = false;
		m_aInfo[i].nId = i;
		m_aInfo[i].nNumDelv = 0;
	}

	m_nNumPlayer = 0;
	m_nMyId = -1;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CMultiResultManager::~CMultiResultManager()
{

}

//==========================================================
// �C���X�^���X���m��
//==========================================================
CMultiResultManager* CMultiResultManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// �g���Ă��Ȃ�
		m_pInstance = DEBUG_NEW CMultiResultManager;
	}

	return m_pInstance;
}

//==========================================================
// �C���X�^���X�������[�X
//==========================================================
void CMultiResultManager::Release(void)
{
	if (m_pInstance != nullptr) {	// �C���X�^���X���m�ۂ���Ă���
		m_pInstance->Uninit();
	}
}

//==========================================================
// ����������
//==========================================================
HRESULT CMultiResultManager::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CMultiResultManager::Uninit(void)
{
	// �I������
	CListManager::Uninit();

	// �C���X�^���X�̔p��
	if (m_pInstance != nullptr) {	// �C���X�^���X���m�ۂ���Ă���
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// �X�V����
//==========================================================
void CMultiResultManager::Update(void)
{

}

//===============================================
// �f�[�^�ݒ�
//===============================================
void CMultiResultManager::DataSet(void)
{
	// �l�b�g���[�N�擾
	CNetWork* pNet = CNetWork::GetInstance();
	if (pNet == nullptr) { return; }

	// ���g��ID�ۑ�
	m_nMyId = pNet->GetIdx();

	// �v���C���[�}�l�[�W���[�̏��擾
	CPlayerManager* pMgr = CPlayerManager::GetInstance();
	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		// �|�C���^�ɕϊ�
		SInfo* pInfo = &m_aInfo[i];

		// �v���C���[���擾
		CPlayer* pPlayer = pMgr->GetPlayer(i);

		// �g�p����Ă�����z�B���ۑ�
		if (pNet->GetConnect(i) && pPlayer != nullptr)
		{
			pInfo->bActive = true;
			pInfo->nNumDelv = pPlayer->GetNumDeliverStatus();
			pInfo->nId = pPlayer->GetId();
			m_nNumPlayer++;
		}
		else	// �g�p����Ă��Ȃ�
		{
			pInfo->bActive = false;
		}
	}
}