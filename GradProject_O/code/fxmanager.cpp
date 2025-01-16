//============================================================
//
//	シェーダー管理関数 [fxmanager.cpp]
//	Author：丹野竜之介
//
//============================================================
//============================================================
//	インクルードファイル
//============================================================
#include "fxmanager.h"
#include "manager.h"
#include "renderer.h"
//============================================================
//	定数宣言
//============================================================
namespace
{
	const char* EFFECT_FX = "code\\shader\\draw.fx";	// シェーダーのエフェクトファイル
	const D3DXCOLOR LIGHT_COLOR = D3DXVECTOR4(0.5f, 0.5f, 0.6f, 1.0f);
	const D3DXVECTOR4 LIGHT_VEC = D3DXVECTOR4(-0.5f, -0.87f, 0.05f, 0.0f);
}
//************************************************************
//	静的メンバ変数宣言
//************************************************************
CFXManager* CFXManager::m_pShader = nullptr;	// シェーダー情報

//============================================================
//	コンストラクタ
//============================================================
CFXManager::CFXManager() :

	m_hDirectLight(nullptr),	
	m_hLightDiffuse(nullptr),	
	m_hUseTex(nullptr),
	m_hWorldMat(nullptr),
	m_hViewMat(nullptr),
	m_hProjMat(nullptr),
	m_pEffect(nullptr),
	m_lightArray(),
	m_numLights(0)
{

}

//============================================================
//	デストラクタ
//============================================================
CFXManager::~CFXManager()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CFXManager::Init(void)
{
	// 変数を宣言
	D3DCAPS9 caps;	// ハードウェア機能
	HRESULT hr;		// 異常終了の確認用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイス情報
	LPD3DXBUFFER pError = nullptr;	// コンパイルエラー情報
	LPD3DXEFFECT pEffect = nullptr;	// エフェクト設定用

	// メンバ変数を初期化
	m_hDirectLight=nullptr;
	m_hLightDiffuse=nullptr;
	m_hUseTex=nullptr;
	m_hWorldMat=nullptr;
	m_hViewMat=nullptr;
	m_hProjMat=nullptr;
	m_pEffect = nullptr;
	// ハードウェア機能を取得
	pDevice->GetDeviceCaps(&caps);

	if (caps.VertexShaderVersion >= D3DVS_VERSION(1, 1)
		&& caps.PixelShaderVersion >= D3DPS_VERSION(2, 0))
	{ // 頂点・ピクセルシェーダのバージョンが使用可能な場合

		// エフェクトファイルの読込
		hr = D3DXCreateEffectFromFile
		( // 引数
			pDevice,	// デバイスへのポインタ
			EFFECT_FX,	// エフェクトファイル
			nullptr,	// プリプロセッサ定義
			nullptr,	// インクルード操作
			0,			// 読込オプションフラグ
			nullptr,	// ハンドルインターフェース
			&pEffect,	// エフェクトインターフェース
			&pError		// コンパイルエラー情報
		);
		if (SUCCEEDED(hr))
		{ // 読込に成功した場合

			// エフェクト情報を設定
			m_pEffect = pEffect;

			// テクニック関数を取得・設定
			SetTechnique(pEffect->GetTechniqueByName("TShader"));

			// ハンドルを取得
			m_hWorldMat = pEffect->GetParameterByName(nullptr, "g_mtxWorld");
			m_hViewMat	= pEffect->GetParameterByName(nullptr, "g_mtxView");
			m_hProjMat	= pEffect->GetParameterByName(nullptr, "g_mtxProj");

			m_hViewvec = pEffect->GetParameterByName(nullptr, "viewDir");
			m_hviewPos = pEffect->GetParameterByName(nullptr, "viewPos");
			m_hDirectLight	 = pEffect->GetParameterByName(nullptr, "m_LightDir");
			m_hLightDiffuse	 = pEffect->GetParameterByName(nullptr, "m_LightCol");
			m_hMatDiffuse	 = pEffect->GetParameterByName(nullptr, "m_diffus");
			m_hMatAmbient = pEffect->GetParameterByName(nullptr, "m_ambient");
			m_hMatEmissive = pEffect->GetParameterByName(nullptr, "m_Emissive");
			m_hMatSpecula = pEffect->GetParameterByName(nullptr, "m_specula");
			m_hMatPower = pEffect->GetParameterByName(nullptr, "m_power");
			m_hUseTex = pEffect->GetParameterByName(nullptr, "bUseTex");
			m_hMatScaleReverse = pEffect->GetParameterByName(nullptr, "g_mMatScaleReverse");
			m_hnumLights = pEffect->GetParameterByName(nullptr, "numLights");
			m_hLights = pEffect->GetParameterByName(nullptr, "lights");
			m_hCubeMap = pEffect->GetParameterByName(nullptr, "CubeMapSampler");
			D3DXCreateCubeTextureFromFile(pDevice, "data\\TEXTURE\\cube.dds", &m_pCubeTexture);
		}
		else
		{ // 読込に失敗した場合
			if (pError)
			{
				OutputDebugStringA((LPCSTR)pError->GetBufferPointer());
				//デバッグコンソールに表示する
				MessageBoxA(NULL, (LPCSTR)pError->GetBufferPointer(), "Shader Error", MB_OK);
			}
			// 読み込み失敗を返す
			assert(false);
			return E_FAIL;
		}
	}
	else
	{ // バージョンが使用不可な場合

		// 古いバージョンによる失敗を返す
		assert(false);
		return E_FAIL;
	}
	
	SetLightVec(LIGHT_VEC);
	SetDiffuse(LIGHT_COLOR*0.75f);
	// 成功を返す
	return S_OK;
}
//============================================================
//	終了処理
//============================================================
void CFXManager::Uninit(void)
{
	SAFE_RELEASE(m_pEffect);
}
//============================================================
//	生成処理
//============================================================
CFXManager* CFXManager::Create(void)
{
	// インスタンス使用中
	assert(m_pShader == nullptr);

	// トゥーンシェーダーの生成
	m_pShader = DEBUG_NEW CFXManager;
	if (m_pShader == nullptr)
	{ // 生成に失敗した場合

		assert(false);
		return nullptr;
	}
	else
	{ // 生成に成功した場合

		// トゥーンシェーダーの初期化
		if (FAILED(m_pShader->Init()))
		{ // 初期化に失敗した場合

			// トゥーンシェーダーの破棄
			SAFE_DELETE(m_pShader);
			return nullptr;
		}

		// 確保したアドレスを返す
		return m_pShader;
	}
}
//============================================================
//	取得処理
//============================================================
CFXManager* CFXManager::GetInstance(void)
{
	if (m_pShader == nullptr)
	{
		m_pShader = CFXManager::Create();
	}

	// ポインタを返す
	return m_pShader;
}

