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

	for (int nCntLight = 0; nCntLight < NUM_LIGHT - 1; nCntLight++)
	{
		// ライトの情報をクリアする
		ZeroMemory(&m_aLight[nCntLight], sizeof(D3DLIGHT9));

		// ライトの種類を設定
		m_aLight[nCntLight].Type = D3DLIGHT_DIRECTIONAL;

		// ライトの拡散光を設定
		switch (nCntLight)
		{
		case 0:
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case 1:
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.65f, 0.65f, 0.65f, 1.0f);
			break;
		case 2:
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.65f, 0.65f, 0.65f, 1.0f);
			break;
		case 3:
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		}
		// ライトの方向を設定
		switch (nCntLight)
		{
		case 0:
			//vecDir = D3DXVECTOR3(0.1f, -0.11f, 0.44f);
			//vecDir = D3DXVECTOR3(0.8f, 0.23f, -0.55f);
			vecDir = D3DXVECTOR3(-0.05f, -0.87f, 0.05f);

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
		pDevice->SetLight(nCntLight, &m_aLight[nCntLight]);

		// ライトを有効化する
		pDevice->LightEnable(nCntLight, TRUE);
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CLight::Uninit(void)
{

}

//==========================================================
// 更新処理
//==========================================================
void CLight::Update(void)
{

}

//==========================================================
// 色設定
//==========================================================
void CLight::SetLight(float fDiff)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		// デバイスへのポインタを取得

	for (int nCntLight = 0; nCntLight < NUM_LIGHT - 1; nCntLight++)
	{
		// ライトの種類を設定
		m_aLight[nCntLight].Type = D3DLIGHT_DIRECTIONAL;

		if (fDiff >= 0.0f && fDiff < 0.5f)
		{
			// ライトの拡散光を設定
			switch (nCntLight)
			{
			case 0:
				m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.1f + fDiff * 2, 0.1f + fDiff * 2, 0.1f + fDiff * 2, 1.0f);
				break;
			case 1:
				m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.065f + fDiff * 2 * 0.65f, 0.065f + fDiff * 2 * 0.65f, 0.065f + fDiff * 2 * 0.65f, 1.0f);
				break;
			case 2:
				m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.065f + fDiff * 2 * 0.65f, 0.065f + fDiff * 2 * 0.65f, 0.065f + fDiff * 2 * 0.65f, 1.0f);
				break;
			}
		}
		else if (fDiff >= 0.75f && fDiff < 0.875f)
		{
			// ライトの拡散光を設定
			switch (nCntLight)
			{
			case 0:
				m_aLight[nCntLight].Diffuse = D3DXCOLOR(1.0f - (fDiff - 0.75f) * 2.0f, 1.0f - (fDiff - 0.75f) * 3.0f, 1.0f - (fDiff - 0.75f) * 3.0f, 1.0f);
				break;
			case 1:
				m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.65f - (fDiff - 0.75f) * 2.0f, 0.65f - (fDiff - 0.75f) * 3 * 0.65f, 0.65f - (fDiff - 0.75f) * 3 * 0.65f, 1.0f);
				break;
			case 2:
				m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.65f - (fDiff - 0.75f) * 2.0f, 0.65f - (fDiff - 0.75f) * 3 * 0.65f, 0.65f - (fDiff - 0.75f) * 3 * 0.65f, 1.0f);
				break;
			}
		}
		else if (fDiff >= 0.875f && fDiff < 1.0f)
		{
			// ライトの拡散光を設定
			switch (nCntLight)
			{
			case 0:
				m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.75f - (fDiff - 0.875f) * 7.0f, 1.0f - (fDiff - 0.75f) * 3.0f, 1.0f - (fDiff - 0.75f) * 3.0f, 1.0f);
				break;
			case 1:
				m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.4f - (fDiff - 0.875f) * 4.0f * 0.4f, 0.65f - (fDiff - 0.75f) * 3 * 0.65f, 0.65f - (fDiff - 0.75f) * 3 * 0.65f, 1.0f);
				break;
			case 2:
				m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.4f - (fDiff - 0.875f) * 4.0f * 0.4f, 0.65f - (fDiff - 0.75f) * 3 * 0.65f, 0.65f - (fDiff - 0.75f) * 3 * 0.65f, 1.0f);
				break;
			}
		}

		// ライトを設定する
		pDevice->SetLight(nCntLight, &m_aLight[nCntLight]);
	}
}

//==========================================================
// ポイントライトのオフ
//==========================================================
void CLight::EnablePointLight(bool bEnable)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		// デバイスへのポインタを取得

	// ライトを指定された状態にする
	pDevice->LightEnable(NUM_LIGHT - 1, bEnable);
}

//==========================================================
// ポイントライトの座標設定
//==========================================================
void CLight::SetPositonPointLight(D3DXVECTOR3 pos)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		// デバイスへのポインタを取得

	// ライトの種類を設定
	m_aLight[NUM_LIGHT - 1].Type = D3DLIGHT_POINT;

	//ライトの位置設定
	m_aLight[NUM_LIGHT - 1].Position = D3DXVECTOR3(pos.x, pos.y + 200.0f, pos.z);

	//ライトの色設定
	m_aLight[NUM_LIGHT - 1].Diffuse = D3DXCOLOR(0.4f, 0.4f, 0.4f, 0.7f);

	//ライトの範囲設定
	m_aLight[NUM_LIGHT - 1].Range = 2000.0f;

	//ライトの減衰0の設定
	m_aLight[NUM_LIGHT - 1].Attenuation0 = 0.94f;

	//ライトの減衰1の設定
	m_aLight[NUM_LIGHT - 1].Attenuation1 = 0.0f;

	//ライトの減衰2の設定
	m_aLight[NUM_LIGHT - 1].Attenuation2 = 0.0f;

	// ライトを設定する
	pDevice->SetLight(NUM_LIGHT - 1, &m_aLight[NUM_LIGHT - 1]);
}