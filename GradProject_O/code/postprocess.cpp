//============================================================
//
//	シェーダー管理関数 [Postprocess.cpp]
//	Author：丹野竜之介
//
//============================================================
//============================================================
//	インクルードファイル
//============================================================
#include "Postprocess.h"
#include "manager.h"
#include "renderer.h"
//============================================================
//	定数宣言
//============================================================
namespace
{
	const char* EFFECT_BRIGHT = "code\\shader\\BrightCheck.fx";	// シェーダーのエフェクトファイル
	const char* EFFECT_GAUSS = "code\\shader\\Gauss.fx";	// シェーダーのエフェクトファイル
	const char* EFFECT_ABERRATION = "code\\shader\\aberration.fx";	// シェーダーのエフェクトファイル
	const D3DXCOLOR LIGHT_COLOR = D3DXVECTOR4(0.6f, 0.6f, 0.7f, 1.0f);
	const D3DXVECTOR4 LIGHT_VEC = D3DXVECTOR4(-0.5f, -0.87f, 0.05f, 0.0f);
}
//************************************************************
//	静的メンバ変数宣言
//************************************************************
CPostprocess* CPostprocess::m_pShader = nullptr;	// シェーダー情報

//============================================================
//	コンストラクタ
//============================================================
CPostprocess::CPostprocess()
{

}

//============================================================
//	デストラクタ
//============================================================
CPostprocess::~CPostprocess()
{

}

//============================================================
//	初期化処理
//============================================================
HRESULT CPostprocess::Init(void)
{
	
	// 変数を宣言
	D3DCAPS9 caps;	// ハードウェア機能
	HRESULT hr;		// 異常終了の確認用

	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイス情報
	LPD3DXBUFFER pError = nullptr;	// コンパイルエラー情報
	

	// メンバ変数を初期化

	// ハードウェア機能を取得
	pDevice->GetDeviceCaps(&caps);

	if (caps.VertexShaderVersion >= D3DVS_VERSION(1, 1)
		&& caps.PixelShaderVersion >= D3DPS_VERSION(2, 0))
	{ // 頂点・ピクセルシェーダのバージョンが使用可能な場合

		Loadshader((char*)EFFECT_BRIGHT, &m_pBright);
		Loadshader((char*)EFFECT_GAUSS, &m_pGauss);
		Loadshader((char*)EFFECT_ABERRATION, &m_pAberration);
	}
	else
	{ // バージョンが使用不可な場合

		// 古いバージョンによる失敗を返す
		assert(false);
		return E_FAIL;
	}

	//テクスチャ生成
	pDevice->CreateTexture(
		(SCREEN_WIDTH),          // テクスチャの幅
		(SCREEN_HEIGHT),          // テクスチャの高さ
		0,              // ミップマップ レベル
		D3DUSAGE_RENDERTARGET,
		D3DFMT_X8R8G8B8, // フォーマット
		D3DPOOL_DEFAULT, // プール
		&m_pGaussTex,  // テクスチャ オブジェクト
		NULL);
	//サーフェイスの設定
	m_pGaussTex->GetSurfaceLevel(0, &m_pGaussSurface);
	D3DXCreateSprite(pDevice, &m_pSprite);// スプライト作成



	pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&pVtxBuff,
		nullptr);



	// 頂点情報へのポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// スクリーン表示状態に応じて通常のサイズ変更
	D3DXVECTOR2 size = D3DXVECTOR2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);


	// 頂点座標の設定
	pVtx[0].pos.x = (SCREEN_WIDTH * 0.5f) - size.x;
	pVtx[0].pos.y = (SCREEN_HEIGHT * 0.5f) - size.y;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = (SCREEN_WIDTH * 0.5f) + size.x;
	pVtx[1].pos.y = (SCREEN_HEIGHT * 0.5f) - size.y;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = (SCREEN_WIDTH * 0.5f) - size.x;
	pVtx[2].pos.y = (SCREEN_HEIGHT * 0.5f) + size.y;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = (SCREEN_WIDTH * 0.5f) + size.x;
	pVtx[3].pos.y = (SCREEN_HEIGHT * 0.5f) + size.y;
	pVtx[3].pos.z = 0.0f;

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックロック
	pVtxBuff->Unlock();
	// 成功を返す
	return S_OK;
}
//============================================================
//	終了処理
//============================================================
void CPostprocess::Uninit(void)
{
	SAFE_RELEASE(m_pBright);
	SAFE_RELEASE(m_pGauss);
}
//============================================================
//	描画処理
//============================================================
void CPostprocess::Draw(void)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイス情報
	//バックバッファの既定値を記録
	LPDIRECT3DSURFACE9 pBackBuffer;
	pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	//特定輝度の抜きだし

	m_pAberration->Begin(NULL, 0);
	m_pAberration->BeginPass(0);
	m_pAberration->SetTechnique("ChromaticAberration");
	Rendering(CManager::GetInstance()->GetRenderer()->GetTextureMT(0), 0xffffffff);
	m_pAberration->EndPass();
	m_pAberration->End();
	pDevice->SetRenderTarget(0, m_pGaussSurface);//レンダーターゲット切り替え

	m_pBright->Begin(NULL, 0);
	m_pBright->BeginPass(0);
	m_pBright->SetTechnique("brightcheck");
	Rendering(CManager::GetInstance()->GetRenderer()->GetTextureMT(0), 0xffffffff);
	m_pBright->EndPass();
	m_pBright->End();

	// ガウスぼかしを適用
	m_pGauss->Begin(NULL, 0);
	m_pGauss->BeginPass(0);
	m_pGauss->SetTechnique("GaussianBlur");
	m_pGauss->SetTexture("InputTexture", m_pGaussTex);
	Rendering(m_pGaussTex, 0xffffffff);
	m_pGauss->EndPass();
	m_pGauss->End();
	// 固定機能に戻す
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
	pDevice->SetRenderTarget(0, pBackBuffer);
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	Rendering(m_pGaussTex, 0xffffffff);
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
void CPostprocess::Rendering(LPDIRECT3DTEXTURE9 pTex, D3DXCOLOR col)
{
	// ポインタを宣言
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// デバイス情報
	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTex);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
