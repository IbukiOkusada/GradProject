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
#include "input_keyboard.h"
#include "effekseerControl.h"
#include "edit_manager.h"
#include "postprocess.h"
namespace
{
	const D3DXCOLOR ALPHACOLOR = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);	// マルチターゲット透明度
	const D3DXCOLOR NONE_ALPHACOLOR = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	const D3DXVECTOR2 NORMALSIZE = D3DXVECTOR2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);
	const D3DXVECTOR2 MINISIZE = NORMALSIZE * 1.0f;
}

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

	// マルチターゲットレンダリング用
	m_MultitargetInfo = MultiTargetInfo();
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

	// グローバル環境光の設定
	m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(0, 0, 0));

	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);  // Zバッファ有効
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);  // ライト有効化
	m_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);  // スペキュラ有効

	// マルチターゲットレンダラーの初期化
	InitMTRender();

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

	for (int cnt = 0; cnt < 2; cnt++)
	{
		if (m_Multitarget.pTextureMT[cnt] != nullptr)
		{
			m_Multitarget.pTextureMT[cnt]->Release();
			m_Multitarget.pTextureMT[cnt] = nullptr;
		}
		if (m_Multitarget.pRenderMT[cnt] != nullptr)
		{
			m_Multitarget.pRenderMT[cnt]->Release();
			m_Multitarget.pRenderMT[cnt] = nullptr;
		}
	}

	if (m_Multitarget.pZBuffMT != nullptr)
	{
		m_Multitarget.pZBuffMT->Release();
		m_Multitarget.pZBuffMT = nullptr;
	}

	if (m_Multitarget.pVtxBuff != nullptr)
	{
		m_Multitarget.pVtxBuff->Release();
		m_Multitarget.pVtxBuff = nullptr;
	}

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

	CPostprocess::Release();
}

//===================================================
// 更新処理
//===================================================
void CRenderer::Update(void)
{
#if _DEBUG	// デバッグ時
	CInputKeyboard *pKey = CInputKeyboard::GetInstance();

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

	// カメラの設定
	CCamera* pCamera = CManager::GetInstance()->GetCamera();

	// 切替用
	LPDIRECT3DTEXTURE9 pTextureWk;	// 切替用用テクスチャ
	LPDIRECT3DSURFACE9 pRenderWk;	// テクスチャレンダリング用インターフェース

	// 描画開始
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{// 描画が成功した場合

		D3DVIEWPORT9 viewportDef;	//ビューポート保存

		// 現在のビューポートを取得
		m_pD3DDevice->GetViewport(&viewportDef);

		// 保存用バッファ
		LPDIRECT3DSURFACE9 pRenderDef = nullptr, pZBuffDef = nullptr;
		D3DXMATRIX mtxView, mtxProjection;

		// デフォルトのレンダラーターゲットを取得
		GetDefaultRenderTarget(&pRenderDef, &pZBuffDef, &mtxView, &mtxProjection);

		// ターゲット切替
		CManager::GetInstance()->GetRenderer()->ChangeTarget(pCamera->GetPositionV(), pCamera->GetPositionR(), D3DXVECTOR3(0.0f, 1.0f, 0.0f));

		// テクスチャ[0]のクリア
		m_pD3DDevice->Clear(
			0, nullptr,
			(D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
			D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f),
			1.0f,
			0);

		// オブジェクトの描画
		CObjectManager::GetInstance()->Draw();

		//if (CEditManager::GetInstance() == nullptr)
		{
			pCamera->SetCamera();
			CEffekseer::GetInstance()->Draw();
		}

		// フィードバックエフェクトにテクスチャ[1]を貼り付けて描画
		float multidef = m_MultitargetInfo.fMulti - m_MultitargetInfo.fStartMulti;
		float multi = m_MultitargetInfo.fStartMulti + multidef * m_MultitargetInfo.fTimer;
		float alphadef = m_MultitargetInfo.fColAlpha - m_MultitargetInfo.fStartColAlpha;
		float alpha = m_MultitargetInfo.fStartColAlpha + alphadef * m_MultitargetInfo.fTimer;
		DrawMultiTargetScreen(1, D3DXCOLOR(ALPHACOLOR.r, ALPHACOLOR.g, ALPHACOLOR.b, alpha), MINISIZE * multi);

		// カメラの設定
		CManager::GetInstance()->GetCamera()->SetCamera();
		// レンダーターゲットをもとに戻す
		CManager::GetInstance()->GetRenderer()->ChangeRendertarget(pRenderDef, pZBuffDef, mtxView, mtxProjection);

		DrawMultiTargetScreen(0, NONE_ALPHACOLOR, NORMALSIZE);

		// テクスチャ0と1の切替
		pTextureWk = m_Multitarget.pTextureMT[0];
		m_Multitarget.pTextureMT[0] = m_Multitarget.pTextureMT[1];
		m_Multitarget.pTextureMT[1] = pTextureWk;

		pRenderWk = m_Multitarget.pRenderMT[0];
		m_Multitarget.pRenderMT[0] = m_Multitarget.pRenderMT[1];
		m_Multitarget.pRenderMT[1] = pRenderWk;

		CManager::GetInstance()->GetCamera()->SetCamera();

		CPostprocess::GetInstance()->Draw();
		CObjectManager::GetInstance()->DrawAllnotShader();
		// デバッグ表示
		if (pDebugProc != NULL)
		{
			pDebugProc->Draw();
		}

		// ビューポートを元に戻す
		m_pD3DDevice->SetViewport(&viewportDef);

		// 描画終了
		m_pD3DDevice->EndScene();
	}

	// バックバッファとフロントバッファの入れ替え
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);

	// マルチターゲット調整
	if (m_MultitargetInfo.bActive) {
		SetMultiTarget();
	}
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

