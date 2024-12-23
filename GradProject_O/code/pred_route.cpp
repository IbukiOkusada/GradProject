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
#include "deltatime.h"
#include "debugproc.h"

// ���O��Ԓ�`
namespace
{
	const int MIN_PRED_NUM = 3;	// �Œ���̓���
	const float STOP_TIME = 5.0f;	// ��~
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
	m_pPredPrevRoad = nullptr;
	m_pPredRoad = nullptr;
	m_fStopCount = 0.0f;

	memset(&m_aTurnCount[0], 0, sizeof(m_aTurnCount));
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
	m_fStopCount = 0.0f;
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

			// �\���n�_�𗧂Ă�
			Pred();
		}
		else
		{
			m_vecOld = VEC::VEC_OVER;
			m_aTurnCount[VEC::VEC_OVER]++;
		}
	}
	else
	{
		// ��~�J�E���g����
		m_fStopCount += CDeltaTime::GetInstance()->GetDeltaTime();

		// ��~�������Ă���
		if (m_fStopCount >= STOP_TIME && m_pPlayer->GetEngine() <= 0.3f) {
			m_vecOld = VEC::VEC_STOP;
			m_aTurnCount[VEC::VEC_STOP]++;
			m_fStopCount = 0.0f;
		}
	}

	// �f�o�b�O�\��
	CDebugProc::GetInstance()->Print("���i [ %d ] : ����] [ %d ] : �E��] [ %d ] : ���� [ %d ] : ��~ [ %d ] : ���O [ %d ]\n",
		m_aTurnCount[VEC::VEC_STRAIGHT], m_aTurnCount[VEC::VEC_TURNLEFT], m_aTurnCount[VEC::VEC_TURNRIGHT], m_aTurnCount[VEC::VEC_TURNING],
		m_aTurnCount[VEC::VEC_STOP], m_aTurnCount[VEC::VEC_OVER]);
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

	// ���݁A�O��A�O�X�񂩂��]���擾����
	CRoad* pNow = m_PassRoad[m_PassRoad.size() - 1];
	CRoad* pOld = m_PassRoad[m_PassRoad.size() - 2];
	CRoad* pOlder = m_PassRoad[m_PassRoad.size() - 3];

	// �����ꏊ�̏ꍇ�͐��񂵂Ă��Ȃ�
	if (pNow == pOld) {

		// ��~�������Ă���
		if (m_fStopCount >= STOP_TIME && m_pPlayer->GetEngine() <= 0.3f) { 
			m_vecOld = VEC::VEC_STOP;
			m_aTurnCount[VEC::VEC_STOP]++;
			m_fStopCount = 0.0f;
		}
		return;
	}
	// �O�X��ƈꏏ
	else if (pNow == pOlder)
	{
		m_vecOld = VEC::VEC_TURNING;
		m_aTurnCount[VEC::VEC_TURNING]++;
		m_fStopCount = 0.0f;
		return;
	}

	// ���������߂�
	float vecnow = atan2f(pNow->GetPosition().x - pOld->GetPosition().x, 
		pNow->GetPosition().z - pOld->GetPosition().z);
	Adjust(&vecnow);

	// �O��̕��������߂�
	float vecold = atan2f(pOld->GetPosition().x - pOlder->GetPosition().x,
		pOld->GetPosition().z - pOlder->GetPosition().z);
	Adjust(&vecold);

	// ���������߂�
	float diff = vecnow - vecold;
	Adjust(&diff);

	float RANGE = D3DX_PI * 0.25f;
	// ��������Ȃ��������������߂�
	if (diff <= RANGE && diff >= -RANGE)
	{// ����
		if (m_vecOld == VEC::VEC_STRAIGHT) { return; }
		m_vecOld = VEC::VEC_STRAIGHT;
		m_fStopCount = 0.0f;
		m_aTurnCount[VEC::VEC_STRAIGHT]++;
	}
	else if (diff <= -RANGE && diff >= -RANGE * 3)
	{// ���ɋȂ�����
		m_vecOld = VEC::VEC_TURNLEFT;
		m_aTurnCount[VEC::VEC_TURNLEFT]++;
	}
	else if (diff <= RANGE * 3 && diff >= RANGE)
	{// �E�ɋȂ�����
		m_vecOld = VEC::VEC_TURNRIGHT;
		m_aTurnCount[VEC::VEC_TURNRIGHT]++;
	}
	else if (diff >= RANGE * 3 && diff <= -RANGE * 3)
	{// ���񂵂�
		m_vecOld = VEC::VEC_TURNING;
		m_aTurnCount[VEC::VEC_TURNING]++;
	}
	else
	{
		m_vecOld = VEC::VEC_STOP;
		m_aTurnCount[VEC::VEC_STOP]++;
	}
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
	std::vector<CRoad::SSearch*> path = AStar::AStar(pOld->GetSearchSelf(), m_pOldRoad->GetSearchSelf());

	// �����ړ��������ꍇ
	if (pOld->GetPosition().x == m_pOldRoad->GetPosition().x ||
		pOld->GetPosition().z == m_pOldRoad->GetPosition().z)
	{
		// �ʂ��������o����
		m_PassRoad.push_back(m_pOldRoad);
		return;
	}

	// �ŒZ�o�H�ŕ�Ԃ���
	if (path.size() <= MIN_PRED_NUM)
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
	// �m�F���ɖ����Ȃ�
	if (m_PassRoad.size() < MIN_PRED_NUM) { return; }

	// ���Ɍ������������擾����
	for (int i = 0; i < VEC::VEC_MAX; i++)
	{
		float rot = GetNextRot(GetSelectRankVec(i));

		// ���݂̈ʒu���擾����
		CRoad* pRoad = m_pOldRoad;

		bool flag = pRoad->GetJunctionRoad(rot, &m_pPredRoad, &m_pPredPrevRoad);

		if (flag) { 
			break;
		}
	}
}

