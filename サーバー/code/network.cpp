//===============================================
//
// 通信処理の管理処理 [network.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "network.h"
#include "tcp_client.h"
#include "tcp_server.h"
#include "deltatime.h"

//===============================================
// 名前空間
//===============================================
namespace
{
	const int DEF_PORT = (22333);			// デフォルトのポート番号
	const float SEND_TIME = 0.0f;			// 送信受付時間
}

//===============================================
// 関数ポインタ
//===============================================
// 状態管理
CNetWork::COMMAND_FUNC CNetWork::m_CommandFunc[] =
{
	&CNetWork::CommandNone,				// 何もない
	&CNetWork::CommandJoin,				// 接続した
	&CNetWork::CommandGetId,			// ID取得
	&CNetWork::CommandDelete,			// 削除
	&CNetWork::CommandPlPos,			// プレイヤー位置
	&CNetWork::CommandPlDamage,			// プレイヤーダメ―ジ
	&CNetWork::CommandPlGoal,			// プレイヤーゴール
	&CNetWork::CommandGmHit,			// ギミック衝突
	&CNetWork::CommandNextGoal,			// 次のゴール
	&CNetWork::CommandGameStartOk,		// ゲーム開始可能になったよ
	&CNetWork::CommandGameStart,		// ゲーム開始
	&CNetWork::CommandTutorialOk,		// ゲーム開始可能になったよ
	&CNetWork::CommandTutorialEnd,		// ゲーム開始
	&CNetWork::CommandSetInspection,	// 検問配置
	&CNetWork::CommandEndInspection,	// 検問廃棄
	&CNetWork::CommandCarPos,			// 車位置
	&CNetWork::CommandPdPos,			// 警察位置
	&CNetWork::CommandAddPdPos,			// 追加警察位置
};

// 静的メンバ変数
CNetWork* CNetWork::m_pInstance = nullptr;

//===============================================
// コンストラクタ
//===============================================
CNetWork::CNetWork()
{
	// 値のクリア
	m_bEnd = false;
	m_nSendByte = 0;
	m_nSledCnt = 0;
	m_nConnectCnt = 0;
	m_pServer = nullptr;
	m_ServerFlag = SFlagInfo();

	memset(m_aSendData, NULL, sizeof(m_aSendData));

	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		m_apClient[i] = nullptr;
		m_aFlag[i] = SFlagInfo();
	}
}

//===============================================
// デストラクタ
//===============================================
CNetWork::~CNetWork()
{

}

//===============================================
// 生成
//===============================================
CNetWork* CNetWork::Create()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = DEBUG_NEW CNetWork;
		if (FAILED(m_pInstance->Init()))
		{
			Release();
		}
	}

	return m_pInstance;
}


//===============================================
// インスタンスを取得
//===============================================
CNetWork* CNetWork::GetInstance()
{
	return m_pInstance;
}


//===============================================
// 解放
//===============================================
void CNetWork::Release()
{
	if (m_pInstance != nullptr)
	{
		m_pInstance->Uninit();
		m_pInstance = nullptr;
	}
}

//===============================================
// 初期化
//===============================================
HRESULT CNetWork::Init()
{
	// Winsockの初期化処理
	WSADATA wsaData;
	int nErr = WSAStartup(WINSOCK_VERSION, &wsaData);	// winsockの初期化関数

	if (nErr != 0)
	{// 初期化に失敗した場合

		printf("*** Winsockの初期化に失敗しました ***\n");

		// キー入力待ち
		rewind(stdin);
		getchar();

		return E_FAIL;	// 終了
	}

	// サーバーの生成
	m_pServer = DEBUG_NEW CServer;

	// 初期化処理
	if (m_pServer->Init(DEF_PORT) == false)
	{
		return E_FAIL;
	}

	// 別スレッドで送信準備
	{
		std::thread th(&CNetWork::Send, this, &m_pServer);
		th.detach();
	}

	// 別スレッドで受け入れ待ち
	{
		std::thread th(&CNetWork::Accept, this, m_pServer);
		th.detach();
	}

	return S_OK;
}

