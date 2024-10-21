//==========================================================
//
// �^�X�N�Ǘ��̏��� [task_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_manager.h"
#include "input_keyboard.h"

CEditManager* CEditManager::m_pInstance = nullptr;

//==========================================================
// �R���X�g���N�^
//==========================================================
CEditManager::CEditManager()
{
	// �l�̃N���A
	m_SelectType = CEdit::TYPE_OBJ;
	m_pEdit = nullptr;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CEditManager::~CEditManager()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CEditManager::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CEditManager::Uninit(void)
{
	if (m_pEdit != nullptr)
	{
		m_pEdit->Uninit();
		m_pEdit = nullptr;
	}

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
void CEditManager::Update(void)
{
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	// ��Ԑ؂�ւ�
	if (pKey->GetTrigger(DIK_F3)) {
		m_SelectType = static_cast<CEdit::TYPE>((m_SelectType + 1) / CEdit::TYPE_MAX);
		ChangeEdit(CEdit::Create(m_SelectType));
	}

	// �G�f�B�^�[�̍X�V����
	if (m_pEdit != nullptr)
	{
		m_pEdit->Update();
	}
}

//===============================================
// �؂�ւ�
//===============================================
void CEditManager::ChangeEdit(CEdit* pEdit)
{
	if (m_pEdit != nullptr)
	{
		m_pEdit->Uninit();
		m_pEdit = nullptr;
	}

	m_pEdit = pEdit;
}

//===============================================
// �C���X�^���X�̐���
//===============================================
CEditManager* CEditManager::Create()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = DEBUG_NEW CEditManager;
	}

	return m_pInstance;
}

//===============================================
// �C���X�^���X�̎擾
//===============================================
CEditManager* CEditManager::GetInstance()
{
	return m_pInstance;
}

//===============================================
// �C���X�^���X�̔p��
//===============================================
void CEditManager::Release(void)
{
	if (m_pInstance != nullptr)
	{
		m_pInstance->Uninit();
	}
}