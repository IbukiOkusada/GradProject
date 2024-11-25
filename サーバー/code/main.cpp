//==========================================================
//
// winsock通信(サーバー)
// Author : Ibuki Okusada
//
//==========================================================
#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include <string.h>
#include "tcp_server.h"
#include "tcp_client.h"
#include <thread>
#include <stdlib.h>
#include "protocol_online.h"
#include "d3dx9.h"
#include "network.h"
#include "deltatime.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")		// システム時刻取得に必要

// マクロ定義
#define MAX_STRING	(2048)	// 最大文字数
#define DEF_PORT	(22333)	// ポート番号

// IP情報クラス定義
void Access(CClient *pClient);
void Join(int nId, CClient *pClient);
void SetPosition(int nId, D3DXVECTOR3 pos, CClient *pClient);
void SetRotation(int nId, D3DXVECTOR3 pos, CClient *pClient);
void SetDamage(int nId, int nDamage, CClient *pClient);
void SetLife(int nId, int nLife, CClient *pClient);
void SetUp(int nId, CClient *pClient);
void Leave(int nId, CClient *pClient);
void Goal(int nId, CClient *pClient);
void Accept(CServer *pServer);
void Send(CServer **ppServer);

// グローバル変数宣言
char g_aSendData[MAX_STRING] = {};	// 送信データ
int g_nSendByte = 0;	// 送信データ量

//==========================================================
// メイン関数
//==========================================================
int main(void)
{
	CDeltaTime::Create();
	auto net = CNetWork::Create();

	while (1)
	{
		CDeltaTime::GetInstance()->Update();
	}

	net->Release();

	CDeltaTime::GetInstance()->Uninit();

	printf("*** Enterキーを押して終了してください ***\n");

	// キー入力待ち
	rewind(stdin);
	getchar();

	return S_OK;
}
