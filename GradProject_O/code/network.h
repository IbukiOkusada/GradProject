//===============================================
//
// 通信処理の管理ヘッダー [game.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "protocol_online.h"

// 前方宣言
class CClient;
class CPlayer;

//===============================================
// ネットワーククラスの定義
//===============================================
class CNetWork
{
public:

	enum STATE
	{
		STATE_SINGLE = 0,	// 通信なし
		STATE_ONLINE,		// オンライン
		STATE_MAX
	};

private:

	CNetWork();		// コンストラクタ
	~CNetWork();	// デストラクタ

public:

	// インスタンス管理関数
	static CNetWork* Create();
	static CNetWork* GetInstance();
	static void Release();

	// メンバ関数
	bool ReConnect();
	bool DisConnect();
	STATE GetState() { return m_state; }

	// メンバ関数(設定)

	// メンバ関数(取得)
	int GetIdx() { return m_nMyIdx; }

private:

	//=============================
	// 関数リスト
	//=============================
	typedef void(CNetWork::* COMMAND_FUNC)(int*, const int, const char*);
	static COMMAND_FUNC m_CommandFunc[];

	//=============================
	// メンバ関数
	//=============================
	// コマンド関数
	void CommandNone(int* pByte, const int nId, const char* pRecvData);
	void CommandJoin(int* pByte, const int nId, const char* pRecvData);
	void CommandGetId(int* pByte, const int nId, const char* pRecvData);
	void CommandDelete(int* pByte, const int nId, const char* pRecvData);

	// メンバ関数
	HRESULT Init();
	void Uninit();
	void LoadAddressFile();
	CPlayer* CreatePlayer(int nId);

	// TCP通信用関数
	void Online(void);
	void ByteCheck(char* pRecvData, int* pRecvByte);
	void DeleteData(char* pRecvData, int* pRecvByte);
	void OnlineEnd(void);

	// メンバ変数
	STATE m_state;			// 状態
	int m_nMyIdx;			// 自分自身のID
	CClient* m_pClient;		// クライアント
	std::string m_Address;	// 接続先のアドレス
	int m_nSledCnt;			// スレッド数
	bool m_bEnd;			// 終了した
	mutex m_mutex;			// ミューテックス
	bool m_aConnect[NetWork::MAX_CONNECT];

	// 静的メンバ変数
	static CNetWork* m_pInstance;	// インスタンス
};

#endif
