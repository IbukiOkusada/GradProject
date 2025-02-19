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
	memset(&m_aSendData[0], '\0', sizeof(m_aSendData));
	m_nSendByte = 0;
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
	addr.sin_port = htons(static_cast<u_short>(nPortNum));	// 接続を受け付けるポート、番号を固定(接続先ポート番号)
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
	addr.sin_port = htons(static_cast<u_short>(nPortNum));	// 接続を受け付けるポート、番号を固定(接続先ポート番号)
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
int CClient::Send()
{
	int nSendByte = 0;
	// データ送信
	if (m_nSendByte > 0)
	{
		nSendByte = send(m_sock, &m_aSendData[0], m_nSendByte, 0);	// send関数: データを送信する
	}
	memset(&m_aSendData[0], '\0', sizeof(m_aSendData));
	m_nSendByte = 0;

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
// 送信データ設定
//==========================================================
void CClient::SetData(const char* pSendData, int nSendDataSize)
{
	if (m_nSendByte + nSendDataSize < NetWork::MAX_SEND_DATA)
	{
		memcpy(&m_aSendData[m_nSendByte], pSendData, nSendDataSize);
		m_nSendByte += nSendDataSize;
	}
}

//==========================================================
// ソケット設定
//==========================================================
void CClient::BindSock(SOCKET sock)
{
	m_sock = sock;
}
