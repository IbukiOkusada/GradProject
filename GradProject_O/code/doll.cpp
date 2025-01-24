//==========================================================
//
// 人形の処理 [doll.cpp]
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
// 定数定義
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
}

//==========================================================
// コンストラクタ
//==========================================================
CDoll::CDoll()
{
	// 値のクリア
	m_pNext = nullptr;
	m_pPrev = nullptr;
	m_pCharacter = nullptr;
	m_Info = SInfo();
}

//==========================================================
// デストラクタ
//==========================================================
CDoll::~CDoll()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CDoll::Init()
{
	return S_OK;
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CDoll::Init(const D3DXVECTOR3& rot)
{
	m_pCharacter = CCharacter::Create(MODEL_PATH);
	m_pCharacter->SetParent(NULL);
	m_pCharacter->GetMotion()->InitSet(MOTION::MOTION_NightofFire);
	m_pCharacter->SetScale(D3DXVECTOR3(7.0f, 7.0f, 7.0f));

	m_Info.state = STATE_WALK;

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CDoll::Uninit(void)
{
	SAFE_UNINIT(m_pCharacter)

	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CDoll::Update(void)
{
	Set();

	if (m_pCharacter != nullptr)
	{
		m_pCharacter->Update();
	}

	CDebugProc::GetInstance()->Print(" いるよ[%f %f %f]\n");
}

//==========================================================
// 生成
//==========================================================
CDoll* CDoll::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	CDoll* pRobot = nullptr;

	pRobot = DEBUG_NEW CDoll;

	if (pRobot != nullptr)
	{
		// 初期化処理
		pRobot->Init(rot);

		// 座標設定
		pRobot->SetPosition(pos);

		// 向き設定
		pRobot->SetRotation(rot);
	}

	return pRobot;
}

//==========================================================
// 座標設定
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
// 向き設定
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
// 設定
//==========================================================
void CDoll::Set()
{
	if (m_pCharacter != nullptr)
	{
		m_pCharacter->SetPosition(m_Info.pos);
		m_pCharacter->SetRotation(m_Info.rot);
	}
}