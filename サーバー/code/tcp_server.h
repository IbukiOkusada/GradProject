//==========================================================
//
// winsock通信(サーバー) [tcp_server.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _TCP_SERVER_H_	// このマクロが定義されていない場合
#define _TCP_SERVER_H_	// 二重インクルード防止のマクロを定義

#include "tcp_listener.h"

//==========================================================
// サーバークラスの定義
//==========================================================
class CServer : public CListener
{
public:	// 誰でもアクセス可能

	CServer();	// コンストラクタ
	~CServer();	// デストラクタ

	// メンバ関数
	bool Init(int nPortNum);
	void Uninit(void);

private:	// 自分だけアクセス可能

};

#endif