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
	const char* EFFECT_FX = "code\shader\draw.fx";	// シェーダーのエフェクトファイル
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
	m_pEffect(nullptr)
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
			m_hWorldMat = pEffect->GetParameterByName(nullptr, "s_mtxWorld");
			m_hViewMat	= pEffect->GetParameterByName(nullptr, "g_mtxView");
			m_hProjMat	= pEffect->GetParameterByName(nullptr, "g_mtxProj");

			m_hDirectLight	 = pEffect->GetParameterByName(nullptr, "m_LightDir");
			m_hLightDiffuse	 = pEffect->GetParameterByName(nullptr, "m_LightCol");
			m_hMatDiffuse	 = pEffect->GetParameterByName(nullptr, "m_diffus");
			m_hUseTex		 = pEffect->GetParameterByName(nullptr, "bUseTex");
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
