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
	const int MAX_CONNECT = 4;				// 最大接続数
	const int MAX_SEND_DATA = (1024);		// 最大データ量
	const double SEND_MS = 75.0f;			// 常に送るデータの送信フレーム

	// コマンド列挙
	enum COMMAND
	{
		COMMAND_NONE = 0,		// 何もしない
		COMMAND_JOIN,			// 接続した
		COMMAND_GETID,			// ID取得
		COMMAND_DELETE,			// 削除
		COMMAND_PL_POS,			// プレイヤー座標
		COMMAND_PL_DAMAGE,		// プレイヤ―ダメージ
		COMMAND_PL_GOAL,		// プレイヤーゴール
		COMMAND_GM_HIT,			// ギミックに衝突
		COMMAND_NEXT_GOAL,		// 次のゴール生成
		COMMAND_GAMESTART_OK,	// ゲーム準備完了
		COMMAND_GAMESTART,		// ゲーム開始
		COMMAND_TUTORIAL_OK,	// チュートリアル準備完了
		COMMAND_TUTORIAL_NO,	// チュートリアル準備完了取り消し
		COMMAND_TUTORIAL_END,	// エントリー終了
		COMMAND_SET_INSP,		// 検問設置
		COMMAND_END_INSP,		// 検問廃棄
		COMMAND_CAR_POS,		// 車座標
		COMMAND_PD_POS,			// 警察座標
		COMMAND_ADDPD_POS,		// 追加警察座標
		COMMAND_PD_CHASE,		// 警察追跡
		COMMAND_ADDPD_CHASE,	// 追加警察座標
		COMMAND_PD_CHASEEND,	// 警察追跡
		COMMAND_ADDPD_CHASEEND,	// 追加警察座標
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