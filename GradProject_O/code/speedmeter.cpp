//==========================================================
//
// スピードメーターの処理 [speedmeter.cpp]
// Author : Ryosuke Ohara
//
//==========================================================
#include "speedmeter.h"
#include "object2D.h"
#include "player.h"
#include "player_manager.h"
#include "texture.h"
#include "manager.h"
#include "deltatime.h"
#include "debugproc.h"

// 静的メンバ変数宣言
CSpeedMeter* CSpeedMeter::m_pInstance = nullptr;

//==========================================================
// 定数定義
//==========================================================
namespace FRAME
{
	const float HEIGHT = 100.0f;  // 高さ
	const float WIDTH = 100.0f;   // 横幅
	const D3DXVECTOR3 POS = D3DXVECTOR3(SCREEN_WIDTH * 0.8f, SCREEN_HEIGHT * 0.8f, 0.0f);
	const char* TEX_PATH = "data\\TEXTURE\\meter00.png";
}

namespace NEEDLE
{
	const float HEIGHT = 100.0f;  // 高さ
	const float WIDTH = 50.0f;   // 横幅
	const float START = -D3DX_PI * 0.7f;
	const float LIMIT = D3DX_PI * 0.7f;
	const float DEST = (LIMIT - START);
	const D3DXVECTOR3 POS = D3DXVECTOR3(SCREEN_WIDTH * 0.8f, SCREEN_HEIGHT * 0.8f, 0.0f);
	const D3DXVECTOR3 ROT = D3DXVECTOR3(0.0f, 0.0f, D3DX_PI/* * 0.7f*/);
	const char* TEX_PATH = "data\\TEXTURE\\Needle.png";
}

//==========================================================
// コンストラクタ
//==========================================================
CSpeedMeter::CSpeedMeter()
{
	m_pMeter = nullptr;
	m_pNeedle = nullptr;

	m_pInstance = this;
}

//==========================================================
// デストラクタ
//==========================================================
CSpeedMeter::~CSpeedMeter()
{
}

//==========================================================
// 生成処理
//==========================================================
CSpeedMeter* CSpeedMeter::Create(void)
{
	CSpeedMeter* pSpeedMeter = DEBUG_NEW CSpeedMeter;

	if (pSpeedMeter != nullptr)
	{
		pSpeedMeter->Init();
	}

	return pSpeedMeter;
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CSpeedMeter::Init(void)
{
	// メーター生成
	if (m_pMeter == nullptr)
	{
		m_pMeter = CObject2D::Create(7);
		m_pMeter->SetPosition(FRAME::POS);
		m_pMeter->SetSize(FRAME::WIDTH, FRAME::HEIGHT);
		m_pMeter->BindTexture(CManager::GetInstance()->GetTexture()->Regist(FRAME::TEX_PATH));
		m_pMeter->SetDraw();
	}

	// 針生成
	if (m_pNeedle == nullptr)
	{
		m_pNeedle = CObject2D::Create(7);
		m_pNeedle->SetPosition(NEEDLE::POS);
		m_pNeedle->SetRotation(NEEDLE::ROT);
		m_pNeedle->SetSize(NEEDLE::WIDTH, NEEDLE::HEIGHT);
		m_pNeedle->BindTexture(CManager::GetInstance()->GetTexture()->Regist(NEEDLE::TEX_PATH));
		m_pNeedle->SetDraw();
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CSpeedMeter::Uninit(void)
{
	// メーターの枠の破棄
	if (m_pMeter != nullptr)
	{
		m_pMeter->Uninit();
		m_pMeter = nullptr;
	}

	// メーターの針の破棄
	if (m_pNeedle != nullptr)
	{
		m_pNeedle->Uninit();
		m_pNeedle = nullptr;
	}

	// 自身の破棄
	if (m_pInstance != nullptr)
	{
		m_pInstance = nullptr;
	}

	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CSpeedMeter::Update(void)
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	
	// どちらかが使用されていない場合処理を抜ける
	if (m_pNeedle == nullptr || pPlayer == nullptr)
		return;

	float fEngineRate = (pPlayer->GetEngine() / 1.0f);

	float fRot = NEEDLE::START + fEngineRate * NEEDLE::DEST;

	m_pNeedle->SetRotation({ 0.0f , 0.0f,  -fRot });

	m_pNeedle->SetVtx();
}

//==========================================================
// 描画処理
//==========================================================
void CSpeedMeter::Draw(void)
{

}