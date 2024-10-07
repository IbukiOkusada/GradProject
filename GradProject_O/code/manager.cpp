//===============================================
//
// マネージャーの処理 [manager.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "sound.h"
#include "camera.h"
#include "light.h"
#include "Xfile.h"
#include "texture.h"
#include "game.h"
#include "title.h"
#include "result.h"
#include "tutorial.h"
#include "fade.h"
#include "object.h"
#include "ranking.h"
#include "slow.h"
#include "task_manager.h"
#include "object_manager.h"
#include "camera_manager.h"

//===============================================
// 静的メンバ変数
//===============================================
CManager *CManager::m_pManager = NULL;

//===================================================
// コンストラクタ
//===================================================
CManager::CManager()
{
	m_pRenderer = NULL;		// レンダラーのポインタ
	m_pInputKeyboard = NULL;	// 入力デバイス(キーボード)へのポインタ
	m_pInputMouse = NULL;		// 入力デバイス(マウス)のポインタ
	m_pInputPad = NULL;
	m_pDebugProc = NULL;		// デバッグ表示のポインタ
	m_pSound = NULL;			// サウンドのポインタ
	m_pCamera = NULL;			// カメラのポインタ
	m_pLight = NULL;			// ライトのポインタ
	m_pTexture = NULL;		// テクスチャのポインタ
	m_pModelFile = NULL;		// Xファイル情報のポインタ
	m_pSlow = NULL;			// スロー状態へのポインタ
	m_pScene = NULL;			// シーンのポインタ
	m_pFade = NULL;			// フェードへのポインタ
}

//===================================================
// デストラクタ
//===================================================
CManager::~CManager()
{

}

