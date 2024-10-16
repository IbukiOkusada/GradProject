//==========================================================
//
// �T���v���}�l�[�W���[ [sample_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "sample_manager.h"
#include "sample.h"

// �ÓI�����o�ϐ��錾
CSampleManager* CSampleManager::m_pInstance = nullptr;	// �C���X�^���X

//==========================================================
// �R���X�g���N�^
//==========================================================
CSampleManager::CSampleManager()
{
	// �l�̃N���A
	m_pCur = nullptr;
	m_pTop = nullptr;
	m_nNum = 0;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CSampleManager::~CSampleManager()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CSampleManager::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CSampleManager::Uninit(void)
{

}

//==========================================================
// �X�V����
//==========================================================
void CSampleManager::Update(void)
{

}

//==========================================================
// �C���X�^���X���m��
//==========================================================
CSampleManager* CSampleManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// �g���Ă��Ȃ�
		m_pInstance = DEBUG_NEW CSampleManager;
	}

	return m_pInstance;
}

//==========================================================
// �C���X�^���X�������[�X
//==========================================================
void CSampleManager::Release(void)
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
void CSampleManager::ListIn(CSample* pSample)
{
	if (m_pTop != nullptr)
	{// �擪�����݂��Ă���ꍇ
		m_pCur->SetNext(pSample);	// ���ݍŌ���̃I�u�W�F�N�g�̃|�C���^�ɂȂ���
		pSample->SetPrev(m_pCur);
		m_pCur = pSample;	// �������g���Ō���ɂȂ�
	}
	else
	{// ���݂��Ȃ��ꍇ
		m_pTop = pSample;	// �������g���擪�ɂȂ�
		m_pCur = pSample;	// �������g���Ō���ɂȂ�
	}

	m_nNum++;
}

//==========================================================
// ���X�g����O��
//==========================================================
void CSampleManager::ListOut(CSample* pSample)
{
	// ���X�g���玩�����g���폜����
	if (m_pTop == pSample)
	{// ���g���擪
		if (pSample->GetNext() != nullptr)
		{// �������݂��Ă���
			m_pTop = pSample->GetNext();	// ����擪�ɂ���
			m_pTop->SetPrev(nullptr);	// ���̑O�̃|�C���^���o���Ă��Ȃ��悤�ɂ���
		}
		else
		{// ���݂��Ă��Ȃ�
			m_pTop = nullptr;	// �擪���Ȃ���Ԃɂ���
			m_pCur = nullptr;	// �Ō�����Ȃ���Ԃɂ���
		}
	}
	else if (m_pCur == pSample)
	{// ���g���Ō��
		if (pSample->GetPrev() != nullptr)
		{// �������݂��Ă���
			m_pCur = pSample->GetPrev();		// �O���Ō���ɂ���
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
		if (pSample->GetNext() != nullptr)
		{
			pSample->GetNext()->SetPrev(pSample->GetPrev());	// ���g�̎��ɑO�̃|�C���^���o��������
		}
		if (pSample->GetPrev() != nullptr)
		{
			pSample->GetPrev()->SetNext(pSample->GetNext());	// ���g�̑O�Ɏ��̃|�C���^���o��������
		}
	}

	m_nNum--;
}
