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
	const int MAX_COMMAND_DATA = ((sizeof(int) + sizeof(int) + 1024));	// データのマックスサイズ
	const int MAX_SEND_DATA = ((sizeof(int) + sizeof(int) + 1024));	// 最大データ量
	const double SEND_MS = 50.0f;	// 常に送るデータの送信フレーム

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
		COMMAND_GM_HIT,			// ギミックに衝突
		COMMAND_NEXT_GOAL,		// 次のゴール生成
		COMMAND_MAX
	};

	// 時間測定用クラス
	class CTime
	{
		// データ
		clock_t start;
		clock_t end;

	public:

		// コンストラクタ
		CTime() : start(), end() {}

		// メンバ関数
		void Start() { start = clock(); }	// 開始
		void End() { end = clock(); }		// 終了
		bool IsOK()	// 指定時間経過したかどうか
		{
			const double time = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0;
			return time >= SEND_MS;
		}
	};
}

#endif