//==========================================================================
// マルチターゲットレンダラーの初期化
//==========================================================================
void CRenderer::InitMTRender()
{
	// 保存用バッファ
	LPDIRECT3DSURFACE9 pRenderDef, pZBuffDef;

	// 現在の画面幅取得
	float screen_width = SCREEN_WIDTH;
	float screen_height = SCREEN_HEIGHT;

	// レンダリングターゲット用テクスチャの生成
	for (int i = 0; i < 2; i++)
	{
		m_pD3DDevice->CreateTexture(screen_width, screen_height,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_X8R8G8B8,
			D3DPOOL_DEFAULT,
			&m_Multitarget.pTextureMT[i],
			NULL);

		// テクスチャレンダリング用インターフェースの生成
		m_Multitarget.pTextureMT[i]->GetSurfaceLevel(0, &m_Multitarget.pRenderMT[i]);
	}

	// テクスチャレンダリング用Zバッファの生成
	m_pD3DDevice->CreateDepthStencilSurface(
		screen_width, screen_height,
		D3DFMT_D24S8,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&m_Multitarget.pZBuffMT,
		NULL);

	// 現在のレンダリングターゲットを取得(保存)
	m_pD3DDevice->GetRenderTarget(0, &pRenderDef);

	// 現在のZバッファを取得(保存)
	m_pD3DDevice->GetDepthStencilSurface(&pZBuffDef);

	// レンダリングターゲットを生成したテクスチャに設定
	m_pD3DDevice->SetRenderTarget(0, m_Multitarget.pRenderMT[0]);

	// Zバッファを生成したZバッファに設定
	m_pD3DDevice->SetDepthStencilSurface(m_Multitarget.pZBuffMT);

	// レンダリングターゲット用のテクスチャのクリア
	m_pD3DDevice->Clear(
		0, nullptr,
		(D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f),
		1.0f,
		0);


	// レンダリングターゲットを元に戻す
	m_pD3DDevice->SetRenderTarget(0, pRenderDef);

	// Zバッファを元に戻す
	m_pD3DDevice->SetDepthStencilSurface(pZBuffDef);

	// 仮置きレンダリングターゲット・Zバッファの開放（これをしないとメモリリークが起きる）
	if (pRenderDef != nullptr)
	{
		pRenderDef->Release();
		pRenderDef = nullptr;
	}
	if (pZBuffDef != nullptr)
	{
		pZBuffDef->Release();
		pZBuffDef = nullptr;
	}

	// テクスチャ用ビューポートの設定
	m_Multitarget.viewportMT.X = 0;						// 描画する画面の左上X座標
	m_Multitarget.viewportMT.Y = 0;						// 描画する画面の左上Y座標
	m_Multitarget.viewportMT.Width = screen_width;		// 描画する画面の幅
	m_Multitarget.viewportMT.Height = screen_height;	// 描画する画面の高さ
	m_Multitarget.viewportMT.MinZ = 0.0f;
	m_Multitarget.viewportMT.MaxZ = 1.0f;


	m_pD3DDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_Multitarget.pVtxBuff,
		nullptr);



	// 頂点情報へのポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_Multitarget.pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// スクリーン表示状態に応じて通常のサイズ変更
	D3DXVECTOR2 size = D3DXVECTOR2(SCREEN_WIDTH* 0.5f, SCREEN_HEIGHT * 0.5f);


	// 頂点座標の設定
	pVtx[0].pos.x = (screen_width * 0.5f) - size.x;
	pVtx[0].pos.y = (screen_height * 0.5f) - size.y;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = (screen_width * 0.5f) + size.x;
	pVtx[1].pos.y = (screen_height * 0.5f) - size.y;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = (screen_width * 0.5f) - size.x;
	pVtx[2].pos.y = (screen_height * 0.5f) + size.y;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = (screen_width * 0.5f) + size.x;
	pVtx[3].pos.y = (screen_height * 0.5f) + size.y;
	pVtx[3].pos.z = 0.0f;

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = ALPHACOLOR;
	pVtx[1].col = ALPHACOLOR;
	pVtx[2].col = ALPHACOLOR;
	pVtx[3].col = ALPHACOLOR;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックロック
	m_Multitarget.pVtxBuff->Unlock();

}