//===================================================
// 初期化処理
//===================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{

	// レンダラーの生成
	if (m_pRenderer == NULL)
	{// 使用していない場合
		m_pRenderer = new CRenderer;
	}

	//初期化処理
	if (m_pRenderer != NULL)
	{// 使用している場合
		if (FAILED(m_pRenderer->Init(hWnd, TRUE)))
		{//初期化が失敗した場合
			return E_FAIL;
		}
	}

	// 入力デバイス(キーボード)の生成
	if (m_pInputKeyboard == NULL)
	{// 使用していない場合
		m_pInputKeyboard = new CInputKeyboard;

		//初期化処理
		if (m_pInputKeyboard != NULL)
		{// 使用している場合
			if (FAILED(m_pInputKeyboard->Init(hInstance, hWnd)))
			{//初期化が失敗した場合
				return E_FAIL;
			}
		}
	}

	// 入力デバイス(マウス)の生成
	if (m_pInputMouse == NULL)
	{// 使用していない場合
		m_pInputMouse = new CInputMouse;

		//初期化処理
		if (m_pInputMouse != NULL)
		{// 使用している場合
			if (FAILED(m_pInputMouse->Init(hInstance, hWnd)))
			{//初期化が失敗した場合
				return E_FAIL;
			}
		}
	}

	// 入力デバイス(パッド)の生成
	if (m_pInputPad == NULL)
	{// 使用していない場合
		m_pInputPad = new CInputPad;

		//初期化処理
		if (m_pInputPad != NULL)
		{// 使用している場合
			if (FAILED(m_pInputPad->Init(hInstance, hWnd)))
			{//初期化が失敗した場合
				return E_FAIL;
			}
		}
	}

	// デバッグ表示の生成
	if (m_pDebugProc == NULL)
	{// 使用していない場合
		m_pDebugProc = new CDebugProc;
	}

	//初期化処理
	if (m_pDebugProc != NULL)
	{// 使用している場合
		m_pDebugProc->Init();
	}

	// サウンドの生成
	if (m_pSound == NULL)
	{// 使用していない場合
		m_pSound = new CSound;

		// 初期化
		if (m_pSound != NULL)
		{
			m_pSound->Init(hWnd);
		}
	}

	// カメラの生成
	if (m_pCamera == NULL)
	{// 使用していない場合
		m_pCamera = new CMultiCamera;

		// 初期化
		if (m_pCamera != NULL)
		{
			m_pCamera->Init();

			D3DVIEWPORT9 viewport;

			//プレイヤー追従カメラの画面位置設定
			viewport.X = 0;
			viewport.Y = 0;
			viewport.Width = (DWORD)(SCREEN_WIDTH * 1.0f);
			viewport.Height = (DWORD)(SCREEN_HEIGHT * 1.0f);
			viewport.MinZ = 0.0f;
			viewport.MaxZ = 1.0f;

			m_pCamera->SetViewPort(viewport);
		}
	}

	// ライトの生成
	if (m_pLight == NULL)
	{// 使用していない場合
		m_pLight = new CLight;

		// 初期化
		if (m_pLight != NULL)
		{
			m_pLight->Init();
		}
	}

	//テクスチャの生成
	if (m_pTexture == NULL)
	{// 使用していない場合
		m_pTexture = new CTexture;

		// 初期読み込み
		if (m_pTexture != NULL)
		{
			m_pTexture->Load();
		}
	}

	// Xファイル情報の生成
	if (m_pModelFile == NULL)
	{// 使用していない場合
		m_pModelFile = new CXFile;
	}

	// スロー情報の生成
	if (m_pSlow == NULL)
	{
		m_pSlow = new CSlow;
		m_pSlow->Init();
	}

	// モードの生成
	SetMode(CScene::MODE_TITLE);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CManager::Uninit(void)
{
	// サウンドの停止
	m_pSound->Stop();

	if (m_pFade != NULL)
	{
		m_pFade->Uninit();
		delete m_pFade;
		m_pFade = NULL;
	}

	if (m_pScene != NULL)
	{
		m_pScene->Uninit();
		delete m_pScene;
		m_pScene = NULL;
	}

	if (m_pSlow != NULL)
	{
		m_pSlow->Uninit();
		delete m_pSlow;
		m_pSlow = NULL;
	}

	if (m_pLight != nullptr)
	{
		m_pLight->Uninit();
		delete m_pLight;
		m_pLight = nullptr;
	}

	if (m_pCamera != nullptr)
	{
		m_pCamera->Uninit();
		delete m_pCamera;
		m_pCamera = nullptr;
	}

	if (m_pSound != nullptr)
	{
		m_pSound->Uninit();
		delete m_pSound;
		m_pSound = nullptr;
	}

	if (m_pInputKeyboard != NULL)
	{// 使用している場合
		// 終了処理
		m_pInputKeyboard->Uninit();

		delete m_pInputKeyboard;	// メモリの開放

		m_pInputKeyboard = NULL;	// 使用していない状態にする
	}

	if (m_pInputMouse != NULL)
	{// 使用している場合
		// 終了処理
		m_pInputMouse->Uninit();

		delete m_pInputMouse;	// メモリの開放

		m_pInputMouse = NULL;	// 使用していない状態にする
	}

	if (m_pInputPad != NULL)
	{// 使用している場合
		// 終了処理
		m_pInputPad->Uninit();

		delete m_pInputPad;	// メモリの開放

		m_pInputPad = NULL;	// 使用していない状態にする
	}

	if (m_pDebugProc != NULL)
	{// 使用している場合
		// 終了処理
		m_pDebugProc->Uninit();

		delete m_pDebugProc;	// メモリの開放

		m_pDebugProc = NULL;	// 使用していない状態にする
	}

	if (m_pRenderer != NULL)
	{// 使用している場合
		// 終了処理
		m_pRenderer->Uninit();

		delete m_pRenderer;	// メモリの開放

		m_pRenderer = NULL;	// 使用していない状態にする
	}

	if (m_pSound != NULL)
	{// 使用している場合
		// 終了処理
		m_pSound->Uninit();

		delete m_pSound;	// メモリの開放

		m_pSound = NULL;	// 使用していない状態にする
	}

	// テクスチャの廃棄
	if (m_pTexture != NULL)
	{// 使用している場合
		// 終了処理
		m_pTexture->Unload();

		delete m_pTexture;	// メモリの開放

		m_pTexture = NULL;	// 使用していない状態にする
	}

	// Xファイル情報の廃棄
	if (m_pModelFile != NULL)
	{// 使用している場合
		// 終了処理
		m_pModelFile->Unload();

		delete m_pModelFile;	// メモリの開放

		m_pModelFile = NULL;	// 使用していない状態にする
	}

	// 各種マネージャーの廃棄
	ManagerRelease();
}

