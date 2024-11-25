//==========================================================
//
// winsock通信(接続受付) [tcp_listener.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _TCP_LISTENER_H_
#define _TCP_LISTENER_H_

#include <winsock2.h>

//==========================================================
// クライアントクラスの定義
//==========================================================
class CListener
{
public:	// 誰でもアクセス可能

	CListener();	// コンストラクタ
	~CListener();	// デストラクタ

	// メンバ関数
	bool Init(int nPortNum);
	SOCKET Accept(void);
	void Uninit(void);
	char *GetIP(void) { return &m_aClientIP[0]; }
	int GetPort(void) { return m_nClientPort; }

private:	// 自分だけアクセス可能

	// メンバ変数
	SOCKET m_sockServer; // サーバー用ソケット
	char m_aClientIP[64];	// IPアドレス
	int m_nClientPort;		// ポート番号
};

#endif