//==========================================================
//
// 警察マネージャー [police_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "police_manager.h"
#include "police.h"
#include "player_manager.h"
#include "road.h"
#include "inspection.h"

// 名前空間
namespace
{
	
}

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

	// 検問を配置する
	SetInspection();
}

//==========================================================
// 検問を配置する
//==========================================================
void CPoliceManager::SetInspection()
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	if (pPlayer == nullptr) { return; }

	// プレイヤーのいる道を取得する
	CRoad* pRoad = pPlayer->GetRoad();
	if (pRoad == nullptr) { return; }

	// プレイヤーの向きを取得して次の道路の方向を導き出す
	float rotY = pPlayer->GetObj()->GetRotation().y;
	Adjust(rotY);

	float range = D3DX_PI * 0.25f;	// 斜め用範囲
	CRoad* pPredRoad = nullptr;	// 次の道

	// 向きによって次の道を見つける
	if (rotY >= -range && rotY <= range)
	{// 下移動
		pPredRoad = pRoad->GetSearchRoad(CRoad::DIRECTION::DIC_DOWN)->pRoad;
	}
	else if (rotY >= range && rotY <= range * 3)
	{// 左移動
		pPredRoad = pRoad->GetSearchRoad(CRoad::DIRECTION::DIC_LEFT)->pRoad;
	}
	else if (rotY >= -range * 3 && rotY <= -range)
	{// 右移動
		pPredRoad = pRoad->GetSearchRoad(CRoad::DIRECTION::DIC_RIGHT)->pRoad;
	}
	else
	{// 上移動
		pPredRoad = pRoad->GetSearchRoad(CRoad::DIRECTION::DIC_UP)->pRoad;
	}

	// 道が見つからなかった
	if (pPredRoad == nullptr) { return; }

	// 方向を取得する
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	rot.y = rotY;
	float targetrot = atan2f(pRoad->GetPosition().x - pPredRoad->GetPosition().x, 
		pRoad->GetPosition().z - pPredRoad->GetPosition().z);

	// 設置
	D3DXVECTOR3 pos = pPredRoad->GetPosition();
	pos.x += sinf(targetrot) * pPredRoad->GetInfo()->size.x;
	pos.z += cosf(targetrot) * pPredRoad->GetInfo()->size.y;

	CInstpection::Create(pos, rot);
}