//============================================================
//	破棄処理
//============================================================
void CFXManager::Release(void)
{
	// トゥーンシェーダーの終了
	assert(m_pShader != nullptr);
	m_pShader->Uninit();

	// メモリ開放
	SAFE_DELETE(m_pShader);
}

//============================================================
//	テクニック関数の設定処理
//============================================================
void CFXManager::SetTechnique(const D3DXHANDLE pTechnique)
{
	// テクニック関数を設定
	m_pTechnique = pTechnique;

	if (m_pEffect != nullptr)
	{ // エフェクトが使用されている場合

		// エフェクトにテクニック関数を設定
		m_pEffect->SetTechnique(m_pTechnique);
	}
	else { assert(false); }	// エフェクト使用不可
}
//============================================================
//	ライト設定処理
//============================================================
void CFXManager::SetLight()
{
	Clist<CShaderLight::SLight*>* List = CShaderLight::GetList();
	if (List != nullptr)
	{
		m_numLights = List->GetNum();
		for (int i = 0; i < m_numLights; i++)
		{
			m_lightArray[i] = *List->Get(i);
		}
	}
	else
	{
		m_numLights = 0;
	}
}
//============================================================
// マテリアル設定
//============================================================
void CFXManager::SetMaterial(const D3DMATERIAL9& rMaterial)
{
	D3DXVECTOR4 Diff;
	Diff.x = rMaterial.Diffuse.r;
	Diff.y = rMaterial.Diffuse.g;
	Diff.z = rMaterial.Diffuse.b;
	Diff.w = rMaterial.Diffuse.a;
	m_MatDiffuse = Diff;
	D3DXVECTOR4 ambient;
	ambient.x = rMaterial.Ambient.r;
	ambient.y = rMaterial.Ambient.g;
	ambient.z = rMaterial.Ambient.b;
	ambient.w = rMaterial.Ambient.a;
	m_MatAmbient = ambient;
	D3DXVECTOR4 Emissive;
	Emissive.x = rMaterial.Emissive.r;
	Emissive.y = rMaterial.Emissive.g;
	Emissive.z = rMaterial.Emissive.b;
	Emissive.w = rMaterial.Emissive.a;
	m_MatEmissive = Emissive;
	D3DXVECTOR4 specula;
	specula.x = rMaterial.Specular.r;
	specula.y = rMaterial.Specular.g;
	specula.z = rMaterial.Specular.b;
	specula.w = rMaterial.Specular.a;
	m_MatSpecula = specula;
	m_MatPower = rMaterial.Power;
}
//============================================================
// 拡散光設定
//============================================================
void CFXManager::SetDiffuse(const D3DXCOLOR& rDiffuse)
{
	D3DXVECTOR4 Diff;
	Diff.x = rDiffuse.r;
	Diff.y = rDiffuse.g;
	Diff.z = rDiffuse.b;
	Diff.w = rDiffuse.a;
	m_LightDiffuse = Diff;

}
//============================================================
// 光源方向設定
//============================================================
void CFXManager::SetLightVec(const D3DXVECTOR4& rVec)
{
	D3DXVECTOR4 vec = rVec;
	D3DXVec4Normalize(&vec, &vec);
	m_DirectLight = vec;

}
//============================================================
// 視点方向設定
//============================================================
void  CFXManager::SetCameraVec(const D3DXVECTOR3& rVec)
{
	D3DXVECTOR4 vec = rVec;
	D3DXVec4Normalize(&vec, &vec);
	m_Viewvec = vec;
}
//============================================================
//	マトリックスの設定
//============================================================
void  CFXManager::SetMatrixWorld(const D3DXMATRIX& rmtxWorld)
{
	m_matWorld = rmtxWorld;

}
void  CFXManager::SetView(const D3DXMATRIX& rmtxView)
{
	m_matView = rmtxView;

}
void CFXManager::SetViewpos(const D3DXVECTOR3& rposView)
{
	D3DXVECTOR4 pos = rposView;
	m_viewPos = pos;
}
void  CFXManager::SetProj(const D3DXMATRIX& rmtxProj)
{
	m_matProj = rmtxProj;

}
void CFXManager::SetScale(const D3DXMATRIX& rmtxScale)
{
	m_matScaleReverse = rmtxScale;
	D3DXMatrixInverse(&m_matScaleReverse, NULL, &m_matWorld);
	D3DXMatrixTranspose(&m_matScaleReverse, &m_matScaleReverse);
}
void CFXManager::SetParamToEffect()
{
	
	m_pEffect->SetMatrix(m_hWorldMat, &m_matWorld);
	m_pEffect->SetMatrix(m_hViewMat, &m_matView);
	m_pEffect->SetMatrix(m_hProjMat, &m_matProj);
	m_pEffect->SetMatrix(m_hMatScaleReverse, &m_matScaleReverse);
	m_pEffect->SetVector(m_hLightDiffuse, &m_LightDiffuse);
	m_pEffect->SetVector(m_hDirectLight, &m_DirectLight);
	m_pEffect->SetVector(m_hMatDiffuse, &m_MatDiffuse);
	m_pEffect->SetVector(m_hMatAmbient, &m_MatAmbient);
	m_pEffect->SetVector(m_hMatEmissive, &m_MatEmissive);
	m_pEffect->SetVector(m_hMatSpecula, &m_MatSpecula);
	m_pEffect->SetFloat(m_hMatPower, m_MatPower);
	m_pEffect->SetVector(m_hViewvec, &m_Viewvec);
	m_pEffect->SetVector(m_hviewPos, &m_viewPos);
	m_pEffect->SetInt(m_hnumLights, m_numLights);
	m_pEffect->SetTexture(m_hCubeMap, m_pCubeTexture);
	m_pEffect->SetValue(m_hLights, m_lightArray, sizeof(m_lightArray));
}	
// 描画の開始を宣言する
HRESULT CFXManager::Begin()
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイス情報
	
	// プログラマブルシェーダのテクニックを設定
	m_pEffect->SetTechnique(m_pTechnique);

	// シェーダの開始を宣言
	UINT Tmp;
	m_pEffect->Begin(&Tmp, 0);
	m_bBegin = true;
	return S_OK;
}


// パスの開始を宣言する
HRESULT CFXManager::BeginPass()
{
	m_pEffect->BeginPass(0);	// Z値計算は1パス
	return S_OK;
}
// パスの終了を宣言する
HRESULT CFXManager::EndPass()
{
	m_pEffect->EndPass();
	return S_OK;
}
// 描画の終了を宣言する
HRESULT CFXManager::End()
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイス情報
	m_pEffect->End();

	// 固定機能に戻す
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
	m_bBegin = false;
	return S_OK;
}