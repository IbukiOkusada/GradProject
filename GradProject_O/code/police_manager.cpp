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
#include "add_police.h"
#include "deltatime.h"
#include "debugproc.h"
#include "pred_route.h"

// 名前空間
namespace
{
	const float INTERVAL = 5.0f;	// インターバル
	const int MAX_POLICE = (15);	// 警察の最大値
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
	m_InspInfo = SInspInfo();
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
	m_InspInfo.fTime = INTERVAL;
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
	m_InspInfo.fInterval += CDeltaTime::GetInstance()->GetDeltaTime();

	CDebugProc::GetInstance()->Print("インターバル : [ %f ]\n", m_InspInfo.fInterval);
}

//==========================================================
// インスタンスを確保
//==========================================================
CPoliceManager* CPoliceManager::GetInstance(void)
{
	if (m_pInstance == nullptr) {	// 使われていない
		m_pInstance = DEBUG_NEW CPoliceManager;
		m_pInstance->Init();
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

	if (m_InspInfo.fInterval >= m_InspInfo.fTime && CAddPolice::GetList()->GetNum() < MAX_POLICE)
	{
		m_InspInfo.fInterval = 0.0f;
		// 検問を配置する
		SetInspection();
	}
}

//==========================================================
// 検問を配置する
//==========================================================
void CPoliceManager::SetInspection()
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	if (pPlayer == nullptr) { return; }

	// プレイヤーの予測を取得する
	CPredRoute* pRoute = pPlayer->GetPredRoute();
	if (pRoute == nullptr) { return; }

	// 目標地点とひとつ前
	CRoad* pRoad = pRoute->GetPredRoad();
	CRoad* pPrev = pRoute->GetPredPrevRoad();

	// 道が見つからなかった
	if (pRoad == nullptr) { return; }

	// 方向を取得する
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	float targetrot = atan2f(pPrev->GetPosition().x - pRoad->GetPosition().x,
		pPrev->GetPosition().z - pRoad->GetPosition().z);
	rot.y = targetrot + D3DX_PI * 0.5f;
	Adjust(rot.y);

	// 設置
	D3DXVECTOR3 pos = pRoad->GetPosition();
	pos.x += sinf(targetrot) * pRoad->GetInfo()->size.x;
	pos.z += cosf(targetrot) * pRoad->GetInfo()->size.y;

	CInstpection::Create(pos, rot, pRoad);
}