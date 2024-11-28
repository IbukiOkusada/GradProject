//==========================================================
//
// 道マネージャー [road_manager.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "road_manager.h"
#include "road.h"
#include "gimmick_firehydrant.h"

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
	m_pList = nullptr;
	m_IdList.clear();
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

	ListRelease();
	m_IdList.clear();

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
	GetList()->Regist(pRoad);
	m_InfoList.Regist(pRoad->GetInfo());
}

//==========================================================
// リストから外す
//==========================================================
void CRoadManager::ListOut(CRoad* pRoad)
{
	GetList()->Delete(pRoad);
	m_InfoList.Delete(pRoad->GetInfo());
}

//==========================================================
// ID管理のリストに挿入
//==========================================================
void CRoadManager::IdListIn(CRoad* pRoad)
{
	m_IdList[pRoad->GetIdx()];
}

//==========================================================
// ID管理のリストに挿入
//==========================================================
void CRoadManager::IdListOut(CRoad* pRoad)
{
	GetList()->Delete(pRoad);
	m_InfoList.Delete(pRoad->GetInfo());
}

//==========================================================
// 攻撃ヒット確認
//==========================================================
bool CRoadManager::Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage)
{
	bool bUse = false;

	//個別判定
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		//CRoad* pRoad = GetList()->Get(i);
		//if (pRoad->HitCheck(pos, fRange, fHeight, nDamage)) {
			//bUse = true;
		//}
	}

	return bUse;
}

//==========================================================
// 全て連結
//==========================================================
void CRoadManager::AllConnect(void)
{

	// 一度綺麗にする
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		CRoad* pRoad = GetList()->Get(i);

		// クリア
		pRoad->Connect(nullptr, CRoad::DIC_UP);
		pRoad->Connect(nullptr, CRoad::DIC_DOWN);
		pRoad->Connect(nullptr, CRoad::DIC_LEFT);
		pRoad->Connect(nullptr, CRoad::DIC_RIGHT);
	}

	// 全て確認
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		CRoad* pRoad = GetList()->Get(i);
		D3DXVECTOR3 pos = pRoad->GetPosition();	// 座標

		// 次以降で繋がっているかを確認する
		for (int j = i + 1; j < GetList()->GetNum(); j++)
		{
			CRoad* pCheck = GetList()->Get(j);
			D3DXVECTOR3 checkpos = pCheck->GetPosition();	// 確認先の座標

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
		}

		// テクスチャ設定
		pRoad->BindTexture();
	}

	// 経路探索用情報も設定
	SearchRoadConnect();
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

//==========================================================
// 経路探索用連結
//==========================================================
void CRoadManager::SearchRoadConnect(void)
{
	for (int i = 0; i < GetList()->GetNum(); i++)
	{

		CRoad* pRoad = GetList()->Get(i);
		D3DXVECTOR3 pos = pRoad->GetPosition();	// 座標

		// 全部確認
		for (int dic = 0; dic < CRoad::DIRECTION::DIC_MAX; dic++)
		{
			CRoad::DIRECTION direction = static_cast<CRoad::DIRECTION>(dic);
			CRoad* pConnect = pRoad->GetConnectRoad(direction);

			// 道が連結していない
			if(pConnect == nullptr)
			{
				continue;
			}

			// 連結している場合交差点もしくは行き止まりまで確認
			while (pConnect != nullptr)
			{
				// 連結しているところで分岐する
				if (pConnect->GetType() != CRoad::TYPE::TYPE_NONE)
				{
					pRoad->SearchConnect(pConnect, direction);
					break;
				}

				// 更に先
				CRoad* pConnectNext = pConnect->GetConnectRoad(direction);
				pConnect = pConnectNext;
			}
		}
	}
}

//==========================================================
// 最も近い道取得
//==========================================================
CRoad* CRoadManager::GetNearRoad(const D3DXVECTOR3& pos)
{
	float length = 1000000.0f;
	CRoad* pRoad = nullptr;
	// 道数分繰り返す
	for (int i = 0; i < GetList()->GetNum(); i++)
	{
		// 確認
		CRoad* pCheck = GetList()->Get(i);
		D3DXVECTOR3 vec = pCheck->GetPosition() - pos;
		float temp = D3DXVec3Length(&vec);
		
		// 距離が近い
		if (temp <= length)
		{
			length = temp;
			pRoad = pCheck;
		}
	}

	return pRoad;
}