//===================================================
// 更新処理
//===================================================
void CManager::Update(void)
{
	if (m_pFade != NULL)
	{
		m_pFade->Update();
	}
	// デバッグ表示の更新処理
	if (m_pDebugProc != NULL)
	{// 使用している場合
		m_pDebugProc->Update();
	}

	// キーボードの更新処理
	if (m_pInputKeyboard != NULL)
	{// 使用している場合
		m_pInputKeyboard->Update();
	}

	// マウスの更新処理
	if (m_pInputMouse != NULL)
	{// 使用している場合
		m_pInputMouse->Update();
	}

	// マウスの更新処理
	if (m_pInputPad != NULL)
	{// 使用している場合
 		m_pInputPad->Update();
	}

	if (m_pScene != NULL)
	{
		m_pScene->Update();
	}
}

//===================================================
// 描画処理
//===================================================
void CManager::Draw(void)
{
	if (m_pScene != NULL)
	{
		m_pScene->Draw();
	}
}

//===================================================
// 描画処理
//===================================================
void CManager::ManagerRelease(void)
{
	//各種マネージャの破棄
	CTaskManager::Release();
	CObjectManager::Release();
	CCameraManager::Release();
}

//===================================================
// レンダラーの取得
//===================================================
CRenderer *CManager::GetRenderer(void)
{
	return m_pRenderer;
}

//===================================================
// 入力デバイスの取得(キーボード)
//===================================================
CInputKeyboard *CManager::GetInputKeyboard(void)
{
	return m_pInputKeyboard;
}

//===================================================
// 入力デバイスの取得(パッド)
//===================================================
CInputPad *CManager::GetInputPad(void)
{
	return m_pInputPad;
}

//===================================================
// 入力デバイスの取得(マウス)
//===================================================
CInputMouse *CManager::GetInputMouse(void)
{
	return m_pInputMouse;
}

//===================================================
// デバッグ表示の取得
//===================================================
CDebugProc *CManager::GetDebugProc(void)
{
	return m_pDebugProc;
}

//===================================================
// サウンドの取得
//===================================================
CSound *CManager::GetSound(void)
{
	return m_pSound;
}

//===================================================
// カメラの取得
//===================================================
CMultiCamera *CManager::GetCamera(void)
{
	return m_pCamera;
}

//===================================================
// ライトの取得
//===================================================
CLight *CManager::GetLight(void)
{
	return m_pLight;
}

//===================================================
// テクスチャの取得
//===================================================
CTexture *CManager::GetTexture(void)
{
	return m_pTexture;
}

//===================================================
// Xファイル情報の取得
//===================================================
CXFile *CManager::GetModelFile(void)
{
	return m_pModelFile;
}

//===================================================
// スロー情報の取得
//===================================================
CSlow *CManager::GetSlow(void)
{
	return m_pSlow;
}

//===================================================
// フェード情報の取得
//===================================================
CFade *CManager::GetFade(void)
{
	return m_pFade;
}

//===================================================
// データ全初期化
//===================================================
CManager *CManager::GetInstance(void)
{
	if (m_pManager == NULL)
	{
		m_pManager = new CManager;
	}

	return m_pManager;
}

//===================================================
// 開放
//===================================================
void CManager::Release(void)
{
	if (m_pManager != NULL)
	{
		m_pManager->Uninit();
		delete m_pManager;
		m_pManager = NULL;
	}
}

