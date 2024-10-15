//==========================================================
//
// ���}�l�[�W���[ [road_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "road_manager.h"
#include "road.h"

// �ÓI�����o�ϐ��錾
CRoadManager* CRoadManager::m_pInstance = nullptr;	// �C���X�^���X

//==========================================================
// �R���X�g���N�^
//==========================================================
CRoadManager::CRoadManager()
{
	// �l�̃N���A
	m_pCur = nullptr;
	m_pTop = nullptr;
	m_nNum = 0;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CRoadManager::~CRoadManager()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CRoadManager::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CRoadManager::Uninit(void)
{

}

//==========================================================
// �X�V����
//==========================================================
void CRoadManager::Update(void)
{

}

//==========================================================
// �C���X�^���X���m��
//==========================================================
CRoadManager* CRoadManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// �g���Ă��Ȃ�
		m_pInstance = DEBUG_NEW CRoadManager;
	}

	return m_pInstance;
}

//==========================================================
// �C���X�^���X�������[�X
//==========================================================
void CRoadManager::Release(void)
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
void CRoadManager::ListIn(CRoad* pRoad)
{
	if (m_pTop != nullptr)
	{// �擪�����݂��Ă���ꍇ
		m_pCur->SetNext(pRoad);	// ���ݍŌ���̃I�u�W�F�N�g�̃|�C���^�ɂȂ���
		pRoad->SetPrev(m_pCur);
		m_pCur = pRoad;	// �������g���Ō���ɂȂ�
	}
	else
	{// ���݂��Ȃ��ꍇ
		m_pTop = pRoad;	// �������g���擪�ɂȂ�
		m_pCur = pRoad;	// �������g���Ō���ɂȂ�
	}

	m_nNum++;
}

//==========================================================
// ���X�g����O��
//==========================================================
void CRoadManager::ListOut(CRoad* pRoad)
{
	// ���X�g���玩�����g���폜����
	if (m_pTop == pRoad)
	{// ���g���擪
		if (pRoad->GetNext() != nullptr)
		{// �������݂��Ă���
			m_pTop = pRoad->GetNext();	// ����擪�ɂ���
			m_pTop->SetPrev(nullptr);	// ���̑O�̃|�C���^���o���Ă��Ȃ��悤�ɂ���
		}
		else
		{// ���݂��Ă��Ȃ�
			m_pTop = nullptr;	// �擪���Ȃ���Ԃɂ���
			m_pCur = nullptr;	// �Ō�����Ȃ���Ԃɂ���
		}
	}
	else if (m_pCur == pRoad)
	{// ���g���Ō��
		if (pRoad->GetPrev() != nullptr)
		{// �������݂��Ă���
			m_pCur = pRoad->GetPrev();		// �O���Ō���ɂ���
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
		if (pRoad->GetNext() != nullptr)
		{
			pRoad->GetNext()->SetPrev(pRoad->GetPrev());	// ���g�̎��ɑO�̃|�C���^���o��������
		}
		if (pRoad->GetPrev() != nullptr)
		{
			pRoad->GetPrev()->SetNext(pRoad->GetNext());	// ���g�̑O�Ɏ��̃|�C���^���o��������
		}
	}

	m_nNum--;
}

//==========================================================
// �U���q�b�g�m�F
//==========================================================
bool CRoadManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	CRoad* pRoad = m_pTop;
	bool bUse = false;

	//�ʔ���
	while (pRoad != nullptr) {
		CRoad* pRoadNext = pRoad->GetNext();
		//if (pRoad->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
		pRoad = pRoadNext;
	}

	return bUse;
}