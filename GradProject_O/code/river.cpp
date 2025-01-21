//===============================================
//
// 川の処理 [river.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "river.h"
#include "meshfield.h"
#include "player.h"
#include "player_manager.h"

namespace
{
	const char* FILENAME[river::MAX] =
	{
		"data\\TEXTURE\\river000.jpg",
		"data\\TEXTURE\\river000.png",
	};
}

//===============================================
// コンストラクタ
//===============================================
CRiver::CRiver(SInfo info) :
	m_Info(info)
{
	// 値のクリア
	m_obj.clear();
}

//===============================================
// デストラクタ
//===============================================
CRiver::~CRiver()
{
	
}

//===============================================
// 生成処理
//===============================================
CRiver* CRiver::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR2& size,
	const D3DXVECTOR2& speed, const int& nWidth, const int& nHeight)
{
	CRiver* pRiver = DEBUG_NEW CRiver(SInfo(pos, rot, size, speed, nWidth, nHeight));

	// 生成失敗
	if (pRiver == nullptr) { return nullptr; }

	// 初期化処理
	pRiver->Init();

	return pRiver;
}

//===============================================
// 初期化処理
//===============================================
HRESULT CRiver::Init(void)
{
	// オブジェクトの初期化
	for (int i = 0; i < river::MAX; i++)
	{
		// 座標設定
		D3DXVECTOR3 pos = VECTOR3_ZERO;
		pos.y += i * 1.0f;
		
		// 生成
		CMeshField* p = CMeshField::Create(m_Info.pos + pos, m_Info.rot, m_Info.size.x, m_Info.size.y, FILENAME[i],
			m_Info.nWidth, m_Info.nHeight);

		// スクロール速度設定
		p->SetTexMove(m_Info.speed + (m_Info.speed * static_cast<float>(i)));

		// 衝突設定
		p->SetCollision(false);

		// リストに挿入
		m_obj.push_back(p);
	}

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CRiver::Uninit(void)
{
	// オブジェクトの終了
	for (const auto& it : m_obj)
	{
		it->Uninit();
	}

	m_obj.clear();

	// 解放
	Release();
}

//===============================================
// 更新処理
//===============================================
void CRiver::Update(void)
{
	// オブジェクトの更新
	for (const auto& it : m_obj)
	{
		it->Update();
	}

	// プレイヤーの落下確認
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
	if (pPlayer == nullptr) { return; }

	if (pPlayer->GetPosition().y <= m_Info.pos.y)
	{
		pPlayer->SetPosition(VECTOR3_ZERO);
		pPlayer->Damage(pPlayer->GetLifeOrigin() * 0.1f);
	}
}