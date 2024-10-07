//===================================================
//
// DirectXのレンダラー処理 [renderer.cpp]
// Author : Ibuki Okusada
//
//===================================================
#include "renderer.h"
#include "debugproc.h"
#include "manager.h"
#include "input.h"
#include "task_manager.h"
#include "object_manager.h"
#include "object.h"
#include "camera.h"

//===================================================
// コンストラクタ
//===================================================
CRenderer::CRenderer()
{
	// 値のクリア
	m_pD3D = NULL;			//Direct3Dオブジェクトのポインタ
	m_pD3DDevice = NULL;	//Direct3Dデバイスへのポインタ
	pTexture = NULL;
	m_pRenderTextureSurface = NULL;
	m_pZSurface = NULL;
	m_pOrgSurface = NULL;
	m_pOrgZBuffer = NULL;
}

//===================================================
// デストラクタ
//===================================================
CRenderer::~CRenderer()
{

}

//===================================================
// 初期化処理
//===================================================
HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;	// ディスプレイモード
	D3DPRESENT_PARAMETERS d3dpp;	// プレゼンテーションパラメータ

	// DirectX3Dオブジェクトの作成
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (m_pD3D == NULL)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.BackBufferWidth = SCREEN_WIDTH;						// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;						// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat = d3ddm.Format;						// バックバッファーの形式
	d3dpp.BackBufferCount = 1;									// バックバッファの数
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					// ダブルバッファの切り替え
	d3dpp.EnableAutoDepthStencil = TRUE;						// デプスバッファとステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;					// デプスバッファとして16bitを使う
	d3dpp.Windowed = bWindow;									// ウインドウモード
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	// リフレッシュレート
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// インターバル
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;					// デプスバッファとして16bitを使う	

	// Direct3Dデバイスの作成
	if (FAILED(m_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_pD3DDevice)))
	{
		if (FAILED(m_pD3D->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&m_pD3DDevice)))
		{
			if (FAILED(m_pD3D->CreateDevice(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&m_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	//レンダーステートの設定
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//サンプラーステートの設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	//テクスチャステージステート
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CRenderer::Uninit(void)
{
	// タスクの廃棄
	//CTaskManager::GetInstance()->Uninit();

	// オブジェクトの廃棄
	//CObjectManager::GetInstance()->Uninit();

	// Direct3Dデバイスの廃棄
	if (m_pD3DDevice != NULL)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	// Direct3Dオブジェクトの廃棄
	if (m_pD3D != NULL)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}

	if (pTexture != NULL)
	{
		pTexture->Release();
		pTexture = NULL;
	}

	if (m_pRenderTextureSurface != NULL)
	{
		m_pRenderTextureSurface->Release();
		m_pRenderTextureSurface = NULL;
	}

	if (m_pZSurface != NULL)
	{
		m_pZSurface->Release();
		m_pZSurface = NULL;
	}

	if (m_pOrgSurface != nullptr)
	{
		m_pOrgSurface->Release();
		m_pOrgSurface = nullptr;
	}

	if (m_pOrgZBuffer != nullptr)
	{
		m_pOrgZBuffer->Release();
		m_pOrgZBuffer = nullptr;
	}
}

//===================================================
// 更新処理
//===================================================
void CRenderer::Update(void)
{
#if _DEBUG	// デバッグ時
	CInputKeyboard *pKey = CManager::GetInstance()->GetInputKeyboard();

	if (pKey->GetTrigger(DIK_F2) == true)
	{
		// ワイヤーフレーム設定
		SetWire(m_bWire ? false : true);
	}
#endif
}

//===================================================
// 描画処理
//===================================================
void CRenderer::Draw(void)
{
	CDebugProc *pDebugProc = CManager::GetInstance()->GetDebugProc();

	// 画面クリア
	m_pD3DDevice->Clear(
		0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0),
		1.0f,
		0);

	// 描画開始
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{// 描画が成功した場合

		// オブジェクトの描画
		CObjectManager::GetInstance()->Draw();
		CManager::GetInstance()->GetCamera()->SetCamera();

		// デバッグ表示
		if (pDebugProc != NULL)
		{
			pDebugProc->Draw();
		}

		// 描画終了
		m_pD3DDevice->EndScene();
	}

	// バックバッファとフロントバッファの入れ替え
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

//===================================================
// デバイスの取得
//===================================================
LPDIRECT3DDEVICE9 CRenderer::GetDevice(void)
{
	return m_pD3DDevice;
}

//===================================================
// ワイヤーフレーム設定
//===================================================
void CRenderer::SetWire(bool bWire)
{
	m_bWire = bWire;
	//描画モードの変更
	switch (m_bWire)
	{
	case true:
		// ワイヤーフレーム描画に変更
		m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		break;
	case false:
		// ソリッド描画に変更
		m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		break;
	}
}