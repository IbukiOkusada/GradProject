//==========================================================
//
// ���\������ [pred_route.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "pred_route.h"
#include "player.h"
#include "road.h"
#include "a_star.h"

// ���O��Ԓ�`
namespace
{
	const int MIN_PRED_NUM = 4;	// �Œ���̓���
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CPredRoute::CPredRoute()
{
	// �l�̃N���A
	m_PassRoad.clear();
	m_pPlayer = nullptr;
	m_pOldRoad = nullptr;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CPredRoute::~CPredRoute()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CPredRoute::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CPredRoute::Uninit(void)
{
	// �J��
	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CPredRoute::Update(void)
{
	if (m_pPlayer == nullptr) { return; }

	// �ړ�����
	if (m_pPlayer->GetRoad() != m_pOldRoad)
	{
		m_pOldRoad = m_pPlayer->GetRoad();

		// ���H�̏�
		if (m_pOldRoad != nullptr)
		{
			// �Ԃ���
			Interp();

			// �m�F
			TurnCheck();
		}
	}
}

//==========================================================
// ����
//==========================================================
CPredRoute* CPredRoute::Create(CPlayer* pPlayer)
{
	CPredRoute* pPredRoute = nullptr;

	pPredRoute = DEBUG_NEW CPredRoute;

	if (pPredRoute != nullptr)
	{
		// ����������
		pPredRoute->m_pPlayer = pPlayer;
		pPredRoute->Init();
	}

	return pPredRoute;
}

//==========================================================
// ��]�m�F
//==========================================================
void CPredRoute::TurnCheck()
{
	// �m�F���ɖ����Ȃ�
	if (m_PassRoad.size() < MIN_PRED_NUM) { return; }
}

//==========================================================
// �Ԃ�⊮����
//==========================================================
void CPredRoute::Interp()
{
	// �܂����o�^
	if (m_PassRoad.size() == 0) {
		// �ʂ��������o����
		m_PassRoad.push_back(m_pOldRoad);
		return;
	}

	CRoad* pOld = m_PassRoad[m_PassRoad.size() - 1];

	// �Ԃ�⊮����
	std::vector<CRoad::SSearch*> path = AStar(pOld->GetSearchSelf(), m_pOldRoad->GetSearchSelf());

	if (path.size() <= 4)
	{
		for (int i = 1; i < path.size(); i++)
		{
			m_PassRoad.push_back(path[i]->pRoad);
		}
	}

	// �ʂ��������o����
	m_PassRoad.push_back(m_pOldRoad);
}

//==========================================================
// ����\�����Ă�
//==========================================================
void CPredRoute::Pred()
{

}