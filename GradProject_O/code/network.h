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

	// 送信関数
	void SendNone();
	void SendJoin();
	void SendGetId();
	void SendDelete();
	void SendPlPos(const D3DXVECTOR3& pos);
	void SendPlRot(const D3DXVECTOR3& rot);
	void SendPlDamage(const float nowlife);
	void SendPlGoal(int nId);

	// メンバ関数(設定)

	// メンバ関数(取得)
	int GetIdx() { return m_nMyIdx; }
	bool GetConnect(int nIdx = 0) { return m_aConnect[nIdx]; }

private:

	//=============================
	// 関数リスト
	//=============================
	typedef void(CNetWork::* RECV_FUNC)(int*, const int, const char*);
	static RECV_FUNC m_RecvFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 受信関数
	void RecvNone(int* pByte, const int nId, const char* pRecvData);
	void RecvJoin(int* pByte, const int nId, const char* pRecvData);
	void RecvGetId(int* pByte, const int nId, const char* pRecvData);
	void RecvDelete(int* pByte, const int nId, const char* pRecvData);
	void RecvPlPos(int* pByte, const int nId, const char* pRecvData);
	void RecvPlRot(int* pByte, const int nId, const char* pRecvData);
	void RecvPlDamage(int* pByte, const int nId, const char* pRecvData);
	void RecvPlGoal(int* pByte, const int nId, const char* pRecvData);

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
	bool GetActive();

	// メンバ変数
	STATE m_state;			// 状態
	int m_nMyIdx;			// 自分自身のID
	CClient* m_pClient;		// クライアント
	std::string m_Address;	// 接続先のアドレス
	int m_nSledCnt;			// スレッド数
	bool m_bEnd;			// 終了した
	mutex m_aPlayerMutex[NetWork::MAX_CONNECT] = {};			// ミューテックス
	mutex m_mutex;
	bool m_aConnect[NetWork::MAX_CONNECT];

	// 静的メンバ変数
	static CNetWork* m_pInstance;	// インスタンス
};

#endif
