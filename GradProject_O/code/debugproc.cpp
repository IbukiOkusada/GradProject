//==========================================================
//
// デバッグ表示処理 [debugproc.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "main.h"
#include "debugproc.h"
#include <string.h>
#include "input.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "Xfile.h"
#include "camera.h"
#include "object_manager.h"
#include "camera_manager.h"
#include "input_gamepad.h"
#include "input_keyboard.h"
#include "task_manager.h"
#include "object_manager.h"

// 静的メンバ変数宣言
LPD3DXFONT CDebugProc::m_pFont = nullptr;	// デバッグフォントへのポインタ
CDebugProc* CDebugProc::m_pInstance = nullptr;
//**********************************************************
// 無名名前空間
//**********************************************************
namespace
{
	const int MAX_FLOATNUM = (2);		//小数点以下の表示桁数
	const int MAX_DEBUGSTRING = (2048);	//デバッグ表示の最大文字数;
}

//**********************************************************
//デバッグONOFF表示メッセージ
//**********************************************************
const char *CDebugProc::m_apMode[CScene::MODE_MAX] =
{
	"タイトル",
	"エントリー",
	"ゲーム",
	"リザルト",
	"マルチリザルト",
};

//==========================================================
// コンストラクタ
//==========================================================
CDebugProc::CDebugProc()
{
	//デバッグ表示情報のクリア
	m_bDisp = false;
	m_pFont = nullptr;
	m_pStr = nullptr;
	memset(&m_aStr[0], NULL, sizeof(m_aStr));
}

//==========================================================
// デストラクタ
//==========================================================
CDebugProc::~CDebugProc()
{
	
}

//==========================================================
//デバッグ表示の初期化処理
//==========================================================
void CDebugProc::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice;		//デバイスへのポインタ

	//デバイスの取得
	pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	//デバッグ表示用フォントの生成
	D3DXCreateFont(pDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Terminal", &m_pFont);

//初期表示設定
#if _DEBUG
	m_bDisp = true;
#else NDEBUG
	m_bDisp = false;
#endif

}

