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
#include "debugproc.h"

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
	m_pGoalOld = nullptr;
	m_pInstance = this;
	m_InfoList.clear();
	m_List.Clear();
	m_nOldIdx = 9;
	m_nNumCreate = 0;
	m_nNextIdx = 0;
	m_nNowIdx = 0;
	m_nNetId = -1;
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
	if (m_pInstance == nullptr)
	{
		m_pInstance = DEBUG_NEW CGoalManager;
		m_pInstance->Init();
	}

	return m_pInstance;
}

//==========================================================
// 解放
//==========================================================
void CGoalManager::Release()
{
	if (m_pInstance != nullptr)
	{
		m_pInstance->Uninit();
		m_pInstance = nullptr;
	}
}

//==========================================================
// リスト管理に入れる
//==========================================================
bool CGoalManager::ListIn(CGoal* pGoal)
{
	if (pGoal == nullptr) { return false; }
	return m_List.Regist(pGoal->GetId(), pGoal);
}

//==========================================================
// リスト管理に含める
//==========================================================
bool CGoalManager::ListOut(CGoal* pGoal)
{
	if (pGoal == nullptr) { return false; }

	return m_List.Delete(pGoal->GetId(), pGoal);
}

//==========================================================
// ゴールを取得
//==========================================================
CGoal* CGoalManager::GetGoal(int nIdx)
{
	auto it = m_List.Get(nIdx);
	return it;
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CGoalManager::Init(void)
{
	if (m_InfoList.size() <= 0) { return E_FAIL; }

	GoalCreate(m_nOldIdx);

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CGoalManager::Uninit(void)
{
	CListManager::Uninit();

	// ゴールの破棄
	if (m_pGoal != nullptr)
	{
		m_pGoal->Uninit();
		m_pGoal = nullptr;
	}

	m_InfoList.clear();

	// 自身の破棄
	if (m_pInstance != nullptr)
	{
		delete this;
		m_pInstance = nullptr;
	}
}

//==========================================================
// 更新処理
//==========================================================
void CGoalManager::Update(void)
{

	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
	if (pPlayer == nullptr) { return; }

	CDebugProc::GetInstance()->Print("ゴールの生成数 [ %d ] : 現在の配置数 [ %d ], 配置の番号[ %d ]\n",m_nNumCreate, m_List.GetNum(), m_nNowIdx);

	if (m_pGoal == nullptr) { return; }

	// 次の番号が設定されている
	if (m_nNetId >= 0)
	{
		GoalCreate(m_nNetId);
		m_nNetId = -1;
	}
}

//==========================================================
// ゴールの生成
//==========================================================
void CGoalManager::GoalCreate(int nId)
{
	// 前回が存在する
	if (m_pGoalOld != nullptr)
	{
		m_pGoalOld->Uninit();
		m_pGoalOld = nullptr;
	}

	// 前回覚えなおし
	m_pGoalOld = m_pGoal;

	// 新しく生成
	auto it = m_InfoList[nId];
	m_nNowIdx = nId;
	m_pGoal = CGoal::Create(it.pos, it.fRange, it.fLimit, m_nNumCreate);
	m_nOldIdx = nId;
	m_nNumCreate++;

	// 次の番号を設定
	while(1)
	{
		m_nNextIdx = rand() % m_InfoList.size();

		if (m_InfoList.size() <= 1 || m_nNextIdx != m_nOldIdx)
		{
			break;
		}
	}
}