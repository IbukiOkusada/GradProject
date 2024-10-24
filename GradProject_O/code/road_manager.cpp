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

namespace
{
	const float HIT_SIZE = 1.0f;	// 連結判定誤差許容判定内
}

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
	// 親クラスの終了処理
	CListManager::Uninit();

	m_InfoList.Clear();

	// インスタンスの廃棄
	if (m_pInstance != nullptr) {	// インスタンスを確保されている
		delete m_pInstance;
		m_pInstance = nullptr;
	}
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

	m_InfoList.Regist(pRoad->GetInfo());

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

//==========================================================
// 全て連結
//==========================================================
void CRoadManager::AllConnect(void)
{
	CRoad* pRoad = GetTop();

	// 一度綺麗にする
	while (pRoad != nullptr)
	{
		CRoad* pNext = pRoad->GetNext();	// 保持

		// クリア
		pRoad->Connect(nullptr, CRoad::DIC_UP);
		pRoad->Connect(nullptr, CRoad::DIC_DOWN);
		pRoad->Connect(nullptr, CRoad::DIC_LEFT);
		pRoad->Connect(nullptr, CRoad::DIC_RIGHT);

		// 次に移動
		pRoad = pNext;
	}

	pRoad = GetTop();

	// 全て確認
	while (pRoad != nullptr)
	{
		D3DXVECTOR3 pos = pRoad->GetPosition();	// 座標

		CRoad* pNext = pRoad->GetNext();	// 保持
		CRoad* pCheck = pNext;	// 確認用を次からにする

		// 次以降で繋がっているかを確認する
		while (pCheck != nullptr)
		{
			D3DXVECTOR3 checkpos = pCheck->GetPosition();	// 確認先の座標

			CRoad* pCheckNext = pCheck->GetNext();	// 保持

			// 座標確認
			if (pos.x + HIT_SIZE >= checkpos.x && pos.x - HIT_SIZE <= checkpos.x)
			{ // x座標一致
				// 縦確認
				VerticalConnectCheck(pRoad, pCheck);
			}
			else if (pos.z + HIT_SIZE >= checkpos.z && pos.z - HIT_SIZE <= checkpos.z)
			{// z座標一致
				// 横確認
				SideConnectCheck(pRoad, pCheck);
			}

			pCheck = pCheckNext;
		}

		// テクスチャ設定
		pRoad->BindTexture();

		// 次に移動
		pRoad = pNext;
	}

	int a = 0;
}

//==========================================================
// 横連結確認
//==========================================================
void CRoadManager::SideConnectCheck(CRoad* pRoad, CRoad* pCheckRoad)
{
	// 座標取得
	D3DXVECTOR3 pos = pRoad->GetPosition();	// 座標
	D3DXVECTOR3 checkpos = pCheckRoad->GetPosition();	// 確認先の座標

	// サイズ取得
	float size = pRoad->GetSize().x;
	float checksize = pCheckRoad->GetSize().x;

	// 差分を求める
	float allsize = size + checksize;
	float diff = fabsf(pos.x - checkpos.x);

	// 判定内の時のみ記憶させる
	if (diff + HIT_SIZE < allsize || diff - HIT_SIZE > allsize) { return; }
	
	if (pos.x >= checkpos.x)
	{// 本体の方が座標が大きい
		pRoad->Connect(pCheckRoad, CRoad::DIC_LEFT);
		pCheckRoad->Connect(pRoad, CRoad::DIC_RIGHT);
	}
	else
	{// 小さい
		pRoad->Connect(pCheckRoad, CRoad::DIC_RIGHT);
		pCheckRoad->Connect(pRoad, CRoad::DIC_LEFT);
	}
}

//==========================================================
// 縦連結確認
//==========================================================
void CRoadManager::VerticalConnectCheck(CRoad* pRoad, CRoad* pCheckRoad)
{
	// 座標取得
	D3DXVECTOR3 pos = pRoad->GetPosition();	// 座標
	D3DXVECTOR3 checkpos = pCheckRoad->GetPosition();	// 確認先の座標

	// サイズ取得
	float size = pRoad->GetSize().y;
	float checksize = pCheckRoad->GetSize().y;

	// 差分を求める
	float allsize = size + checksize;
	float diff = fabsf(pos.z - checkpos.z);

	// 判定内の時のみ記憶させる
	if (diff + HIT_SIZE < allsize || diff - HIT_SIZE > allsize) { return; }

	if (pos.z >= checkpos.z)
	{// 本体の方が座標が大きい
		pRoad->Connect(pCheckRoad, CRoad::DIC_DOWN);
		pCheckRoad->Connect(pRoad, CRoad::DIC_UP);
	}
	else
	{// 小さい
		pRoad->Connect(pCheckRoad, CRoad::DIC_UP);
		pCheckRoad->Connect(pRoad, CRoad::DIC_DOWN);
	}
}