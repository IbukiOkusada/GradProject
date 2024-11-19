//==========================================================
//
// 跳ね橋 [Bridge.cpp]
// Author : 丹野竜之介
//
//==========================================================
#include "manager.h"
#include "Bridge.h"
#include "deltatime.h"
// マクロ定義

//==========================================================
// コンストラクタ
//==========================================================
CBridge::CBridge()
{
	// 値のクリア
	m_bPass = false;
	m_fDownTime = 0.0f;
	m_fUpTime = 300.0f;
	m_fTimeCount = 300.0f;

	for (int i = 0; i < 2; i++)
	{
		m_pBridge[i] = nullptr;
	}
}

//==========================================================
// デストラクタ
//==========================================================
CBridge::~CBridge()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CBridge::Init(void)
{

	for (int i = 0; i < BRIDGE_NUM; i++)
	{
		D3DXMATRIX mtxRot;
		D3DXVECTOR3 pos = GetPos();
		D3DXVECTOR3 offset = VECTOR3_ZERO;
		D3DXVECTOR3 rot = GetRot();
		D3DXVECTOR3 scale = GetScale();
		offset.z += (GetScale().z * 0.5f) ;
		rot.y += D3DX_PI * i;
		D3DXMatrixIdentity(&mtxRot);
		D3DXMatrixRotationY(&mtxRot, rot.y);
		D3DXVec3TransformCoord(&offset, &offset, &mtxRot);
		m_pBridge[i] = CObjectX::Create(pos+offset, rot, "data\\MODEL\\map\\cube_bridge.x");
		scale.z *= 0.5f;
		m_pBridge[i]->SetScale(scale);
	}

	SetType(TYPE::TYPE_BRIDGE);

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CBridge::Uninit(void)
{
	for (int i = 0; i < BRIDGE_NUM; i++)
	{
		if (m_pBridge[i] == nullptr) { continue; }
		m_pBridge[i]->Uninit();
		m_pBridge[i] = nullptr;
	}

	// 終了
	CGimmick::Uninit();
}

//==========================================================
// 更新処理
//==========================================================
void CBridge::Update(void)
{
	m_fTimeCount += 1.0f;
	if (m_bPass)
	{
		if (m_fTimeCount > m_fDownTime)
		{
			m_fTimeCount = 0.0f;
			m_bPass = false;
		}
	}
	else
	{
		if (m_fTimeCount > m_fUpTime)
		{
			m_fTimeCount = 0.0f;
			m_bPass = true;
		}
	}
	MoveBridge();
	//CManager::GetInstance()->GetDeltaTime()->GetDeltaTime();
}
//==========================================================
// 橋の可動
//==========================================================
void CBridge::MoveBridge(void)
{
	float fAngle;
	if (m_bPass)
	{
		fAngle = 0.0f;
	}
	else
	{
		fAngle = D3DX_PI * 0.4f;
		
	}
	for (int i = 0; i < BRIDGE_NUM; i++)
	{
		D3DXVECTOR3 rot = m_pBridge[i]->GetRotation();
		D3DXVECTOR3 targetrot = rot;
		targetrot.x = fAngle;
		m_pBridge[i]->SetRotation(rot + ((targetrot - rot) * 0.03f));
	}
}
//==========================================================
// 生成
//==========================================================
CBridge* CBridge::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fdown, float fup)
{
	CBridge* pBridge = nullptr;

	pBridge = DEBUG_NEW CBridge;

	if (pBridge != nullptr)
	{
		pBridge->SetPos(pos);
		pBridge->SetRot(rot);
		pBridge->SetScale(scale);
		pBridge->m_fDownTime = fdown;
		pBridge->m_fUpTime = fup;
		// 初期化処理
		pBridge->Init();
	}

	return pBridge;
}

//==========================================================
// 色倍率変更
//==========================================================
void CBridge::SetColMulti(const D3DXCOLOR& col)
{
	for (int i = 0; i < BRIDGE_NUM; i++)
	{
		if (m_pBridge[i] == nullptr) { continue; }
		m_pBridge[i]->SetColMulti(col);
	}
}

//==========================================================
// 座標反映
//==========================================================
void CBridge::SetObjPos(const D3DXVECTOR3& pos)
{
	for (int i = 0; i < BRIDGE_NUM; i++)
	{
		if (m_pBridge[i] == nullptr) { continue; }
		m_pBridge[i]->SetPosition(pos);
	}
}

//==========================================================
// 向き反映
//==========================================================
void CBridge::SetObjRot(const D3DXVECTOR3& rot)
{
	for (int i = 0; i < BRIDGE_NUM; i++)
	{
		if (m_pBridge[i] == nullptr) { continue; }
		m_pBridge[i]->SetRotation(rot);
	}
}

//==========================================================
// スケール反映
//==========================================================
void CBridge::SetObjScale(const D3DXVECTOR3& scale)
{
	for (int i = 0; i < BRIDGE_NUM; i++)
	{
		if (m_pBridge[i] == nullptr) { continue; }
		m_pBridge[i]->SetScale(scale);
	}
}