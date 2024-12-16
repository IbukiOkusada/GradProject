//===============================================
//
// �}���`���U���g��ʂ̏�ԊǗ�.cpp [multi_result_state.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "multi_result_state.h"
#include "multi_result_manager.h"
#include "multi_result.h"
#include "input_keyboard.h"
#include "input_gamepad.h"
#include "scrollText2D.h"
#include "deltatime.h"
#include "manager.h"
#include "player.h"
#include "police.h"
#include "fade.h"
#include "camera.h"
#include "camera_action.h"
#include "character.h"
#include "motion.h"

//===============================================
// �萔��`
//===============================================
// �����N
namespace Rank
{
	const float PLAYER_INER = 0.075f;			// �v���C���[�ړ��ʊ���
	const float PLAYER_TARGET_POSX = -800.0f;	// �v���C���[�����N�\�����ڕWZ���W
	const float PLAYER_SPACE = 275.0f;
	
}

// ���U���g�\��
namespace Result
{
	const int NUM_POLICE = 2;					// �x�@�̈�񐶐���
	const float POLICE_SPACE = 200.0f;			// �x�@�̑O��SPACE
	const float LENGTH = 1000.0f;				// �J�����̋���
	const float ROTZ = D3DX_PI * 0.4f;
}

// �I��
namespace End
{
	const float ADD_ROTZ = D3DX_PI * 0.1f;
	const float ADD_ROTY = D3DX_PI * 0.015f;
	const float ADD_ROTX = D3DX_PI * 0.3f;
	const float ROT_INER = 0.1f;
	const float FADE_CNT = 7.0f;				// �t�F�[�h�܂ł̃J�E���g
}

//*************************************************************************************
// �J�n��ԃN���X �R���X�g���N�^
//===============================================
CMultiResultStateStart::CMultiResultStateStart() : CMultiResultState(STATE::STATE_START)
{

}

//===============================================
// �f�X�g���N�^
//===============================================
CMultiResultStateStart::~CMultiResultStateStart()
{

}

//===============================================
// ���쏈���֘A
//===============================================
void CMultiResultStateStart::Controll(CMultiResult* pResult)
{
	// ��������
	if (pResult->GetTitleStr() == nullptr) { return; }

	// �X�N���[���I�����Ă��Ȃ�
	if (!pResult->GetTitleStr()->GetEnd()) { return; }

	// �I���������Ԃ������N�\����ԂɕύX����
	pResult->ChangeState(DEBUG_NEW CMultiResultStateRank);
}

//*************************************************************************************
// �����N�\����ԃN���X �R���X�g���N�^
//===============================================
CMultiResultStateRank::CMultiResultStateRank() : CMultiResultState(STATE::STATE_RANK)
{

}

//===============================================
// �f�X�g���N�^
//===============================================
CMultiResultStateRank::~CMultiResultStateRank()
{

}

//===============================================
// ���쏈���֘A
//===============================================
void CMultiResultStateRank::Controll(CMultiResult* pResult)
{
	// �}�l�[�W���[����
	CMultiResultManager* pMgr = pResult->GetMgr();
	if (pMgr == nullptr) { return; }

	// �Ō�܂Ō���
	if (pResult->GetNowScr() >= pMgr->GetNumPlayer()) {
		// ���ʕ\����ԂɕύX
		pResult->ChangeState(DEBUG_NEW CMultiResultStateResult);
		return;
	}

	CMultiResult::SPlayerInfo* pInfo = pResult->GetInfo();
	CScrollText2D* pStr = pInfo[pResult->GetNowScr()].pString;

	// �������Ȃ�
	if (pStr == nullptr) { 
		pResult->SetNowScr(pResult->GetNowScr() + 1); 
		return; 
	}

	// �������X�N���[�����Ă��Ȃ�
	if (!pStr->IsScroll()) {
		pStr->SetEnableScroll(true);
	}

	// �������Ȃ�
	if (pStr == nullptr) { return; }

	// �������Ō�܂ŕ\�����Ă��Ȃ�
	if (!pStr->GetEnd()) {
		// �v���C���[�ړ�
		RankPlayerMove(pResult);
		return;
	}

	// �\�����Ă����玟�̕�����\������
	pResult->SetNowScr(pResult->GetNowScr() + 1);
}


