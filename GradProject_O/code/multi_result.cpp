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
#include "player.h"
#include "objectX.h"

//==========================================================
// �萔��`
//==========================================================
namespace TEXT
{
	const D3DXVECTOR3 TITLE_POS = D3DXVECTOR3(SCREEN_CENTER.x, 100.0f, 0.0f);	// �^�C�g�����W
	const D3DXVECTOR3 RESULT_POS = D3DXVECTOR3(SCREEN_CENTER.x, SCREEN_HEIGHT - 150.0f, 0.0f);	// ���ʍ��W
	const D3DXVECTOR3 SETPOS = D3DXVECTOR3(170.0f, SCREEN_CENTER.y * 0.9f, 0.0f);		// �v���C���[���Ƃ̊�{���W

	const float STR_SPACE = 300.0f;	// �v���C���[�̕�����
	const float STR_SIZE = 80.0f;	// �v���C���[�g�p�̕����T�C�Y
	const float TITLE_SIZE = 150.0f;	// �^�C�g���̕����T�C�Y
	const float RESULT_SIZE = 100.0f;	// �^�C�g���̕����T�C�Y
}

namespace CAMERA
{
	const D3DXVECTOR3 POSV = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	const D3DXVECTOR3 ROT = D3DXVECTOR3(0.0f, D3DX_PI * 1.0f, D3DX_PI * 0.3f);
	const float LENGTH = 1700.0f;
}

namespace
{
	const float PLAYER_INER = 0.075f;			// �v���C���[�ړ��ʊ���
	const float PLAYER_TARGET_POSZ = -800.0f;	// �v���C���[�����N�\�����ڕWZ���W
	const float PLAYER_SPACE = 275.0f;
}

// using��`
using namespace TEXT;

//===============================================
// �R���X�g���N�^
//===============================================
CMultiResult::CMultiResult() :
m_pMgr(nullptr),
m_pInfo(nullptr),
m_pTitleStr(nullptr),
m_pEndStr(nullptr),
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
		// �e����ۑ�
		m_pInfo[i].nId = pInfo[i].nId;
		m_pInfo[i].nScore = pInfo[i].nNumDelv;
		m_pInfo[i].bActive = pInfo[i].bActive;

		// �A�N�e�B�u�ł͂Ȃ�
		if (!m_pInfo[i].bActive) { m_pInfo[i].nScore = 100; continue; }

		// �v���C���[�̍��W�ݒ�
		D3DXVECTOR3 pos = TEXT::SETPOS;
		pos.z -= PLAYER_SPACE * 0.5f;
		pos.z += PLAYER_SPACE * m_pMgr->GetNumPlayer() * 0.5f;
		pos.z -= PLAYER_SPACE * m_pInfo[i].nId;
		pos.x = -PLAYER_TARGET_POSZ * 2;

		// �����ݒ�
		D3DXVECTOR3 rot = VECTOR3_ZERO;
		rot.y += D3DX_PI;

		// �v���C���[����
		m_pInfo[i].pPlayer = CPlayer::Create(pos, rot, VECTOR3_ZERO, m_pInfo[i].nId);
		m_pInfo[i].pPlayer->SetType(CPlayer::TYPE::TYPE_NONE);
	}

	// �\�[�g����
	Sort();

	// �v���C���[���Ƃ̕����ݒ�
	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		if (!m_pInfo[i].bActive) { continue; }

		// ���W�ݒ�
		D3DXVECTOR3 pos = SETPOS;
		pos.x += STR_SPACE * m_pInfo[i].nId;

		// ����
		m_pInfo[i].pString = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, pos, 
			0.1f, STR_SIZE, STR_SIZE, XALIGN_CENTER, YALIGN_CENTER, VECTOR3_ZERO, m_pInfo[i].pPlayer->GetObj()->GetColMuliti());
		
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
		str += "�z�B�� : " + score.str();

		// �����}��
		m_pInfo[i].pString->PushBackString(str);
	}

	// ���ʔ��\�̕�������
	m_pTitleStr = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, TITLE_POS,
		0.4f, TITLE_SIZE, TITLE_SIZE, XALIGN_CENTER, YALIGN_CENTER, VECTOR3_ZERO, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	if (m_pTitleStr != nullptr)
	{
		// �����ݒ�
		m_pTitleStr->PushBackString("<<< �z�B�� >>>");
		m_pTitleStr->SetEnableScroll(true);
	}

	// ���s�̕�������
	int topid = m_pMgr->GetNumPlayer() - 1;	// 1�ʂ�ID
	int myId = -1;
	if (m_pMgr != nullptr) { myId = m_pMgr->GetMyId(); }

	m_pEndStr = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, RESULT_POS,
		0.1f, RESULT_SIZE, RESULT_SIZE, XALIGN_CENTER, YALIGN_CENTER, VECTOR3_ZERO, m_pInfo[topid].pPlayer->GetObj()->GetColMuliti());
	if (m_pEndStr != nullptr)
	{
		// �����ݒ�
		std::string str;
		str.clear();

		// ID
		std::ostringstream id;
		id.clear();
		id << m_pInfo[topid].nId + 1;
		str += "*** �ŗD�G�� : " + id.str() + "P ***";

		m_pEndStr->PushBackString(str);

		if (myId == m_pInfo[topid].nId)
		{
			m_pEndStr->PushBackString("���Ȃ��ł� ���߂łƂ��������܂�");
		}
		else
		{
			m_pEndStr->PushBackString("���̕��ł� �܂��撣��܂��傤");
		}
	}

	// �J���������ݒ�
	InitCameraSet();

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
	// �����m�F
	StrCheck();

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

