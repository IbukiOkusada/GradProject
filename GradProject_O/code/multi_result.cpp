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
#include "network.h"
#include "scrollText2D.h"

//==========================================================
// �萔��`
//==========================================================
namespace TEXT
{
	const D3DXVECTOR3 SETPOS = D3DXVECTOR3(100.0f, SCREEN_CENTER.y, 0.0f);
}

//===============================================
// �R���X�g���N�^
//===============================================
CMultiResult::CMultiResult() :
m_pMgr(nullptr),
m_pInfo(nullptr),
m_nNowScrPlayer(0)
{

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

	// �����i�[
	CMultiResultManager::SInfo* pInfo = m_pMgr->GetInfo();
	m_pInfo = DEBUG_NEW SPlayerInfo[NetWork::MAX_CONNECT]();

	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		m_pInfo[i].nId = pInfo[i].nId;
		m_pInfo[i].nScore = pInfo[i].nNumDelv;
		m_pInfo[i].bActive = pInfo[i].bActive;
	}

	// �\�[�g����
	Sort();

	// �����ݒ�
	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		if (!m_pInfo[i].bActive) { continue; }

		D3DXVECTOR3 pos = TEXT::SETPOS;
		pos.x += 200.0f * m_pInfo[i].nId;
		m_pInfo[i].pString = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, pos, 0.25f, 50.0f, 50.0f, XALIGN_CENTER, YALIGN_CENTER, VECTOR3_ZERO, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
		
		// �����ݒ�
		std::string str;
		str.clear();

		// ID
		std::ostringstream id;
		id << m_pInfo[i].nId + 1;
		str += id.str() + "P ";

		// ����
		std::ostringstream rank;
		rank << m_pMgr->GetNumPlayer() - i;
		str += rank.str() + "��";

		// �����}��
		m_pInfo[i].pString->PushBackString(str);

		// �X�R�A
		str.clear();
		std::ostringstream score;
		score << m_pInfo[i].nScore;
		str += "�X�R�A : " + score.str();

		// �����}��
		m_pInfo[i].pString->PushBackString(str);
	}

	// �����Đ�
	m_pInfo[m_nNowScrPlayer].pString->SetEnableScroll(true);

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

	// �f�[�^�p��
	if (m_pInfo != nullptr)
	{
		delete[] m_pInfo;
		m_pInfo = nullptr;
	}

	// �}�b�v���p��
	CMapManager::Release();

	// �J��������\��
	CManager::GetInstance()->GetCamera()->SetActive(true);

	// �T�E���h��~
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

//===============================================
// �\�[�g����
//===============================================
void CMultiResult::Sort()
{
	for (int i = 0; i < NetWork::MAX_CONNECT - 1; i++)
	{
		int temp = i;

		for (int j = i + 1; j < NetWork::MAX_CONNECT; j++)
		{
			// �����\�[�g
			if (m_pInfo[j].nScore < m_pInfo[temp].nScore)
			{
				// �L��
				temp = j;
			}
		}

		// �ύX���ꂽ
		if (i != temp)
		{
			SPlayerInfo info = m_pInfo[temp];
			m_pInfo[temp] = m_pInfo[i];
			m_pInfo[i] = info;
		}
	}
}