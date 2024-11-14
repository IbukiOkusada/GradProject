//==========================================================
//
// 警察マネージャー [police_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "police_manager.h"
#include "police.h"

// 静的メンバ変数宣言
CPoliceManager* CPoliceManager::m_pInstance = nullptr;	// インスタンス

//==========================================================
// コンストラクタ
//==========================================================
CPoliceManager::CPoliceManager()
{
	// 値のクリア
	m_pList = nullptr;
	m_nNum = 0;
}

//==========================================================
// デストラクタ
//==========================================================
CPoliceManager::~CPoliceManager()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CPoliceManager::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CPoliceManager::Uninit(void)
{
	// 親クラスの終了処理
	CListManager::Uninit();

	// リスト廃棄
	ListRelease();

	// インスタンスの廃棄
	if (m_pInstance != nullptr) {	// インスタンスを確保されている
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
// 更新処理
//==========================================================
void CPoliceManager::Update(void)
{

}

//==========================================================
// インスタンスを確保
//==========================================================
CPoliceManager* CPoliceManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// 使われていない
		m_pInstance = DEBUG_NEW CPoliceManager;
	}

	return m_pInstance;
}

//==========================================================
// インスタンスをリリース
//==========================================================
void CPoliceManager::Release(void)
{
	if (m_pInstance != nullptr) {	// インスタンスを確保されている
		m_pInstance->Uninit();
	}
}

//==========================================================
// リストに挿入
//==========================================================
void CPoliceManager::ListIn(CPolice* pPolice)
{
	GetList()->Regist(pPolice);
}

//==========================================================
// リストから外す
//==========================================================
void CPoliceManager::ListOut(CPolice* pPolice)
{
	// リストから自分自身を削除する
	GetList()->Delete(pPolice);
}

//==========================================================
// 攻撃ヒット確認
//==========================================================
bool CPoliceManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	bool bUse = false;

	//個別判定
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		CPolice* pPolice = GetList()->Get(i);
		//if (pPolice->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
	}

	return bUse;
}

//==========================================================
// 全員を警戒状態にする
//==========================================================
void CPoliceManager::Warning(CPolice* pPolice)
{
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		// 警察と自分が一致してたら駄目
		CPolice* pP = GetList()->Get(i);
		if (pP == pPolice) { continue; }

		// 現在追跡中も駄目
		CPolice::STATE outstate = CPolice::STATE::STATE_CHASE;
		if (pP->GetState() == outstate) { continue; }

		// 警戒状態にする
		pP->SetState(CPolice::STATE::STATE_SEARCH);
	}
}