//===============================================
//
// �}���`���U���g��ʂ̊Ǘ����� [multi_result.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "multi_result.h"
#include "multi_result_manager.h"
#include "map_manager.h"
#include "manager.h"
#include "sound.h"
#include "camera.h"

//==========================================================
// �萔��`
//==========================================================

//===============================================
// �R���X�g���N�^
//===============================================
CMultiResult::CMultiResult()
{
	m_pMgr = nullptr;
}

//===============================================
// �f�X�g���N�^
//===============================================
CMultiResult::~CMultiResult()
{

}

//===============================================
// ����������
//===============================================
HRESULT CMultiResult::Init(void)
{
	// �}�b�v�ǂݍ���
	CMapManager::GetInstance()->Load();

	// �}�l�[�W���[�擾
	m_pMgr = CMultiResultManager::GetInstance();

	return S_OK;
}

//===============================================
// �I������
//===============================================
void CMultiResult::Uninit(void)
{
	// �}�l�[�W���[�p��
	if(m_pMgr != nullptr)
	{
		m_pMgr->Release();
		m_pMgr = nullptr;
	}

	// �}�b�v���p��
	CMapManager::Release();
	CManager::GetInstance()->GetCamera()->SetActive(true);
	CManager::GetInstance()->GetSound()->Stop();
}

//===============================================
// �X�V����
//===============================================
void CMultiResult::Update(void)
{
	CScene::Update();
}

//===============================================
// �`�揈��
//===============================================
void CMultiResult::Draw(void)
{
	CScene::Draw();
}
