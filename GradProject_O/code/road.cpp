//==========================================================
//
// 道処理 [road.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "road.h"
#include "object3D.h"
#include "road_manager.h"

//==========================================================
// コンストラクタ
//==========================================================
CRoad::CRoad(const SInfo& info)
{
	// 値のクリア
	m_Info = info;
	m_pObj = nullptr;

	for (int i = 0; i < DIRECTION::DIC_MAX; i++)
	{
		m_apConnectRoad[i] = nullptr;
	}

	CRoadManager::GetInstance()->ListIn(this);
}

//==========================================================
// デストラクタ
//==========================================================
CRoad::~CRoad()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CRoad::Init(void)
{
	// オブジェクトの生成
	m_pObj = CObject3D::Create(m_Info.pos, m_Info.rot);
	m_pObj->SetpVtx(m_Info.size.x, m_Info.size.y);

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CRoad::Uninit(void)
{
	// 連結道路の記憶を廃棄
	for (int i = 0; i < DIRECTION::DIC_MAX; i++)
	{
		m_apConnectRoad[i] = nullptr;
	}

	// 描画オブジェクトを廃棄
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	CRoadManager::GetInstance()->ListOut(this);

	// 開放
	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CRoad::Update(void)
{

}

//==========================================================
// 生成
//==========================================================
CRoad* CRoad::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR2& size)
{
	// 生成
	CRoad* pRoad = nullptr;
	pRoad = new CRoad(SInfo(pos, rot, size));

	// 生成成功
	if (pRoad != nullptr)
	{
		// 初期化処理
		pRoad->Init();
	}

	return pRoad;
}

//==========================================================
// 連結
//==========================================================
void CRoad::Connect(CRoad* pRoad, const DIRECTION dic)
{
	// 範囲外確認
	if (dic < DIRECTION::DIC_UP || dic >= DIRECTION::DIC_MAX) { return; }

	m_apConnectRoad[dic] = pRoad;
}