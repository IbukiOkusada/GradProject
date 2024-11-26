//==========================================================
//
// winsock通信(クライアント) [tcp_client.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include <winsock2.h>
#include "protocol_online.h"

#define MAX_NAME	(1024)

//==========================================================
// クライアントクラスの定義
//==========================================================
class CClient
{
public:	// 誰でもアクセス可能

	CClient();	// コンストラクタ
	~CClient();	// デストラクタ

	// メンバ関数
	void BindSock(SOCKET sock);
	void Uninit(void);
	int Send(const char *pSendData, int nSendDataSize);
	int Recv(char *pRecvData, int nRecvDataSize);
	void SetIP(char *pIp);
	void SetCliePort(int nPort) { m_nClientPort = nPort; }
	char *GetIP(void) { return &m_aClientIP[0]; }
	int GetPort(void) { return m_nClientPort; }
	int GetId(void) { return m_nId; }
	void SetId(int nId) { m_nId = nId;}
	bool GetSend(void) { return m_bSend; }
	void SetSend(bool bSend) { m_bSend = bSend; }
	void SetData(char *pChar, int nByte);
	void ResetData(void);
	int GetSendByte(void) { return m_nSendByte; }
	char *GetSendData(void) { return &m_aSendData[0]; }
	bool GetDeath(void) { return m_bDeath; }
	void SetDeath(bool bDeath) { m_bDeath = bDeath; }

private:	// 自分だけアクセス可能

	// メンバ変数
	SOCKET m_sock;	// ソケット
	char m_aSendData[NetWork::MAX_SEND_DATA] = {};	// 送信するデータ
	char m_aTempSendData[NetWork::MAX_SEND_DATA] = {};	// 送信するデータ
	char m_aClientIP[64];	// クライアントのIP番号
	int m_nClientPort;	// クライアントのポート番号
	int m_nId;			// ID
	bool m_bSend;		// データ送信中
	int m_nTempSendByte;	// データ数
	int m_nSendByte;	// データ数
	bool m_bDeath;		// 切断しているか否か
};

#endif