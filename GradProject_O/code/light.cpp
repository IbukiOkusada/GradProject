//==========================================================
//
// ライトの処理全般 [light.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "light.h"
#include "input.h"
#include "manager.h"
#include "renderer.h"

// 静的メンバ変数宣言
int CLight::m_nNumLightCnt = 0;

//==========================================================
// コンストラクタ
//==========================================================
CLight::CLight()
{
	
}

//==========================================================
// デストラクタ
//==========================================================
CLight::~CLight()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CLight::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		// デバイスへのポインタを取得
	D3DXVECTOR3 vecDir;		// 設定変更用ベクトル

	for (int nCntLight = 0; nCntLight < NUM_LIGHT; nCntLight++)
	{
		// ライトの情報をクリアする
		ZeroMemory(&m_aLight[nCntLight], sizeof(D3DLIGHT9));

		// ライトの種類を設定
		m_aLight[nCntLight].Type = D3DLIGHT_DIRECTIONAL;

		// ライトの拡散光を設定
		switch (nCntLight)
		{
		case 0:
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.6f, 0.6f, 0.7f, 1.0f);
			break;
		case 1:
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.52f, 0.52f, 0.52f, 1.0f);
			break;
		case 2:
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.52f, 0.52f, 0.52f, 1.0f);
			break;
		case 3:
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.6f, 0.6f, 0.7f, 1.0f);
			break;
		}
		// ライトの方向を設定
		switch (nCntLight)
		{
		case 0:
			//vecDir = D3DXVECTOR3(0.1f, -0.11f, 0.44f);
			//vecDir = D3DXVECTOR3(0.8f, 0.23f, -0.55f);
			vecDir = D3DXVECTOR3(-0.5f, -0.87f, 0.05f);

			break;
		case 1:
			vecDir = D3DXVECTOR3(0.46f, 0.88f, -0.44f);
			break;
		case 2:
			vecDir = D3DXVECTOR3(-0.60f, -0.82f, 0.42f);
			break;
		case 3:
			vecDir = D3DXVECTOR3(0.22f, -0.87f, -0.44f);
			break;
		}

		D3DXVec3Normalize(&vecDir, &vecDir);	// ベクトルを正規化する
		m_aLight[nCntLight].Direction = vecDir;

		// ライトを設定する
		pDevice->SetLight(m_nNumLightCnt, &m_aLight[nCntLight]);

		// ライトを有効化する
		pDevice->LightEnable(m_nNumLightCnt, TRUE);
		AddNumLight();
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CLight::Uninit(void)
{
	for (int nCntLight = 0; nCntLight < NUM_LIGHT; nCntLight++)
	{
		MinusNumLight();
	}
}

//==========================================================
// 更新処理
//==========================================================
void CLight::Update(void)
{

}