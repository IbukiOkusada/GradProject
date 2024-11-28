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
#include "effect3D.h"
namespace
{
	const char* FILENAME[CRoad::TYPE_MAX] =	{
		"data\\TEXTURE\\road\\road009.jpg",
		"data\\TEXTURE\\road\\road005.jpg",
		"data\\TEXTURE\\road\\road006.jpg",
		"data\\TEXTURE\\road\\road007.jpg",
		"data\\TEXTURE\\road\\road008.jpg",
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
	m_Type = TYPE_MAX;

	for (int i = 0; i < DIRECTION::DIC_MAX; i++)
	{
		m_apConnectRoad[i] = nullptr;
		m_aSearchRoad[i] = SSearch();
	}

	m_nIdx = CRoadManager::GetInstance()->GetList()->GetNum();
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
	CTexture* pTex = CManager::GetInstance()->GetTexture();
	m_pObj = CObject3D::Create(m_Info.pos, m_Info.rot);
	m_pObj->SetpVtx(m_Info.size.x, m_Info.size.y);
	m_pObj->BindTexture(pTex->Regist(FILENAME[TYPE_CROSSING]));
	m_Searchself.pos = m_Info.pos;
	m_Searchself.pRoad = this;
	for (int i = 0; i < 4; i++)
	{
		m_aVtxPos[i] = VECTOR3_ZERO;
	}
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
		m_aSearchRoad[i] = SSearch();
	}

	// 描画オブジェクトを廃棄
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	// リストから開放
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
	pRoad = DEBUG_NEW CRoad(SInfo(pos, rot, size));

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

	if (pRoad != nullptr)
	{
		m_apConnectLength[dic] = D3DXVec3Length(&(m_Info.pos - pRoad->GetPosition()));
	}
	else
	{
		m_apConnectLength[dic] = FLT_MAX;
	}
}

//==========================================================
// 経路探索用連結
//==========================================================
void CRoad::SearchConnect(CRoad* pRoad, const DIRECTION dic)
{
	// 範囲外確認
	if (dic < DIRECTION::DIC_UP || dic >= DIRECTION::DIC_MAX) { return; }

	m_aSearchRoad[dic].pRoad = pRoad;

	if (pRoad != nullptr)
	{
		m_aSearchRoad[dic].pos = pRoad->GetPosition();
		m_aSearchRoad[dic].bActive = true;
		return;
	}

	// 使用されていない状態にする
	m_aSearchRoad[dic] = SSearch();
}

//==========================================================
// テクスチャ設定
//==========================================================
void CRoad::BindTexture()
{
	int num = 0;
	CTexture* pTex = CManager::GetInstance()->GetTexture();

	// 総数確認
	for (int i = 0; i < DIC_MAX; i++)
	{
		if (m_apConnectRoad[i] == nullptr) { continue; }
		num++;
	}

	// 割り当て
	m_Type = static_cast<TYPE>(num);

	// 直線かどうかを確認
	if (num == 2 && 
		((m_apConnectRoad[DIC_UP] != nullptr && m_apConnectRoad[DIC_DOWN] != nullptr) || 
		(m_apConnectRoad[DIC_LEFT] != nullptr && m_apConnectRoad[DIC_RIGHT] != nullptr)))
	{
		m_Type = TYPE_NONE;
	}
	else if (num == 1)
	{
		m_Type = TYPE_STOP;
	}


	// 範囲外ならテクスチャを割り当てない
	if (m_Type >= TYPE_MAX) { return; }

	if (m_pObj == nullptr) { return; }
	m_pObj->BindTexture(pTex->Regist(FILENAME[m_Type]));

	// 向き設定
	Rotation(m_Type);
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

	// 座標取得
	VERTEX_3D* pVtx = m_pObj->GetVertex();
	for (int i = 0; i < 4; i++)
	{
		m_aVtxPos[i] = pVtx[i].pos;
	}
}

//==========================================================
// 座標
//==========================================================
void CRoad::SetPosition(const D3DXVECTOR3& pos)
{
	m_Info.pos = pos;
	
	if (m_pObj == nullptr) { return; }

	m_pObj->SetPosition(m_Info.pos);
}

//==========================================================
// サイズ
//==========================================================
void CRoad::SetSize(const D3DXVECTOR2& size)
{
	m_Info.size = size;
	m_Info.sizeOrigin = size;

	if (m_pObj == nullptr) { return; }

	m_pObj->SetpVtx(m_Info.size.x, m_Info.size.y);
}

//==========================================================
// 指定方向の交差点までを割り出し
//==========================================================
bool CRoad::GetJunctionRoad(float fRot, CRoad** pGoalOut, CRoad** pGoalPrevOut)
{
	// 最初に向かう方向を産出
	DIRECTION dic = GetDic(fRot);

	// 方向ナシ
	if (dic == DIRECTION::DIC_MAX) { return false; }

	// 連結していない
	if (!m_aSearchRoad[dic].bActive) { return false; }

	// 連結個所から交差点を導き出す
	{
		CRoad* pThis = this;
	}
	CRoad* pRoad = m_aSearchRoad[dic].pRoad;
	CRoad* pOld = this;

	// 行き止まりまで確認
	while (pRoad->GetType() != TYPE::TYPE_STOP)
	{
		// 交差点なら終了!
		if (pRoad->GetType() == TYPE::TYPE_T_JUNCTION || pRoad->GetType() == TYPE::TYPE_CROSSING)
		{
			*pGoalPrevOut = pOld;
			*pGoalOut = pRoad;
			return true;
		}

		// 交差点ではないので確認します
		CRoad* pCheck = nullptr;

		for (int i = 0; i < DIRECTION::DIC_MAX; i++)
		{
			pCheck = pRoad->m_aSearchRoad[i].pRoad;

			// 同じ道なら確認しない
			if (pCheck == pOld) { continue; }
			if (pCheck == nullptr) { continue; }

			break;
		}

		// 道がない
		if (pCheck == nullptr)
		{
			break;
		}

		// 行き止まり
		if (pCheck->GetType() == TYPE::TYPE_STOP)
		{
			break;
		}

		// カーブの場合さらに検索する
		pOld = pRoad;
		pRoad = pCheck;			// 次へ
	}

	// 存在していないので中身ナシ
	pGoalOut = nullptr;
	pGoalPrevOut = nullptr;

	return false;
}

//==========================================================
// 向きから方向を産出
//==========================================================
CRoad::DIRECTION CRoad::GetDic(float fRot)
{
	float range = D3DX_PI * 0.25f;

	// 差分から曲がった方向を求める
	if (fRot <= range && fRot >= -range)
	{// 下
		return DIRECTION::DIC_DOWN;
	}
	else if (fRot <= -range && fRot >= -range * 3)
	{// 左
		return DIRECTION::DIC_LEFT;
	}
	else if (fRot <= range * 3 && fRot >= range)
	{// 右
		return DIRECTION::DIC_RIGHT;
	}
	else if (fRot >= range * 3 && fRot <= -range * 3)
	{// 上
		return DIRECTION::DIC_UP;
	}

	return DIRECTION::DIC_MAX;
}