//===============================================
// �����N�\�����̃v���C���[�̓�������
//===============================================
void CMultiResultStateRank::RankPlayerMove(CMultiResult* pResult)
{
	// �}�l�[�W���[����
	CMultiResultManager* pMgr = pResult->GetMgr();
	if (pMgr == nullptr) { return; }

	// �Ō�܂Ō���
	if (pResult->GetNowScr() >= pMgr->GetNumPlayer()) { return; }

	// �v���C���[�擾
	CMultiResult::SPlayerInfo* pInfo = pResult->GetInfo();
	CPlayer* pPlayer = pInfo[pResult->GetNowScr()].pPlayer;
	if (pPlayer == nullptr) { return; }

	// ���W��␳����
	D3DXVECTOR3 pos = pPlayer->GetPosition();
	D3DXVECTOR3 posdest = pos;
	posdest.x = Rank::PLAYER_TARGET_POSX;
	pos += ((posdest - pos) * Rank::PLAYER_INER);

	pPlayer->SetPosition(pos);
}

//*************************************************************************************
// ���ʕ\����ԃN���X �R���X�g���N�^
//===============================================
CMultiResultStateResult::CMultiResultStateResult() : CMultiResultState(STATE::STATE_MULTIRANK),
m_ppPolice(nullptr),
m_nPassingPolice(0),
m_nNumCreatePolice(0)
{
	
}

//===============================================
// �f�X�g���N�^
//===============================================
CMultiResultStateResult::~CMultiResultStateResult()
{

}

//===============================================
// ���쏈���֘A
//===============================================
void CMultiResultStateResult::Controll(CMultiResult* pResult)
{
	// �x�@�X�V
	PoliceUpdate(pResult);

	// �������Ȃ�
	if (pResult->GetEndStr() == nullptr) { return; }

	// �}�l�[�W���[����
	CMultiResultManager* pMgr = pResult->GetMgr();
	if (pMgr == nullptr) { return; }

	// �������X�N���[������Ă���
	if (pResult->GetEndStr()->GetEnd() && m_nPassingPolice >= (pMgr->GetNumPlayer() -1) * Result::NUM_POLICE) 
	{ 
		Uninit(pResult);
		return; 
	}

	// �������X�N���[������Ă���
	if (pResult->GetEndStr()->IsScroll()) { return; }

	pResult->GetEndStr()->SetEnableScroll(true);
}

