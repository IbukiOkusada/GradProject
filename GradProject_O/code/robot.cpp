//==========================================================
//
// ���{�b�g�̏��� [robot.cpp]
// Author : Kenta Hashimoto
//
//==========================================================
#include "robot.h"
#include "robot_manager.h"
#include "manager.h"

// �}�N����`

//==========================================================
// �R���X�g���N�^
//==========================================================
CRobot::CRobot()
{
	// �l�̃N���A
	m_pNext = nullptr;
	m_pPrev = nullptr;

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
HRESULT CRobot::Init(void)
{
	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, "data\\MODEL\\sample_box.x");

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CRobot::Uninit(void)
{
	CRobotManager::GetInstance()->ListOut(this);

	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CRobot::Update(void)
{
	// �Ίp���̊p�x���Z�o
	m_Info.rot.y = atan2f(m_Info.pos.x - m_PosTarget[m_Info.nTargetID].x,
						  m_Info.pos.z - m_PosTarget[m_Info.nTargetID].z);

	m_Info.move.x = -sinf(D3DX_PI * 0.0f + m_Info.rot.y) * 10.0f;
	m_Info.move.z = -cosf(D3DX_PI * 0.0f + m_Info.rot.y) * 10.0f;

	m_Info.pos += m_Info.move;

	bool bReach = TergetReach();

	if (bReach)
	{
		if (m_Info.nTargetID == 0)
		{
			m_Info.nTargetID = 1;
		}
		else if (m_Info.nTargetID == 1)
		{
			m_Info.nTargetID = 0;
		}
	}

	Set();
}

//==========================================================
// ����
//==========================================================
CRobot* CRobot::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move, const float& Distance)
{
	CRobot* pRobot = nullptr;

	pRobot = DEBUG_NEW CRobot;

	if (pRobot != nullptr)
	{
		// ����������
		pRobot->Init();

		// ���W�ݒ�
		pRobot->SetPosition(pos);

		// �����ݒ�
		pRobot->SetRotation(rot);

		// �ړ��ʐݒ�
		pRobot->SetMove(move);

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

	if (m_pObj != nullptr)
	{
		m_pObj->SetPosition(m_Info.pos);
	}
}

//==========================================================
// �����ݒ�
//==========================================================
void CRobot::SetRotation(const D3DXVECTOR3& rot)
{
	m_Info.rot = rot;

	if (m_pObj != nullptr)
	{
		m_pObj->SetRotation(m_Info.rot);
	}
}

//==========================================================
// �ݒ�
//==========================================================
void CRobot::Set()
{
	if (m_pObj != nullptr)
	{
		m_pObj->SetPosition(m_Info.pos);
		m_pObj->SetRotation(m_Info.rot);
	}
}

//==========================================================
// �ڕW�ʒu�̐ݒ�
//==========================================================
void CRobot::SetPosTerget(const float& Distance)
{
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
	bool bReach = false;

	if (m_Info.move.x > 0.0f)
	{
		if (m_Info.move.z > 0.0f)
		{
			if (m_Info.pos.z >= m_PosTarget[m_Info.nTargetID].z && m_Info.pos.x >= m_PosTarget[m_Info.nTargetID].x)
			{
				m_Info.pos.z = m_PosTarget[m_Info.nTargetID].z;
				m_Info.pos.x = m_PosTarget[m_Info.nTargetID].x;
				bReach = true;
			}
		}

		else if (m_Info.move.z < 0.0f)
		{
			if (m_Info.pos.z <= m_PosTarget[m_Info.nTargetID].z && m_Info.pos.x >= m_PosTarget[m_Info.nTargetID].x)
			{
				m_Info.pos.z = m_PosTarget[m_Info.nTargetID].z;
				m_Info.pos.x = m_PosTarget[m_Info.nTargetID].x;
				bReach = true;
			}
		}
	}

	else if (m_Info.move.x < 0.0f)
	{
		if (m_Info.move.z > 0.0f)
		{
			if (m_Info.pos.z >= m_PosTarget[m_Info.nTargetID].z && m_Info.pos.x <= m_PosTarget[m_Info.nTargetID].x)
			{
				m_Info.pos.z = m_PosTarget[m_Info.nTargetID].z;
				m_Info.pos.x = m_PosTarget[m_Info.nTargetID].x;
				bReach = true;
			}
		}

		else if (m_Info.move.z < 0.0f)
		{
			if (m_Info.pos.z <= m_PosTarget[m_Info.nTargetID].z && m_Info.pos.x <= m_PosTarget[m_Info.nTargetID].x)
			{
				m_Info.pos.z = m_PosTarget[m_Info.nTargetID].z;
				m_Info.pos.x = m_PosTarget[m_Info.nTargetID].x;
				bReach = true;
			}
		}
	}

	return bReach;
}
