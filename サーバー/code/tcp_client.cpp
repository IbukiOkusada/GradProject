//==========================================================
//
// winsock通信(クライアント) [tcp_client.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include <stdio.h>
#include "tcp_client.h"
#include <string.h>

CClient *CClient::m_pTop = NULL;	// 先頭のオブジェクトへのポインタ
CClient *CClient::m_pCur = NULL;	// 最後尾のオブジェクトへのポインタ

//==========================================================
// コンストラクタ
//==========================================================
CClient::CClient()
{
	// 値をクリアする
	m_sock = NULL;

	m_nId = -1;

	// 自分自身をリストに追加
	if (m_pTop != NULL)
	{// 先頭が存在している場合
		m_pCur->m_pNext = this;	// 現在最後尾のオブジェクトのポインタにつなげる
		m_pPrev = m_pCur;
		m_pCur = this;	// 自分自身が最後尾になる
	}
	else
	{// 存在しない場合
		m_pTop = this;	// 自分自身が先頭になる
		m_pCur = this;	// 自分自身が最後尾になる
	}

	m_bSend = false;
	memset(&m_aSendData[0], '\0', sizeof(m_aSendData));
	memset(&m_aTempSendData[0], '\0', sizeof(m_aTempSendData));
	m_nSendByte = 0;
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

		// リストから自分自身を削除する
	if (m_pTop == this)
	{// 自身が先頭
		if (m_pNext != NULL)
		{// 次が存在している
			m_pTop = m_pNext;	// 次を先頭にする
			m_pNext->m_pPrev = NULL;	// 次の前のポインタを覚えていないようにする
		}
		else
		{// 存在していない
			m_pTop = NULL;	// 先頭がない状態にする
			m_pCur = NULL;	// 最後尾がない状態にする
		}
	}
	else if (m_pCur == this)
	{// 自身が最後尾
		if (m_pPrev != NULL)
		{// 次が存在している
			m_pCur = m_pPrev;			// 前を最後尾にする
			m_pPrev->m_pNext = NULL;	// 前の次のポインタを覚えていないようにする
		}
		else
		{// 存在していない
			m_pTop = NULL;	// 先頭がない状態にする
			m_pCur = NULL;	// 最後尾がない状態にする
		}
	}
	else
	{
		if (m_pNext != NULL)
		{
			m_pNext->m_pPrev = m_pPrev;	// 自身の次に前のポインタを覚えさせる
		}
		if (m_pPrev != NULL)
		{
			m_pPrev->m_pNext = m_pNext;	// 自身の前に次のポインタを覚えさせる
		}
	}
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
		int nowbyte = strlen(&m_aTempSendData[0]);
		if (nowbyte + nByte < NetWork::MAX_COMMAND_DATA)
		{
			// 仮に入れておく
			memcpy(&m_aTempSendData[nowbyte], pChar, nByte);
		}
	}
}

//==========================================================
// 送信データのリセット
//==========================================================
void CClient::ResetData(void)
{
	memset(&m_aSendData[0], '\0', sizeof(m_aSendData));
	m_nSendByte = strlen(&m_aSendData[0]);

	int nowbyte = strlen(&m_aSendData[0]);
	if (nowbyte < NetWork::MAX_COMMAND_DATA)
	{
		// 仮に入れておく
		memcpy(&m_aSendData[0], &m_aTempSendData[0], nowbyte);
	}

	memset(&m_aTempSendData[0], '\0', sizeof(m_aTempSendData));
}