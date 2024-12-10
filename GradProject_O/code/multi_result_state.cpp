//===============================================
//
// �}���`���U���g��ʂ̏�ԊǗ�.cpp [multi_result_state.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "multi_result_state.h"
#include "multi_result_manager.h"
#include "multi_result.h"
#include "scrollText2D.h"
#include "player.h"

//===============================================
// �萔��`
//===============================================
namespace
{
	const float PLAYER_INER = 0.075f;			// �v���C���[�ړ��ʊ���
	const float PLAYER_TARGET_POSZ = -800.0f;	// �v���C���[�����N�\�����ڕWZ���W
	const float PLAYER_SPACE = 275.0f;
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
	posdest.x = PLAYER_TARGET_POSZ;
	pos += ((posdest - pos) * PLAYER_INER);

	pPlayer->SetPosition(pos);
}

//*************************************************************************************
// ���ʕ\����ԃN���X �R���X�g���N�^
//===============================================
CMultiResultStateResult::CMultiResultStateResult() : CMultiResultState(STATE::STATE_MULTIRANK)
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
	// �������Ȃ�
	if (pResult->GetEndStr() == nullptr) { return; }

	// �������X�N���[������Ă���
	if (pResult->GetEndStr()->IsScroll()) { return; }

	pResult->GetEndStr()->SetEnableScroll(true);
}

//*************************************************************************************
// �I���\��ԃN���X �R���X�g���N�^
//===============================================
CMultiResultStateEnd::CMultiResultStateEnd() : CMultiResultState(STATE::STATE_END)
{

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

}
