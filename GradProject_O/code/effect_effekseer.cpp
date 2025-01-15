//===============================================
//
// エフェクトの処理 [effect.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "effect_effekseer.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "texture.h"
#include "deltatime.h"
#include "game.h"
#include "meshfield.h"
#include "billboard.h"
#include "deltatime.h"

//===============================================
// 無名名前空間
//===============================================
namespace {
	const std::string EFFECTNAMEPATH[CEffectEffekseer::TYPE_MAX] = {	// エフェクトのパス
		"data\\EFFEKSEER\\patrollamp.efkefc",
	};
}

//===============================================
// コンストラクタ
//===============================================
CEffectEffekseer::CEffectEffekseer()
{
	m_Info = SInfo();
}

//===============================================
// デストラクタ
//===============================================
CEffectEffekseer::~CEffectEffekseer()
{

}

//===============================================
// 初期化処理
//===============================================
HRESULT CEffectEffekseer::Init(void)
{
	m_pEffekseer = CEffekseer::GetInstance()->Create(EFFECTNAMEPATH[m_Info.Type],
		m_Info.pos, m_Info.rot, m_Info.move, m_Info.fScale, false, false);

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CEffectEffekseer::Uninit(void)
{
	SAFE_DELETE(m_pEffekseer)

	Release();
}

//===============================================
// 更新処理
//===============================================
void CEffectEffekseer::Update(void)
{
	
}

//===============================================
// 生成
//===============================================
CEffectEffekseer *CEffectEffekseer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, float fScale, TYPE type)
{
	CEffectEffekseer *pEffectEffekseer = nullptr;
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ

	// エフェクトの生成
	pEffectEffekseer = DEBUG_NEW CEffectEffekseer;

	if (pEffectEffekseer != NULL)
	{// 生成できた場合

		// 座標設定
		pEffectEffekseer->SetPosition(pos);

		// 角度設定
		pEffectEffekseer->SetRotation(rot);

		// 角度設定
		pEffectEffekseer->SetMove(move);

		// 角度設定
		pEffectEffekseer->SetScale(fScale);

		// 種類の設定
		pEffectEffekseer->SetType(type);

		// 初期化処理
		pEffectEffekseer->Init();
	}
	else
	{// 生成に失敗した場合
		return NULL;
	}

	return pEffectEffekseer;
}