//==========================================================
// ���̉�]����
//==========================================================
float CPredRoute::GetNextRot(const VEC& vec)
{
	float rot = GetRot(vec);

	// �O��ƍ��񂩂�l���擾
	CRoad* pNow = m_PassRoad[m_PassRoad.size() - 1];
	CRoad* pOld = m_PassRoad[m_PassRoad.size() - 2];

	// ���������߂�
	float vecnow = atan2f(pNow->GetPosition().x - pOld->GetPosition().x,
		pNow->GetPosition().z - pOld->GetPosition().z);
	Adjust(&vecnow);

	vecnow += rot;
	Adjust(&vecnow);

	return vecnow;
}

//==========================================================
// �w�肳�ꂽ���ʂ̉�]�������擾
//==========================================================
CPredRoute::VEC CPredRoute::GetSelectRankVec(const int nRank)
{
	std::vector<VEC> ranklist;
	bool flag[VEC::VEC_MAX] = {false};

	// �S���ʕ��m�F
	for (int j = 0; j < VEC::VEC_MAX; j++)
	{
		VEC vec = VEC::VEC_STRAIGHT;

		for (int i = 0; i < VEC::VEC_MAX; i++)
		{
			auto result = find(ranklist.begin(), ranklist.end(), vec);

			if (m_aTurnCount[i] > m_aTurnCount[vec])
			{
				vec = static_cast<VEC>(i);
			}
			else if (result != ranklist.end())
			{
				vec = static_cast<VEC>(i);
			}
		}

		ranklist.push_back(vec);
		flag[vec] = true;
	}

	return ranklist[nRank];
}

//==========================================================
// �������擾
//==========================================================
float CPredRoute::GetRot(const VEC& vec)
{
	switch (vec)
	{
	case VEC::VEC_STRAIGHT:
	{
		return 0.0f;
	}
	break;

	case VEC::VEC_TURNLEFT:
	{
		return -D3DX_PI * 0.5f;
	}
	break;

	case VEC::VEC_TURNRIGHT:
	{
		return D3DX_PI * 0.5f;
	}
		break;

	case VEC::VEC_TURNING:
	{
		return D3DX_PI;
	}
		break;

	case VEC::VEC_OVER:
	{
		return -D3DX_PI;
	}
		break;

	case VEC::VEC_STOP:
	{
		return D3DX_PI;
	}
		break;

	default:
	{

	}
		break;
	}

	return 0.0f;
}