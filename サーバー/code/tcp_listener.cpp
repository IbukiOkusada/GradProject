//==========================================================
//
// winsock通信(接続受付) [tcp_listener.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "tcp_listener.h"
#include "tcp_client.h"
#include <stdio.h>

//==========================================================
// コンストラクタ
//==========================================================
CListener::CListener()
{
	// 値をクリアする
	m_sockServer = NULL;
	m_nClientPort = 0;
	memset(&m_aClientIP[0], '\0', sizeof(m_aClientIP));
}

//==========================================================
// デストラクタ
//==========================================================
CListener::~CListener()
{

}

//==========================================================
// 初期化処理
//==========================================================
bool CListener::Init(int nPortNum)
{
	// ソケットの作成
	m_sockServer = socket(AF_INET, SOCK_STREAM, 0);	//接続受付用のソケットの作成する関数

	if (m_sockServer == INVALID_SOCKET)
	{// 作成に失敗した場合

		return FALSE;	// 終了
	}

	// 接続を受け付けるための準備
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPortNum);	// 接続を受け付けるポート、番号を固定
	addr.sin_addr.S_un.S_addr = INADDR_ANY;	// ソケットにポートを割り当て
	bind(m_sockServer, (struct sockaddr*)&addr, sizeof(addr));	// 接続受付の準備、保留中の接続最大数を10で固定
	listen(m_sockServer, 10);

	return TRUE;
}

//==========================================================
// 終了処理
//==========================================================
void CListener::Uninit(void)
{
	// 接続受付用ソケットを閉じる
	closesocket(m_sockServer);	// closesocket関数 : ソケットを閉じる
}

//==========================================================
// 接続受付処理
//==========================================================
SOCKET CListener::Accept(void)
{
	SOCKET sock;	// ソケット

	// 接続を待つ
	printf("    *** サーバー接続受付中... ***\n");
	struct sockaddr_in addrClient;
	int nLengthAddr = sizeof(addrClient);
	sock = accept(m_sockServer,	// accept関数 : 接続を受け入れる SOCKET型の変数->ソケットディスクリプタ
		(struct sockaddr*)&addrClient, &nLengthAddr);	// クライアントが接続したらソケットが生成される

	// 接続元のIPアドレス・ポート番号を取得
	const char *pClientIP = inet_ntoa(addrClient.sin_addr);
	strcpy(&m_aClientIP[0], pClientIP);
	m_nClientPort = ntohs(addrClient.sin_port);

	// 接続メッセージの表示
	printf("*** クライアントと接続しました!!! ***\n");
	printf("IPアドレス: %s\n", &m_aClientIP[0]);
	printf("ポート番号: %d\n", m_nClientPort);

	CClient *pClient = CClient::GetTop();	// 先頭を取得

	int nCnt = 0;

	while (pClient != NULL)
	{// 使用されていない状態まで

		CClient *pClientNext = pClient->GetNext();	// 次のオブジェクトへのポインタを取得

		nCnt++;
		pClient = pClientNext;	// 次のオブジェクトに移動
	}

	printf("*** 現在%d人接続中***\n", nCnt);

	return sock;
}
