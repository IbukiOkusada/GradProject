//===============================================
//
// 通信処理の管理ヘッダー [network.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "protocol_online.h"

// 前方宣言
class CClient;
class CServer;

//===============================================
// ネットワーククラスの定義
//===============================================
class CNetWork
{
private:

	// フラグまとめ
	struct SFlagInfo
	{
		bool bStart;	// 開始
		bool bTutorial;	// チュートリアル

		// コンストラクタ
		SFlagInfo() : bStart(false), bTutorial(false) {}
	};

private:

	CNetWork();		// コンストラクタ
	~CNetWork();	// デストラクタ

public:

	// インスタンス管理関数
	static CNetWork* Create();
	static CNetWork* GetInstance();
	static void Release();

private:

	//=============================
	// 関数リスト
	//=============================
	typedef void(CNetWork::* COMMAND_FUNC)(const int, const char*, CClient*, int*);
	static COMMAND_FUNC m_CommandFunc[];

	//=============================
	// メンバ関数
	//=============================
	// コマンド関数
	void CommandNone(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandJoin(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandGetId(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandDelete(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandPlPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandPlDamage(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandPlGoal(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandGmHit(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandNextGoal(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandGameStartOk(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandGameStart(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandTutorialOk(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandTutorialEnd(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandSetInspection(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandEndInspection(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandCarPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandPdPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandAddPdPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandPdChase(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandAddPdChase(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandPdChaseEnd(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandAddPdChaseEnd(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);

	// メンバ関数
	HRESULT Init();
	void Uninit();

	// TCP通信用関数
	void Access(CClient* pClient);
	void Accept(CServer* pServer);
	void Send(CServer** ppServer);
	void Leave(int nId, CClient* pClient);

	// メンバ変数
	CClient* m_apClient[NetWork::MAX_CONNECT];	// クライアント
	char m_aSendData[NetWork::MAX_SEND_DATA] = {};	// 送信データ
	int m_nSendByte;	// 送信データ量
	int m_nSledCnt;		// スレッド数
	bool m_bEnd;		// 終了した
	CServer* m_pServer;	// サーバー
	int m_nConnectCnt;	// 接続数
	bool m_bStart;		// 開始フラグ

	// フラグ関連
	SFlagInfo m_aFlag[NetWork::MAX_CONNECT];	// クライアントそれぞれのフラグ
	SFlagInfo m_ServerFlag;	// サーバーのフラグ

	// 静的メンバ変数
	static CNetWork* m_pInstance;	// インスタンス
};

#endif