//==========================================================
//デバッグ表示の終了処理
//==========================================================
void CDebugProc::Uninit(void)
{
	//デバッグ表示用フォントの廃棄
	if (m_pFont != nullptr)
	{
		m_pFont->Release();
		m_pFont = nullptr;
	}

	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

//==========================================================
//デバッグ表示の更新処理
//==========================================================
void CDebugProc::Update(void)
{
	CInputKeyboard *pInputKeyboard = CInputKeyboard::GetInstance();	// キーボードのポインタ

	if(pInputKeyboard->GetTrigger(DIK_F1) == true)
	{//F1キーが押されたとき
		m_bDisp = m_bDisp ? false : true;
	}

	Print("FPS : %d\n", GetFPS());
	//Print("現在のモード [ %s ] : [Enter]で遷移\n", m_apMode[CManager::GetInstance()->GetMode()]);
	Print("優先順位別(%d個分) ↓\n", NUM_PRIORITY);
	for (int nCntPri = 0; nCntPri < NUM_PRIORITY; nCntPri++)
	{
		if ((nCntPri + 1) % 4 == 0)
		{
			Print("\n");
		}
	}

	Print("\n");
	Print("テクスチャの読み込み総数(%d) \n", CManager::GetInstance()->GetTexture()->GetNumAll());
	Print("モデルの読み込み総数    (%d) \n", CManager::GetInstance()->GetModelFile()->GetNumAll());
	Print("タスクの総数            (%d) \n", CTaskManager::GetInstance()->GetList()->GetNum());
	Print("オブジェクトの総数      (%d) \n", CObjectManager::GetInstance()->GetNumAll());

	{
		D3DXVECTOR3 CamPosV = CManager::GetInstance()->GetCamera()->GetPositionV();
		Print("カメラ 視点 : [%f, %f, %f]\n", CamPosV.x, CamPosV.y, CamPosV.z);
	}

	{
		D3DXVECTOR3 CamPosR = CManager::GetInstance()->GetCamera()->GetPositionR();
		Print("       注視点 : [%f, %f, %f]\n", CamPosR.x, CamPosR.y, CamPosR.z);
	}

	{
		D3DXVECTOR3 CamRot = CManager::GetInstance()->GetCamera()->GetRotation();
		Print("       向き : [%f, %f, %f]\n", CamRot.x, CamRot.y, CamRot.z);
	}

	{
		Print("       カメラ数 : [%d] : 距離 [%f]\n", CCameraManager::GetInstance()->GetNum(), CManager::GetInstance()->GetCamera()->GetLength());
	}

	{
		Print("\n-------------------------------------------------------------------------------------\n");
	}
}

//==========================================================
//デバッグ表示の描画処理(古い方)
//==========================================================
void CDebugProc::Draw(void)
{
	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	if (m_bDisp == true)
	{//デバックモードがオンの時
		//テキストの描画
		m_pFont->DrawText(nullptr, &m_aStr[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	}

	//デバッグ表示情報のクリア
	memset(&m_aStr[0], NULL, sizeof(m_aStr));
}

//==========================================================
//デバッグ表示の描画処理(新しい方)
//==========================================================
void CDebugProc::DrawNew(void)
{

	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	if (m_bDisp == true && m_pStr != nullptr)
	{//デバックモードがオンの時
		//テキストの描画
		m_pFont->DrawText(nullptr, m_pStr, -1, &rect, DT_LEFT, D3DCOLOR_RGBA(0, 0, 0, 255));
	}

	if (m_pStr != nullptr)
	{
		delete[] m_pStr;
		m_pStr = nullptr;
	}
}

//==========================================================
//デバッグ表示の設定処理(古い方)
//==========================================================
void CDebugProc::Print(const char* fmt, ...)
{
#if _DEBUG

	if (!m_bDisp) { return; }

#else
	if (!m_bDisp) { return; }
#endif
	va_list args;
	char aString[MAX_DEBUGSTRING];		// 指定文字格納用
	char aSaveString[MAX_DEBUGSTRING];	// 可変引数中身格納用
	int nLength = 0;	// 可変引数内の文字の長さ
	int nStopLength;	// 可変引数挿入場所より

	// 文字列の代入
	strcpy(&aString[0], fmt);

	va_start(args, fmt);

	for (char* p = &aString[0]; *p != '\0'; p++)
	{
		if (*p == '%')
		{// 確認文字がある場合

			p++;	// 確認文字まで進める

			nLength = 0;	// 文字の長さリセット

			// 指定された種類ごとに変数の値を抜き出す(va_arg)
			switch (*p)
			{
			case 'd':	// 数字

				sprintf(&aSaveString[0], "%d", va_arg(args, int));

				break;
			case 'f':	// 小数

				sprintf(&aSaveString[0], "%f", va_arg(args, double));

				// 文字列の長さを取得
				nLength = (int)strlen(&aSaveString[0]);

				// 小数点以下の1文字目まで確認
				for (int nCntlength = 0; aSaveString[nCntlength] != '\0'; nCntlength++)
				{
					if (aSaveString[nCntlength] == '.')
					{// 小数点があった場合

						// 小数点以下の桁数を求める
						int nMin = (int)strlen(&aSaveString[nCntlength + 1]);

						// 小数点より先の文字数と指定した値までの差分を求める
						nMin -= MAX_FLOATNUM;

						// 文字列の長さを小数点以下第二位までに補正
						nLength -= nMin;
					}
				}

				break;
			case 'c':	// 文字

				sprintf(&aSaveString[0], "%c", va_arg(args, char));

				break;
			case 's':	// 文字列

				sprintf(&aSaveString[0], "%s", va_arg(args, const char*));

				break;
			}

			if (nLength == 0)
			{// 文字の長さを取得していない場合

				// 文字列の長さを取得
				nLength = (int)strlen(&aSaveString[0]);
			}

			nStopLength = (int)strlen(p) + 1;

			// メモリ内の確認文字(%～)より後ろの文字列をずらす
			memmove(p + nLength - 2, p, nStopLength);

			p--;	// ポインタを%まで戻す

			// 可変引数を指定された場所に挿入
			memcpy(p, &aSaveString[0], nLength);
		}
	}

	va_end(args);

	if (strlen(&m_aStr[0]) + strlen(&aString[0]) >= 2048) { return; }

	// 今回の文字
	strcat(&m_aStr[0], &aString[0]);
}

//==========================================================
//デバッグ表示の設定処理(新しい方)
//==========================================================
void CDebugProc::PrintNew(const char *fmt, ...)
{
#if _DEBUG

	if (!m_bDisp) { return; }

#else
	if (!m_bDisp) { return; }
#endif
	va_list args = nullptr;
	char* pTemp = m_pStr;	// 借入
	m_pStr = nullptr;
	char aString[MAX_DEBUGSTRING] = "";		// 指定文字格納用
	char aSaveString[MAX_DEBUGSTRING] = "";	// 可変引数中身格納用
	int nLength = 0;	// 可変引数内の文字の長さ
	int nStopLength;	// 可変引数挿入場所より
	int nNowLength = 0;	// 現在格納中文字列の長さ

	// 文字列の長さを取得
	if (pTemp != nullptr)
	{
		nNowLength = static_cast<int>(strlen(pTemp));
	}

	// 文字列の代入
	strcpy(&aString[0], fmt);

	va_start(args, fmt);

	for (char *p = &aString[0]; *p != '\0'; p++)
	{
		if (*p == '%')
		{// 確認文字がある場合

			p++;	// 確認文字まで進める

			nLength = 0;	// 文字の長さリセット

			// 指定された種類ごとに変数の値を抜き出す(va_arg)
			switch (*p)
			{
			case 'd':	// 数字

				sprintf(&aSaveString[0], "%d", va_arg(args, int));

				break;
			case 'f':	// 小数

				sprintf(&aSaveString[0], "%f", va_arg(args, double));

				// 文字列の長さを取得
				nLength = (int)strlen(&aSaveString[0]);

				// 小数点以下の1文字目まで確認
				for (int nCntlength = 0; aSaveString[nCntlength] != '\0'; nCntlength++)
				{
					if (aSaveString[nCntlength] == '.')
					{// 小数点があった場合

						// 小数点以下の桁数を求める
						int nMin = (int)strlen(&aSaveString[nCntlength + 1]);

						// 小数点より先の文字数と指定した値までの差分を求める
						nMin -= MAX_FLOATNUM;

						// 文字列の長さを小数点以下第二位までに補正
						nLength -= nMin;
					}
				}

				break;
			case 'c':	// 文字

				sprintf(&aSaveString[0], "%c", va_arg(args, char));

				break;
			case 's':	// 文字列

				sprintf(&aSaveString[0], "%s", va_arg(args, const char*));

				break;

			default:	// それ以外
				{
				continue;
				}
				break;
			}

			if (nLength == 0)
			{// 文字の長さを取得していない場合

				// 文字列の長さを取得
				nLength = (int)strlen(&aSaveString[0]);
			}

			nStopLength = (int)strlen(p) + 1;

			// メモリ内の確認文字(%～)より後ろの文字列をずらす
			memmove(p + nLength - 2, p, nStopLength);

			p--;	// ポインタを%まで戻す

			// 可変引数を指定された場所に挿入
			memcpy(p, &aSaveString[0], nLength);
		}
	}

	va_end(args);

	// 今回連結する文字列の長さを取得
	int nCurrentLength = (int)strlen(&aString[0]);

	// 文字列のメモリを確保
	m_pStr = DEBUG_NEW char[nNowLength + nCurrentLength + 1];
	memset(m_pStr, NULL, sizeof(*m_pStr));

	// 文字列を連結する
	if (pTemp != nullptr)
	{ // 元の文字
		strcat(m_pStr, pTemp);

		// 元のデータを廃棄
		delete[] pTemp;
	}

	// 今回の文字
	strcat(m_pStr, &aString[0]);
}

//==========================================================
//デバッグ表示の設定処理
//==========================================================
CDebugProc* CDebugProc::Create()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = DEBUG_NEW CDebugProc;
		m_pInstance->Init();
	}

	return m_pInstance;
}
