//==========================================================
//
// タスク管理の処理 [task_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_manager.h"
#include "input_keyboard.h"

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

	// 状態切り替え
	if (pKey->GetTrigger(DIK_F3)) {
		m_SelectType = static_cast<CEdit::TYPE>((m_SelectType + 1) / CEdit::TYPE_MAX);
		ChangeEdit(CEdit::Create(m_SelectType));
	}

	// エディターの更新処理
	if (m_pEdit != nullptr)
	{
		m_pEdit->Update();
	}
}

//===============================================
// 切り替え
//===============================================
void CEditManager::ChangeEdit(CEdit* pEdit)
{
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
	if (m_pInstance != nullptr)
	{
		m_pInstance->Uninit();
	}
}