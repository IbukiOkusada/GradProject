//==========================================================
//
// エディット管理の処理 [edit_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_manager.h"
#include "input_keyboard.h"
#include "input_mouse.h"
#include "debugproc.h"
#include "manager.h"
#include "map_manager.h"
#include "map_obstacle.h"
#include "road.h"
#include "road_manager.h"

CEditManager* CEditManager::m_pInstance = nullptr;

//==========================================================
// コンストラクタ
//==========================================================
CEditManager::CEditManager()
{
	// 値のクリア
	m_SelectType = CEdit::TYPE_OBJ;
	m_pEdit = nullptr;
}

//==========================================================
// デストラクタ
//==========================================================
CEditManager::~CEditManager()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CEditManager::Init(void)
{
	m_bFlag = false;

	// エディター初期化
	m_SelectType = CEdit::TYPE_OBJ;
	ChangeEdit(CEdit::Create(m_SelectType));
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CEditManager::Uninit(void)
{
	if (m_pEdit != nullptr)
	{
		m_pEdit->Uninit();
		m_pEdit = nullptr;
	}

	// インスタンスの廃棄
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// 更新処理
//==========================================================
void CEditManager::Update(void)
{
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();
	CDebugProc* pProc = CManager::GetInstance()->GetDebugProc();

	// デバッグ表示
	pProc->Print("+---------------------------------------------------------------\n");
	pProc->Print("<エディター起動中> モデル読み込み数 [ %d ]", CMapManager::GetInstance()->GetFileNameList().size());
	if (CMapObstacle::GetList() != nullptr) { pProc->Print(" : 障害物配置数 [ %d ]", CMapObstacle::GetList()->GetNum()); }
	if (CRoadManager::GetInstance() != nullptr) { pProc->Print(" : 道配置数 [ %d ]\n", CRoadManager::GetInstance()->GetList()->GetNum());}
	pProc->Print("終了[ F4 ] : モード切替[ F3 ] : ");
	//pProc->Print("<マウス> [ %f, %f, %f ]\n", pMouse->GetWorldPos().x, pMouse->GetWorldPos().y, pMouse->GetWorldPos().z);

	// エディター終了
	if (pKey->GetTrigger(DIK_F4) && m_bFlag)
	{
		Uninit();
		return;
	}
	else
	{
		m_bFlag = true;
	}

	// 状態切り替え
	if (pKey->GetTrigger(DIK_F3)) {
		m_SelectType = static_cast<CEdit::TYPE>((m_SelectType + 1) % CEdit::TYPE_MAX);
		ChangeEdit(CEdit::Create(m_SelectType));
	}

	// エディターの更新処理
	if (m_pEdit != nullptr)
	{
		m_pEdit->Update();
	}
	else
	{
		pProc->Print("\n");
	}

	pProc->Print("+---------------------------------------------------------------\n");
}

//===============================================
// 切り替え
//===============================================
void CEditManager::ChangeEdit(CEdit* pEdit)
{
	// 使用されていたら廃棄
	if (m_pEdit != nullptr)
	{
		m_pEdit->Uninit();
		m_pEdit = nullptr;
	}

	m_pEdit = pEdit;
}

//===============================================
// インスタンスの生成
//===============================================
CEditManager* CEditManager::Create()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = DEBUG_NEW CEditManager;
		m_pInstance->Init();
	}

	return m_pInstance;
}

//===============================================
// インスタンスの取得
//===============================================
CEditManager* CEditManager::GetInstance()
{
	return m_pInstance;
}

//===============================================
// インスタンスの廃棄
//===============================================
void CEditManager::Release(void)
{
	// 終了
	if (m_pInstance != nullptr)
	{
		m_pInstance->Uninit();
	}
}