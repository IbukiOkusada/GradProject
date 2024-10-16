//==========================================================
//
// �ԏ��� [car.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "car.h"
#include "road.h"
#include "road_manager.h"

// �}�N����`

//==========================================================
// �R���X�g���N�^
//==========================================================
CCar::CCar()
{
	// �l�̃N���A
	m_Info.pos = VECTOR3_ZERO;
	m_Info.posOld = VECTOR3_ZERO;
	m_Info.rot = VECTOR3_ZERO;
	m_Info.move = VECTOR3_ZERO;
	m_Info.pRoadStart = nullptr;
	m_Info.pRoadTarget = nullptr;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CCar::~CCar()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CCar::Init(void)
{
	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, "data\\MODEL\\car002.x");
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CCar::Uninit(void)
{
	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CCar::Update(void)
{
	MoveRoad();

	D3DXVECTOR3 vecTarget = m_Info.pos - m_Info.pRoadTarget->GetPosition();
	m_Info.rot.y = atan2f(vecTarget.x, vecTarget.z);

	m_Info.move.x += 0.05f * sinf(m_Info.rot.y);
	m_Info.move.z += 0.05f * cosf(m_Info.rot.y);
	//m_Info.pos += m_Info.move;

	m_Info.pos += (m_Info.pRoadTarget->GetPosition() - m_Info.pos) * 0.1f;

	if (m_pObj != nullptr)
	{
		m_pObj->SetPosition(m_Info.pos);
		m_pObj->SetRotation(m_Info.rot);
	}
}

//==========================================================
// ����
//==========================================================
CCar *CCar::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move)
{
	CCar *pCar = nullptr;

	pCar = new CCar;

	if (pCar != nullptr)
	{
		// ����������
		pCar->Init();

		// ����������
		pCar->Init();

		// ���W�ݒ�
		pCar->SetPosition(pos);

		// �����ݒ�
		pCar->SetRotation(rot);

		// �ړ��ʐݒ�
		pCar->SetMove(move);
	}

	return pCar;
}

//==========================================================
// ���ړ�����
//==========================================================
void CCar::MoveRoad()
{
	if (m_Info.pRoadTarget == nullptr)
		SearchRoad();

	float length = D3DXVec3Length(&(m_Info.pRoadTarget->GetPosition() - m_Info.pos));
	if (length < 250.0f)
		ReachRoad();
}

//==========================================================
// ���T������
//==========================================================
void CCar::SearchRoad()
{
	CRoadManager* pRoadManager = CRoadManager::GetInstance();

	CRoad* pRoad = pRoadManager->GetTop();
	CRoad* pRoadClose = pRoadManager->GetTop();
	float length = D3DXVec3Length(&(pRoadClose->GetPosition() - m_Info.pos));
	float lengthClose = 0.0f;

	while (pRoad != nullptr)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		CRoad* pRoadNext = pRoad->GetNext();	// ���̃I�u�W�F�N�g�ւ̃|�C���^���擾

		// �������菈��
		lengthClose = D3DXVec3Length(&(pRoad->GetPosition() - m_Info.pos));

		if (length > lengthClose)
		{
			length = lengthClose;
			pRoadClose = pRoad;
		}

		pRoad = pRoadNext;	// ���̃I�u�W�F�N�g�Ɉړ�
	}

	m_Info.pRoadTarget = pRoadClose;
}

//==========================================================
// �����B������
//==========================================================
void CCar::ReachRoad()
{
	CRoadManager* pRoadManager = CRoadManager::GetInstance();
	CRoad* pRoadNext = nullptr;

	while (1)
	{// �n�_������܂�
		int roadPoint = rand() % CRoad::DIC_MAX;

		pRoadNext = m_Info.pRoadTarget->GetConnectRoad((CRoad::DIRECTION)roadPoint);

		if (pRoadNext- pRoadNext->TYPE_CROSSING || pRoadNext->TYPE_T_JUNCTION)
		{
			if (pRoadNext == m_Info.pRoadStart)
			{

			}
		}
		else
		{
			
		}

		if (pRoadNext == m_Info.pRoadStart)
		{
			continue;
		}

		if (pRoadNext != nullptr)
			break;
	}

	m_Info.pRoadStart = m_Info.pRoadTarget;
	m_Info.pRoadTarget = pRoadNext;
}