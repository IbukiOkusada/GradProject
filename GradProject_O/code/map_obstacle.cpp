//==========================================================
//
// マップ内障害物の処理 [map_obstacle.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "map_obstacle.h"
#include "map_manager.h"
#include "objectX.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"
#include "camera_manager.h"
#include "game.h"
#include "edit_manager.h"

namespace
{
	const float RANGE_WIDTH = SCREEN_WIDTH * 0.1f;
	const float RANGE_HEIGHT = SCREEN_HEIGHT * 0.1f;
}

// 静的メンバ変数宣言
Clist<CMapObstacle*>* CMapObstacle::m_pList = nullptr;

//==========================================================
// コンストラクタ
//==========================================================
CMapObstacle::CMapObstacle(const SInfo& info)
{
	//自身をリストに登録
	GetInstance()->Regist(this);
	m_Info = info;
	m_pObj = nullptr;
}

//==========================================================
// デストラクタ
//==========================================================
CMapObstacle::~CMapObstacle()
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
HRESULT CMapObstacle::Init(void)
{
	std::vector<std::string> str = CMapManager::GetInstance()->GetFileNameList();
	correction::Adjust(&m_Info.rot);
	m_pObj = CObjectX::Create(m_Info.pos, m_Info.rot, str[m_Info.fileidx].c_str());
	m_pObj->SetScale(m_Info.scale);

	return S_OK;
}

//==========================================================
// 初期化処理(オーバーロード)
//==========================================================
HRESULT CMapObstacle::Init(const std::string& filename)
{
	m_pObj = CObjectX::Create(m_Info.pos, m_Info.rot, filename.c_str());

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CMapObstacle::Uninit(void)
{
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CMapObstacle::Update(void)
{

	// オブジェクトの更新
	if (m_pObj != nullptr)
	{
		m_pObj->DrawCheck();
		m_pObj->SetPosition(m_Info.pos);
		m_pObj->SetRotation(m_Info.rot);
		m_pObj->SetScale(m_Info.scale);
	}
}

//==========================================================
// 生成
//==========================================================
CMapObstacle* CMapObstacle::Create(const SInfo& info)
{
	CMapObstacle* pGoal = nullptr;

	pGoal = DEBUG_NEW CMapObstacle(info);

	if (pGoal != nullptr)
	{
		pGoal->m_Info = info;
		// 初期化処理
		pGoal->Init();
	}

	return pGoal;
}

//==========================================================
// 座標設定
//==========================================================
void CMapObstacle::SetPosition(const D3DXVECTOR3& pos)
{
	m_Info.pos = pos;

	// オブジェクトの更新
	if (m_pObj != nullptr)
	{
		m_pObj->SetPosition(m_Info.pos);
	}
}

//==========================================================
// 向き設定
//==========================================================
void CMapObstacle::SetRotation(const D3DXVECTOR3& rot)
{
	m_Info.rot = rot;
	correction::Adjust(&m_Info.rot);

	// オブジェクトの更新
	if (m_pObj != nullptr)
	{
		m_pObj->SetRotation(m_Info.rot);
	}
}

//==========================================================
// スケール設定
//==========================================================
void CMapObstacle::SetScale(const D3DXVECTOR3& scale)
{
	m_Info.scale = scale;

	// オブジェクトの更新
	if (m_pObj != nullptr)
	{
		m_pObj->SetScale(m_Info.scale);
	}
}

//==========================================================
// モデル設定
//==========================================================
void CMapObstacle::BindModel(const int& idx)
{
	CMapManager* pMgr = CMapManager::GetInstance();
	if (idx < 0 || idx >= static_cast<int>(pMgr->GetFileNameList().size())) { m_Info.fileidx = 0;}

	m_Info.fileidx = idx;

	if (m_pObj == nullptr) { return; }

	std::vector<std::string> str = pMgr->GetFileNameList();

	m_pObj->BindFile(str[idx].c_str());
}
