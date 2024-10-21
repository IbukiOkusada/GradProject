//==========================================================
//
// タスク管理の処理 [task_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_manager.h"
#include "input_keyboard.h"
#include "manager.h"
#include "debugproc.h"
#include "input_mouse.h"

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
	CInputMouse* pMouse = CInputMouse::GetInstance();

	// デバッグ表示
	pProc->Print("+---------------------------------------------------------------\n");
	pProc->Print("<エディター起動中> 終了[ F4 ]\n");
	pProc->Print("<マウス> [ %f, %f, %f ]\n", pMouse->GetWorldPos().x, pMouse->GetWorldPos().y, pMouse->GetWorldPos().z);

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
		m_SelectType = static_cast<CEdit::TYPE>((m_SelectType + 1) / CEdit::TYPE_MAX);
		ChangeEdit(CEdit::Create(m_SelectType));
	}

	// エディターの更新処理
	if (m_pEdit != nullptr)
	{
		m_pEdit->Update();
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