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
	const float SEND_TIME = 1.0f;			// 送信時間
}

//===============================================
// 関数ポインタ
//===============================================
// 状態管理
CNetWork::COMMAND_FUNC CNetWork::m_CommandFunc[] =
{
	&CNetWork::CommandNone,		// 何もない
	&CNetWork::CommandJoin,		// 接続した
	&CNetWork::CommandGetId,	// ID取得
	&CNetWork::CommandDelete,	// 削除
	&CNetWork::CommandPlPos,	// プレイヤー位置
	&CNetWork::CommandPlRot,	// プレイヤー向き
	&CNetWork::CommandPlDamage,	// プレイヤーダメ―ジ
	&CNetWork::CommandPlGoal,	// プレイヤーゴール
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
	m_pServer = nullptr;

	memset(m_aSendData, NULL, sizeof(m_aSendData));

	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		m_apClient[i] = nullptr;
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

			m_apClient[i] = pClient;
		}
	}
}

//==========================================================
// 送信
//==========================================================
void CNetWork::Send(CServer** ppServer)
{
	auto delta = CDeltaTime::GetInstance();
	float time = 0.0f;
	// サーバーが閉じられるまで繰り返し
	while (*ppServer != nullptr)
	{
		time += delta->GetDeltaTime();

		if (time >= SEND_TIME)
		{// 送信時間経過
			time = 0.0f;

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
					pClient->ResetData();	// データのリセット
					pClient->SetSend(false);	// 書き換え可能な状態にする
				}
				else
				{
					if (pClient != nullptr)
					{
						pClient->Uninit();
						delete pClient;
						pClient = nullptr;
						m_apClient[i] = nullptr;
					}
				}
			}

			for (int i = 0; i < NetWork::MAX_CONNECT; i++)
			{// 使用されていない状態まで
				// データの合成
				CClient* pClient = m_apClient[i];	// 先頭を取得

				if (pClient == nullptr) { continue; }

				// 送信
				pClient->Send(&m_aSendData[0], m_nSendByte);
			}

			// 送信データをクリア
			memset(&m_aSendData[0], '\0', sizeof(m_aSendData));
			m_nSendByte = 0;

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

	// 設定
	CommandJoin(pClient->GetId(), nullptr, pClient);

	while (1)
	{
		D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		char recvdata[NetWork::MAX_COMMAND_DATA] = {};	// 受信用
		int command = NetWork::COMMAND_NONE;

		// 受信
		int recvbite = pClient->Recv(&recvdata[0], NetWork::MAX_COMMAND_DATA);

		memcpy(&command, &recvdata[0], sizeof(int));

		if (recvbite <= 0)
		{
			Leave(pClient->GetId(), pClient);
			break;
		}
		else
		{
			if (pClient->GetSend() == true)
			{
				continue;
			}

			// コマンドを実行
			(this->*(m_CommandFunc[command]))(id, &recvdata[sizeof(int)], pClient);
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

	// IDを挿入
	memcpy(&aSendData[0], &nId, sizeof(int));
	memcpy(&aSendData[sizeof(int)], &nProt, sizeof(int));

	// プロトコルを挿入
	pClient->ResetData();
	pClient->SetData(&aSendData[0], sizeof(int) * 2);
	pClient->SetDeath(true);
}

//==========================================================
// 何もしない
//==========================================================
void CNetWork::CommandNone(const int nId, const char* pRecvData, CClient* pClient)
{

}

//==========================================================
// 入室
//==========================================================
void CNetWork::CommandJoin(const int nId, const char* pRecvData, CClient* pClient)
{
	int nProt = -1;	// プロトコル番号
	char aSendData[sizeof(int) * 2] = {};	// 送信用まとめデータ

	// IDを挿入
	nProt = NetWork::COMMAND_JOIN;

	// IDを挿入
	memcpy(&aSendData[0], &nId, sizeof(int));
	memcpy(&aSendData[sizeof(int)], &nProt, sizeof(int));

	// プロトコルを挿入
	pClient->SetData(&aSendData[0], sizeof(int) * 2);

	// IDを返す
	CommandGetId(nId, pRecvData, pClient);
}

//==========================================================
// IDを取得
//==========================================================
void CNetWork::CommandGetId(const int nId, const char* pRecvData, CClient* pClient)
{
	char aRecv[sizeof(int) * 2] = {};
	int command = NetWork::COMMAND_GETID;

	// IDを挿入
	memcpy(&aRecv[0], &nId, sizeof(int));
	memcpy(&aRecv[sizeof(int)], &command, sizeof(int));

	// プロトコルを送信
	pClient->Send(&aRecv[0], sizeof(int) * 2);
}

//==========================================================
// 削除
//==========================================================
void CNetWork::CommandDelete(const int nId, const char* pRecvData, CClient* pClient)
{
	int nProt = NetWork::COMMAND_DELETE;	// プロトコル番号
	char aSendData[sizeof(int) * 2] = {};	// 送信用まとめデータ

	// IDを挿入
	memcpy(&aSendData[0], &nId, sizeof(int));
	memcpy(&aSendData[sizeof(int)], &nProt, sizeof(int));

	// プロトコルを挿入
	pClient->ResetData();
	pClient->SetData(&aSendData[0], sizeof(int) * 2);
	pClient->SetDeath(true);
}

//==========================================================
// プレイヤーの座標
//==========================================================
void CNetWork::CommandPlPos(const int nId, const char* pRecvData, CClient* pClient)
{
	int nProt = -1;	// プロトコル番号
	char aSendData[sizeof(int) * 2 + sizeof(D3DXVECTOR3) + 1] = {};	// 送信用まとめデータ

	D3DXVECTOR3 pos;

	// 座標挿入
	memcpy(&pos, pRecvData, sizeof(D3DXVECTOR3));

	// IDを挿入
	nProt = NetWork::COMMAND_PL_POS;

	// IDを挿入
	memcpy(&aSendData[0], &nId, sizeof(int));

	// コマンド挿入
	memcpy(&aSendData[sizeof(int)], &nProt, sizeof(int));

	// 座標挿入
	memcpy(&aSendData[sizeof(int) * 2], &pos, sizeof(D3DXVECTOR3));

	// プロトコルを挿入
	pClient->SetData(&aSendData[0], sizeof(int) * 2 + sizeof(D3DXVECTOR3));
}

//==========================================================
// プレイヤーの向き
//==========================================================
void CNetWork::CommandPlRot(const int nId, const char* pRecvData, CClient* pClient)
{
	int nProt = -1;	// プロトコル番号
	char aSendData[sizeof(int) * 2 + sizeof(D3DXVECTOR3) + 1] = {};	// 送信用まとめデータ

	// IDを挿入
	nProt = NetWork::COMMAND_PL_ROT;

	D3DXVECTOR3 rot;

	// 座標挿入
	memcpy(&rot, pRecvData, sizeof(D3DXVECTOR3));

	// IDを挿入
	memcpy(&aSendData[0], &nId, sizeof(int));

	// コマンド挿入
	memcpy(&aSendData[sizeof(int)], &nProt, sizeof(int));

	// 座標挿入
	memcpy(&aSendData[sizeof(int) * 2], &rot, sizeof(D3DXVECTOR3));

	// プロトコルを挿入
	pClient->SetData(&aSendData[0], sizeof(int) * 2 + sizeof(D3DXVECTOR3));

}

//==========================================================
// プレイヤーのダメージ後体力
//==========================================================
void CNetWork::CommandPlDamage(const int nId, const char* pRecvData, CClient* pClient)
{

}

//==========================================================
// プレイヤーをゴール
//==========================================================
void CNetWork::CommandPlGoal(const int nId, const char* pRecvData, CClient* pClient)
{
	int nProt = -1;	// プロトコル番号
	char aSendData[sizeof(int) * 2 + sizeof(int) + 1] = {};	// 送信用まとめデータ

	// IDを挿入
	nProt = NetWork::COMMAND_PL_GOAL;

	// IDを挿入
	memcpy(&aSendData[0], &nId, sizeof(int));

	// コマンド挿入
	memcpy(&aSendData[sizeof(int)], &nProt, sizeof(int));

	// ゴールID挿入
	memcpy(&aSendData[sizeof(int) * 2], pRecvData, sizeof(int));

	// プロトコルを挿入
	pClient->SetData(&aSendData[0], sizeof(int) * 2 + sizeof(int));
}