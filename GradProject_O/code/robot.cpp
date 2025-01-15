//==========================================================
//
// ロボットの処理 [robot.cpp]
// Author : Kenta Hashimoto
//
//==========================================================
#include "robot.h"
#include "robot_manager.h"
#include "manager.h"

// マクロ定義

//==========================================================
// コンストラクタ
//==========================================================
CRobot::CRobot()
{
	// 値のクリア
	m_pNext = nullptr;
	m_pPrev = nullptr;

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
HRESULT CRobot::Init(void)
{
	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, "data\\MODEL\\sample_box.x");

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
	// 対角線の角度を算出
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
// 生成
//==========================================================
CRobot* CRobot::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move, const float& Distance)
{
	CRobot* pRobot = nullptr;

	pRobot = DEBUG_NEW CRobot;

	if (pRobot != nullptr)
	{
		// 初期化処理
		pRobot->Init();

		// 座標設定
		pRobot->SetPosition(pos);

		// 向き設定
		pRobot->SetRotation(rot);

		// 移動量設定
		pRobot->SetMove(move);

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
