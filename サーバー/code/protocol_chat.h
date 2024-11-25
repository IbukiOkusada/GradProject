//==========================================================
//
// チャット送受信プロトコル [protocol_chat.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PROTOCOL_CHAT_H_
#define _PROTOCOL_CHAT_H_

// マクロ定義

//**********************************************************
// 送受信用プロトコル列挙を定義
//**********************************************************
enum COMMAND_TYPE
{
	COMMAND_TYPE_NONE = 0,	// 何もしない
	COMMAND_TYPE_CHAT,		// チャット
	COMMAND_TYPE_JOIN,		// 入室
	COMMAND_TYPE_SETNAME,	// 名前設定
	COMMAND_TYPE_LEAVE,		// 退室
	COMMAND_TYPE_MAX
};

#endif
