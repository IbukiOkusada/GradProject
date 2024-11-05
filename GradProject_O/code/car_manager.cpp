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
	m_pList = nullptr;
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
	}
}

//==========================================================
// リストに挿入
//==========================================================
void CCarManager::ListIn(CCar* pCar)
{
	GetList()->Regist(pCar);
}

//==========================================================
// リストから外す
//==========================================================
void CCarManager::ListOut(CCar* pCar)
{
	// リストから自分自身を削除する
	GetList()->Delete(pCar);
}

//==========================================================
// 攻撃ヒット確認
//==========================================================
bool CCarManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	bool bUse = false;

	//個別判定
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		CCar* pCar = GetList()->Get(i);
		//if (pCar->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
	}

	return bUse;
}