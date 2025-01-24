//==========================================================
//
// �G�f�B�b�g�Ǘ��̏��� [edit_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_manager.h"
#include "input_keyboard.h"
#include "input_mouse.h"
#include "debugproc.h"
#include "manager.h"
#include "map_manager.h"
#include "map_obstacle.h"
#include "road.h"
#include "road_manager.h"

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
	m_bFlag = false;

	// �G�f�B�^�[������
	m_SelectType = CEdit::TYPE_OBJ;
	ChangeEdit(CEdit::Create(m_SelectType));
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
	CDebugProc* pProc = CManager::GetInstance()->GetDebugProc();

	// �f�o�b�O�\��
	pProc->Print("+---------------------------------------------------------------\n");
	pProc->Print("<�G�f�B�^�[�N����> ���f���ǂݍ��ݐ� [ %d ]", CMapManager::GetInstance()->GetFileNameList().size());
	if (CMapObstacle::GetList() != nullptr) { pProc->Print(" : ��Q���z�u�� [ %d ]", CMapObstacle::GetList()->GetNum()); }
	if (CRoadManager::GetInstance() != nullptr) { pProc->Print(" : ���z�u�� [ %d ]\n", CRoadManager::GetInstance()->GetList()->GetNum());}
	pProc->Print("�I��[ F4 ] : ���[�h�ؑ�[ F3 ] : ");
	//pProc->Print("<�}�E�X> [ %f, %f, %f ]\n", pMouse->GetWorldPos().x, pMouse->GetWorldPos().y, pMouse->GetWorldPos().z);

	// �G�f�B�^�[�I��
	if (pKey->GetTrigger(DIK_F4) && m_bFlag)
	{
		Uninit();
		return;
	}
	else
	{
		m_bFlag = true;
	}

	// ��Ԑ؂�ւ�
	if (pKey->GetTrigger(DIK_F3)) {
		m_SelectType = static_cast<CEdit::TYPE>((m_SelectType + 1) % CEdit::TYPE_MAX);
		ChangeEdit(CEdit::Create(m_SelectType));
	}

	// �G�f�B�^�[�̍X�V����
	if (m_pEdit != nullptr)
	{
		m_pEdit->Update();
	}
	else
	{
		pProc->Print("\n");
	}

	pProc->Print("+---------------------------------------------------------------\n");
}

//===============================================
// �؂�ւ�
//===============================================
void CEditManager::ChangeEdit(CEdit* pEdit)
{
	// �g�p����Ă�����p��
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
		m_pInstance->Init();
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
	// �I��
	if (m_pInstance != nullptr)
	{
		m_pInstance->Uninit();
	}
}