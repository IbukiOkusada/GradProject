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
	const float NORMAL = (10.0f);	// �ʏ�
	const float SEARCH = (5.0f);	// �x��
	const float FADEOUT = (2.0f);	// �t�F�[�h�A�E�g
}

// �ÓI�����o�ϐ��錾
Clist<CAddPolice*> CAddPolice::m_List = {};

//==========================================================
// �R���X�g���N�^
//==========================================================
CAddPolice::CAddPolice()
{
	m_SpawnPos = VECTOR3_ZERO;
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
CAddPolice* CAddPolice::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move)
{
	CAddPolice* pCar = nullptr;

	pCar = DEBUG_NEW CAddPolice;

	if (pCar != nullptr)
	{
		// ����������
		pCar->Init();

		// ���W�ݒ�
		pCar->SetPosition(pos);
		pCar->m_SpawnPos = pos;

		// �����ݒ�
		pCar->SetRotation(rot);

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

	CDebugProc::GetInstance()->Print("���������������`�I\n");

	// �폜
	Uninit();
}

//==========================================================
// �x����Ԑݒ�
//==========================================================
void CAddPolice::SetStateNormal(void)
{
	SetStateTimer(STATETIME::NORMAL);
}

//==========================================================
// �x����Ԑݒ�
//==========================================================
void CAddPolice::SetStateSearch(void)
{
	SetStateTimer(STATETIME::SEARCH);
}

//==========================================================
// �t�F�[�h�A�E�g��Ԑݒ�
//==========================================================
void CAddPolice::SetStateFadeOut(void)
{
	SetStateTimer(STATETIME::FADEOUT);
}