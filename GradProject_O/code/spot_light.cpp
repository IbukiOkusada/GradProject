//==========================================================
//
// スポットライトの処理 [spot_light.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "spot_light.h"
#include "renderer.h"
#include "manager.h"
#include "light.h"
#include "debugproc.h"

//==========================================================
// コンストラクタ
//==========================================================
CSpotLight::CSpotLight()
{
	m_nId = -1;
}

//==========================================================
// デストラクタ
//==========================================================
CSpotLight::~CSpotLight()
{

}

//==========================================================
// 生成処理
//==========================================================
CSpotLight* CSpotLight::Create()
{
	CSpotLight* pLight = DEBUG_NEW CSpotLight;

	if (pLight != nullptr)
	{
		pLight->Init();
	}

	return pLight;
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CSpotLight::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		// デバイスへのポインタを取得

	D3DXVECTOR3 vecDir;		// 設定変更用ベクトル

	// ライトの情報をクリアする
	ZeroMemory(&m_Light, sizeof(D3DLIGHT9));

	// ライトの種類を設定
	m_Light.Type = D3DLIGHT_SPOT;
	m_Light.Position = D3DXVECTOR3(0.0f, 500.0f, 0.0f);

	// ライトの拡散光を設定
	m_Light.Diffuse = D3DXCOLOR(7.0f, 7.0f, 7.0f, 1.0f);

	// 照らす方向を正しい値に設定
	D3DXVECTOR3 target = D3DXVECTOR3(0.0f, 0.0f, 0.0f);  // 照らしたい場所
	D3DXVECTOR3 position = m_Light.Position;            // ライトの位置
	D3DXVECTOR3 direction = target - position;          // ライトからターゲットへの方向
	D3DXVec3Normalize(&direction, &direction);          // 正規化
	m_Light.Direction = direction;

	//ライトの範囲設定
	m_Light.Range = 2000.0f;

	m_Light.Phi = D3DXToRadian(45.0f);	// 外側のコーンの角度
	m_Light.Theta = D3DXToRadian(00.0f);	// 内側のコーンの角度
	m_Light.Falloff = 1.0f;				// 内側と外側のコーンの減衰率

	//ライトの減衰0の設定
	m_Light.Attenuation0 = 0.94f;

	//ライトの減衰1の設定
	m_Light.Attenuation1 = 0.0f;

	//ライトの減衰2の設定
	m_Light.Attenuation2 = 0.0f;

	// ライトを設定する
	m_nId = CLight::GetNumLight();
	CLight::AddNumLight();
	pDevice->SetLight(m_nId, &m_Light);

	// ライトを有効化する
	pDevice->LightEnable(m_nId, TRUE);
	CLight::AddNumLight();

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CSpotLight::Uninit()
{
	// ライトを無効化する
	CRenderer* pRenderer = CManager::GetInstance()->GetRenderer();
	if (pRenderer != nullptr)
	{
		LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタを取得
		if (pDevice != nullptr)
		{
			pDevice->LightEnable(m_nId, false);
		}
	}

	CLight::MinusNumLight();
	delete this;
}

//==========================================================
// 更新処理
//==========================================================
void CSpotLight::Update()
{
	// ライトを設定する
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		// デバイスへのポインタを取得
	pDevice->SetLight(m_nId, &m_Light);

	CDebugProc* pProc = CDebugProc::GetInstance();
	pProc->Print("ライトの座標 : [ %f, %f, %f ]\n", m_Light.Position.x, m_Light.Position.y, m_Light.Position.z);
	pProc->Print("ライトの方向 : [ %f, %f, %f ]\n", m_Light.Direction.x, m_Light.Direction.y, m_Light.Direction.z);
	pProc->Print("ライトの色 : [ %f, %f, %f, %f ]\n", m_Light.Diffuse.r, m_Light.Diffuse.g, m_Light.Diffuse.b, m_Light.Diffuse.a);
}

//==========================================================
// 座標設定
//==========================================================
void CSpotLight::SetPositon(const D3DXVECTOR3& pos)
{
	// ライトの位置
	m_Light.Position = pos;
}

//==========================================================
// 方向設定
//==========================================================
void CSpotLight::SetDirection(const D3DXVECTOR3& dic)
{
	// ライトの方向
	m_Light.Direction = dic;
}

//==========================================================
// 拡散光設定
//==========================================================
void CSpotLight::SetDiffuse(const D3DXCOLOR& dif)
{
	// ライトの
	m_Light.Diffuse = dif;
}

//==========================================================
// 範囲設定
//==========================================================
void CSpotLight::SetRange(const float fRange)
{
	// ライトの位置 (車のライトの位置に相当)
	m_Light.Range = fRange;
}

//==========================================================
// パラメータ設定
//==========================================================
void CSpotLight::SetParameter(float fOutRadian, float fInRadian, float fFallOff)
{
	m_Light.Phi = fOutRadian;	// 外側のコーンの角度
	m_Light.Theta = fInRadian;	// 内側のコーンの角度
	m_Light.Falloff = fFallOff; // 内側と外側のコーンの減衰率
}

//==========================================================
// 減衰設定
//==========================================================
void CSpotLight::SetAttenuation(float fConstant, float fLinear, float fSecondary)
{
	// 減衰設定
	m_Light.Attenuation0 = fConstant;  // 定数減衰
	m_Light.Attenuation1 = fLinear;		// 線形減衰
	m_Light.Attenuation2 = fSecondary; // 二次減衰
}