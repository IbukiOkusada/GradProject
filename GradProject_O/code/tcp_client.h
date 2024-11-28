//==========================================================
//
// winsock通信(クライアント)
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include "network.h"

//==========================================================
// クライアントクラスの定義
//==========================================================
class CClient
{
public:	// 誰でもアクセス可能

	CClient();	// コンストラクタ
	~CClient();	// デストラクタ

	// メンバ関数
	bool Init(const char *pIPAddress, int nPortNum);
	void BindSock(SOCKET sock);
	void Uninit(void);
	bool Reconnect(const char* pIPAddress, int nPortNum);

	// 通信
	int Send();
	int Recv(char *pRecvData, int nRecvDataSize);
	void SetData(const char* pSendData, int nSendDataSize);

	// メンバ関数(取得)
	void SetCliePort(int nPort) { m_nClientPort = nPort; }
	const char *GetIP(void) { return m_ClientIP.c_str(); }
	int GetPort(void) { return m_nClientPort; }
	int GetId(void) { return m_nId; }
	int GetByte() { return m_nSendByte; }
	void SetId(int nId) { m_nId = nId; }
	SOCKET GetSock(void) { return m_sock; }

private:	// 自分だけアクセス可能

	// メンバ変数
	char m_aSendData[NetWork::MAX_SEND_DATA];
	int m_nSendByte;

	SOCKET m_sock;	// ソケット
	std::string m_ClientIP;
	int m_nClientPort;
	int m_nId;
};

#endif