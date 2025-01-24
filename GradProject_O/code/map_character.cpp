//==========================================================
//
// マップ配置のキャラクター処理 [map_character.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "map_character.h"
#include "character.h"
#include "motion.h"

//==========================================================
// 定数定義
//==========================================================
namespace
{
	
}

//==========================================================
// コンストラクタ
//==========================================================
CMapCharacter::CMapCharacter()
{
	// 値のクリア
	m_pCharacter = nullptr;
	m_Info = SInfo();
}

//==========================================================
// デストラクタ
//==========================================================
CMapCharacter::~CMapCharacter()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CMapCharacter::Init()
{
	return S_OK;
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CMapCharacter::Init(const D3DXVECTOR3& rot, const char* pPath, int nPlayMotion)
{
	m_pCharacter = CCharacter::Create(pPath);
	m_pCharacter->SetParent(NULL);
	m_pCharacter->GetMotion()->InitSet(nPlayMotion);
	m_pCharacter->SetScale(D3DXVECTOR3(7.0f, 7.0f, 7.0f));

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CMapCharacter::Uninit(void)
{
	SAFE_UNINIT(m_pCharacter)

	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CMapCharacter::Update(void)
{
	Set();

	if (m_pCharacter != nullptr)
	{
		m_pCharacter->Update();
	}
}

//==========================================================
// 生成
//==========================================================
CMapCharacter* CMapCharacter::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale, const char* pPath, int nPlayMotion)
{
	CMapCharacter* pMapCharacter = nullptr;

	pMapCharacter = DEBUG_NEW CMapCharacter;

	if (pMapCharacter != nullptr)
	{
		// 初期化処理
		pMapCharacter->Init(rot, pPath, nPlayMotion);

		// 座標設定
		pMapCharacter->SetPosition(pos);

		// 向き設定
		pMapCharacter->SetRotation(rot);

		// 大きさ設定
		pMapCharacter->SetScale(scale);
	}

	return pMapCharacter;
}

//==========================================================
// 座標設定
//==========================================================
void CMapCharacter::SetPosition(const D3DXVECTOR3& pos)
{
	m_Info.pos = pos;

	if (m_pCharacter != nullptr)
	{
		m_pCharacter->SetPosition(m_Info.pos);
	}
}

//==========================================================
// 向き設定
//==========================================================
void CMapCharacter::SetRotation(const D3DXVECTOR3& rot)
{
	m_Info.rot = rot;
	correction::Adjust(&m_Info.rot);

	if (m_pCharacter != nullptr)
	{
		m_pCharacter->SetRotation(m_Info.rot);
	}
}

//==========================================================
// 大きさ設定
//==========================================================
void CMapCharacter::SetScale(const D3DXVECTOR3& scale)
{
	m_Info.scale = scale;

	if (m_pCharacter != nullptr)
	{
		m_pCharacter->SetScale(m_Info.scale);
	}
}

//==========================================================
// 設定
//==========================================================
void CMapCharacter::Set()
{
	if (m_pCharacter != nullptr)
	{
		m_pCharacter->SetPosition(m_Info.pos);
		m_pCharacter->SetRotation(m_Info.rot);
	}
}