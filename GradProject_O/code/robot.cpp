//==========================================================
//
// ロボットの処理 [robot.cpp]
// Author : Kenta Hashimoto
//
//==========================================================
#include "robot.h"
#include "robot_manager.h"
#include "manager.h"

//==========================================================
// 定数定義
//==========================================================
namespace
{
	const float MOVE_MAG = 10.0f;
	const float ROT_INERTIA = 10.0f;
	const char* MODEL_PATH = "data\\MODEL\\sample_box.x";
}

//==========================================================
// コンストラクタ
//==========================================================
CRobot::CRobot()
{
	// 値のクリア
	m_pNext = nullptr;
	m_pPrev = nullptr;
	m_PosTarget[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_PosTarget[1] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	CRobotManager::GetInstance()->ListIn(this);
}

//==========================================================
// デストラクタ
//==========================================================
CRobot::~CRobot()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CRobot::Init()
{
	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, MODEL_PATH);

	return S_OK;
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CRobot::Init(const D3DXVECTOR3& rot)
{
	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, MODEL_PATH);

	m_Info.move.x = -sinf(D3DX_PI * 0.0f + rot.y) * MOVE_MAG;
	m_Info.move.z = -cosf(D3DX_PI * 0.0f + rot.y) * MOVE_MAG;

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CRobot::Uninit(void)
{
	CRobotManager::GetInstance()->ListOut(this);

	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CRobot::Update(void)
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

		// 対角線の角度を算出
		m_Info.rotDest.y = atan2f(m_Info.pos.x - m_PosTarget[m_Info.nTargetID].x,
			m_Info.pos.z - m_PosTarget[m_Info.nTargetID].z);
	
		m_Info.move.x = -sinf(D3DX_PI * 0.0f + m_Info.rotDest.y) * MOVE_MAG;
		m_Info.move.z = -cosf(D3DX_PI * 0.0f + m_Info.rotDest.y) * MOVE_MAG;
	}

	Set();
}

//==========================================================
// 生成
//==========================================================
CRobot* CRobot::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const float& Distance)
{
	CRobot* pRobot = nullptr;

	pRobot = DEBUG_NEW CRobot;

	if (pRobot != nullptr)
	{
		// 初期化処理
		pRobot->Init(rot);

		// 座標設定
		pRobot->SetPosition(pos);

		// 向き設定
		pRobot->SetRotation(rot);

		// 目標位置設定
		pRobot->SetPosTerget(Distance);
	}

	return pRobot;
}

//==========================================================
// 座標設定
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
// 向き設定
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
// 設定
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
// 目標位置の設定
//==========================================================
void CRobot::SetPosTerget(const float& Distance)
{
	m_PosTarget[0].x = m_Info.pos.x + sinf(m_Info.rot.y) * Distance;
	m_PosTarget[0].z = m_Info.pos.z + cosf(m_Info.rot.y) * Distance;
	m_PosTarget[1].x = m_Info.pos.x + sinf(m_Info.rot.y) * -Distance;
	m_PosTarget[1].z = m_Info.pos.z + cosf(m_Info.rot.y) * -Distance;
}

//==========================================================
// 目標位置に到達したかどうか
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
