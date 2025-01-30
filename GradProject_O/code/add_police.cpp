//==========================================================
//
// �ǉ��p�g�J�[���� [police.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "police.h"
#include "car.h"
#include "car_manager.h"
#include "road.h"
#include "road_manager.h"
#include "manager.h"
#include "debugproc.h"
#include "collision.h"
#include "player.h"
#include "player_manager.h"
#include "debugproc.h"
#include "police_manager.h"
#include "deltatime.h"
#include "add_police.h"

// ��ԊǗ����ԗp���O���
namespace STATETIME
{
	const float NORMAL = (40.0f);	// �ʏ�
	const float SEARCH = (5.0f);	// �x��
	const float FADEOUT = (2.0f);	// �t�F�[�h�A�E�g
}

// �ÓI�����o�ϐ��錾
Clist<CAddPolice*> CAddPolice::m_List = {};

//==========================================================
// �R���X�g���N�^
//==========================================================
CAddPolice::CAddPolice(int nId) : CPolice(nId, CAR_TYPE::CAR_TYPE_ADDPOLICE),
m_SpawnPos(VECTOR3_ZERO),
m_Path(),
m_nNowRoad(0)
{
	// ���X�g�ɓ����
	m_List.Regist(this);
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CAddPolice::~CAddPolice()
{
	m_List.Delete(this);
}

//==========================================================
// ����
//==========================================================
CAddPolice* CAddPolice::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move, int nId)
{
	if (m_List.GetNum() >= AddPolice::MAX) { return nullptr; }
	CAddPolice* pCar = nullptr;

	pCar = DEBUG_NEW CAddPolice(nId);

	if (pCar != nullptr)
	{
		// ���W�ݒ�
		pCar->SetPosition(pos);
		pCar->SetRecvPosition(pos);

		// ����������
		pCar->Init();

		pCar->m_SpawnPos = pos;

		// �����ݒ�
		pCar->SetRotation(rot);
		pCar->SetRecvRotation(rot);

		// �ړ��ʐݒ�
		pCar->SetMove(move);

		// ��Ԑݒ�
		pCar->SetState(STATE::STATE_NORMAL);
	}

	return pCar;
}

//==========================================================
// �ʏ���
//==========================================================
void CAddPolice::StateNormal(void)
{
	// �J�E���g�Ȃ��Ȃ�܂�
	if (m_stateInfo.fTimer > 0.0f) { return; }

	if (m_Path.size() > 0) { return; }

	SetState(STATE::STATE_FADEOUT);
}

//==========================================================
// �x�����
//==========================================================
void CAddPolice::StateSearch(void)
{
	// �J�E���g�Ȃ��Ȃ�܂�
	if (m_stateInfo.fTimer > 0.0f) { return; }

	SetState(STATE::STATE_FADEOUT);
}

//==========================================================
// �t�F�[�h�A�E�g���
//==========================================================
void CAddPolice::StateFadeOut(void)
{
	// �J�E���g�Ȃ��Ȃ�܂�
	if (m_stateInfo.fTimer > 0.0f) 
	{
		float fMulti = m_stateInfo.fTimer / m_stateInfo.fTimerOrigin;

		m_pObj->SetColMulti(D3DXCOLOR(1.0f, 1.0f, 1.0f, fMulti));
		return; 
	}

	// �폜
	Uninit();
}

//==========================================================
// �x����Ԑݒ�
//==========================================================
void CAddPolice::SetStateNormal(void)
{
	// ������
	m_stateInfo = SState();

	// ��ԕύX
	m_stateInfo.state = STATE::STATE_NORMAL;

	SetStateTimer(STATETIME::NORMAL);
}

//==========================================================
// �x����Ԑݒ�
//==========================================================
void CAddPolice::SetStateSearch(void)
{
	if (m_Path.size() > 0) { return; }

	// ������
	m_stateInfo = SState();

	// ��ԕύX
	m_stateInfo.state = STATE::STATE_SEARCH;

	SetStateTimer(STATETIME::SEARCH);
}

//==========================================================
// �t�F�[�h�A�E�g��Ԑݒ�
//==========================================================
void CAddPolice::SetStateFadeOut(void)
{
	// ������
	m_stateInfo = SState();

	// ��ԕύX
	m_stateInfo.state = STATE::STATE_FADEOUT;

	SetStateTimer(STATETIME::FADEOUT);
}

//==========================================================
// �T��
//==========================================================
void CAddPolice::SearchRoad()
{
	if (m_Path.size() == 0)
	{
		CCar::SearchRoad();
		return;
	}

	if (m_nNowRoad >= static_cast<int>(m_Path.size()) - 1) {

		//if (GetState() == STATE::STATE_NORMAL)
		{
			SetState(STATE::STATE_STOP);
			m_Path.clear();
		}

		return; 
	}
	SetRoadTarget(m_Path[m_nNowRoad]->pRoad);
}

//==========================================================
// �T���I����
//==========================================================
void CAddPolice::ReachRoad()
{
	if (static_cast<int>(m_Path.size()) == 0)
	{
		CCar::ReachRoad();
		return;
	}

	if (m_nNowRoad >= static_cast<int>(m_Path.size()) - 1)
	{ 
			SetState(STATE::STATE_STOP);
			m_Path.clear();

		return; 
	}
	m_nNowRoad++;
	SetRoadTarget(m_Path[m_nNowRoad]->pRoad);
}

//===============================================
// �ǉ��x�@�̍��W�̑��M
//===============================================
void CAddPolice::SendPosition()
{
	if (GetState() == STATE_FADEOUT) { return; }
	CNetWork* pNet = CNetWork::GetInstance();

	pNet->SendAddPdPos(GetId(), GetPosition(), GetRotation().y);
}

//===============================================
// �ǉ��x�@�̒ǐՊJ�n���M
//===============================================
void CAddPolice::SendChase()
{
	if (GetPlayer() == nullptr) { return; }
	CNetWork* pNet = CNetWork::GetInstance();

	pNet->SendAddPdChase(GetId(), GetPlayer()->GetId());
}

//===============================================
// �ǉ��x�@�̒ǐՏI�����M
//===============================================
void CAddPolice::SendChaseEnd()
{
	CNetWork* pNet = CNetWork::GetInstance();

	pNet->SendAddPdChaseEnd(GetId());
}