//===============================================
// 終了
//===============================================
void CNetWork::Uninit()
{
	//Winsock終了処理
	WSACleanup();	// WSACleanup関数 : winsockの終了処理

	m_bEnd = true;

	while (1)
	{
		if (m_nSledCnt <= 0)
		{
			break;
		}
	}

	// サーバーの終了
	if (m_pServer != nullptr)
	{
		m_pServer->Uninit();
		delete m_pServer;
		m_pServer = nullptr;
	}

	// クライアントの終了
	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		if (m_apClient[i] == nullptr) { continue; }

		m_apClient[i]->Uninit();
		delete m_apClient[i];
		m_apClient[i] = nullptr;
	}

	delete this;
}

//==========================================================
// 受け入れ待ち
//==========================================================
void CNetWork::Accept(CServer* pServer)
{
	while (1)
	{
		for (int i = 0; i < NetWork::MAX_CONNECT; i++)
		{
			if (m_apClient[i] != nullptr) { continue; }

			// 送受信用生成
			CClient* pClient = new CClient;
			pClient->BindSock(pServer->Accept());
			pClient->SetCliePort(pServer->GetPort());
			pClient->SetIP(pServer->GetIP());

			// ID設定
			pClient->SetId(i);

			// マルチスレッド
			std::thread th(&CNetWork::Access, this, pClient);
			th.detach();

			// フラグ初期化
			m_aFlag[i] = SFlagInfo();

			m_apClient[i] = pClient;
			m_nConnectCnt++;
		}
	}
}

//==========================================================
// 送信
//==========================================================
void CNetWork::Send(CServer** ppServer)
{
	auto delta = CDeltaTime::GetInstance();
	
	// 計測用
	NetWork::CTime time;
	time.Start();

	// サーバーが閉じられるまで繰り返し
	while (*ppServer != nullptr)
	{	
		time.End();

		// 指定送信時間ごとに
		if(time.IsOK())
		{// 送信時間経過

			// 開始時間リセット
			time.Start();

			for(int i = 0; i < NetWork::MAX_CONNECT; i++)
			{// 使用されていない状態まで
				// データの合成
				CClient* pClient = m_apClient[i];	// 先頭を取得

				if (pClient == nullptr) { continue; }

				pClient->SetSend(true);	// 書き換え不可能な状態にする

				memcpy(&m_aSendData[m_nSendByte], pClient->GetSendData(), pClient->GetSendByte());
				m_nSendByte += pClient->GetSendByte();

				if (pClient->GetDeath() == false)
				{
					pClient->ResetData();		// データのリセット
					pClient->SetSend(false);	// 書き換え可能な状態にする
				}
				else
				{
					if (pClient != nullptr)
					{
						pClient->ResetData();		// データのリセット
						pClient->Uninit();
						delete pClient;
						pClient = nullptr;
						m_apClient[i] = nullptr;
						m_nConnectCnt--;
						printf("クライアント切断\n");
					}
				}
			}


			if (m_nSendByte > 0)
			{
				for (int i = 0; i < NetWork::MAX_CONNECT; i++)
				{// 使用されていない状態まで
					// データの合成
					CClient* pClient = m_apClient[i];	// 先頭を取得

					if (pClient == nullptr) { continue; }

					// 送信
					if (pClient->Send(&m_aSendData[0], m_nSendByte) <= 0)
					{
						m_nConnectCnt--;
						pClient->Uninit();
						delete pClient;
						pClient = nullptr;
						m_apClient[i] = nullptr;
						printf("クライアント切断\n");
					}
				}

				printf("%d バイト送ったよ\n", m_nSendByte);
			}

			// 送信データをクリア
			memset(&m_aSendData[0], '\0', sizeof(m_aSendData));
			m_nSendByte = 0;

			// フラグをリセット
			if (m_ServerFlag.bStart) { m_ServerFlag.bStart = false; }
			if (m_ServerFlag.bTutorial) { m_ServerFlag.bTutorial = false; }
		}
	}
}

