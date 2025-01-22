//===============================================
//
// エフェクトの処理 [effect.cpp]
// Author : Riku Nakamura
//
//===============================================
#include "effect_effekseer.h"
#include "debugproc.h"

Clist<CEffectEffekseer*> CEffectEffekseer::m_List = {};

//===============================================
// 無名名前空間
//===============================================
namespace {
	const std::string EFFECTNAMEPATH[CEffectEffekseer::TYPE_MAX] = {	// エフェクトのパス
		"data\\EFFEKSEER\\0_signage.efkefc",
		"data\\EFFEKSEER\\0_stop.efkefc",
		"data\\EFFEKSEER\\0_curve.efkefc",
		"data\\EFFEKSEER\\0_curve_turnleft.efkefc",
		"data\\EFFEKSEER\\0_no_more_sweet.efkefc",
		"data\\EFFEKSEER\\0_crossing.efkefc",
		"data\\EFFEKSEER\\0_line.efkefc",
		"data\\EFFEKSEER\\0_electric.efkefc",
		"data\\EFFEKSEER\\0_graffiti.efkefc",
		"data\\EFFEKSEER\\0_spark.efkefc",
	};
}

//===============================================
// コンストラクタ
//===============================================
CEffectEffekseer::CEffectEffekseer()
{
	m_Info = SInfo();
	m_nId = m_List.GetNum();
	m_List.Regist(this);
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
	m_List.Delete(this);
	SAFE_DELETE(m_pEffekseer)

	Release();
}

//===============================================
// 更新処理
//===============================================
void CEffectEffekseer::Update(void)
{
	m_pEffekseer->m_pos = m_Info.pos;
	m_pEffekseer->m_rot = m_Info.rot;
	m_pEffekseer->m_move = m_Info.move;
	m_pEffekseer->m_Scale.X = m_Info.fScale;
	m_pEffekseer->m_Scale.Y = m_Info.fScale;
	m_pEffekseer->m_Scale.Z = m_Info.fScale;
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