//===================================================
// データ全初期化
//===================================================
void CManager::DataReset(void)
{
	// タスクマネージャーの終了
	CTaskManager::GetInstance()->Uninit();

	// オブジェクトマネージャーの終了
	CObjectManager::GetInstance()->Uninit();

	// テクスチャの廃棄
	if (m_pTexture != NULL)
	{// 使用している場合
	 // 終了処理
		m_pTexture->Unload();

		delete m_pTexture;	// メモリの開放

		m_pTexture = NULL;	// 使用していない状態にする
	}

	// Xファイル情報の廃棄
	if (m_pModelFile != NULL)
	{// 使用している場合
		// 終了処理
		m_pModelFile->Unload();

		delete m_pModelFile;	// メモリの開放

		m_pModelFile = NULL;	// 使用していない状態にする
	}

	// ポイントライトを無効にする
	m_pLight->EnablePointLight(false);

	//フェードの削除
	m_pFade = NULL;

	//テクスチャの生成
	if (m_pTexture == NULL)
	{// 使用していない場合
		m_pTexture = new CTexture;

		// 初期読み込み
		if (m_pTexture != NULL)
		{
			m_pTexture->Load();
		}
	}

	// Xファイル情報の生成
	if (m_pModelFile == NULL)
	{// 使用していない場合
		m_pModelFile = new CXFile;
	}

	// タスクマネージャーの初期化
	CTaskManager::GetInstance()->Init();

	// オブジェクトマネージャーの初期化
	CObjectManager::GetInstance()->Init();
}

//===================================================
// モード設定
//===================================================
void CManager::SetMode(CScene::MODE mode)
{
	// サウンドの停止
	if (m_pSound != NULL)
	{
		m_pSound->Stop();
	}

	// 現在のモードの停止
	if (m_pScene != NULL)
	{
		m_pScene->Uninit();
		delete m_pScene;
		m_pScene = NULL;
	}

	if (m_pFade != NULL)
	{
		m_pFade->Uninit();
		delete m_pFade;
		m_pFade = NULL;
	}

	// データリセット
	DataReset();

	// 新しいモードの生成
	m_pScene = CScene::Create(mode);
	m_pScene->SetMode(mode);

	if (m_pScene != NULL)
	{
		m_pScene->Init();
	}

	// フェードの生成
	if (m_pFade == NULL)
	{
		m_pFade = CFade::Create(mode);
	}
}

//===================================================
//
// シーンクラス
//
//===================================================
// コンストラクタ
//===================================================
CScene::CScene()
{

}

//===================================================
// デストラクタ
//===================================================
CScene::~CScene()
{

}

//===================================================
// 生成
//===================================================
CScene *CScene::Create(MODE mode)
{
	CScene *pScene = NULL;

	// モード別に生成
	switch (mode)
	{
	case MODE_TITLE:
		pScene = new CTitle;
		break;

	case MODE_TUTORIAL:
		pScene = new CTutorial;
		break;

	case MODE_GAME:
		pScene = new CGame;
		break;

	case MODE_RESULT:
		pScene = new CResult;
		break;

	case MODE_RANKING:
		pScene = new CRanking;
		break;
	}

	return pScene;
}

//===================================================
// 初期化
//===================================================
HRESULT CScene::Init(void)
{
	return S_OK;
}

//===================================================
// 終了
//===================================================
void CScene::Uninit(void)
{

}

//===================================================
// 更新
//===================================================
void CScene::Update(void)
{
	// カメラの更新処理
	if (CManager::GetInstance()->GetCamera() != NULL)
	{
		CManager::GetInstance()->GetCamera()->Update();
	}

	// スローの更新処理
	if (CManager::GetInstance()->GetSlow() != NULL)
	{
		CManager::GetInstance()->GetSlow()->Update();
	}

	// レンダラーの更新処理
	if (CManager::GetInstance()->GetRenderer() != NULL)
	{// 使用している場合
		CManager::GetInstance()->GetRenderer()->Update();
	}

	// タスクの全更新
	CTaskManager::GetInstance()->Update();
}

//===================================================
// 描画
//===================================================
void CScene::Draw(void)
{
	// 描画処理
	if (CManager::GetInstance()->GetRenderer() != NULL)
	{// 使用している場合
		CManager::GetInstance()->GetRenderer()->Draw();
	}
}