//==========================================================
// 受信
//==========================================================
void CNetWork::Access(CClient* pClient)
{
	int id = pClient->GetId();

	char aSendData[sizeof(int) * 2] = {};	// 送信用
	int nowbyte = 0;

	// 設定
	CommandJoin(pClient->GetId(), nullptr, pClient, &nowbyte);

	while (1)
	{
		char recvdata[NetWork::MAX_COMMAND_DATA] = {};	// 受信用
		int command = NetWork::COMMAND_NONE;

		// 受信
		int recvbyte = pClient->Recv(&recvdata[0], NetWork::MAX_COMMAND_DATA);
		nowbyte = 0;

		if (recvbyte <= 0)
		{
			if (!pClient->GetDeath())
			{
				Leave(pClient->GetId(), pClient);
			}
			return;
		}

		while (1)
		{
			memcpy(&command, &recvdata[nowbyte], sizeof(int));
			nowbyte += sizeof(int);

			if (command > 0 && command <= NetWork::COMMAND::COMMAND_MAX)
			{

				// コマンドを実行
				(this->*(m_CommandFunc[command]))(id, &recvdata[nowbyte], pClient, &nowbyte);
			}

			if (command == NetWork::COMMAND_DELETE)
			{
				return;
			}

			if (nowbyte >= recvbyte)
			{
				break;
			}
		}
	}
}

//==========================================================
// 退室
//==========================================================
void CNetWork::Leave(int nId, CClient* pClient)
{
	int nProt = NetWork::COMMAND_DELETE;	// プロトコル番号
	char aSendData[sizeof(int) * 2] = {};	// 送信用まとめデータ
	int byte = 0;

	// IDを挿入
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコル挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// プロトコルを挿入
	pClient->SetData(&aSendData[0], byte);
	pClient->SetDeath(true);
}

//==========================================================
// 何もしない
//==========================================================
void CNetWork::CommandNone(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// プロトコル番号
	char aSendData[sizeof(int) * 2] = {};	// 送信用まとめデータ
	int byte = 0;

	// IDを挿入
	nProt = NetWork::COMMAND_NONE;
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコルを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// プロトコルを挿入
	pClient->SetData(&aSendData[0], byte);

	// IDを返す
	CommandGetId(nId, pRecvData, pClient, pNowByte);
}

//==========================================================
// 入室
//==========================================================
void CNetWork::CommandJoin(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// プロトコル番号
	char aSendData[sizeof(int) * 2] = {};	// 送信用まとめデータ
	int byte = 0;

	// IDを挿入
	nProt = NetWork::COMMAND_JOIN;

	// IDを挿入
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコルを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	pClient->SetData(&aSendData[0], byte);

	// IDを返す
	CommandGetId(nId, pRecvData, pClient, pNowByte);
}

