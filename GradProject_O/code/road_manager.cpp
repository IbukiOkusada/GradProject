//==========================================================
//
// 道マネージャー [road_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "road_manager.h"
#include "road.h"

// 静的メンバ変数宣言
CRoadManager* CRoadManager::m_pInstance = nullptr;	// インスタンス

//==========================================================
// コンストラクタ
//==========================================================
CRoadManager::CRoadManager()
{
	// 値のクリア
	m_pCur = nullptr;
	m_pTop = nullptr;
	m_nNum = 0;
}

//==========================================================
// デストラクタ
//==========================================================
CRoadManager::~CRoadManager()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CRoadManager::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CRoadManager::Uninit(void)
{

}

//==========================================================
// 更新処理
//==========================================================
void CRoadManager::Update(void)
{

}

//==========================================================
// インスタンスを確保
//==========================================================
CRoadManager* CRoadManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// 使われていない
		m_pInstance = DEBUG_NEW CRoadManager;
	}

	return m_pInstance;
}

//==========================================================
// インスタンスをリリース
//==========================================================
void CRoadManager::Release(void)
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
void CRoadManager::ListIn(CRoad* pRoad)
{
	if (m_pTop != nullptr)
	{// 先頭が存在している場合
		m_pCur->SetNext(pRoad);	// 現在最後尾のオブジェクトのポインタにつなげる
		pRoad->SetPrev(m_pCur);
		m_pCur = pRoad;	// 自分自身が最後尾になる
	}
	else
	{// 存在しない場合
		m_pTop = pRoad;	// 自分自身が先頭になる
		m_pCur = pRoad;	// 自分自身が最後尾になる
	}

	m_nNum++;
}

//==========================================================
// リストから外す
//==========================================================
void CRoadManager::ListOut(CRoad* pRoad)
{
	// リストから自分自身を削除する
	if (m_pTop == pRoad)
	{// 自身が先頭
		if (pRoad->GetNext() != nullptr)
		{// 次が存在している
			m_pTop = pRoad->GetNext();	// 次を先頭にする
			m_pTop->SetPrev(nullptr);	// 次の前のポインタを覚えていないようにする
		}
		else
		{// 存在していない
			m_pTop = nullptr;	// 先頭がない状態にする
			m_pCur = nullptr;	// 最後尾がない状態にする
		}
	}
	else if (m_pCur == pRoad)
	{// 自身が最後尾
		if (pRoad->GetPrev() != nullptr)
		{// 次が存在している
			m_pCur = pRoad->GetPrev();		// 前を最後尾にする
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
		if (pRoad->GetNext() != nullptr)
		{
			pRoad->GetNext()->SetPrev(pRoad->GetPrev());	// 自身の次に前のポインタを覚えさせる
		}
		if (pRoad->GetPrev() != nullptr)
		{
			pRoad->GetPrev()->SetNext(pRoad->GetNext());	// 自身の前に次のポインタを覚えさせる
		}
	}

	m_nNum--;
}

//==========================================================
// 攻撃ヒット確認
//==========================================================
bool CRoadManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	CRoad* pRoad = m_pTop;
	bool bUse = false;

	//個別判定
	while (pRoad != nullptr) {
		CRoad* pRoadNext = pRoad->GetNext();
		//if (pRoad->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
		pRoad = pRoadNext;
	}

	return bUse;
}