//===============================================
// ��������
//===============================================
void CMultiResultStateResult::Create(CMultiResult* pResult)
{
	// �}�l�[�W���[����
	CMultiResultManager* pMgr = pResult->GetMgr();
	if (pMgr == nullptr) { return; }

	// �v���C���[�擾
	CMultiResult::SPlayerInfo* pInfo = pResult->GetInfo();

	// �|�C���^����
	m_nNumCreatePolice = Result::NUM_POLICE * pMgr->GetNumPlayer();
	m_ppPolice = DEBUG_NEW CPolice*[m_nNumCreatePolice];

	for (int i = 0; i < pMgr->GetNumPlayer(); i++)
	{
		// �z��J�E���g
		int cnt = i * Result::NUM_POLICE;

		// �v���C���[�擾
		CPlayer* pPlayer = pInfo[i].pPlayer;

		// �������g�̏ꍇ
		if (pInfo[i].nId == pResult->GetTopId()) { continue; }

		// �ŏ��̈��ڂ�����������
		for (int j = 0; j < Result::NUM_POLICE; j++)
		{			
			// ���ݒ�
			D3DXVECTOR3 pos = pPlayer->GetPosition();
			if (j == 0)
			{
				pos.x = -Rank::PLAYER_TARGET_POSX * 3.0f - ((rand() % 6) * 50);
			}
			else
			{
				pos.x = m_ppPolice[j - 1]->GetPosition().x + 600.0f + ((rand() % 6) * 50);
			}

			pos.y = 10.0f;

			D3DXVECTOR3 rot = VECTOR3_ZERO;
			rot.y = -D3DX_PI * 0.5f;

			D3DXVECTOR3 move = VECTOR3_ZERO;

			// ����̂�
			{
				move.x = -(rand() % 10 + 30.0f);
			}

			// �x�@����
			m_ppPolice[cnt + j] = CPolice::Create(pos, rot, move, cnt + j);
			m_ppPolice[cnt + j]->SetType(CCar::TYPE::TYPE_NONE);
		}
	}

	// ��ʂ̃v���C���[�̍��W���擾����
	D3DXVECTOR3 toppos = pInfo[pMgr->GetNumPlayer() - 1].pPlayer->GetPosition();

	// �J�����̃A�N�V�����ݒ�
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	CCameraAction* pCamAc = pCamera->GetAction();

	// ����
	D3DXVECTOR3 rot = pCamera->GetRotation();
	rot.z = Result::ROTZ;

	pCamAc->Set(pCamera, toppos, rot, Result::LENGTH, 2.5f, 5.0f, CCameraAction::MOVE::MOVE_POSV);
}

//===============================================
// �x�@�X�V
//===============================================
void CMultiResultStateResult::PoliceUpdate(CMultiResult* pResult)
{
	m_nPassingPolice = 0;

	// �}�l�[�W���[����
	CMultiResultManager* pMgr = pResult->GetMgr();
	if (pMgr == nullptr) { return; }

	// �v���C���[�擾
	CMultiResult::SPlayerInfo* pInfo = pResult->GetInfo();

	for (int i = 0; i < pMgr->GetNumPlayer(); i++)
	{
		// �z��J�E���g
		int cnt = i * Result::NUM_POLICE;

		// �v���C���[�擾
		CPlayer* pPlayer = pInfo[i].pPlayer;

		// �������g�̏ꍇ
		if (pInfo[i].nId == pResult->GetTopId()) { continue; }

		for (int j = 0; j < Result::NUM_POLICE; j++)
		{
			if (pPlayer == nullptr) { continue; }

			if (m_ppPolice[cnt + j] == nullptr) { continue; }

			// ���ݒ�
			CPolice* pPolice = m_ppPolice[cnt + j];
			D3DXVECTOR3 pos = pPolice->GetPosition();
			pos += pPolice->GetMove();
			pPolice->SetPosition(pos);

			// ���𐶐�����n�_�ɒB���Ă��Ȃ�
			if (pos.x >= -Rank::PLAYER_TARGET_POSX) { continue; }

			if (pos.x <= Rank::PLAYER_TARGET_POSX * 2.0f)
			{
				m_nPassingPolice++;
			}

			// �����Ȃ�
			if ((j + 1) % Result::NUM_POLICE == 0) { continue; }

			CPolice* pNext = m_ppPolice[cnt + (j + 1)];

			// �擪�ԗ��̏ꍇ
			if (j == 0)
			{
				// ���W��␳���Ĉړ�������
				D3DXVECTOR3 playpos = pPlayer->GetPosition();
				D3DXVECTOR3 posdest = playpos;
				posdest.x = Rank::PLAYER_TARGET_POSX * 2;
				playpos += ((posdest - playpos) * Rank::PLAYER_INER);

				pPlayer->SetPosition(playpos);
			}
		}
	}
}

