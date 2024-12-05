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
class CRoad;

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
	void Update();

	// 送信関数
	void SendNone();
	void SendJoin();
	void SendGetId();
	void SendDelete();
	void SendPlPos(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);
	void SendPlDamage(const float nowlife);
	void SendPlGoal(int nId);
	void SendGmHit(const int nId, const D3DXVECTOR3& HitPos, const float fSpeed);
	void SendNextGoal(const int nId);
	void SendGameStartOk();
	void SendTutorialOk();
	void SendSetInspection(const int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, int nIdx, int nStartPdId);
	void SendEndInspection(int nId);
	void SendCarPos(int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);
	void SendPdPos(int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);
	void SendAddPdPos(int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);
	void SendPdChase(int nId, int plyid);
	void SendAddPdChase(int nId, int plyid);
	void SendPdChaseEnd(int nId);
	void SendAddPdChaseEnd(int nId);

	// メンバ関数(設定)

	// メンバ関数(取得)
	int GetIdx() { return m_nMyIdx; }
	bool GetConnect(int nIdx = 0) { return m_aConnect[nIdx]; }
	NetWork::CTime* GetTime() { return &m_SendTime; }

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
	void RecvPlDamage(int* pByte, const int nId, const char* pRecvData);
	void RecvPlGoal(int* pByte, const int nId, const char* pRecvData);
	void RecvGmHit(int* pByte, const int nId, const char* pRecvData);
	void RecvNextGoal(int* pByte, const int nId, const char* pRecvData);
	void RecvGameStartOk(int* pByte, const int nId, const char* pRecvData);
	void RecvGameStart(int* pByte, const int nId, const char* pRecvData);
	void RecvTutorialOk(int* pByte, const int nId, const char* pRecvData);
	void RecvTutorialEnd(int* pByte, const int nId, const char* pRecvData);
	void RecvSetInspection(int* pByte, const int nId, const char* pRecvData);
	void RecvEndInspection(int* pByte, const int nId, const char* pRecvData);
	void RecvCarPos(int* pByte, const int nId, const char* pRecvData);
	void RecvPdPos(int* pByte, const int nId, const char* pRecvData);
	void RecvAddPdPos(int* pByte, const int nId, const char* pRecvData);
	void RecvPdChase(int* pByte, const int nId, const char* pRecvData);
	void RecvAddPdChase(int* pByte, const int nId, const char* pRecvData);
	void RecvPdChaseEnd(int* pByte, const int nId, const char* pRecvData);
	void RecvAddPdChaseEnd(int* pByte, const int nId, const char* pRecvData);

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
	NetWork::CTime m_SendTime;

	// 静的メンバ変数
	static CNetWork* m_pInstance;	// インスタンス
};

#endif
