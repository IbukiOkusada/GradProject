//==========================================================
//
// ゴール [goal.cpp]
// Author : 丹野竜之介
//
//==========================================================
#include "goal.h"
#include "player_manager.h"
#include "player.h"
#include "convenience.h"
#include "meshcylinder.h"
// マクロ定義
Clist<CGole*> * CGole::pList = nullptr;
//==========================================================
// コンストラクタ
//==========================================================
CGole::CGole()
{
	pEffect = nullptr;
	m_fRange = 0.0f;
	m_fLimit = 0.0f;
	//自身をリストに登録
	GetInstance()->Regist(this);
}

//==========================================================
// デストラクタ
//==========================================================
CGole::~CGole()
{
	//自身をリストから削除
	GetInstance()->Delete(this);
	if (GetInstance()->Empty())
	{//リストの中身が空ならリストを破棄
		ListRelease();
	}
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CGole::Init(void)
{
	pEffect = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\goal_radius.efkefc", m_pos, VECTOR3_ZERO, VECTOR3_ZERO, m_fRange, false, false);
	
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CGole::Uninit(void)
{
	SAFE_DELETE(pEffect);
	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CGole::Update(void)
{
	if (CheckRange() && CheckSpeed())
	{
		Uninit();
	}

}
//==========================================================
// 距離のチェック
//==========================================================
bool CGole::CheckRange()
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	if (pPlayer != NULL)
	{
		float fDis = GetDistance(m_pos, pPlayer->GetPosition());
		return (m_fRange >= fDis);
	}
	return false;
}
//==========================================================
// 速度のチェック
//==========================================================
bool CGole::CheckSpeed()
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	if (pPlayer != NULL)
	{
		float fDis = GetDistance(VECTOR3_ZERO, pPlayer->GetMove());
		return (m_fLimit >= fDis);
	}
	return false;
}
//==========================================================
// 生成
//==========================================================
CGole* CGole::Create(D3DXVECTOR3 pos, float fRange, float fLimit)
{
	CGole* pGoal = nullptr;

	pGoal = DEBUG_NEW CGole;

	if (pGoal != nullptr)
	{
		pGoal->m_pos = pos;
		pGoal->m_fRange = fRange;
		pGoal->m_fLimit = fLimit;
		// 初期化処理
		pGoal->Init();
	}

	return pGoal;
}
