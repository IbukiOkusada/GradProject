//==========================================================
//
// プレイヤーマネージャー [car_manager.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "car_manager.h"
#include "car.h"

// 静的メンバ変数宣言
CCarManager* CCarManager::m_pInstance = nullptr;	// インスタンス

//==========================================================
// コンストラクタ
//==========================================================
CCarManager::CCarManager()
{
	// 値のクリア
	m_pCur = nullptr;
	m_pTop = nullptr;
	m_nNum = 0;
}

//==========================================================
// デストラクタ
//==========================================================
CCarManager::~CCarManager()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CCarManager::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CCarManager::Uninit(void)
{

}

//==========================================================
// 更新処理
//==========================================================
void CCarManager::Update(void)
{

}

//==========================================================
// インスタンスを確保
//==========================================================
CCarManager* CCarManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// 使われていない
		m_pInstance = DEBUG_NEW CCarManager;
	}

	return m_pInstance;
}

//==========================================================
// インスタンスをリリース
//==========================================================
void CCarManager::Release(void)
{
	if (m_pInstance != nullptr) {	// インスタンスを確保されている
		m_pInstance->Uninit();
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// リストに挿入
//==========================================================
void CCarManager::ListIn(CCar* pCar)
{
	if (m_pTop != nullptr)
	{// 先頭が存在している場合
		m_pCur->SetNext(pCar);	// 現在最後尾のオブジェクトのポインタにつなげる
		pCar->SetPrev(m_pCur);
		m_pCur = pCar;	// 自分自身が最後尾になる
	}
	else
	{// 存在しない場合
		m_pTop = pCar;	// 自分自身が先頭になる
		m_pCur = pCar;	// 自分自身が最後尾になる
	}

	m_nNum++;
}

//==========================================================
// リストから外す
//==========================================================
void CCarManager::ListOut(CCar* pCar)
{
	// リストから自分自身を削除する
	if (m_pTop == pCar)
	{// 自身が先頭
		if (pCar->GetNext() != nullptr)
		{// 次が存在している
			m_pTop = pCar->GetNext();	// 次を先頭にする
			m_pTop->SetPrev(nullptr);	// 次の前のポインタを覚えていないようにする
		}
		else
		{// 存在していない
			m_pTop = nullptr;	// 先頭がない状態にする
			m_pCur = nullptr;	// 最後尾がない状態にする
		}
	}
	else if (m_pCur == pCar)
	{// 自身が最後尾
		if (pCar->GetPrev() != nullptr)
		{// 次が存在している
			m_pCur = pCar->GetPrev();		// 前を最後尾にする
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
		if (pCar->GetNext() != nullptr)
		{
			pCar->GetNext()->SetPrev(pCar->GetPrev());	// 自身の次に前のポインタを覚えさせる
		}
		if (pCar->GetPrev() != nullptr)
		{
			pCar->GetPrev()->SetNext(pCar->GetNext());	// 自身の前に次のポインタを覚えさせる
		}
	}

	m_nNum--;
}

//==========================================================
// 攻撃ヒット確認
//==========================================================
bool CCarManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	CCar* pCar = m_pTop;
	bool bUse = false;

	//個別判定
	while (pCar != nullptr) {
		CCar* pCarNext = pCar->GetNext();
		//if (pCar->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
		pCar = pCarNext;
	}

	return bUse;
}