//============================================================
//
//	シェーダー管理関数 [ShaderLight.cpp]
//	Author：丹野竜之介
//
//============================================================
//============================================================
//	インクルードファイル
//============================================================
#include "shaderlight.h"
#include "manager.h"
#include "renderer.h"
//============================================================
//	定数宣言
//============================================================
namespace
{
	const char* EFFECT_FX = "code\\shader\\draw.fx";	// シェーダーのエフェクトファイル
	const D3DXCOLOR LIGHT_COLOR = D3DXVECTOR4(0.6f, 0.6f, 0.7f, 1.0f);
	const D3DXVECTOR4 LIGHT_VEC = D3DXVECTOR4(-0.5f, -0.87f, 0.05f, 0.0f);
}
//************************************************************
//	静的メンバ変数宣言
//************************************************************
Clist<CShaderLight::SLight*>* CShaderLight::m_List = nullptr;	// シェーダー情報

//============================================================
//	コンストラクタ
//============================================================
CShaderLight::CShaderLight()
{
	
}

//============================================================
//	デストラクタ
//============================================================
CShaderLight::~CShaderLight()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CShaderLight::Init(void)
{
	
	// 成功を返す
	return S_OK;
}
//============================================================
//	終了処理
//============================================================
void CShaderLight::Uninit(void)
{
	
}
//============================================================
//	更新処理
//============================================================
void CShaderLight::Update(void)
{

}
//============================================================
//	生成処理
//============================================================
CShaderLight::SLight* CShaderLight::Create(D3DXVECTOR3 pos, D3DXVECTOR3 col, float intens, float range, D3DXVECTOR3 vec, float Angle)
{
	if (m_List == nullptr)
	{
		m_List = DEBUG_NEW Clist<CShaderLight::SLight*>;
		m_List->Clear();
	}
	CShaderLight::SLight* plight;
	// トゥーンシェーダーの生成
		plight = DEBUG_NEW CShaderLight::SLight;
	if (plight == nullptr)
	{ // 生成に失敗した場合

		assert(false);
		return nullptr;
	}
	else
	{ // 生成に成功した場合
	
		plight->position = pos;
		plight->color = col;
		plight->intensity = intens;
		plight->range = range;
		D3DXVec3Normalize(&vec, &vec);
		plight->direction = vec;
		plight->spotAngle = Angle;
		m_List->Regist(plight);
		// 確保したアドレスを返す
		return plight;
	}
}
