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
	m_NextInfo = SNextInfo();
	m_List.Clear();
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

	return m_List.Regist(pInspection->GetId(), pInspection);
}

//==========================================================
// ���X�g�Ǘ��Ɋ܂߂�
//==========================================================
bool CInspectionManager::ListOut(CInspection* pInspection)
{
	if (pInspection == nullptr) { return false; }

	return m_List.Delete(pInspection->GetId(), pInspection);
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
	if (m_NextInfo.bActive)
	{
		// ���݂��Ă��Ȃ������琶��
		if (Get(m_NextInfo.nId) == nullptr)
		{
			CInspection::Create(m_NextInfo.pos, m_NextInfo.rot, m_NextInfo.pRoad, m_NextInfo.nId, m_NextInfo.nStartpdid);
		}
		m_NextInfo = SNextInfo();
		m_NextInfo.bActive = false;
	}
}

//==========================================================
// ���̌���̏��ݒ�
//==========================================================
void CInspectionManager::SetNextInspection(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, CRoad* pRoad, int nId, int startpdid)
{
	if (m_NextInfo.bActive) { return; }
	m_NextInfo.bActive = true;
	m_NextInfo.pos = pos;
	m_NextInfo.rot = rot;
	m_NextInfo.pRoad = pRoad;
	m_NextInfo.nId = nId;
	m_NextInfo.nStartpdid = startpdid;
}
