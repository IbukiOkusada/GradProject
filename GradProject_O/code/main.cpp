//==========================================================
//
// メイン処理 [main.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "main.h"
#include "manager.h"
#include <time.h>

//メモリ破壊検出
#include <crtdbg.h>

//マクロ定義
#define CLASS_NAME	"WindowClass"		//ウインドウクラスの名前
#define WINDOW_NAME	"GradProject"	//ウインドウの名前

//プロトタイプ宣言
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// グローバル変数
int g_nCountFPS = 0;		//FPSカウンタ

//===========================================================
//メイン関数
//===========================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, LPSTR lpCmdLine, int nCmdShow)
{
	//メモリ破壊検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);

	DWORD dwCurrentTime;	//現在時刻
	DWORD dwExecLastTime;	//最後に処理した時刻
	DWORD dwFrameCount;		//フレームカウント
	DWORD dwFPSLastTime;	//最後にFPSを計測した時刻

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),				//WNDCLASSEXのメモリサイズ
		CS_CLASSDC,						//ウインドウのスタイル
		WindowProc,						//ウインドウプロシージャ
		0,								//0にする(通常は使用しない)
		0,								//0にする(通常は使用しない)
		hInstance,						//インスタンスハンドル
		LoadIcon(NULL,IDI_APPLICATION),	//タスクバーのアイコン
		LoadCursor(NULL,IDC_ARROW),		//マウスカーソル
		(HBRUSH)(COLOR_WINDOW + 1),		//クライアント領域の背景色
		NULL,							//メニューバー
		CLASS_NAME,						//ウインドウクラスの名前
		LoadIcon(NULL,IDI_APPLICATION)	//ファイルのアイコン
	};

	HWND hWnd;	//ウインドウハンドル
	MSG msg;	//メッセージを格納する変数

				//ウインドウクラスの登録
	RegisterClassEx(&wcex);

	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	//ウインドウクラスの登録
	RegisterClassEx(&wcex);

	//クライアント形成を指定のサイズに調整
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	//ウインドウを生成
	hWnd = CreateWindowEx
	(
		0,								//拡張ウインドウスタイル
		CLASS_NAME,						//ウインドウクラスの名前
		WINDOW_NAME,					//ウインドウの名前
		WS_OVERLAPPEDWINDOW,			//ウインドウスタイル
		CW_USEDEFAULT,					//ウインドウの左上x座標
		CW_USEDEFAULT,					//ウインドウの左上y座標
		(rect.right - rect.left),		//ウインドウの幅
		(rect.bottom - rect.top),		//ウインドウの高さ
		NULL,							//親ウインドウのハンドル
		NULL,							//メニューハンドルまたは子ウインドウID
		hInstance,						//インスタンスハンドル
		NULL							//ウインドウ作成データ
	);

	// マネージャーの初期化処理
	if (FAILED(CManager::GetInstance()->Init(hInstance, hWnd, TRUE)))
	{// 失敗した場合
		return -1;
	}

	//分解能を設定
	timeBeginPeriod(1);

	dwCurrentTime = 0;	//初期化
	dwExecLastTime = timeGetTime();	//現在時刻を取得(保存)
	dwFrameCount = 0;	//初期化
	dwFPSLastTime = timeGetTime();	//現在時刻を取得(保存)

	// ランダムの種を再生成
	srand((unsigned)time(0));

	//ウインドウの表示
	ShowWindow(hWnd, nCmdShow);

	UpdateWindow(hWnd);

	//メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{//windowsの処理
			if (msg.message == WM_QUIT)
			{
				break;
			}
			else
			{
				//メッセージの設定
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{//DirectXの処理

			dwCurrentTime = timeGetTime();	//現在時刻を取得
			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{//0.5秒経過
				//FPSを計測
				g_nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);

				dwFPSLastTime = dwCurrentTime;	//FPSを測定した時刻を保存
				dwFrameCount = 0;	//フレームカウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{//60分の1秒経過
				dwExecLastTime = dwCurrentTime;	//処理開始の時刻[現在時刻]を保存

				if (CManager::GetInstance() != NULL)
				{// 生成できた場合

					// 更新処理
					CManager::GetInstance()->Update();

					// 描画処理
					CManager::GetInstance()->Draw();
				}

				dwFrameCount++;	//フレームカウントを加算
			}
		}
	}

	// 終了処理
	if (CManager::GetInstance() != NULL)
	{// 使用している場合
		// 終了処理
		CManager::Release();
	}

	//ウインドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	return (int)msg.wParam;
}

//===========================================================
//ウインドウプロシージャ
//===========================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nID;					//返り値を格納

	const RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };		//ウインドウの領域(形)

	switch (uMsg)
	{
	case WM_DESTROY:		//ウィンドウ廃棄のメッセージ

		//WM_QUETメッセージを送る
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:		//キー押下のメッセージ

		switch (wParam)
		{
		case VK_ESCAPE:

			nID = MessageBox(hWnd, "終了しますか？", "終了メッセージ", MB_YESNO);

			if (nID == IDYES)
			{
				//ウインドウを廃棄する(WM_DESTROYメッセージを送る)
				DestroyWindow(hWnd);
			}
			break;
		}
		break;
	case WM_COMMAND:	//コマンド発行メッセージ
		break;
	case WM_CLOSE:		//閉じるボタン押下
		nID = MessageBox(hWnd, "終了しますか？", "終了メッセージ", MB_YESNO);

		if (nID == IDYES)
		{//はいを選択したとき
			//ウインドウを廃棄する(WM_DESTROYメッセージを送る)
			DestroyWindow(hWnd);
		}
		else
		{//いいえを選択したとき
			return 0;	//0を返さないと終了してしまう
		}
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);	//既定の処理を返す
}

//================================================
//FPSの取得
//================================================
int GetFPS(void)
{
	return g_nCountFPS;
}