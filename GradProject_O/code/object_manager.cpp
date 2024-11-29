//==========================================================
//
// �I�u�W�F�N�g�Ǘ��̏��� [object_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "object_manager.h"
#include "camera_manager.h"
#include "camera.h"
#include "object.h"
#include "manager.h"
#include <assert.h>
#include "debugproc.h"

CObjectManager *CObjectManager::m_pInstance = nullptr;

//==========================================================
// �R���X�g���N�^
//==========================================================
CObjectManager::CObjectManager()
{
	// �l�̃N���A
	for (int nCntPri = 0; nCntPri < NUM_PRIORITY; nCntPri++)
	{
		m_apTop[nCntPri] = nullptr;	// �擪���Ȃ���Ԃɂ���
		m_apCur[nCntPri] = nullptr;	// �Ō�����Ȃ���Ԃɂ���
		m_aPriNumAll[nCntPri] = 0;
	}

	m_nNumAll = 0;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CObjectManager::~CObjectManager()
{
	for (int nCntPri = 0; nCntPri < NUM_PRIORITY; nCntPri++)
	{
		if (m_apTop[nCntPri] != nullptr) {
			assert(false);
		}
		if (m_apCur[nCntPri] != nullptr) {
			assert(false);
		}
		if (m_aPriNumAll[nCntPri] > 0) {
			assert(false);
		}
		
	}
}

//==========================================================
// ����������
//==========================================================
HRESULT CObjectManager::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CObjectManager::Uninit(void)
{
	// �e�N���X�̏I������
	CListManager::Uninit();

	// ���X�g�̑S�p��
	ReleaseAll();

	// �C���X�^���X�̔p��
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// �`�揈��
//==========================================================
void CObjectManager::Draw(void)
{
	CCamera *pCamera = CCameraManager::GetInstance()->GetTop();
	CDebugProc::GetInstance()->Print("�I�u�W�F�N�g�� [ %d ]\n", m_nNumAll);

	while (pCamera != nullptr) {

		CCamera *pCameraNext = pCamera->GetNext();

		// �ݒ�
		pCamera->SetCamera();

		if (pCamera->GetDraw()) {	// �`�悷��ꍇ

			if (pCamera->GetDrawState() == CCamera::DRAWSTATE::PLAYER_ONLY)
			{
				// �v���C���[�����`��
				DrawOnlyPlayer();
			}
			else
			{
				// ���X�g�̑S�`��
				DrawAll();
			}
			
			//DrawOneDimension(TYPE_3D);
		}

		pCamera = pCameraNext;
	}
}

//===============================================
// �S�Ẵ^�X�N�̔p��
//===============================================
void CObjectManager::ReleaseAll(void)
{
	for (int nCntPri = 0; nCntPri < NUM_PRIORITY; nCntPri++)
	{
		CObject *pObject = m_apTop[nCntPri];	// �擪���擾

		while (pObject != nullptr)
		{// �g�p����Ă��Ȃ���Ԃ܂�

			CObject *pObjectNext = pObject->GetNext();	// ���̃I�u�W�F�N�g�ւ̃|�C���^���擾

			// �I������
			pObject->Uninit();

			pObject = pObjectNext;	// ���̃I�u�W�F�N�g�Ɉړ�
		}
	}

	// ���S�t���O���`�F�b�N
	DeathCheck();
}

//===============================================
// �S�ẴI�u�W�F�N�g�̕`��
//===============================================
void CObjectManager::DrawAll(void)
{
	// ���S�t���O���`�F�b�N
	DeathCheck();

	for (int nCntPri = 0; nCntPri < NUM_PRIORITY; nCntPri++)
	{
		CObject *pObject = m_apTop[nCntPri];	// �擪���擾

		while (pObject != nullptr)
		{// �g�p����Ă��Ȃ���Ԃ܂�

			CObject *pObjectNext = pObject->GetNext();	// ���̃I�u�W�F�N�g�ւ̃|�C���^���擾

			// �`�揈��
			if (pObject->GetDraw())
			{
				pObject->Draw();
			}

			pObject = pObjectNext;	// ���̃I�u�W�F�N�g�Ɉړ�
		}
	}
}

//===============================================
// �v���C���[�̂ݕ`��
//===============================================
void CObjectManager::DrawOnlyPlayer(void)
{
	// ���S�t���O���`�F�b�N
	DeathCheck();

	for (int nCntPri = 0; nCntPri < NUM_PRIORITY; nCntPri++)
	{
		CObject* pObject = m_apTop[nCntPri];	// �擪���擾

		while (pObject != nullptr)
		{// �g�p����Ă��Ȃ���Ԃ܂�

			CObject* pObjectNext = pObject->GetNext();	// ���̃I�u�W�F�N�g�ւ̃|�C���^���擾

			// �`�揈��
			if (pObject->GetDraw() && pObject->GetType() == CObject::TYPE::TYPE_PLAYER)
			{
				pObject->Draw();
			}

			pObject = pObjectNext;	// ���̃I�u�W�F�N�g�Ɉړ�
		}
	}
}

//===============================================
// ���S�t���O���`�F�b�N
//===============================================
void CObjectManager::DeathCheck(void)
{
	for (int nCntPri = 0; nCntPri < NUM_PRIORITY; nCntPri++)
	{
		CObject *pObject = m_apTop[nCntPri];	// �擪���擾

		while (pObject != nullptr)
		{// �g�p����Ă��Ȃ���Ԃ܂�
			CObject *pObjectNext = pObject->GetNext();	// ���̃I�u�W�F�N�g�ւ̃|�C���^���擾

			if (pObject->GetDeath() == true)
			{
				// ���X�g���玩�����g���폜����
				if (m_apTop[nCntPri] == pObject)
				{// ���g���擪
					if (pObject->GetNext() != nullptr)
					{// �������݂��Ă���
						m_apTop[nCntPri] = pObject->GetNext();	// ����擪�ɂ���
						m_apTop[nCntPri]->SetPrev(nullptr);	// ���̑O�̃|�C���^���o���Ă��Ȃ��悤�ɂ���
					}
					else
					{// ���݂��Ă��Ȃ�
						m_apTop[nCntPri] = nullptr;	// �擪���Ȃ���Ԃɂ���
						m_apCur[nCntPri] = nullptr;	// �Ō�����Ȃ���Ԃɂ���
					}
				}
				else if (m_apCur[nCntPri] == pObject)
				{// ���g���Ō��
					if (pObject->GetPrev() != nullptr)
					{// �������݂��Ă���
						m_apCur[nCntPri] = pObject->GetPrev();		// �O���Ō���ɂ���
						m_apCur[nCntPri]->SetNext(nullptr);			// �O�̎��̃|�C���^���o���Ă��Ȃ��悤�ɂ���
					}
					else
					{// ���݂��Ă��Ȃ�
						m_apTop[nCntPri] = nullptr;	// �擪���Ȃ���Ԃɂ���
						m_apCur[nCntPri] = nullptr;	// �Ō�����Ȃ���Ԃɂ���
					}
				}
				else
				{
					if (pObject->GetNext() != nullptr)
					{
						pObject->GetNext()->SetPrev(pObject->GetPrev());	// ���g�̎��ɑO�̃|�C���^���o��������
					}
					if (pObject->GetPrev() != nullptr)
					{
						pObject->GetPrev()->SetNext(pObject->GetNext());	// ���g�̑O�Ɏ��̃|�C���^���o��������
					}
				}

				delete pObject;	// �������̊J��
				pObject = nullptr;
			}

			if (pObject == nullptr)
			{
				m_aPriNumAll[nCntPri]--;
				m_nNumAll--;
			}

			pObject = pObjectNext;	// ���̃I�u�W�F�N�g�Ɉړ�
		}
	}
}

//===============================================
// ���X�g�ɑ}������
//===============================================
void CObjectManager::ListIn(CObject *pObject)
{
	int nPri = pObject->GetPri();

	if (m_apTop[nPri] != nullptr)
	{// �擪�����݂��Ă���ꍇ
		m_apCur[nPri]->SetNext(pObject);	// ���ݍŌ���̃I�u�W�F�N�g�̃|�C���^�ɂȂ���
		pObject->SetPrev(m_apCur[nPri]);
		m_apCur[nPri] = pObject;	// �������g���Ō���ɂȂ�
	}
	else
	{// ���݂��Ȃ��ꍇ
		m_apTop[nPri] = pObject;	// �������g���擪�ɂȂ�
		m_apCur[nPri] = pObject;	// �������g���Ō���ɂȂ�
	}

	m_aPriNumAll[nPri]++;
	m_nNumAll++;
}

//===============================================
// �C���X�^���X�̎擾
//===============================================
CObjectManager* CObjectManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = DEBUG_NEW CObjectManager;
		m_pInstance->Init();
	}

	return m_pInstance;
}

//===============================================
// �C���X�^���X�̔p��
//===============================================
void CObjectManager::Release(void)
{
	if (m_pInstance != nullptr)
	{
		m_pInstance->Uninit();
	}
}
