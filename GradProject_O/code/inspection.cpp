//==========================================================
//
// 検問処理 [inspection.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "inspection.h"
#include "add_police.h"
#include "objectX.h"

// 無名名前空間を定義
namespace
{
	const float POLICE_SETROT = D3DX_PI * 0.5f;
	const float POLICE_SETLENGTH = 500.0f;
}

//==========================================================
// コンストラクタ
//==========================================================
CInstpection::CInstpection()
{
	// 値のクリア
	m_Guard = SGuardInfo();
	m_Info = SInfo();
}

//==========================================================
// デストラクタ
//==========================================================
CInstpection::~CInstpection()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CInstpection::Init(void)
{
	// 柵の生成
	m_Guard.pObj = CObjectX::Create(m_Info.pos, m_Info.rot, "data\\MODEL\\map\\guardrail001.x");

	// パトカーの設定
	{
		// 向き取得
		D3DXVECTOR3 rot = m_Info.rot;
		rot.y = POLICE_SETROT + m_Info.rot.y;
		Adjust(rot);

		// 座標取得
		D3DXVECTOR3 pos = m_Info.pos;
		pos.x += sinf(rot.y) * POLICE_SETLENGTH;
		pos.z += cosf(rot.y) * POLICE_SETLENGTH;

		// 生成
		m_pPolice = CAddPolice::Create(pos, rot, VECTOR3_ZERO);
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CInstpection::Uninit(void)
{
	// 警察廃棄
	if (m_pPolice != nullptr)
	{
		m_pPolice->Uninit();
		m_pPolice = nullptr;
	}

	if (m_Guard.pObj != nullptr)
	{
		m_Guard.pObj->Uninit();
		m_Guard.pObj = nullptr;
	}

	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CInstpection::Update(void)
{
	if (m_Guard.pObj != nullptr)
	{
		if (m_Guard.pObj->GetHit() || m_Guard.pObj->GetHitOld())
		{
			m_bHit = true;
		}
	}

	// 衝突していない
	if (m_bHit) { return; }
}

//==========================================================
// 生成
//==========================================================
CInstpection* CInstpection::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	CInstpection* pInsp = nullptr;

	pInsp = DEBUG_NEW CInstpection;

	if (pInsp != nullptr)
	{
		// 値設定
		pInsp->SetPosition(pos);
		pInsp->SetRotation(rot);

		// 初期化処理
		pInsp->Init();
	}

	return pInsp;
}