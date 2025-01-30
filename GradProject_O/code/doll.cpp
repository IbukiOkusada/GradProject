//==========================================================
//
// �l�`�̏��� [doll.cpp]
// Author : Kenta Hashimoto
//
//==========================================================
#include "doll.h"
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
	const float AVOID_MOVE_MAG = 40.0f;
	const float AVOID_ROT = 0.25f;
	const float ROT_INERTIA = 10.0f;
	const float AVOID_COLLISION = 800.0f;
	//const float AVOID_COLLISION_EMERGENCY = 800.0f;
	const char* MODEL_PATH = "data\\TXT\\character\\doll\\motion_doll.txt";

	const D3DXVECTOR3 POS_NIGHTOFFIRE[7] =
	{
		D3DXVECTOR3(32000.0f, 0.0f, -3000.0f),
		D3DXVECTOR3(32100.0f, 0.0f, -2500.0f),
		D3DXVECTOR3(32200.0f, 0.0f, -3000.0f),
		D3DXVECTOR3(32400.0f, 0.0f, -2200.0f),
		D3DXVECTOR3(32200.0f, 0.0f, -2200.0f),
		D3DXVECTOR3(32000.0f, 0.0f, -2200.0f),
		D3DXVECTOR3(31800.0f, 0.0f, -2200.0f),
	};

	const D3DXVECTOR3 POS_DROPS[7] =
	{
		D3DXVECTOR3(32200.0f, 0.0f, -2500.0f),
		D3DXVECTOR3(31900.0f, 0.0f, -2200.0f),
		D3DXVECTOR3(32400.0f, 0.0f, -2500.0f),
		D3DXVECTOR3(32000.0f, 0.0f, -2500.0f),
		D3DXVECTOR3(32100.0f, 0.0f, -2200.0f),
		D3DXVECTOR3(32300.0f, 0.0f, -2200.0f),
		D3DXVECTOR3(31800.0f, 0.0f, -2500.0f),
	};
}

int CDoll::m_nNumDance = -1;

//==========================================================
// �R���X�g���N�^
//==========================================================
CDoll::CDoll()
{
	// �l�̃N���A
	m_pCharacter = nullptr;
	m_Info = SInfo();
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CDoll::~CDoll()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CDoll::Init()
{
	return S_OK;
}

//==========================================================
// ����������
//==========================================================
HRESULT CDoll::Init(const D3DXVECTOR3& rot)
{
	m_pCharacter = CCharacter::Create(MODEL_PATH);
	m_pCharacter->SetParent(NULL);
	m_pCharacter->SetScale(D3DXVECTOR3(7.0f, 7.0f, 7.0f));

	if (m_nNumDance == -1)
	{
		m_nNumDance = rand() % 2;
	}

	if (m_nNumDance == 0)
	{
		m_Info.state = STATE_NIGHTOFFIRE;

		if (m_Info.nId == 0 || m_Info.nId == 2)
		{
			m_pCharacter->GetMotion()->InitSet(MOTION::MOTION_WAIT);
		}
		else
		{
			m_pCharacter->GetMotion()->InitSet(MOTION::MOTION_NIGHTOFFIRE);
		}
	}
	if (m_nNumDance == 1)
	{
		m_Info.state = STATE_DROPS_FIRST;
		m_pCharacter->GetMotion()->InitSet(MOTION::MOTION_DROPS_FIRST);
	}

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CDoll::Uninit(void)
{
	SAFE_UNINIT(m_pCharacter)

	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CDoll::Update(void)
{
	Set();
	m_nNumDance = -1;

	if (m_pCharacter != nullptr)
	{
		m_pCharacter->Update();

		Dance();
	}

	CDebugProc::GetInstance()->Print(" �����[%f %f %f]\n", GetPosition().x, GetPosition().y, GetPosition().z);
}

//==========================================================
// ����
//==========================================================
CDoll* CDoll::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, int nID)
{
	CDoll* pRobot = nullptr;

	pRobot = DEBUG_NEW CDoll;

	if (pRobot != nullptr)
	{
		// ID�ݒ�
		pRobot->SetID(nID);

		// ����������
		pRobot->Init(rot);

		// ���W�ݒ�
		pRobot->SetPosition(pos);

		// �����ݒ�
		pRobot->SetRotation(rot);

		
	}

	return pRobot;
}

//==========================================================
// ���W�ݒ�
//==========================================================
void CDoll::SetPosition(const D3DXVECTOR3& pos)
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
void CDoll::SetRotation(const D3DXVECTOR3& rot)
{
	m_Info.rot = rot;
	correction::Adjust(&m_Info.rot);

	if (m_pCharacter != nullptr)
	{
		m_pCharacter->SetRotation(m_Info.rot);
	}
}

//==========================================================
// �ݒ�
//==========================================================
void CDoll::Set()
{
	if (m_pCharacter != nullptr)
	{
		m_pCharacter->SetPosition(m_Info.pos);
		m_pCharacter->SetRotation(m_Info.rot);
	}
}

//==========================================================
// �x��ݒ�
//==========================================================
void CDoll::Dance(void)
{
	if (m_Info.state == STATE_NIGHTOFFIRE)
	{
		m_Info.pos = POS_NIGHTOFFIRE[m_Info.nId];

		if (m_Info.nId == 0 || m_Info.nId == 2)
		{
			m_Info.rot.y = 3.14f;
		}
	}

	if (m_Info.state >= STATE_DROPS_FIRST && m_Info.state <= STATE_DROPS_POSE)
	{
		m_Info.pos = POS_DROPS[m_Info.nId];
	}

	// ���[�V�������I�������
	if (!m_pCharacter->GetMotion()->GetEnd()) { return; }

	if (m_Info.state == STATE_DROPS_FIRST)
	{
		m_pCharacter->GetMotion()->BlendSet(MOTION::MOTION_DROPS_POSE1 + m_Info.nId);
		m_Info.state = STATE_DROPS_POSE;
		return;
	}

	if (m_Info.state == STATE_DROPS_POSE)
	{
		m_pCharacter->GetMotion()->BlendSet(MOTION::MOTION_DROPS_SECOND);
		m_Info.state = STATE_DROPS_SECOND;
		return;
	}

	if (m_Info.state == STATE_DROPS_SECOND)
	{
		m_pCharacter->GetMotion()->InitSet(MOTION::MOTION_DROPS_FIRST);
		m_Info.state = STATE_DROPS_FIRST;
		return;
	}
}