//==========================================================
//
// ���{�b�g�̏��� [robot.cpp]
// Author : Kenta Hashimoto
//
//==========================================================
#include "robot.h"
#include "robot_manager.h"
#include "manager.h"
#include "character.h"
#include "motion.h"
#include "player.h"
#include "player_manager.h"
#include "debugproc.h"
#include "edit_manager.h"
#include "bridge.h"

//==========================================================
// �萔��`
//==========================================================
namespace
{
	const float WALK_MOVE_MAG = 10.0f;
	const float AVOID_MOVE_MAG = 70.0f;
	const float AVOID_ROT = 0.25f;
	const float ROT_INERTIA = 10.0f;
	const float AVOID_COLLISION = 800.0f;
	//const float AVOID_COLLISION_EMERGENCY = 800.0f;
	const char* MODEL_PATH = "data\\TXT\\character\\robot\\motion_robot.txt";
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CRobot::CRobot()
{
	// �l�̃N���A
	m_pNext = nullptr;
	m_pPrev = nullptr;
	m_pCharacter = nullptr;
	m_PosTarget[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_PosTarget[1] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	CRobotManager::GetInstance()->ListIn(this);
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CRobot::~CRobot()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CRobot::Init()
{
	//m_pCharacter = CCharacter::Create(MODEL_PATH);
	//m_pCharacter->SetParent(NULL);
	//m_pCharacter->GetMotion()->InitSet(MOTION::MOTION_WALK);
	//m_pCharacter->SetScale(D3DXVECTOR3(5.0f, 5.0f, 5.0f));

	return S_OK;
}

//==========================================================
// ����������
//==========================================================
HRESULT CRobot::Init(const D3DXVECTOR3& rot)
{
	m_pCharacter = CCharacter::Create(MODEL_PATH);
	m_pCharacter->SetParent(NULL);
	m_pCharacter->GetMotion()->InitSet(MOTION::MOTION_WALK);
	m_pCharacter->SetScale(D3DXVECTOR3(5.0f, 5.0f, 5.0f));

	// �ړ��ʂ̏����l�ݒ�
	m_Info.move.x = -sinf(rot.y) * WALK_MOVE_MAG;
	m_Info.move.z = -cosf(rot.y) * WALK_MOVE_MAG;

	m_Info.state = STATE_WALK;

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CRobot::Uninit(void)
{
	SAFE_UNINIT(m_pCharacter)
	CRobotManager::GetInstance()->ListOut(this);

	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CRobot::Update(void)
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();

	switch (m_Info.state)
	{
	case STATE_NONE:
		
		if (CEditManager::GetInstance() == nullptr)
		{// �G�f�B�b�g�}�l�[�W����null�̎�

			// �ړ���Ԃɂ���
			m_Info.state = STATE_WALK;
		}

		break;

	case STATE_WALK:

		Walk();
		AvoidCollision(pPlayer->GetPosition(), AVOID_COLLISION);

		break;

	case STATE_AVOID:

		Avoid();
		AvoidCollision(pPlayer->GetPosition(), AVOID_COLLISION);

		if (m_Info.move.x <= 3.0f && m_Info.move.z <= 3.0f &&
			m_Info.move.x >= -3.0f && m_Info.move.z >= -3.0f)
		{// �قڐÎ~��Ԃ̎�
			SetWalk();
		}

		break;
	}

	CollisionObjX();
	Set();

	if (m_pCharacter != nullptr)
	{
		m_pCharacter->Update();
	}
}

//==========================================================
// ����
//==========================================================
CRobot* CRobot::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const float& Distance)
{
	CRobot* pRobot = nullptr;

	pRobot = DEBUG_NEW CRobot;

	if (pRobot != nullptr)
	{
		// ����������
		pRobot->Init(rot);

		// ���W�ݒ�
		pRobot->SetPosition(pos);

		// �����ݒ�
		pRobot->SetRotation(rot);

		// �ڕW�ʒu�ݒ�
		pRobot->SetPosTerget(Distance);
	}

	return pRobot;
}

//==========================================================
// ���W�ݒ�
//==========================================================
void CRobot::SetPosition(const D3DXVECTOR3& pos)
{
	m_Info.pos = pos;

	if (m_pCharacter != nullptr)
	{
		m_pCharacter->SetPosition(m_Info.pos);
	}
}

//==========================================================
// �����ݒ�
//==========================================================
void CRobot::SetRotation(const D3DXVECTOR3& rot)
{
	m_Info.rot = rot;

	if (m_pCharacter != nullptr)
	{
		m_pCharacter->SetRotation(m_Info.rot);
	}
}

//==========================================================
// �ݒ�
//==========================================================
void CRobot::Set()
{
	if (m_pCharacter != nullptr)
	{
		m_pCharacter->SetPosition(m_Info.pos);
		m_pCharacter->SetRotation(m_Info.rot);
	}
}

//==========================================================
// �ڕW�ʒu�̐ݒ�
//==========================================================
void CRobot::SetPosTerget(const float& Distance)
{
	m_Info.fDistance = Distance;

	m_PosTarget[0].x = m_Info.pos.x + sinf(m_Info.rot.y) * Distance;
	m_PosTarget[0].z = m_Info.pos.z + cosf(m_Info.rot.y) * Distance;
	m_PosTarget[1].x = m_Info.pos.x + sinf(m_Info.rot.y) * -Distance;
	m_PosTarget[1].z = m_Info.pos.z + cosf(m_Info.rot.y) * -Distance;
}

//==========================================================
// �ڕW�ʒu�ɓ��B�������ǂ���
//==========================================================
bool CRobot::TergetReach()
{
	bool Reach = false;

	if (m_Info.move.x == 0.0f)
	{
		if (m_Info.move.z > 0.0f)
		{
			if (m_Info.pos.z >= m_PosTarget[m_Info.nTargetID].z)
			{
				m_Info.pos.z = m_PosTarget[m_Info.nTargetID].z;
				m_Info.pos.x = m_PosTarget[m_Info.nTargetID].x;
				Reach = true;
			}
		}

		else if (m_Info.move.z < 0.0f)
		{
			if (m_Info.pos.z <= m_PosTarget[m_Info.nTargetID].z)
			{
				m_Info.pos.z = m_PosTarget[m_Info.nTargetID].z;
				m_Info.pos.x = m_PosTarget[m_Info.nTargetID].x;
				Reach = true;
			}
		}
	}
	
	else if (m_Info.move.x > 0.0f)
	{
		if (m_Info.move.z == 0.0f)
		{
			if (m_Info.pos.x >= m_PosTarget[m_Info.nTargetID].x)
			{
				m_Info.pos.z = m_PosTarget[m_Info.nTargetID].z;
				m_Info.pos.x = m_PosTarget[m_Info.nTargetID].x;
				Reach = true;
			}
		}
		
		else if (m_Info.move.z > 0.0f)
		{
			if (m_Info.pos.z >= m_PosTarget[m_Info.nTargetID].z && m_Info.pos.x >= m_PosTarget[m_Info.nTargetID].x)
			{
				m_Info.pos.z = m_PosTarget[m_Info.nTargetID].z;
				m_Info.pos.x = m_PosTarget[m_Info.nTargetID].x;
				Reach = true;
			}
		}

		else if (m_Info.move.z < 0.0f)
		{
			if (m_Info.pos.z <= m_PosTarget[m_Info.nTargetID].z && m_Info.pos.x >= m_PosTarget[m_Info.nTargetID].x)
			{
				m_Info.pos.z = m_PosTarget[m_Info.nTargetID].z;
				m_Info.pos.x = m_PosTarget[m_Info.nTargetID].x;
				Reach = true;
			}
		}
	}

	else if (m_Info.move.x < 0.0f)
	{
		if (m_Info.move.z == 0.0f)
		{
			if (m_Info.pos.x <= m_PosTarget[m_Info.nTargetID].x)
			{
				m_Info.pos.z = m_PosTarget[m_Info.nTargetID].z;
				m_Info.pos.x = m_PosTarget[m_Info.nTargetID].x;
				Reach = true;
			}
		}
		
		else if (m_Info.move.z > 0.0f)
		{
			if (m_Info.pos.z >= m_PosTarget[m_Info.nTargetID].z && m_Info.pos.x <= m_PosTarget[m_Info.nTargetID].x)
			{
				m_Info.pos.z = m_PosTarget[m_Info.nTargetID].z;
				m_Info.pos.x = m_PosTarget[m_Info.nTargetID].x;
				Reach = true;
			}
		}

		else if (m_Info.move.z < 0.0f)
		{
			if (m_Info.pos.z <= m_PosTarget[m_Info.nTargetID].z && m_Info.pos.x <= m_PosTarget[m_Info.nTargetID].x)
			{
				m_Info.pos.z = m_PosTarget[m_Info.nTargetID].z;
				m_Info.pos.x = m_PosTarget[m_Info.nTargetID].x;
				Reach = true;
			}
		}
	}

	return Reach;
}

//==========================================================
// ��������
//==========================================================
void CRobot::Walk()
{
	m_Info.pos += m_Info.move;

	m_Info.rot.y += (m_Info.rotDest.y - m_Info.rot.y) / ROT_INERTIA;

	if (TergetReach() == true)
	{
		if (m_Info.nTargetID == 0)
		{
			m_Info.nTargetID = 1;
		}
		else if (m_Info.nTargetID == 1)
		{
			m_Info.nTargetID = 0;
		}

		// �Ίp���̊p�x���Z�o
		m_Info.rotDest.y = atan2f(m_Info.pos.x - m_PosTarget[m_Info.nTargetID].x,
			m_Info.pos.z - m_PosTarget[m_Info.nTargetID].z);

		m_Info.move.x = -sinf(m_Info.rotDest.y) * WALK_MOVE_MAG;
		m_Info.move.z = -cosf(m_Info.rotDest.y) * WALK_MOVE_MAG;
	}
}

//==========================================================
// �������
//==========================================================
void CRobot::Avoid()
{
	m_Info.pos += m_Info.move;
	m_Info.rot.y += (m_Info.rotDest.y - m_Info.rot.y) / 4.0f;

	m_Info.move.x = m_Info.move.x - m_Info.move.x / 12.0f;
	m_Info.move.z = m_Info.move.z - m_Info.move.z / 12.0f;
}

//==========================================================
// ����R���W����
//==========================================================
void CRobot::AvoidCollision(D3DXVECTOR3 pos, const float CollisionDistance)
{
	if (sqrtf((m_Info.pos.x - pos.x) * (m_Info.pos.x - pos.x)
		+ (m_Info.pos.z - pos.z) * (m_Info.pos.z - pos.z)) <= CollisionDistance)
	{
		m_pCharacter->GetMotion()->Set(MOTION::MOTION_AVOID);
		m_Info.state = STATE_AVOID;

		CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();

		SetAvoid(pos, pPlayer->GetRotation());
	}
}

//===============================================
// �I�u�W�F�N�g�Ƃ̓����蔻�菈��
//===============================================
bool CRobot::CollisionObjX()
{
	auto mgr = CObjectX::GetList();
	for (int i = 0; i < mgr->GetNum(); i++)
	{// �g�p����Ă��Ȃ���Ԃ܂�

		CObjectX* pObjectX = mgr->Get(i);	// �擪���擾

		D3DXVECTOR3 posObjectX = pObjectX->GetPosition();
		D3DXVECTOR3 rotObjectX = pObjectX->GetRotation();
		D3DXVECTOR3 sizeMax = pObjectX->GetVtxMax();
		D3DXVECTOR3 sizeMin = pObjectX->GetVtxMin();
		D3DXVECTOR3 pVecCollision;

		bool bCollision = collision::CollidePointToOBB(&pVecCollision, &m_Info.pos, m_Info.posOld, posObjectX, rotObjectX, (sizeMax - sizeMin) * 0.5f);
		if (bCollision)
		{
			return true;
		}
	}

	return false;
}

//==========================================================
// ����̐ݒ�
//==========================================================
void CRobot::SetAvoid(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// �Ώە��Ƃ̊p�x���Z�o	
	m_Info.rotDest.y = atan2f(m_Info.pos.x - pos.x, m_Info.pos.z - pos.z);
	D3DXVECTOR3 rotCal = VECTOR3_ZERO;

	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();

	if (pPlayer->GetPosition() == pos && pPlayer->GetRotation() == rot)
	{// �������v���C���[�̒l��������
		rot.y = rot.y - D3DX_PI / 2;
	}

	if (rot.y > 0.0f)
	{
		rot.y -= D3DX_PI;
	}
	else if (rot.y < 0.0f)
	{
		rot.y += D3DX_PI;
	}

	rotCal.y = m_Info.rotDest.y - rot.y;

	// �v���C���[�̌����Ă�������ɂ���ĉ�������ς���
	if (rotCal.y < 0.0f)
	{
		m_Info.rotDest.y += D3DX_PI * -AVOID_ROT;

		m_Info.move.x = sinf(m_Info.rotDest.y) * AVOID_MOVE_MAG;
		m_Info.move.z = cosf(m_Info.rotDest.y) * AVOID_MOVE_MAG;
	}
	else if(rotCal.y > 0.0f)
	{
		m_Info.rotDest.y += D3DX_PI * AVOID_ROT;
		
		m_Info.move.x = sinf(m_Info.rotDest.y) * AVOID_MOVE_MAG;
		m_Info.move.z = cosf(m_Info.rotDest.y) * AVOID_MOVE_MAG;
	}
}

//==========================================================
// ������ݒ�
//==========================================================
void CRobot::SetWalk()
{
	m_pCharacter->GetMotion()->Set(MOTION::MOTION_WALK);
	m_Info.state = STATE_WALK;

	m_Info.rotDest.y = atan2f(m_Info.pos.x - m_PosTarget[m_Info.nTargetID].x,
		m_Info.pos.z - m_PosTarget[m_Info.nTargetID].z);

	m_Info.move.x = -sinf(m_Info.rotDest.y) * WALK_MOVE_MAG;
	m_Info.move.z = -cosf(m_Info.rotDest.y) * WALK_MOVE_MAG;
}