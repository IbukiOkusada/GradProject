//==========================================================
//
// オンライン送受信プロトコル [protocol_online.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PROTOCOL_ONLINE_H_
#define _PROTOCOL_ONLINE_H_

//**********************************************************
// 送受信用プロトコル列挙を定義
//**********************************************************
namespace NetWork
{
	const int MAX_CONNECT = 4;	// 最大接続数
	const int MAX_SEND_DATA = 4096;	// 最大データ量

	// コマンド列挙
	enum COMMAND
	{
		COMMAND_NONE = 0,		// 何もしない
		COMMAND_JOIN,			// 接続した
		COMMAND_GETID,			// ID取得
		COMMAND_DELETE,			// 削除
		COMMAND_PL_POS,			// プレイヤー座標
		COMMAND_PL_ROT,			// プレイヤーの向き
		COMMAND_PL_DAMAGE,		// プレイヤ―ダメージ
		COMMAND_PL_GOAL,		// プレイヤーゴール
		COMMAND_MAX
	};
}

#endif