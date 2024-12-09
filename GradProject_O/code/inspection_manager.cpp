//==========================================================
//
// 検問管理の処理 [inspeciton_manager.cpp]
// Author : Ryosuke Oohara
//
//==========================================================
#include "inspection_manager.h"
#include "inspection.h"
#include "debugproc.h"

//==========================================================
// 静的メンバ変数
//==========================================================
CInspectionManager* CInspectionManager::m_pInstance = nullptr;

namespace
{
	
}

//==========================================================
// コンストラクタ
//==========================================================
CInspectionManager::CInspectionManager()
{
	m_pInstance = this;
	m_NextInfo = SNextInfo();
	m_List.Clear();
}

//==========================================================
// デストラクタ
//==========================================================
CInspectionManager::~CInspectionManager()
{

}

//==========================================================
// 生成処理
//==========================================================
CInspectionManager* CInspectionManager::GetInstance(void)
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = DEBUG_NEW CInspectionManager;
		m_pInstance->Init();
	}

	return m_pInstance;
}

//==========================================================
// 解放
//==========================================================
void CInspectionManager::Release()
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
bool CInspectionManager::ListIn(CInspection* pInspection)
{
	if (pInspection == nullptr) { return false; }

	return m_List.Regist(pInspection->GetId(), pInspection);
}

//==========================================================
// リスト管理に含める
//==========================================================
bool CInspectionManager::ListOut(CInspection* pInspection)
{
	if (pInspection == nullptr) { return false; }

	return m_List.Delete(pInspection->GetId(), pInspection);
}

//==========================================================
// ゴールを取得
//==========================================================
CInspection* CInspectionManager::Get(int nIdx)
{
	auto it = m_List.Get(nIdx);
	return it;
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CInspectionManager::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CInspectionManager::Uninit(void)
{
	// 自身の破棄
	if (m_pInstance != nullptr)
	{
		m_pInstance = nullptr;
	}

	delete this;
}

//==========================================================
// 更新処理
//==========================================================
void CInspectionManager::Update(void)
{
	if (m_NextInfo.bActive)
	{
		// 存在していなかったら生成
		if (Get(m_NextInfo.nId) == nullptr)
		{
			CInspection::Create(m_NextInfo.pos, m_NextInfo.rot, m_NextInfo.pRoad, m_NextInfo.nId, m_NextInfo.nStartpdid);
		}
		m_NextInfo = SNextInfo();
		m_NextInfo.bActive = false;
	}
}

//==========================================================
// 次の検問の情報設定
//==========================================================
void CInspectionManager::SetNextInspection(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, CRoad* pRoad, int nId, int startpdid)
{
	if (m_NextInfo.bActive) { return; }
	m_NextInfo.bActive = true;
	m_NextInfo.pos = pos;
	m_NextInfo.rot = rot;
	m_NextInfo.pRoad = pRoad;
	m_NextInfo.nId = nId;
	m_NextInfo.nStartpdid = startpdid;
}