//===============================================
// �I������
//===============================================
void CMultiResultStateResult::Uninit(CMultiResult* pResult)
{
	// �x�@�p��
	if (m_ppPolice != nullptr)
	{
		for (int i = 0; i < m_nNumCreatePolice; i++)
		{
			if (m_ppPolice[i] == nullptr) { continue; }
			m_ppPolice[i] = nullptr;
		}

		// �|�C���^���p��
		delete[] m_ppPolice;
		m_ppPolice = nullptr;
	}

	// ��Ԃ��I���ɕύX
	pResult->ChangeState(DEBUG_NEW CMultiResultStateEnd);
}

//*************************************************************************************
// �I���\��ԃN���X �R���X�g���N�^
//===============================================
CMultiResultStateEnd::CMultiResultStateEnd() : CMultiResultState(STATE::STATE_END)
{
	m_pExplosion = nullptr;
	m_fFadeCnt = 0.0f;
}

//===============================================
// �f�X�g���N�^
//===============================================
CMultiResultStateEnd::~CMultiResultStateEnd()
{

}

//===============================================
// ���쏈���֘A
//===============================================
void CMultiResultStateEnd::Controll(CMultiResult* pResult)
{
	CInputKeyboard* pkey = CInputKeyboard::GetInstance();
	CInputPad* ppad = CInputPad::GetInstance();
	CInputPad::BUTTON button1 = CInputPad::BUTTON::BUTTON_A, 
		button2 = CInputPad::BUTTON::BUTTON_START;

	CDeltaTime* pdel = CDeltaTime::GetInstance();

	// �����J�ڎ��ԉ��Z
	if (pdel != nullptr)
	{
		m_fFadeCnt += pdel->GetDeltaTime();
	}

	if (pResult->GetEndStr())
	{
		pResult->GetEndStr()->SetAlpha(fabsf(sinf(m_fFadeCnt)));
	}

	// ���͂܂��͎��Ԍo�߂Ń^�C�g���ɑJ��
	if (m_fFadeCnt >= End::FADE_CNT || ppad->GetTrigger(button1, 0) || ppad->GetTrigger(button2, 0) ||
		pkey->GetTrigger(DIK_RETURN) || pkey->GetTrigger(DIK_SPACE))
	{
		if (m_pExplosion != nullptr)
		{
			SAFE_DELETE(m_pExplosion);
		}
		m_fFadeCnt = End::FADE_CNT;
		CManager::GetInstance()->GetFade()->Set(CScene::MODE::MODE_TITLE);
	}
	else
	{
		// �}�l�[�W���[����
		CMultiResultManager* pMgr = pResult->GetMgr();
		if (pMgr == nullptr) { return; }

		// �v���C���[�擾
		CMultiResult::SPlayerInfo* pInfo = pResult->GetInfo();
		D3DXVECTOR3 rot = pInfo[pMgr->GetNumPlayer() - 1].pPlayer->GetRotation();

		// �����␳
		D3DXVECTOR3 rotdiff;
		rotdiff.x = End::ADD_ROTX - rot.x;
		rotdiff.z = End::ADD_ROTZ - rot.z;
		rotdiff.y = 0.0f;
		Adjust(rotdiff);

		rot += rotdiff * End::ROT_INER;

		rot.y += End::ADD_ROTY;
		Adjust(rot);
		pInfo[pMgr->GetNumPlayer() - 1].pPlayer->SetRotation(rot);
	}
}


//===============================================
// ����
//===============================================
void CMultiResultStateEnd::Create(CMultiResult* pResult)
{
	// �}�l�[�W���[����
	CMultiResultManager* pMgr = pResult->GetMgr();
	if (pMgr == nullptr) { return; }

	// �v���C���[�擾
	CMultiResult::SPlayerInfo* pInfo = pResult->GetInfo();
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	pos.x = (-Rank::PLAYER_TARGET_POSX) * 10;
	m_pExplosion = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\explosion.efkefc", pos, VECTOR3_ZERO, VECTOR3_ZERO, 120.0f, false, false);

	// �v���C���[�擾
	pInfo[pMgr->GetNumPlayer() - 1].pPlayer->GetCharacteter()->GetMotion()->BlendSet(3);
}
