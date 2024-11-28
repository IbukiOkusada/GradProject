//==========================================================
//
// メーター [Container.cpp]
// Author : 丹野竜之介
//
//==========================================================
#include "Container.h"
#include "manager.h"
#include "debugproc.h"
#include "deltatime.h"
#include "player_manager.h"
#include "texture.h"
// マクロ定義

//==========================================================
// コンストラクタ
//==========================================================
CContainer::CContainer()
{
	m_state = STATE_NONE;
	m_pos = CONTAINER_POS;
	m_fScroll = 0.0f;
	// 値のクリア
}

//==========================================================
// デストラクタ
//==========================================================
CContainer::~CContainer()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CContainer::Init(void)
{
	CTexture* pTexture = CManager::GetInstance()->GetTexture();
	m_pContainer = CObject2D::Create(m_pos, VECTOR3_ZERO, 7);
	m_pContainer->SetSize(142.5f, 101.0f);
	m_pContainer->BindTexture(pTexture->Regist("data\\TEXTURE\\UI\\container.png"));
	for (int i = 0; i < ICE_NUM; i++)
	{
		m_pIce[i] = DEBUG_NEW SIce;
		m_pIce[i]->pos = m_pos + ICE_POS +D3DXVECTOR3(OFFSET_ICE.x * (i%(ICE_NUM/2)), OFFSET_ICE.y*(i / (ICE_NUM / 2)), 0.0f);
		m_pIce[i]->pIce = CObject2D::Create(m_pIce[i]->pos, VECTOR3_ZERO, 7);
		m_pIce[i]->pIce->SetSize(36.5f, 29.5f);
		m_pIce[i]->pIce->BindTexture(pTexture->Regist("data\\TEXTURE\\UI\\ice.png"));
	}
	for (int i = 0; i < LOVE_NUM; i++)
	{
		m_pLove[i] = CObject2D::Create(m_pos+(OFFSET_LOVE*(1.0f-(i*2.0f))), VECTOR3_ZERO, 7);
		m_pLove[i]->SetSize(117.0f, 8.0f);
		m_pLove[i]->BindTexture(pTexture->Regist("data\\TEXTURE\\UI\\love.png"));
		m_pLove[i]->SetFusion(CObject2D::FUSION_ADD);
	}
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CContainer::Uninit(void)
{
	for (int i = 0; i < ICE_NUM; i++)
	{
		SAFE_DELETE(m_pIce[i]);
	}
	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CContainer::Update(void)
{
	
	m_fScroll += 0.001f;
	m_pLove[0]->SetVtx(m_fScroll, 0.0f, 117.0f, 8.0f);
	m_pLove[1]->SetVtx(-m_fScroll, 0.0f, 117.0f, 8.0f);
	
	switch (m_state)
	{
	case CContainer::STATE_NONE:
		break;
	case CContainer::STATE_BOOT:
		BootAnimation();
		break;
	case CContainer::STATE_NORMAL:
	
		break;
	case CContainer::STATE_MAX:
		break;
	default:
		break;
	}
	m_nStateCount--;
	if (m_nStateCount <= 0)
	{
		m_state = STATE_NORMAL;
	}

}
//==========================================================
// 更新処理
//==========================================================
void CContainer::BootAnimation(void)
{

}

//==========================================================
// 生成
//==========================================================
CContainer* CContainer::Create(void)
{
	CContainer* pContainer = nullptr;

	pContainer = DEBUG_NEW CContainer;

	if (pContainer != nullptr)
	{
		// 初期化処理
		pContainer->Init();
	}

	return pContainer;
}
