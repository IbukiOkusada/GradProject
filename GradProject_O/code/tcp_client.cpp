//==========================================================
//
// winsock通信(クライアント)
// Author : Ibuki Okusada
//
//==========================================================
#include <stdio.h>
#include "tcp_client.h"

//==========================================================
// コンストラクタ
//==========================================================
CClient::CClient()
{
	// 値をクリアする
	m_sock = NULL;
}

//==========================================================
// デストラクタ
//==========================================================
CClient::~CClient()
{

}

//==========================================================
// 初期化処理
//==========================================================
bool CClient::Init(const char *pIPAddress, int nPortNum)
{
	// Winsockの初期化処理
	WSADATA wsaData;
	int nErr = WSAStartup(WINSOCK_VERSION, &wsaData);	// winsockの初期化関数

	if (nErr != 0)
	{// 初期化に失敗した場合

		return false;	// 終了
	}

	m_sock = socket(AF_INET, SOCK_STREAM, 0);	//接続受付用のソケットの作成する関数

	if (m_sock == INVALID_SOCKET)
	{// 作成に失敗した場合
		return false;	// 終了
	}

	// 接続先の設定
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPortNum);	// 接続を受け付けるポート、番号を固定(接続先ポート番号)
	addr.sin_addr.S_un.S_addr = inet_addr(pIPAddress);	// 接続先IPアドレスを入れる

	// 接続する
	if (connect(m_sock, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{// 接続できなかった場合

		// キー入力待ち
		rewind(stdin);
		getchar();

		return false;	// 終了
	}
	else
	{// 接続できたとき
		m_ClientIP = pIPAddress;
	}

	return true;
}

//==========================================================
// 再接続処理
//==========================================================
bool CClient::Reconnect(const char* pIPAddress, int nPortNum)
{
	// Winsockの初期化処理
	WSADATA wsaData;
	int nErr = WSAStartup(WINSOCK_VERSION, &wsaData);	// winsockの初期化関数

	if (nErr != 0)
	{// 初期化に失敗した場合

		return false;	// 終了
	}

	m_sock = socket(AF_INET, SOCK_STREAM, 0);	//接続受付用のソケットの作成する関数

	if (m_sock == INVALID_SOCKET)
	{// 作成に失敗した場合
		return false;	// 終了
	}

	// 接続先の設定
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPortNum);	// 接続を受け付けるポート、番号を固定(接続先ポート番号)
	addr.sin_addr.S_un.S_addr = inet_addr(pIPAddress);	// 接続先IPアドレスを入れる

	// 接続する
	if (connect(m_sock, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{// 接続できなかった場合

		// キー入力待ち
		rewind(stdin);
		getchar();

		return FALSE;	// 終了
	}
	else
	{// 接続できたとき
		m_ClientIP = pIPAddress;
	}

	return TRUE;
}

//==========================================================
// 終了処理
//==========================================================
void CClient::Uninit(void)
{
	// 接続を切断する
	closesocket(m_sock);	// クライアントとの接続を閉じる

	//Winsock終了処理
	WSACleanup();	// WSACleanup関数 : winsockの終了処理
}

//==========================================================
// 送信処理
//==========================================================
int CClient::Send(const char *pSendData, int nSendDataSize)
{
	// データ送信
	int nSendByte = send(m_sock, pSendData, nSendDataSize, 0);	// send関数: データを送信する

	return nSendByte;
}

//==========================================================
// 受信処理
//==========================================================
int CClient::Recv(char *pRecvData, int nRecvDataSize)
{
	int nRecvByte = recv(m_sock, pRecvData, nRecvDataSize, 0);	// recv関数: データを受信する

	return nRecvByte;
}

//==========================================================
// ソケット設定
//==========================================================
void CClient::BindSock(SOCKET sock)
{
	m_sock = sock;
}
