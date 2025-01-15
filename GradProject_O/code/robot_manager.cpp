//==========================================================
//
// ロボットマネージャー [robot_manager.cpp]
// Author : Kenta Hashimoto
//
//==========================================================
#include "robot_manager.h"
#include "robot.h"

// 静的メンバ変数宣言
CRobotManager* CRobotManager::m_pInstance = nullptr;	// インスタンス

//==========================================================
// コンストラクタ
//==========================================================
CRobotManager::CRobotManager()
{
	// 値のクリア
	m_pCur = nullptr;
	m_pTop = nullptr;
	m_nNum = 0;
}

//==========================================================
// デストラクタ
//==========================================================
CRobotManager::~CRobotManager()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CRobotManager::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CRobotManager::Uninit(void)
{
	// 親クラス終了
	CListManager::Uninit();

	// インスタンス廃棄
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// 更新処理
//==========================================================
void CRobotManager::Update(void)
{

}

//==========================================================
// インスタンスを確保
//==========================================================
CRobotManager* CRobotManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// 使われていない
		m_pInstance = DEBUG_NEW CRobotManager;
	}

	return m_pInstance;
}

//==========================================================
// インスタンスをリリース
//==========================================================
void CRobotManager::Release(void)
{
	if (m_pInstance != nullptr) {	// インスタンスを確保されている
		m_pInstance->Uninit();
	}
}

//==========================================================
// リストに挿入
//==========================================================
void CRobotManager::ListIn(CRobot* pRobot)
{
	if (m_pTop != nullptr)
	{// 先頭が存在している場合
		m_pCur->SetNext(pRobot);	// 現在最後尾のオブジェクトのポインタにつなげる
		pRobot->SetPrev(m_pCur);
		m_pCur = pRobot;	// 自分自身が最後尾になる
	}
	else
	{// 存在しない場合
		m_pTop = pRobot;	// 自分自身が先頭になる
		m_pCur = pRobot;	// 自分自身が最後尾になる
	}

	m_nNum++;
}

//==========================================================
// リストから外す
//==========================================================
void CRobotManager::ListOut(CRobot* pRobot)
{
	// リストから自分自身を削除する
	if (m_pTop == pRobot)
	{// 自身が先頭
		if (pRobot->GetNext() != nullptr)
		{// 次が存在している
			m_pTop = pRobot->GetNext();	// 次を先頭にする
			m_pTop->SetPrev(nullptr);	// 次の前のポインタを覚えていないようにする
		}
		else
		{// 存在していない
			m_pTop = nullptr;	// 先頭がない状態にする
			m_pCur = nullptr;	// 最後尾がない状態にする
		}
	}
	else if (m_pCur == pRobot)
	{// 自身が最後尾
		if (pRobot->GetPrev() != nullptr)
		{// 次が存在している
			m_pCur = pRobot->GetPrev();		// 前を最後尾にする
			m_pCur->SetNext(nullptr);			// 前の次のポインタを覚えていないようにする
		}
		else
		{// 存在していない
			m_pTop = nullptr;	// 先頭がない状態にする
			m_pCur = nullptr;	// 最後尾がない状態にする
		}
	}
	else
	{
		if (pRobot->GetNext() != nullptr)
		{
			pRobot->GetNext()->SetPrev(pRobot->GetPrev());	// 自身の次に前のポインタを覚えさせる
		}
		if (pRobot->GetPrev() != nullptr)
		{
			pRobot->GetPrev()->SetNext(pRobot->GetNext());	// 自身の前に次のポインタを覚えさせる
		}
	}

	m_nNum--;
}
