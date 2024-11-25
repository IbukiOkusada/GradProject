//==========================================================
//
// オンライン対戦送受信プロトコル [protocol_online.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PROTOCOL_RANKING_H_
#define _PROTOCOL_RANKING_H_

// マクロ定義

//**********************************************************
// 送受信用プロトコル列挙を定義
//**********************************************************
namespace NetWork
{
	const int MAX_CONNECT = 4;	// 最大接続数

	// コマンド列挙
	enum COMMAND
	{
		COMMAND_NONE = 0,		// 何もしない
		COMMAND_JOIN,			// 接続した
		COMMAND_GETID,			// ID取得
		COMMAND_DELETE,			// 削除
		COMMAND_MAX
	};
}

#endif