//==========================================================
// IDを取得
//==========================================================
void CNetWork::CommandGetId(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	char aRecv[sizeof(int) * 2] = {};
	int command = NetWork::COMMAND_GETID;
	int byte = 0;

	// IDを挿入
	memcpy(&aRecv[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコルを挿入
	memcpy(&aRecv[byte], &command, sizeof(int));
	byte += sizeof(int);


	// プロトコルを送信
	pClient->Send(&aRecv[0], byte);
}

//==========================================================
// 削除
//==========================================================
void CNetWork::CommandDelete(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = NetWork::COMMAND_DELETE;	// プロトコル番号
	char aSendData[sizeof(int) * 2] = {};	// 送信用まとめデータ
	int byte = 0;

	// IDを挿入
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコルを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// データリセット
	pClient->ResetData();

	// 挿入
	pClient->SetData(&aSendData[0], sizeof(int) * 2);

	// 使用しない状態に
	pClient->SetDeath(true);
}

//==========================================================
// プレイヤーの座標
//==========================================================
void CNetWork::CommandPlPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// プロトコル番号
	char aSendData[sizeof(int) * 2 + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3)] = {};	// 送信用まとめデータ
	int byte = 0;

	nProt = NetWork::COMMAND_PL_POS;

	// IDを挿入
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコル挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// 座標挿入
	memcpy(&aSendData[byte], pRecvData, sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// 座標挿入
	memcpy(&aSendData[byte], &pRecvData[sizeof(D3DXVECTOR3)], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// 挿入
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// プレイヤーのダメージ後体力
//==========================================================
void CNetWork::CommandPlDamage(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// プロトコル番号
	char aSendData[sizeof(int) * 2 + sizeof(float)] = {};	// 送信用まとめデータ
	int byte = 0;

	nProt = NetWork::COMMAND_PL_DAMAGE;

	// IDを挿入
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコル挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// 体力挿入
	memcpy(&aSendData[byte], pRecvData, sizeof(float));
	*pNowByte += sizeof(float);
	byte += sizeof(float);

	// 挿入
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// プレイヤーをゴール
//==========================================================
void CNetWork::CommandPlGoal(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// プロトコル番号
	char aSendData[sizeof(int) * 2 + sizeof(int)] = {};	// 送信用まとめデータ
	int byte = 0;

	nProt = NetWork::COMMAND_PL_GOAL;

	// IDを挿入
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコル挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// ゴールID挿入
	memcpy(&aSendData[byte], pRecvData, sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);

	// 挿入
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// ギミックにヒット
//==========================================================
void CNetWork::CommandGmHit(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// プロトコル番号
	char aSendData[sizeof(int) * 2 + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(float)] = {};	// 送信用まとめデータ
	int byte = 0;
	int recvbyte = 0;

	nProt = NetWork::COMMAND_GM_HIT;

	// IDを挿入
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコル挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// ギミックID挿入
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);
	recvbyte += sizeof(int);

	// 衝突座標挿入
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);
	recvbyte += sizeof(D3DXVECTOR3);

	// 速度挿入
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(float));
	*pNowByte += sizeof(float);
	byte += sizeof(float);
	recvbyte += sizeof(float);

	// 挿入
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// IDを取得
//==========================================================
void CNetWork::CommandNextGoal(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	char aRecv[sizeof(int) * 2 + sizeof(int)] = {};
	int command = NetWork::COMMAND_NEXT_GOAL;
	int byte = 0;

	// IDを挿入
	memcpy(&aRecv[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコルを挿入
	memcpy(&aRecv[byte], &command, sizeof(int));
	byte += sizeof(int);

	// ゴールIDを挿入
	memcpy(&aRecv[byte], pRecvData, sizeof(int));
	byte += sizeof(int);
	*pNowByte += sizeof(int);


	// プロトコルを送信
	pClient->SetData(&aRecv[0], byte);
}

//==========================================================
// ゲーム開始準備完了
//==========================================================
void CNetWork::CommandGameStartOk(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	// フラグオン
	m_aFlag[nId].bStart = true;

	CommandGameStart(nId, pRecvData, pClient, pNowByte);
}

//==========================================================
// IDを取得
//==========================================================
void CNetWork::CommandGameStart(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	if (m_ServerFlag.bStart) { return; }

	int cnt = 0;

	// フラグ確認
	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		if (m_apClient[i] != nullptr && m_aFlag[i].bStart)
		{
			cnt++;
		}
	}

	// 総数より多い
	if (cnt >= m_nConnectCnt)
	{
		m_ServerFlag.bStart = true;

		// リセット
		for (int i = 0; i < NetWork::MAX_CONNECT; i++)
		{
			m_aFlag[i].bStart = false;
		}
	}
	else
	{
		return;
	}

	char aRecv[sizeof(int) * 2] = {};
	int command = NetWork::COMMAND_GAMESTART;
	int byte = 0;

	// IDを挿入
	memcpy(&aRecv[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコルを挿入
	memcpy(&aRecv[byte], &command, sizeof(int));
	byte += sizeof(int);

	// プロトコルを送信
	pClient->SetData(&aRecv[0], byte);
}

//==========================================================
// チュートリアルOK
//==========================================================
void CNetWork::CommandTutorialOk(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	// フラグオン
	m_aFlag[nId].bTutorial = true;

	CommandTutorialEnd(nId, pRecvData, pClient, pNowByte);
}

//==========================================================
// チュートリアル終了
//==========================================================
void CNetWork::CommandTutorialEnd(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	if (m_ServerFlag.bTutorial) { return; }

	int cnt = 0;

	// フラグ確認
	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		if (m_apClient[i] != nullptr && m_aFlag[i].bTutorial)
		{
			cnt++;
		}
	}

	// 総数より多い
	if (cnt >= m_nConnectCnt)
	{
		m_ServerFlag.bTutorial = true;

		// リセット
		for (int i = 0; i < NetWork::MAX_CONNECT; i++)
		{
			m_aFlag[i].bTutorial = false;
		}
	}
	else
	{
		return;
	}

	char aRecv[sizeof(int) * 2] = {};
	int command = NetWork::COMMAND_TUTORIAL_END;
	int byte = 0;

	// IDを挿入
	memcpy(&aRecv[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコルを挿入
	memcpy(&aRecv[byte], &command, sizeof(int));
	byte += sizeof(int);

	// プロトコルを送信
	pClient->SetData(&aRecv[0], byte);
}

//==========================================================
// 検問設置
//==========================================================
void CNetWork::CommandSetInspection(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	char aRecv[sizeof(int) * 2 + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3) + sizeof(int)] = {};
	int command = NetWork::COMMAND_SET_INSP;
	int byte = 0;
	int recvbyte = 0;

	// IDを挿入
	memcpy(&aRecv[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコルを挿入
	memcpy(&aRecv[byte], &command, sizeof(int));
	byte += sizeof(int);

	// 検問IDを挿入
	memcpy(&aRecv[byte], &pRecvData[recvbyte], sizeof(int));
	byte += sizeof(int);
	*pNowByte += sizeof(int);
	recvbyte += sizeof(int);

	// 座標挿入
	memcpy(&aRecv[byte], &pRecvData[recvbyte], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);
	recvbyte += sizeof(D3DXVECTOR3);

	// 向き挿入
	memcpy(&aRecv[byte], &pRecvData[recvbyte], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);
	recvbyte += sizeof(D3DXVECTOR3);

	// 道IDを挿入
	memcpy(&aRecv[byte], &pRecvData[recvbyte], sizeof(int));
	byte += sizeof(int);
	*pNowByte += sizeof(int);
	recvbyte += sizeof(int);

	// プロトコルを送信
	pClient->SetData(&aRecv[0], byte);
}

//==========================================================
// 検問廃棄
//==========================================================
void CNetWork::CommandEndInspection(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	char aRecv[sizeof(int) * 2 + sizeof(int)] = {};
	int command = NetWork::COMMAND_END_INSP;
	int byte = 0;

	// IDを挿入
	memcpy(&aRecv[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコルを挿入
	memcpy(&aRecv[byte], &command, sizeof(int));
	byte += sizeof(int);

	// 検問IDを挿入
	memcpy(&aRecv[byte], pRecvData, sizeof(int));
	byte += sizeof(int);
	*pNowByte += sizeof(int);

	// プロトコルを送信
	pClient->SetData(&aRecv[0], byte);
}

//==========================================================
// 車の座標
//==========================================================
void CNetWork::CommandCarPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// プロトコル番号
	char aSendData[sizeof(int) * 2 + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3)] = {};	// 送信用まとめデータ
	int byte = 0;
	int recvbyte = 0;

	nProt = NetWork::COMMAND_CAR_POS;

	// IDを挿入
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコルを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// 車のID挿入
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);
	recvbyte += sizeof(int);

	// 座標挿入
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);
	recvbyte += sizeof(D3DXVECTOR3);

	// 向き挿入
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);
	recvbyte += sizeof(D3DXVECTOR3);

	// 挿入
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// 警察の座標
//==========================================================
void CNetWork::CommandPdPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// プロトコル番号
	char aSendData[sizeof(int) * 2 + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3)] = {};	// 送信用まとめデータ
	int byte = 0;
	int recvbyte = 0;

	nProt = NetWork::COMMAND_PD_POS;

	// IDを挿入
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコルを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// 車のID挿入
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);
	recvbyte += sizeof(int);

	// 座標挿入
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);
	recvbyte += sizeof(D3DXVECTOR3);

	// 向き挿入
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);
	recvbyte += sizeof(D3DXVECTOR3);

	// 挿入
	pClient->SetData(&aSendData[0], byte);
}

//==========================================================
// 追加警察の座標
//==========================================================
void CNetWork::CommandAddPdPos(const int nId, const char* pRecvData, CClient* pClient, int* pNowByte)
{
	int nProt = -1;	// プロトコル番号
	char aSendData[sizeof(int) * 2 + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3)] = {};	// 送信用まとめデータ
	int byte = 0;
	int recvbyte = 0;

	nProt = NetWork::COMMAND_ADDPD_POS;

	// IDを挿入
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// プロトコルを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// 車のID挿入
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(int));
	*pNowByte += sizeof(int);
	byte += sizeof(int);
	recvbyte += sizeof(int);

	// 座標挿入
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);
	recvbyte += sizeof(D3DXVECTOR3);

	// 向き挿入
	memcpy(&aSendData[byte], &pRecvData[recvbyte], sizeof(D3DXVECTOR3));
	*pNowByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);
	recvbyte += sizeof(D3DXVECTOR3);

	// 挿入
	pClient->SetData(&aSendData[0], byte);
}