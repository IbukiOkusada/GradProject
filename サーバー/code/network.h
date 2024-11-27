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
public:

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
	void CommandPlRot(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandPlDamage(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);
	void CommandPlGoal(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte);

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
	int m_nSledCnt;			// スレッド数
	bool m_bEnd;			// 終了した
	CServer* m_pServer;		// サーバー

	// 静的メンバ変数
	static CNetWork* m_pInstance;	// インスタンス
};

#endif
