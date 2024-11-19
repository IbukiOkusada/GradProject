//==========================================================
//
// ゴール管理の処理 [goal_manager.cpp]
// Author : Ryosuke Oohara
//
//==========================================================
#include "goal_manager.h"
#include "goal.h"
#include "camera.h"
#include "camera_action.h"
#include "camera_manager.h"
#include "player.h"
#include "player_manager.h"

//==========================================================
// 静的メンバ変数
//==========================================================
CGoalManager *CGoalManager::m_pInstance = nullptr;

namespace
{
	const D3DXVECTOR3 POS[2] =
	{
		D3DXVECTOR3(-8600.0f, 0.0f, -10600.0f),
		D3DXVECTOR3(0.0f, 0.0f, -4000.0f),
	};
}

//==========================================================
// コンストラクタ
//==========================================================
CGoalManager::CGoalManager()
{
	m_pGoal = nullptr;

	m_pInstance = this;
}

//==========================================================
// デストラクタ
//==========================================================
CGoalManager::~CGoalManager()
{

}

//==========================================================
// 生成処理
//==========================================================
CGoalManager* CGoalManager::Create(void)
{
	CGoalManager* pGoalManager = DEBUG_NEW CGoalManager;

	if (pGoalManager != nullptr)
	{
		pGoalManager->Init();
	}

	return pGoalManager;
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CGoalManager::Init(void)
{
	m_pGoal = CGole::Create(D3DXVECTOR3(10000.0f, 0.0f, 12500.0f), 600.0f, 20.0f);

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CGoalManager::Uninit(void)
{
	// ゴールの破棄
	if (m_pGoal != nullptr)
	{
		m_pGoal->Uninit();
		m_pGoal = nullptr;
	}

	// 自身の破棄
	if (m_pInstance != nullptr)
	{
		m_pInstance = nullptr;
	}
}

//==========================================================
// 更新処理
//==========================================================
void CGoalManager::Update(void)
{
	CCamera* pCamera = CCameraManager::GetInstance()->GetTop();

	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	int nNum = pPlayer->GetNumDeliverStatus();

	if (m_pGoal == nullptr) { return; }

	if (m_pGoal->GetEnd() && pCamera->GetAction()->IsFinish())
	{
		m_pGoal->Uninit();
		m_pGoal = nullptr;

		if (nNum >= 3) { return; }

		m_pGoal = CGole::Create(POS[nNum - 1], 600.0f, 20.0f);
	}
}