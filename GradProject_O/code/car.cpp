//==========================================================
//
// �ԏ��� [car.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "car.h"
#include "car_manager.h"
#include "road.h"
#include "road_manager.h"
#include "manager.h"
#include "debugproc.h"
#include "collision.h"
#include "deltatime.h"
#include "slow.h"
#include "player.h"
#include "player_manager.h"

// �}�N����`

// �������O��Ԃ��`
namespace
{
	const float SPEED_STREET = (15.0f);		// ���i���̑��x
	const float SPEED_CURVE = (10.0f);		// �J�[�u���̑��x
	const float SPEED_INER = (0.05f);		// ���x�̊���
	const float ROT_MULTI = (0.06f);		// �����␳�{��
	const float ROT_CURVE = (0.15f);		// �J�[�u����p�x
	const float LENGTH_POINT = (200.0f);	// ���B���苗��
	const float FRAME_RATE_SCALER = 60.0f;  // �t���[�����[�g���l���������x�̒���
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CCar::CCar()
{
	// �l�̃N���A
	m_Info.pos = VECTOR3_ZERO;
	m_Info.posOld = VECTOR3_ZERO;
	m_Info.rot = VECTOR3_ZERO;
	m_Info.rotDest = VECTOR3_ZERO;
	m_Info.move = VECTOR3_ZERO;
	m_Info.pRoadStart = nullptr;
	m_Info.pRoadTarget = nullptr;
	m_Info.speed = 0.0f;
	m_Info.speedDest = 0.0f;
	m_Info.bBreak = false;
	m_Info.bBack = false;
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
	TailLamp();
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CCar::Uninit(void)
{
	// �`��I�u�W�F�N�g��p��
	SAFE_UNINIT(m_pObj);
	SAFE_DELETE(m_pTailLamp);
	CCarManager::GetInstance()->ListOut(this);

	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CCar::Update(void)
{
	m_Info.posOld = m_Info.pos;
	m_Info.speedDest = 0.0f;

	// �ړ���̌���
	MoveRoad();

	// �ړ�����
	Move();

	// �����蔻�菈��
	Collision();

	if (m_pObj != nullptr)
	{
		m_Info.rot.y += D3DX_PI;
		m_pObj->SetPosition(m_Info.pos);
		m_pObj->SetRotation(m_Info.rot);
		m_Info.rot.y -= D3DX_PI;
	}
	m_pTailLamp->m_pos = m_Info.pos;
	m_pTailLamp->m_rot = m_Info.rot;
}

//==========================================================
// ����
//==========================================================
CCar *CCar::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move)
{
	CCar *pCar = nullptr;

	pCar = DEBUG_NEW CCar;

	if (pCar != nullptr)
	{
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
void CCar::TailLamp()
{
	m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\taillamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);

}
//==========================================================
// �ړ�����
//==========================================================
void CCar::Move()
{
	if (!m_Info.bBreak)
	{
		// �p�x����
		Rot();

		m_Info.speed += (m_Info.speedDest - m_Info.speed) * SPEED_INER;
		CManager::GetInstance()->GetDebugProc()->Print("�Ԃ̑��x [ %f ]\n", m_Info.speed);

		m_Info.move.x = m_Info.speed * sinf(m_Info.rot.y);
		m_Info.move.y = 0.0f;
		m_Info.move.z = m_Info.speed * cosf(m_Info.rot.y);

		// �f���^�^�C���擾
		float DeltaTime = CDeltaTime::GetInstance()->GetDeltaTime();

		m_Info.pos += m_Info.move * FRAME_RATE_SCALER * DeltaTime;
	}
}

//==========================================================
// �p�x���菈��
//==========================================================
void CCar::Rot()
{
	float fRotMove, fRotDest, fRotDiff;				//�p�x�����p�ϐ�

	D3DXVECTOR3 vecTarget = m_Info.posTarget - m_Info.pos;

	fRotMove = m_Info.rot.y;
	fRotDest = atan2f(vecTarget.x, vecTarget.z);
	fRotDiff = fRotDest - fRotMove;

	if (fRotDiff > D3DX_PI)
	{
		fRotDiff -= D3DX_PI * 2.0f;
	}
	else if (fRotDiff < -D3DX_PI)
	{
		fRotDiff += D3DX_PI * 2.0f;
	}

	//�����ǉ�
	fRotMove += fRotDiff * ROT_MULTI;

	//�p�x��v����
	if (fabsf(fRotDiff) >= ROT_CURVE)
	{
		m_Info.speedDest += SPEED_CURVE;
	}
	else
	{
		m_Info.speedDest += SPEED_STREET;
	}

	if (fRotMove > D3DX_PI)
	{
		fRotMove -= D3DX_PI * 2.0f;
	}
	else if (fRotMove <= -D3DX_PI)
	{
		fRotMove += D3DX_PI * 2.0f;
	}

	m_Info.rot.y = fRotMove;

	if (m_Info.rot.y > D3DX_PI)
	{
		m_Info.rot.y -= D3DX_PI * 2.0f;
	}
	else if (m_Info.rot.y <= -D3DX_PI)
	{
		m_Info.rot.y += D3DX_PI * 2.0f;
	}
}

//==========================================================
// ���ړ�����
//==========================================================
void CCar::MoveRoad()
{
	if (m_Info.pRoadTarget == nullptr)
		SearchRoad();

	float length = D3DXVec3Length(&(m_Info.pRoadTarget->GetPosition() - m_Info.pos));
	if (length < LENGTH_POINT)
		ReachRoad();

	m_Info.posTarget = m_Info.pRoadTarget->GetPosition();
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

		if (m_Info.pRoadTarget->GetType() == CRoad::TYPE_STOP)
		{
			
		}
		else
		{
			if (pRoadNext == m_Info.pRoadStart)
			{
				continue;
			}
		}

		if (pRoadNext != nullptr)
			break;
	}

	m_Info.pRoadStart = m_Info.pRoadTarget;
	m_Info.pRoadTarget = pRoadNext;
}

//==========================================================
// �����蔻�菈��
//==========================================================
bool CCar::Collision()
{
	CObjectX* pObjectX = CObjectX::GetTop();	// �擪���擾

	while (pObjectX != nullptr)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		CObjectX* pObjectXNext = pObjectX->GetNext();	// ���̃I�u�W�F�N�g�ւ̃|�C���^���擾

		D3DXVECTOR3 posObjectX = pObjectX->GetPosition();
		D3DXVECTOR3 rotObjectX = pObjectX->GetRotation();
		D3DXVECTOR3 sizeMax = pObjectX->GetVtxMax();
		D3DXVECTOR3 sizeMin = pObjectX->GetVtxMin();

		bool bCollision = collision::CollidePointToOBB(&m_Info.pos, m_Info.posOld, posObjectX, rotObjectX, (sizeMax - sizeMin) * 0.5f);

		if (bCollision)
		{
			CRoad* pRoadNext = m_Info.pRoadTarget;
			m_Info.pRoadTarget = m_Info.pRoadStart;
			m_Info.pRoadStart = pRoadNext;

			CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
			if (pPlayer->GetModelIndex() == pObjectX->GetIdx())
			{
				m_Info.bBreak = true;
			}

			return true;
		}

		pObjectX = pObjectXNext;	// ���̃I�u�W�F�N�g�Ɉړ�
	}

	return false;
}