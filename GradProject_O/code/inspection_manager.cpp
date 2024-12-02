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
	m_List.Clear();
	m_nCreateCnt = 0;
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

	bool bflag = m_List.Regist(pInspection->GetId(), pInspection);

	// 成功
	if (bflag) { m_nCreateCnt++; }

	return bflag;
}

//==========================================================
// リスト管理に含める
//==========================================================
bool CInspectionManager::ListOut(CInspection* pInspection)
{
	if (pInspection == nullptr) { return false; }

	bool bflag = m_List.Delete(pInspection->GetId(), pInspection);

	return bflag;
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
	
}