//===============================================
// ������m�F
//===============================================
void CMultiResult::StrCheck()
{
	// �}�l�[�W���[����
	if (m_pMgr == nullptr) { return; }

	// �J�n��������
	if (m_pTitleStr == nullptr) { return; }

	// �J�n�����I�����Ă��Ȃ�
	if (!m_pTitleStr->GetEnd()) { return; }

	// �Ō�܂Ō���
	if (m_nNowScrPlayer >= m_pMgr->GetNumPlayer()) {
		EndStr(); 
		return; 
	}

	CScrollText2D* pStr = m_pInfo[m_nNowScrPlayer].pString;

	if (pStr == nullptr) { m_nNowScrPlayer++;  return; }

	// �������X�N���[�����Ă��Ȃ�
	if (!pStr->IsScroll()) {
		pStr->SetEnableScroll(true);
	}

	// �������Ȃ�
	if (pStr == nullptr) { return; }

	// �������Ō�܂ŕ\�����Ă��Ȃ�
	if (!pStr->GetEnd()) {
		// �v���C���[�ړ�
		RankPlayerMove(); 
		return; 
	}

	// �\�����Ă����玟�̕�����\������
	m_nNowScrPlayer++;
}

//===============================================
// �I�������\��
//===============================================
void CMultiResult::EndStr()
{
	// �������Ȃ�
	if (m_pEndStr == nullptr) { return; }

	// �������X�N���[������Ă���
	if (m_pEndStr->IsScroll()) { return; }

	m_pEndStr->SetEnableScroll(true);
}

//===============================================
// �����J�����ݒ�
//===============================================
void CMultiResult::InitCameraSet()
{
	// �J�����擾
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	if (pCamera == nullptr) { return; }

	// �J�����̒l�ݒ�
	pCamera->SetLength(CAMERA::LENGTH);
	pCamera->SetRotation(CAMERA::ROT);
	pCamera->SetPositionR(CAMERA::POSV);
}

//===============================================
// �����N�\�����̃v���C���[�̓�������
//===============================================
void CMultiResult::RankPlayerMove()
{
	// �}�l�[�W���[����
	if (m_pMgr == nullptr) { return; }

	// �Ō�܂Ō���
	if (m_nNowScrPlayer >= m_pMgr->GetNumPlayer()) { return; }

	// �v���C���[�擾
	CPlayer* pPlayer = m_pInfo[m_nNowScrPlayer].pPlayer;
	if (pPlayer == nullptr) { return; }

	// ���W��␳����
	D3DXVECTOR3 pos = pPlayer->GetPosition();
	D3DXVECTOR3 posdest = pos;
	posdest.x = PLAYER_TARGET_POSZ;
	pos += ((posdest - pos) * PLAYER_INER);

	pPlayer->SetPosition(pos);
}