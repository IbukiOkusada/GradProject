//===============================================
//
// 通信処理の管理処理 [network.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "network.h"
#include "tcp_client.h"
#include "player.h"
#include "player_manager.h"

//===============================================
// 名前空間
//===============================================
namespace
{
	const int DEF_PORT = (22333);			// デフォルトのポート番号
	const std::string ADDRESS_FILENAME =	// アドレスのファイル名
		"data\\TXT\\address.txt";
	const int MAX_STRING = (2048);
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
};

// 静的メンバ変数
CNetWork* CNetWork::m_pInstance = nullptr;

//===============================================
// コンストラクタ
//===============================================
CNetWork::CNetWork()
{
	// 値のクリア
	m_pClient = nullptr;
	m_state = STATE::STATE_SINGLE;
	m_nMyIdx = -1;
	m_bEnd = true;
	m_nSledCnt = 0;

	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		m_aConnect[i] = false;
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
		m_pInstance->Init();
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
	// クライアントを作成
	m_pClient = DEBUG_NEW CClient;

	// 接続
	if (ReConnect())
	{
		m_state = STATE_ONLINE;
	}
	else
	{
		m_nMyIdx = 0;
		m_state = STATE_SINGLE;
	}

	return S_OK;
}


//===============================================
// 終了
//===============================================
void CNetWork::Uninit()
{
	m_bEnd = true;

	// クライアントの終了
	if (m_pClient != nullptr)
	{
		m_pClient->Uninit();
		delete m_pClient;
		m_pClient = nullptr;
	}

	delete this;
}

//===============================================
// アドレスファイル読み込み
//===============================================
void CNetWork::LoadAddressFile()
{
	// ファイルを開く
	std::ifstream File(ADDRESS_FILENAME);
	if (!File.is_open()) {
		return;
	}

	// コメント用
	std::string hoge;

	// データ読み込み
	std::string line;

	while (std::getline(File, line))
	{
		// コメントはスキップ
		if (line.empty() ||
			line[0] == '#')
		{
			continue;
		}

		// ストリーム作成
		std::istringstream lineStream(line);

		if (line.find("ADDLESS") != std::string::npos)
		{// TEXT_FILENAMEでモデル名読み込み

			// ストリーム作成
			std::istringstream lineStr(line);

			// 情報渡す
			lineStr >>
				hoge >>
				hoge >>		// ＝
				m_Address;	// アドレス

			continue;
		}

		if (line.find("END_SCRIPT") != std::string::npos)
		{
			break;
		}
	}

	// ファイルを閉じる
	File.close();
}

//===============================================
// 再接続
//===============================================
bool CNetWork::ReConnect()
{
	// 既に接続済み
	if (m_state == STATE::STATE_ONLINE) { return true; }

	m_nMyIdx = -1;

	// アドレスを再取得
	LoadAddressFile();

	// 生成
	if (m_pClient == nullptr)
	{
		m_pClient = DEBUG_NEW CClient;
	}

	// 接続失敗
	if (!m_pClient->Init(m_Address.c_str(), DEF_PORT)) 
	{
		m_pClient->Uninit();
		delete m_pClient;
		m_pClient = nullptr;
		m_state = STATE_SINGLE;
		m_nMyIdx = 0;
		return false; 
	}

	m_bEnd = false;

	// マルチスレッド
	std::thread th(&CNetWork::Online, this);
	th.detach();

	m_state = STATE_ONLINE;
	return true;
}

//===============================================
// 切断
//===============================================
bool CNetWork::DisConnect()
{
	// スレッドがなくなったら
	while (1)
	{
		OnlineEnd();
		if (m_nSledCnt <= 0)
		{
			break;
		}
	}

	// 終了処理
	m_pClient->Uninit();

	m_bEnd = true;

	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		m_aConnect[i] = false;
	}

	return true;
}

