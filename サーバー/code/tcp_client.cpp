//==========================================================
//
// winsock通信(クライアント) [tcp_client.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include <stdio.h>
#include "tcp_client.h"
#include "tcp_listener.h"
#include <string.h>

//==========================================================
// コンストラクタ
//==========================================================
CClient::CClient()
{
	// 値をクリアする
	m_sock = NULL;
	m_nId = -1;

	m_bSend = false;
	m_nSendByte = 0;
	m_nTempSendByte = 0;
	m_bDeath = false;
}

//==========================================================
// デストラクタ
//==========================================================
CClient::~CClient()
{

}

//==========================================================
// ソケットの割り当て
//==========================================================
void CClient::BindSock(SOCKET sock)
{
	m_sock = sock;
}

//==========================================================
// 終了処理
//==========================================================
void CClient::Uninit(void)
{
	// 接続を切断する
	closesocket(m_sock);	// クライアントとの接続を閉じる
	CListener::SetNum(CListener::GetNum() - 1);
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
// IPの設定
//==========================================================
void CClient::SetIP(char *pIp)
{
	memcpy(&m_aClientIP[0], pIp, strlen(pIp));
}

//==========================================================
// 送信データの設定
//==========================================================
void CClient::SetData(char *pChar, int nByte)
{
	if (!m_bSend)
	{
		if (m_nSendByte + nByte < NetWork::MAX_COMMAND_DATA)
		{
			memcpy(&m_aSendData[m_nSendByte], pChar, nByte);
			m_nSendByte += nByte;
		}
	}
	else
	{
		if (m_nTempSendByte + nByte < NetWork::MAX_COMMAND_DATA)
		{
			// 仮に入れておく
			memcpy(&m_aTempSendData[m_nTempSendByte], pChar, nByte);
			m_nTempSendByte += nByte;
		}
	}
}

//==========================================================
// 送信データのリセット
//==========================================================
void CClient::ResetData(void)
{
	memset(&m_aSendData[0], '\0', sizeof(m_aSendData));
	m_nSendByte = 0;

	if (m_nTempSendByte > 0)
	{
		// 仮に入れておく
		memcpy(&m_aSendData[0], &m_aTempSendData[0], m_nTempSendByte);
		m_nSendByte += m_nTempSendByte;
	}

	memset(&m_aTempSendData[0], '\0', sizeof(m_aTempSendData));
	m_nTempSendByte = 0;
}