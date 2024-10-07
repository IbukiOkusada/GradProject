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
enum COMMAND_TYPE
{
	COMMAND_TYPE_NONE = 0,			// 何もしない
	COMMAND_TYPE_SETPOSITION,		// 座標設定
	COMMAND_TYPE_SETROTATION,		// 向き設定
	COMMAND_TYPE_SETLIFE,			// 体力設定
	COMMAND_TYPE_SETCHECKPOINT,	// チェックポイント通過
	COMMAND_TYPE_GOAL,			// ゴール
	COMMAND_TYPE_START_OK,			// 開始準備完了
	COMMAND_TYPE_CREATE,			// 生成
	COMMAND_TYPE_DAMAGE,			// ダメージ
	COMMAND_TYPE_DELETE,			// 消滅
	COMMAND_TYPE_GETID,			// ID取得
	COMMAND_TYPE_MAX
};

#endif