//===================================================
// オンライン通信
//===================================================
void CNetWork::Online(void)
{
	m_nSledCnt = 1;

	while (1)
	{
		if (m_bEnd == true || m_pClient == nullptr)
		{
			break;
		}

		char* pData = DEBUG_NEW char[MAX_STRING];	// 受信用

		// 受信
		int* pRecvByte = DEBUG_NEW int;
		*pRecvByte = m_pClient->Recv(&pData[0], MAX_STRING);

		// マルチスレッド
		std::thread th(&CNetWork::ByteCheck, this, pData, pRecvByte);
		th.detach();
	}

	m_nSledCnt--;
}

//===================================================
// 受信確認
//===================================================
void CNetWork::ByteCheck(char* pRecvData, int* pRecvByte)
{
	m_mutex.lock();
	m_nSledCnt++;
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	if (*pRecvByte <= 0)
	{
		// データ削除
		DeleteData(pRecvData, pRecvByte);

		m_nSledCnt--;
		m_mutex.unlock();
		return;
	}

	int nByte = 0;	// バイト数

	// 終端文字まで確認する
	while (nByte < *pRecvByte)
	{
		int id = -1;		// ID
		int command = NetWork::COMMAND_NONE;	// コマンド番号

		// ID取得
		memcpy(&id, &pRecvData[nByte], sizeof(int));
		nByte += sizeof(int);

		// コマンド取得
		memcpy(&command, &pRecvData[nByte], sizeof(int));
		nByte += sizeof(int);

		// コマンドがオーバーした
		if (command >= NetWork::COMMAND_MAX)
		{
			// データ削除
			DeleteData(pRecvData, pRecvByte);

			m_nSledCnt--;
			m_mutex.unlock();
			return;
		}

		// コマンドを実行
		(this->*(m_CommandFunc[command]))(&nByte, id, &pRecvData[nByte]);
	}

	// データ削除
	DeleteData(pRecvData, pRecvByte);

	m_nSledCnt--;
	m_mutex.unlock();
}

//===================================================
// データ削除
//===================================================
void CNetWork::DeleteData(char* pRecvData, int* pRecvByte)
{
	if (pRecvData != nullptr)
	{
		delete[] pRecvData;
	}

	if (pRecvByte != nullptr)
	{
		delete pRecvByte;
	}
}

//===================================================
// 終了送信
//===================================================
void CNetWork::OnlineEnd(void)
{
	if (m_pClient != nullptr)
	{
		std::string senddata;
		senddata.resize(sizeof(int) + 1);
		int nProt = NetWork::COMMAND_DELETE;

		// protocolを挿入
		memcpy(&senddata[0], &nProt, sizeof(int));

		// 送信
		m_pClient->Send(senddata.c_str(), sizeof(int));
	}
}

//===================================================
// 生成プレイヤー
//===================================================
CPlayer* CNetWork::CreatePlayer(int nId)
{
	// プレイヤー生成
	CPlayer* pPlayer = CPlayer::Create(VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, nullptr, nullptr);
	auto mgr = CPlayerManager::GetInstance();

	// リストに挿入しなおす
	mgr->ListOut(pPlayer);
	pPlayer->BindId(nId);
	mgr->ListIn(pPlayer);

	return pPlayer;
}

//===================================================
// 何もない
//===================================================
void CNetWork::CommandNone(int* pByte, const int nId, const char* pRecvData)
{

}

//===================================================
// 接続した
//===================================================
void CNetWork::CommandJoin(int* pByte, const int nId, const char* pRecvData)
{
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }

	m_aConnect[nId] = true;
}

//===================================================
// IDを取得した
//===================================================
void CNetWork::CommandGetId(int* pByte, const int nId, const char* pRecvData)
{
	m_nMyIdx = nId;
}

//===================================================
// 削除
//===================================================
void CNetWork::CommandDelete(int* pByte, const int nId, const char* pRecvData)
{
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }

	m_aConnect[nId] = false;
}