//==========================================================================
// デフォルトのレンダーターゲット取得
//==========================================================================
void CRenderer::GetDefaultRenderTarget(LPDIRECT3DSURFACE9* pRender, LPDIRECT3DSURFACE9* pZBuff, D3DXMATRIX* viewport, D3DXMATRIX* projection)
{
	// 現在のレンダリングターゲットを取得(保存)
	m_pD3DDevice->GetRenderTarget(0, &(*pRender));

	// 現在のZバッファを取得(保存)
	m_pD3DDevice->GetDepthStencilSurface(&(*pZBuff));

	m_pD3DDevice->GetTransform(D3DTS_VIEW, &(*viewport));
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &(*projection));
}

//==========================================================================
// レンダリングターゲットの切替
//==========================================================================
void CRenderer::ChangeRendertarget(LPDIRECT3DSURFACE9 pRender, LPDIRECT3DSURFACE9 pZBuff, D3DXMATRIX viewport, D3DXMATRIX projection)
{
	// レンダリングターゲットを生成したテクスチャに設定
	m_pD3DDevice->SetRenderTarget(0, pRender);

	// Zバッファを生成したZバッファに設定
	m_pD3DDevice->SetDepthStencilSurface(pZBuff);

	// テクスチャレンダリング用のビューポートを設定
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &viewport);

	// テクスチャレンダリング用のプロジェクションマトリックスを設定
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &projection);
}

//==========================================================================
// レンダリングターゲットの切替
//==========================================================================
void CRenderer::ChangeTarget(const D3DXVECTOR3& posV, const D3DXVECTOR3& posR, const D3DXVECTOR3& vecU)
{
	D3DXMATRIX mtxview, mtxProjection;

	// レンダリングターゲットを生成したテクスチャに設定
	m_pD3DDevice->SetRenderTarget(0, m_Multitarget.pRenderMT[0]);

	// Zバッファを生成したZバッファに設定
	m_pD3DDevice->SetDepthStencilSurface(m_Multitarget.pZBuffMT);

	// テクスチャレンダリング用のビューポートを設定
	m_pD3DDevice->SetViewport(&m_Multitarget.viewportMT);


	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&mtxProjection);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(
		&mtxProjection,
		D3DXToRadian(45.0f),	// 視野角
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,	// アスペクト比
		10.0f,		// 手前の制限
		100000.0f);	// 奥行きの制限

	// プロジェクションマトリックスの設定
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &mtxProjection);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&mtxview);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(
		&mtxview,
		&posV,	// 視点
		&posR,	// 注視点
		&vecU);	// 上方向ベクトル

	// ビューマトリックスの設定
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &mtxview);
}

//==========================================================================
// マルチターゲット画面の描画判定
//==========================================================================
void CRenderer::SetEnableDrawMultiScreen(float fGoalAlpha, float fGoalMulti, float fTimer)
{
	// パラメーターの設定
	m_MultitargetInfo.fTimer = 0.0f;
	if (fTimer != 0.0f) { m_MultitargetInfo.fAddTimer = 1.0f / fTimer; }	// 1フレームごとの加算量設定
	else { m_MultitargetInfo.fAddTimer = 1.0f; }	// 0なら即
	m_MultitargetInfo.fStartColAlpha = m_MultitargetInfo.fColAlpha;
	m_MultitargetInfo.fColAlpha = fGoalAlpha;
	m_MultitargetInfo.fStartMulti = m_MultitargetInfo.fMulti;
	m_MultitargetInfo.fMulti = fGoalMulti;
	m_MultitargetInfo.bActive = true;

	return;
}

//==========================================================================
// マルチターゲット画面の描画調整
//==========================================================================
void CRenderer::SetMultiTarget()
{
	m_MultitargetInfo.fTimer += m_MultitargetInfo.fAddTimer;

	if (m_MultitargetInfo.fTimer >= 1.0f) {
		m_MultitargetInfo.bActive = false;
	}
}

//==========================================================================
// マルチターゲット画面の描画
//==========================================================================
void CRenderer::DrawMultiTargetScreen(int texIdx, const D3DXCOLOR& col, const D3DXVECTOR2& size)
{
	// 頂点情報へのポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_Multitarget.pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos.x = SCREEN_WIDTH*0.5f - size.x;
	pVtx[0].pos.y = SCREEN_HEIGHT * 0.5f - size.y;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = SCREEN_WIDTH * 0.5f + size.x;
	pVtx[1].pos.y = SCREEN_HEIGHT * 0.5f - size.y;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = SCREEN_WIDTH * 0.5f - size.x;
	pVtx[2].pos.y = SCREEN_HEIGHT * 0.5f + size.y;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = SCREEN_WIDTH * 0.5f + size.x;
	pVtx[3].pos.y = SCREEN_HEIGHT * 0.5f + size.y;
	pVtx[3].pos.z = 0.0f;

	// 頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// 頂点バッファをアンロックロック
	m_Multitarget.pVtxBuff->Unlock();

	// 頂点バッファをデータストリームに設定
	m_pD3DDevice->SetStreamSource(0, m_Multitarget.pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	m_pD3DDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	m_pD3DDevice->SetTexture(0, m_Multitarget.pTextureMT[texIdx]);

	// ポリゴンの描画
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}