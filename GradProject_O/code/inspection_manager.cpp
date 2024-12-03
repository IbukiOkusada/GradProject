//==========================================================
//
// ����Ǘ��̏��� [inspeciton_manager.cpp]
// Author : Ryosuke Oohara
//
//==========================================================
#include "inspection_manager.h"
#include "inspection.h"
#include "debugproc.h"

//==========================================================
// �ÓI�����o�ϐ�
//==========================================================
CInspectionManager* CInspectionManager::m_pInstance = nullptr;

namespace
{
	
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CInspectionManager::CInspectionManager()
{
	m_pInstance = this;
	m_List.Clear();
	m_nCreateCnt = 0;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CInspectionManager::~CInspectionManager()
{

}

//==========================================================
// ��������
//==========================================================
CInspectionManager* CInspectionManager::GetInstance(void)
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = DEBUG_NEW CInspectionManager;
		m_pInstance->Init();
	}

	return m_pInstance;
}

//==========================================================
// ���
//==========================================================
void CInspectionManager::Release()
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
bool CInspectionManager::ListIn(CInspection* pInspection)
{
	if (pInspection == nullptr) { return false; }

	bool bflag = m_List.Regist(pInspection->GetId(), pInspection);

	// ����
	if (bflag) { m_nCreateCnt++; }

	return bflag;
}

//==========================================================
// ���X�g�Ǘ��Ɋ܂߂�
//==========================================================
bool CInspectionManager::ListOut(CInspection* pInspection)
{
	if (pInspection == nullptr) { return false; }

	bool bflag = m_List.Delete(pInspection->GetId(), pInspection);

	return bflag;
}

//==========================================================
// �S�[�����擾
//==========================================================
CInspection* CInspectionManager::Get(int nIdx)
{
	auto it = m_List.Get(nIdx);
	return it;
}

//==========================================================
// ����������
//==========================================================
HRESULT CInspectionManager::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CInspectionManager::Uninit(void)
{
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
void CInspectionManager::Update(void)
{
	
}
