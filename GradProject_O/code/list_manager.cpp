//==========================================================
//
// マネージャー管理(リスト管理) [list_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "list_manager.h"

// 静的メンバ変数宣言
Clist<CListManager*>* CListManager::m_pList = nullptr;

//==========================================================
// コンストラクタ
//==========================================================
CListManager::CListManager()
{
	if (m_pList == nullptr)
	{
		m_pList = m_pList->Create();
	}

	// 値のクリア
	m_pList->Regist(this);
}

//==========================================================
// デストラクタ
//==========================================================
CListManager::~CListManager()
{
	
}

//==========================================================
// 終了処理
//==========================================================
void CListManager::Uninit(void)
{
	m_pList->Delete(this);

	if (m_pList->Empty())
	{
		delete m_pList;
		m_pList = nullptr;
	}
}

//==========================================================
// 開放
//==========================================================
void CListManager::Release()
{
	if (m_pList == nullptr) { return; }

	while (m_pList->GetNum() != 0)
	{
		std::vector<CListManager*> list;

		for (int i = 0; i < m_pList->GetNum(); i++)
		{
			list.push_back(m_pList->Get(i));
		}

		for (auto& it : list)
		{
			it->Uninit();
		}

		if (m_pList == nullptr)
		{
			break;
		}
	}
}