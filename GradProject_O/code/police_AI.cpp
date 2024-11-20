//==========================================================
//
// �p�g�J�[AI���� [police_AI.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "police_AI.h"
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
#include "a_star.h"

// �}�N����`

// �������O��Ԃ��`
namespace
{

}

//==========================================================
// �R���X�g���N�^
//==========================================================
CPoliceAI::CPoliceAI()
{
	// �l�̃N���A
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CPoliceAI::~CPoliceAI()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CPoliceAI::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CPoliceAI::Uninit(void)
{

}

//==========================================================
// �X�V����
//==========================================================
void CPoliceAI::Update(void)
{

}

//==========================================================
// ����
//==========================================================
CPoliceAI *CPoliceAI::Create()
{
	CPoliceAI *pPoliceAI = nullptr;

	pPoliceAI = DEBUG_NEW CPoliceAI;

	if (pPoliceAI != nullptr)
	{
		// ����������
		pPoliceAI->Init();
	}

	return pPoliceAI;
}