//============================================================
//	生成処理
//============================================================
CPostprocess* CPostprocess::Create(void)
{
	// インスタンス使用中
	assert(m_pShader == nullptr);

	// トゥーンシェーダーの生成
	m_pShader = DEBUG_NEW CPostprocess;
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
CPostprocess* CPostprocess::GetInstance(void)
{
	if (m_pShader == nullptr)
	{
		m_pShader = CPostprocess::Create();
	}

	// ポインタを返す
	return m_pShader;
}

//============================================================
//	破棄処理
//============================================================
void CPostprocess::Release(void)
{
	// トゥーンシェーダーの終了
	if (m_pShader != nullptr)
	{
		m_pShader->Uninit();
	}

	// メモリ開放
	SAFE_DELETE(m_pShader);
}


//シェーダーファイル読み込み
HRESULT  CPostprocess::Loadshader(char* path, LPD3DXEFFECT* pEffect)
{

	ID3DXBuffer* pError = NULL;
	CRenderer* pRenderer = CManager::GetInstance()->GetRenderer();
	LPDIRECT3DDEVICE9 pDevice; //デバイスのポインタ
	pDevice = pRenderer->GetDevice();
	HRESULT hr = D3DXCreateEffectFromFile(
		pDevice,				// デバイスへのポインタ
		path,			// fxファイルのパス
		NULL,                  // プリプロセッサ定義
		NULL,                  // プリプロセッサ定義ファイル
		D3DXSHADER_DEBUG,                     // フラグ
		NULL,                  // カスタムエフェクトプール
		pEffect,              // 読み込まれたエフェクト
		&pError                // エラーメッセージ
	);
	if (pError)
	{
		OutputDebugStringA((LPCSTR)pError->GetBufferPointer());
		//デバッグコンソールに表示する
		MessageBoxA(NULL, (LPCSTR)pError->GetBufferPointer(), "Shader Error", MB_OK);
	}
	return hr;
}