//==========================================================
//
// 道処理 [road.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "road.h"
#include "object3D.h"
#include "road_manager.h"
#include "manager.h"
#include "texture.h"

namespace
{
	const char* FILENAME[CRoad::TYPE_MAX] =	{
		"data\\TEXTURE\\road\\road004.jpg",
		"data\\TEXTURE\\road\\road000.jpg",
		"data\\TEXTURE\\road\\road001.jpg",
		"data\\TEXTURE\\road\\road002.jpg",
		"data\\TEXTURE\\road\\road003.jpg",
	};
}

//==========================================================
// コンストラクタ
//==========================================================
CRoad::CRoad(const SInfo& info)
{
	// 値のクリア
	m_Info = info;
	m_pObj = nullptr;
	m_nIdx = -1;

	for (int i = 0; i < DIRECTION::DIC_MAX; i++)
	{
		m_apConnectRoad[i] = nullptr;
	}

	m_nIdx = CRoadManager::GetInstance()->GetNum();
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

//==========================================================
// テクスチャ設定
//==========================================================
void CRoad::BindTexture()
{
	int idx = -1;
	int num = 0;
	CTexture* pTex = CManager::GetInstance()->GetTexture();

	// 総数確認
	for (int i = 0; i < DIC_MAX; i++)
	{
		if (m_apConnectRoad[i] == nullptr) { continue; }
		num++;
	}

	// 割り当て
	idx = num;

	// 直線かどうかを確認
	if (num == 2 && 
		((m_apConnectRoad[DIC_UP] != nullptr && m_apConnectRoad[DIC_DOWN] != nullptr) || 
		(m_apConnectRoad[DIC_LEFT] != nullptr && m_apConnectRoad[DIC_RIGHT] != nullptr)))
	{
		idx = TYPE_NONE;
	}
	else if (num == 1)
	{
		idx = TYPE_STOP;
	}


	// 範囲外ならテクスチャを割り当てない
	if (idx < 0) { return; }

	if (m_pObj == nullptr) { return; }
	m_pObj->BindTexture(pTex->Regist(FILENAME[idx]));

	// 向き設定
	Rotation(static_cast<TYPE>(idx));
}

//==========================================================
// 回転
//==========================================================
void CRoad::Rotation(TYPE type)
{
	switch (type)
	{
	case CRoad::TYPE_STOP:
	{
		if (m_apConnectRoad[DIC_UP] != nullptr)
		{
			m_Info.rot.y = D3DX_PI;
		}
		else if (m_apConnectRoad[DIC_LEFT] != nullptr)
		{
			m_Info.rot.y = D3DX_PI * 0.5f;
		}
		else if (m_apConnectRoad[DIC_RIGHT] != nullptr)
		{
			m_Info.rot.y = -D3DX_PI * 0.5f;
		}
	}
		break;
	case CRoad::TYPE_NONE:
	{
		if (m_apConnectRoad[DIC_LEFT] != nullptr && m_apConnectRoad[DIC_RIGHT] != nullptr)
		{
			m_Info.rot.y = D3DX_PI * 0.5f;
		}
	}
		break;
	case CRoad::TYPE_CURVE:
	{
		if (m_apConnectRoad[DIC_UP] != nullptr)
		{
			if (m_apConnectRoad[DIC_LEFT] != nullptr)
			{
				m_Info.rot.y = D3DX_PI;
			}
			else if (m_apConnectRoad[DIC_RIGHT] != nullptr)
			{
				m_Info.rot.y = -D3DX_PI * 0.5f;
			}
		}
		else if (m_apConnectRoad[DIC_DOWN] != nullptr)
		{
			if (m_apConnectRoad[DIC_LEFT] != nullptr)
			{
				m_Info.rot.y = D3DX_PI * 0.5f;
			}
			else if (m_apConnectRoad[DIC_RIGHT] != nullptr)
			{
				m_Info.rot.y = 0.0f;
			}
		}
	}
		break;
	case CRoad::TYPE_T_JUNCTION:
	{
		if (m_apConnectRoad[DIC_UP] == nullptr)
		{
			m_Info.rot.y = D3DX_PI * 0.5f;
		}
		else if (m_apConnectRoad[DIC_DOWN] == nullptr)
		{
			m_Info.rot.y = -D3DX_PI * 0.5f;
		}
		else if (m_apConnectRoad[DIC_LEFT] == nullptr)
		{
			m_Info.rot.y = 0.0f;
		}
		else if (m_apConnectRoad[DIC_RIGHT] == nullptr)
		{
			m_Info.rot.y = D3DX_PI;
		}
	}
		break;
	default:
		break;
	}

	// 4/1回転
	if (m_Info.rot.y == -D3DX_PI * 0.5f || m_Info.rot.y == D3DX_PI * 0.5f)
	{
		float f = m_Info.size.x;
		m_Info.size.x = m_Info.size.y;
		m_Info.size.y = f;
		m_pObj->SetpVtx(m_Info.size.x, m_Info.size.y);
	}

	m_pObj->SetRotation(m_Info.rot);
}