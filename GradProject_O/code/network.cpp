//===============================================
//
// 通信処理の管理処理 [network.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "network.h"
#include "tcp_client.h"

// データ受け渡し用
#include "player_manager.h"
#include "player.h"
#include "goal.h"
#include "gimmick.h"
#include "debugproc.h"
#include "goal_manager.h"
#include "manager.h"
#include "fade.h"
#include "inspection_manager.h"
#include "inspection.h"
#include "road_manager.h"
#include "road.h"
#include "car_manager.h"
#include "car.h"
#include "police_manager.h"
#include "police.h"
#include "add_police.h"
#include "police_AI.h"
#include "manager.h"

//===============================================
// 名前空間
//===============================================
namespace
{
	const int DEF_PORT = (22333);			// デフォルトのポート番号
	const std::string ADDRESS_FILENAME =	// アドレスのファイル名
		"data\\TXT\\address.txt";
}

//===============================================
// 関数ポインタ
//===============================================
// 状態管理
CNetWork::RECV_FUNC CNetWork::m_RecvFunc[] =
{
	&CNetWork::RecvNone,	// 何もない
	&CNetWork::RecvJoin,	// 接続した
	&CNetWork::RecvGetId,	// ID取得
	&CNetWork::RecvDelete,	// 削除
	&CNetWork::RecvPlPos,	// プレイヤー座標
	&CNetWork::RecvPlDamage,	// プレイヤーダメージ
	&CNetWork::RecvPlGoal,	// プレイヤーゴール
	&CNetWork::RecvGmHit,	// ギミックヒット
	&CNetWork::RecvNextGoal,	// 次のゴール設置
	&CNetWork::RecvGameStartOk,	// ゲーム開始準備完了
	&CNetWork::RecvGameStart,	// ゲーム開始
	&CNetWork::RecvTutorialOk,	// チュートリアル終了可能
	&CNetWork::RecvTutorialNo,	// チュートリアル終了可能
	&CNetWork::RecvTutorialEnd,	// チュートリアル終了
	&CNetWork::RecvSetInspection,	// チュートリアル終了可能
	&CNetWork::RecvEndInspection,	// チュートリアル終了
	&CNetWork::RecvCarPos,		// 車座標
	&CNetWork::RecvPdPos,		// 警察座標
	&CNetWork::RecvAddPdPos,	// 追加座標
	&CNetWork::RecvPdChase,		// 警察座標
	&CNetWork::RecvAddPdChase,	// 追加座標
	&CNetWork::RecvPdChaseEnd,	// 警察座標
	&CNetWork::RecvAddPdChaseEnd,	// 追加座標
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
	
	m_nMyIdx = -1;
	m_state = STATE_SINGLE;

	return S_OK;
}


//===============================================
// 終了
//===============================================
void CNetWork::Uninit()
{
	m_bEnd = true;

	// 切断
	DisConnect();

	// クライアントの終了
	if (m_pClient != nullptr)
	{
		m_pClient->Uninit();
		delete m_pClient;
		m_pClient = nullptr;
	}

	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		m_aConnect[i] = false;
	}

	m_nMyIdx = -1;

	delete this;
}

//===============================================
// 終了
//===============================================
void CNetWork::Update()
{
	if (m_SendTime.IsOK())
	{
		m_SendTime.Start();

		if (m_pClient == nullptr) { return; }
		m_pClient->Send();
	}

	if (m_pClient == nullptr) { return; }
	CDebugProc::GetInstance()->Print("現在のバイト数 [ %d ]\n", m_pClient->GetByte());
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
	m_state = STATE_MAX;

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
		m_nMyIdx = -1;
		m_bEnd = true;
		return false; 
	}

	m_bEnd = false;

	// マルチスレッド
	std::thread th(&CNetWork::Online, this);
	th.detach();

	SendGetId();

	m_state = STATE_ONLINE;
	return true;
}

//===============================================
// 切断
//===============================================
bool CNetWork::DisConnect()
{
	m_bEnd = true;

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
	if (m_pClient != nullptr)
	{
		m_pClient->Uninit();
		delete m_pClient;
		m_pClient = nullptr;
	}

	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		m_aConnect[i] = false;
	}

	m_nMyIdx = -1;
	m_state = STATE::STATE_SINGLE;

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
		if (!GetActive())
		{
			break;
		}

		char* pData = DEBUG_NEW char[NetWork::MAX_SEND_DATA];	// 受信用

		// 受信
		int* pRecvByte = DEBUG_NEW int;
		*pRecvByte = m_pClient->Recv(pData, NetWork::MAX_SEND_DATA);

		if (*pRecvByte > 0)
		{
			// マルチスレッド
#if 0
			std::thread th(&CNetWork::ByteCheck, this, pData, pRecvByte);
			th.detach();
#else
			ByteCheck(pData, pRecvByte);
#endif
		}
		else
		{
			DeleteData(pData, pRecvByte);
			break;
		}
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
	CFade* pFade = CManager::GetInstance()->GetFade();

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
	while (1)
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
		if (command >= NetWork::COMMAND_MAX || command < NetWork::COMMAND_NONE)
		{
			// データ削除
			DeleteData(pRecvData, pRecvByte);

			m_nSledCnt--;
			m_mutex.unlock();
			return;
		}

		// コマンドを実行
		(this->*(m_RecvFunc[command]))(&nByte, id, &pRecvData[nByte]);

		// 全て見た
		if (nByte >= *pRecvByte)
		{
			break;
		}
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
		m_pClient->SetData(senddata.c_str(), sizeof(int));
		m_pClient->Send();
	}
}

//===================================================
// 使用可能確認
//===================================================
bool CNetWork::GetActive()
{
	if (m_bEnd || m_pClient == nullptr)
	{
		return false;
	}

	return true;
}

//===================================================
// 生成プレイヤー
//===================================================
CPlayer* CNetWork::CreatePlayer(int nId)
{
	m_aPlayerMutex[nId].lock();

	// プレイヤー生成
	CPlayer* pPlayer = CPlayer::Create(VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, nId);

	// 受信用プレイヤーに
	pPlayer->SetType(CPlayer::TYPE::TYPE_RECV);

	m_aPlayerMutex[nId].unlock();

	return pPlayer;
}

//===================================================
// 何もない
//===================================================
void CNetWork::RecvNone(int* pByte, const int nId, const char* pRecvData)
{
	
}

//===================================================
// 接続した
//===================================================
void CNetWork::RecvJoin(int* pByte, const int nId, const char* pRecvData)
{
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }
	if (m_nMyIdx < 0) { return; }

	// 接続されたことにする
	m_aConnect[nId] = true;
}

//===================================================
// IDを取得した
//===================================================
void CNetWork::RecvGetId(int* pByte, const int nId, const char* pRecvData)
{
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }

	// 接続されたことにする
	m_aConnect[nId] = true;
	m_nMyIdx = nId;
}

//===================================================
// 削除
//===================================================
void CNetWork::RecvDelete(int* pByte, const int nId, const char* pRecvData)
{
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }

	m_aConnect[nId] = false;
}

//===================================================
// プレイヤーの座標を取得
//===================================================
void CNetWork::RecvPlPos(int* pByte, const int nId, const char* pRecvData)
{
	if (m_pClient == nullptr) { return; }
	// 座標に変換
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	memcpy(&pos, pRecvData, sizeof(D3DXVECTOR3));

	// 確認バイト数を加算
	*pByte += sizeof(D3DXVECTOR3);

	// 座標に変換
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	memcpy(&rot, &pRecvData[sizeof(D3DXVECTOR3)], sizeof(D3DXVECTOR3));

	// 確認バイト数を加算
	*pByte += sizeof(D3DXVECTOR3);

	// プレイヤーの存在確認
	if (nId < 0 || nId >= NetWork::MAX_CONNECT || nId == m_nMyIdx) { return; }
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(nId);

	// 生成していない場合
	if (pPlayer == nullptr) { 
		RecvJoin(pByte, nId, pRecvData);
		return; 
	}

	// 座標設定
	pPlayer->SetRecvPosition(pos);
	pPlayer->SetRecvRotation(rot);
}

//===================================================
// プレイヤーのダメージを取得
//===================================================
void CNetWork::RecvPlDamage(int* pByte, const int nId, const char* pRecvData)
{
	// 体力に変換
	float life = 0.0f;
	memcpy(&life, pRecvData, sizeof(float));

	// 確認バイト数を加算
	*pByte += sizeof(float);

	// プレイヤーの存在確認
	if (nId < 0 || nId >= NetWork::MAX_CONNECT || nId == m_nMyIdx) { return; }
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(nId);

	// 生成していない場合
	if (pPlayer == nullptr) {
		RecvJoin(pByte, nId, pRecvData);
		return;
	}

	// 座標設定
	pPlayer->Damage(life - pPlayer->GetLife());
}

//===================================================
// プレイヤーのゴールを取得
//===================================================
void CNetWork::RecvPlGoal(int* pByte, const int nId, const char* pRecvData)
{
	// ゴールのIdを得る
	int goalid = -1;
	memcpy(&goalid, pRecvData, sizeof(int));
	*pByte += 4;

	CGoal* pGoal = CGoalManager::GetInstance()->GetGoal(goalid);

	if (pGoal == nullptr) {
		return;
	}

	pGoal->RecvEnd(nId);
}

//===================================================
// ギミックのヒットを取得
//===================================================
void CNetWork::RecvGmHit(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// ギミックのIdを得る
	int gmid = -1;
	memcpy(&gmid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// 衝突位置を取得
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	memcpy(&pos, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// 速度を取得
	float speed = 0.0f;
	memcpy(&speed, &pRecvData[byte], sizeof(float));
	*pByte += sizeof(float);
	byte += sizeof(float);

	CGimmick* pGimmick = CGimmick::GetList()->Get(gmid);

	if (pGimmick == nullptr) {
		return;
	}

	// 衝突した状態にする
	pGimmick->Hit(pos, speed);
}

//===================================================
// 次のゴールを生成
//===================================================
void CNetWork::RecvNextGoal(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// 次に生成するゴールのIDを得る
	int goalid = -1;
	memcpy(&goalid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);

	CGoalManager::GetInstance()->SetNetId(goalid);
}

//===================================================
// 開始可能なのを受信
//===================================================
void CNetWork::RecvGameStartOk(int* pByte, const int nId, const char* pRecvData)
{

}

//===================================================
// ゲーム開始可能
//===================================================
void CNetWork::RecvGameStart(int* pByte, const int nId, const char* pRecvData)
{
	// プレイヤーの存在確認
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();

	// 生成していない場合
	if (pPlayer == nullptr) {
		return;
	}

	// 座標設定
	pPlayer->SetType(CPlayer::TYPE::TYPE_GAMESTARTOK);
}

//===================================================
// チュートリアルOk
//===================================================
void CNetWork::RecvTutorialOk(int* pByte, const int nId, const char* pRecvData)
{
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }

	CScene* pScene = CManager::GetInstance()->GetScene();

	if (pScene == nullptr) { return; }

	// ID取得
	pScene->SetID(nId);

	// 準備完了フラグを立てる
	pScene->ChangeFlag(true);

	pScene->ReadyUp(nId);
}

//===================================================
// チュートリアルOkキャンセル
//===================================================
void CNetWork::RecvTutorialNo(int* pByte, const int nId, const char* pRecvData)
{
	if (nId < 0 || nId >= NetWork::MAX_CONNECT) { return; }

	CScene* pScene = CManager::GetInstance()->GetScene();

	if (pScene == nullptr) { return; }

	// ID取得
	pScene->SetID(nId);

	// 準備完了フラグを折る
	pScene->ChangeFlag(false);

	pScene->ReadyUp(nId);
}

//===================================================
// チュートリアル終了
//===================================================
void CNetWork::RecvTutorialEnd(int* pByte, const int nId, const char* pRecvData)
{
	//// 次の画面に遷移
	//if (CManager::GetInstance()->GetMode() == CScene::MODE::MODE_ENTRY)
	//{
	//	CManager::GetInstance()->GetFade()->Set(CScene::MODE::MODE_GAME);
	//}

	CScene* pScene = CManager::GetInstance()->GetScene();

	if (pScene == nullptr) { return; }

	pScene->EndTutorial();
}

//===================================================
// 検問開始
//===================================================
void CNetWork::RecvSetInspection(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// 検問のIDを得る
	int inspid = -1;
	memcpy(&inspid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// 位置を取得
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	memcpy(&pos, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// 向きを取得
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	memcpy(&rot, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// 隣接する道のID取得
	int roadid = -1;
	memcpy(&roadid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// 警察の開始ID取得
	int startpdid = -1;
	memcpy(&startpdid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	CInspection* pInsp = CInspectionManager::GetInstance()->Get(inspid);

	if (pInsp != nullptr) { return; }

	CRoad* pRoad = CRoadManager::GetInstance()->GetList()->Get(roadid);

	if (pRoad == nullptr) {
		return;
	}

	// 検問生成
	CInspectionManager::GetInstance()->SetNextInspection(pos, rot, pRoad, inspid, startpdid);
}

//===================================================
// 検問終了
//===================================================
void CNetWork::RecvEndInspection(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// 検問のIDを得る
	int inspid = -1;
	memcpy(&inspid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	CInspection* pInsp = CInspectionManager::GetInstance()->Get(inspid);

	if (pInsp == nullptr) { return; }

	// 検問解放
	pInsp->Start();
}

//===================================================
// 車の座標
//===================================================
void CNetWork::RecvCarPos(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// 検問のIDを得る
	int carid = -1;
	memcpy(&carid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// 位置を取得
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	memcpy(&pos, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// 向きを取得
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	memcpy(&rot, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	CCar* pCar = CCarManager::GetInstance()->GetMapList()->Get(carid);

	if (pCar == nullptr)
	{
		pCar = CCar::Create(pos, rot, VECTOR3_ZERO, carid);
		pCar->SetType(CCar::TYPE::TYPE_RECV);
	}

	if (pCar->GetType() == CCar::TYPE::TYPE_ACTIVE) { return; }

	pCar->SetRecvPosition(pos);
	pCar->SetRecvRotation(rot);
}

//===================================================
// 警察の座標
//===================================================
void CNetWork::RecvPdPos(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// 検問のIDを得る
	int carid = -1;
	memcpy(&carid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// 位置を取得
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	memcpy(&pos, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// 向きを取得
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	memcpy(&rot, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	CCar* pCar = CCarManager::GetInstance()->GetMapList()->Get(carid);

	if (pCar == nullptr)
	{
		pCar = CPolice::Create(pos, rot, VECTOR3_ZERO, carid);
		pCar->SetType(CCar::TYPE::TYPE_RECV);
	}

	if (pCar->GetType() == CCar::TYPE::TYPE_ACTIVE) { return; }

	pCar->SetRecvPosition(pos);
	pCar->SetRecvRotation(rot);
}

//===================================================
// 追加警察の座標
//===================================================
void CNetWork::RecvAddPdPos(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// 検問のIDを得る
	int carid = -1;
	memcpy(&carid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// 位置を取得
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	memcpy(&pos, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	// 向きを取得
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	memcpy(&rot, &pRecvData[byte], sizeof(D3DXVECTOR3));
	*pByte += sizeof(D3DXVECTOR3);
	byte += sizeof(D3DXVECTOR3);

	CCar* pCar = CCarManager::GetInstance()->GetMapList()->Get(carid);

	if (pCar == nullptr)
	{
		CAddPolice* pPolice = CAddPolice::Create(pos, rot, VECTOR3_ZERO, carid);
		pPolice->SetType(CCar::TYPE::TYPE_RECV);

		// 応援の警察は応援を呼ばないようにする
		pPolice->GetAi()->SetCall(true);

		pCar = pPolice;
	}

	if (pCar->GetType() == CCar::TYPE::TYPE_ACTIVE) { return; }

	pCar->SetRecvPosition(pos);
	pCar->SetRecvRotation(rot);
}

//===================================================
// 警察の追跡開始
//===================================================
void CNetWork::RecvPdChase(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// 車のIDを得る
	int carid = -1;
	memcpy(&carid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// プレイヤーIDを取得
	int plyid = -1;
	memcpy(&plyid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	CPolice* pCar = CPoliceManager::GetInstance()->GetMapList()->Get(carid);
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(plyid);

	// プレイヤーがいない
	if (pPlayer == nullptr)
	{
		RecvJoin(pByte, nId, pRecvData);
		return;
	}

	// 車が存在していない
	if (pCar == nullptr)
	{
		pCar = CPolice::Create(VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, carid);
		pCar->SetType(CCar::TYPE::TYPE_RECV);
	}

	pCar->SetChaseNext(CPolice::CHASE::CHASE_BEGIN);

	// 自分自身
	if (plyid == m_nMyIdx)
	{
		pCar->SetTypeNext(CCar::TYPE::TYPE_ACTIVE);
		pCar->SetNextPlayer(pPlayer);
	}
	// それ以外
	else
	{
		pCar->SetTypeNext(CCar::TYPE::TYPE_RECV);
		pCar->SetNextPlayer(pPlayer);
	}
}

//===================================================
// 追加警察の追跡開始
//===================================================
void CNetWork::RecvAddPdChase(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// 車のIDを得る
	int carid = -1;
	memcpy(&carid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	// プレイヤーIDを取得
	int plyid = -1;
	memcpy(&plyid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	CPolice* pCar = CPoliceManager::GetInstance()->GetMapList()->Get(carid);
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(plyid);

	// プレイヤーがいない
	if (pPlayer == nullptr)
	{
		RecvJoin(pByte, nId, pRecvData);
		return;
	}

	// 車が存在していない
	if (pCar == nullptr)
	{
		CAddPolice* pPolice = CAddPolice::Create(VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, carid);
		pPolice->SetType(CCar::TYPE::TYPE_RECV);

		// 応援の警察は応援を呼ばないようにする
		pPolice->GetAi()->SetCall(true);

		pCar = pPolice;
	}

	// 追跡状態にする
	pCar->SetChaseNext(CPolice::CHASE::CHASE_BEGIN);

	// 自分自身
	if (plyid == m_nMyIdx)
	{
		pCar->SetTypeNext(CCar::TYPE::TYPE_ACTIVE);
		pCar->SetNextPlayer(pPlayer);
	}
	// それ以外
	else
	{
		pCar->SetTypeNext(CCar::TYPE::TYPE_RECV);
		pCar->SetNextPlayer(pPlayer);
	}
}

//===================================================
// 警察の追跡終了
//===================================================
void CNetWork::RecvPdChaseEnd(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// 車のIDを得る
	int carid = -1;
	memcpy(&carid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	CPolice* pCar = CPoliceManager::GetInstance()->GetMapList()->Get(carid);

	// 車が存在していない
	if (pCar == nullptr)
	{
		pCar = CPolice::Create(VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, carid);
		pCar->SetType(CCar::TYPE::TYPE_RECV);
	}

	// 追跡終了状態にする
	pCar->SetChaseNext(CPolice::CHASE::CHASE_END);
}

//===================================================
// 追加警察の追跡終了
//===================================================
void CNetWork::RecvAddPdChaseEnd(int* pByte, const int nId, const char* pRecvData)
{
	int byte = 0;

	// 車のIDを得る
	int carid = -1;
	memcpy(&carid, &pRecvData[byte], sizeof(int));
	*pByte += sizeof(int);
	byte += sizeof(int);

	CPolice* pCar = CPoliceManager::GetInstance()->GetMapList()->Get(carid);

	// 車が存在していない
	if (pCar == nullptr)
	{
		pCar = CAddPolice::Create(VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, carid);
		pCar->SetType(CCar::TYPE::TYPE_RECV);
	}

	// 追跡終了状態にする
	pCar->SetChaseNext(CPolice::CHASE::CHASE_END);
}

//===================================================
// 何も送信しない
//===================================================
void CNetWork::SendNone()
{
	if (!GetActive()) { return; }
}

//===================================================
// 入室したことを送信
//===================================================
void CNetWork::SendJoin()
{
	if (!GetActive()) { return; }
}

//===================================================
// ID取得を送信
//===================================================
void CNetWork::SendGetId()
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int)] = {};	// 送信用
	int nProt = NetWork::COMMAND_GETID;

	// protocolを挿入
	memcpy(&aSendData[0], &nProt, sizeof(int));

	// 送信
	m_pClient->SetData(&aSendData[0], sizeof(int));
}

//===================================================
// 削除することを送信
//===================================================
void CNetWork::SendDelete()
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int)] = {};	// 送信用
	int nProt = NetWork::COMMAND_DELETE;
	int byte = 0;

	// protocolを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// 送信
	m_pClient->SetData(&aSendData[0], byte);

	DisConnect();
}

//===================================================
// プレイヤーの座標送信
//===================================================
void CNetWork::SendPlPos(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3) + 1] = {};	// 送信用
	int nProt = NetWork::COMMAND_PL_POS;
	int byte = 0;

	// protocolを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// 座標を挿入
	memcpy(&aSendData[byte], &pos, sizeof(D3DXVECTOR3));
	byte += sizeof(D3DXVECTOR3);

	// 向きを挿入
	memcpy(&aSendData[byte], &rot, sizeof(D3DXVECTOR3));
	byte += sizeof(D3DXVECTOR3);

	// 送信
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// プレイヤーのダメージ後体力送信
//===================================================
void CNetWork::SendPlDamage(const float nowlife)
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int) + sizeof(float)] = {};	// 送信用
	int nProt = NetWork::COMMAND_PL_DAMAGE;
	int byte = 0;

	// protocolを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// 体力を挿入
	memcpy(&aSendData[byte], &nowlife, sizeof(float));
	byte += sizeof(float);

	// 送信
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// プレイヤーがゴールしたことを送信
//===================================================
void CNetWork::SendPlGoal(int nId)
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int) + sizeof(int) + 1] = {};	// 送信用
	int nProt = NetWork::COMMAND_PL_GOAL;
	int byte = 0;

	// protocolを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// ゴールしたIDを挿入
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// 送信
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// ギミックに衝突したことを送信
//===================================================
void CNetWork::SendGmHit(const int nId, const D3DXVECTOR3& HitPos, const float fSpeed)
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int) + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(float) + 1] = {};	// 送信用
	int nProt = NetWork::COMMAND_GM_HIT;
	int byte = 0;

	// protocolを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// 衝突したギミックのIDを挿入
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// 衝突した座標を挿入
	memcpy(&aSendData[byte], &HitPos, sizeof(D3DXVECTOR3));
	byte += sizeof(D3DXVECTOR3);

	// 衝突速度を挿入
	memcpy(&aSendData[byte], &fSpeed, sizeof(float));
	byte += sizeof(float);

	// 送信
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// 次のゴール情報番号を送信
//===================================================
void CNetWork::SendNextGoal(const int nId)
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int) + sizeof(int) + 1] = {};	// 送信用
	int nProt = NetWork::COMMAND_NEXT_GOAL;
	int byte = 0;

	// protocolを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// ゴールしたIDを挿入
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// 送信
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// ゲーム開始可能なのを送信
//===================================================
void CNetWork::SendGameStartOk()
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int)] = {};	// 送信用
	int nProt = NetWork::COMMAND_GAMESTART_OK;
	int byte = 0;

	// protocolを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// 送信
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// チュートリアルOK
//===================================================
void CNetWork::SendTutorialOk()
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int)] = {};	// 送信用
	int nProt = NetWork::COMMAND_TUTORIAL_OK;
	int byte = 0;

	// protocolを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// 送信
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// チュートリアルOkキャンセル
//===================================================
void CNetWork::SendTutorialNo()
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int)] = {};	// 送信用
	int nProt = NetWork::COMMAND_TUTORIAL_NO;
	int byte = 0;

	// protocolを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// 送信
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// 検問設置を送信
//===================================================
void CNetWork::SendSetInspection(const int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, int nIdx, int nStartPdId)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3) + sizeof(int) + sizeof(int)] = {};	// 送信用
	//int nProt = NetWork::COMMAND_SET_INSP;
	//int byte = 0;

	//// protocolを挿入
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// ID
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// 座標
	//memcpy(&aSendData[byte], &pos, sizeof(D3DXVECTOR3));
	//byte += sizeof(D3DXVECTOR3);

	//// 向き
	//memcpy(&aSendData[byte], &rot, sizeof(D3DXVECTOR3));
	//byte += sizeof(D3DXVECTOR3);

	//// 道のID
	//memcpy(&aSendData[byte], &nIdx, sizeof(int));
	//byte += sizeof(int);

	//// 警察の開始ID
	//memcpy(&aSendData[byte], &nStartPdId, sizeof(int));
	//byte += sizeof(int);

	//// 送信
	//m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// 検問終了を送信
//===================================================
void CNetWork::SendEndInspection(int nId)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int)] = {};	// 送信用
	//int nProt = NetWork::COMMAND_END_INSP;
	//int byte = 0;

	//// protocolを挿入
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// 検問のID
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// 送信
	//m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// 車の座標を送信
//===================================================
void CNetWork::SendCarPos(int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	if (!GetActive()) { return; }

	char aSendData[sizeof(int) + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3)] = {};	// 送信用
	int nProt = NetWork::COMMAND_CAR_POS;
	int byte = 0;

	// protocolを挿入
	memcpy(&aSendData[byte], &nProt, sizeof(int));
	byte += sizeof(int);

	// IDを挿入
	memcpy(&aSendData[byte], &nId, sizeof(int));
	byte += sizeof(int);

	// 座標を挿入
	memcpy(&aSendData[byte], &pos, sizeof(D3DXVECTOR3));
	byte += sizeof(D3DXVECTOR3);

	// 向きを挿入
	memcpy(&aSendData[byte], &rot, sizeof(D3DXVECTOR3));
	byte += sizeof(D3DXVECTOR3);

	// 送信
	m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// 警察の座標を送信
//===================================================
void CNetWork::SendPdPos(int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3)] = {};	// 送信用
	//int nProt = NetWork::COMMAND_PD_POS;
	//int byte = 0;

	//// protocolを挿入
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// Idを挿入
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// 座標を挿入
	//memcpy(&aSendData[byte], &pos, sizeof(D3DXVECTOR3));
	//byte += sizeof(D3DXVECTOR3);

	//// 向きを挿入
	//memcpy(&aSendData[byte], &rot, sizeof(D3DXVECTOR3));
	//byte += sizeof(D3DXVECTOR3);

	//// 送信
	//m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// 追加警察の座標を送信
//===================================================
void CNetWork::SendAddPdPos(int nId, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int) + sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3)] = {};	// 送信用
	//int nProt = NetWork::COMMAND_ADDPD_POS;
	//int byte = 0;

	//// protocolを挿入
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// Idを挿入
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// 座標を挿入
	//memcpy(&aSendData[byte], &pos, sizeof(D3DXVECTOR3));
	//byte += sizeof(D3DXVECTOR3);

	//// 向きを挿入
	//memcpy(&aSendData[byte], &rot, sizeof(D3DXVECTOR3));
	//byte += sizeof(D3DXVECTOR3);

	//// 送信
	//m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// 警察の追跡開始送信
//===================================================
void CNetWork::SendPdChase(int nId, int plyid)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int) + sizeof(int)] = {};	// 送信用
	//int nProt = NetWork::COMMAND_PD_CHASE;
	//int byte = 0;

	//// protocolを挿入
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// Idを挿入
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// 追跡するプレイヤーIdを挿入
	//memcpy(&aSendData[byte], &plyid, sizeof(int));
	//byte += sizeof(int);

	//// 送信
	//m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// 追加警察の追跡開始送信
//===================================================
void CNetWork::SendAddPdChase(int nId, int plyid)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int) + sizeof(int)] = {};	// 送信用
	//int nProt = NetWork::COMMAND_ADDPD_CHASE;
	//int byte = 0;

	//// protocolを挿入
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// Idを挿入
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// 追跡するプレイヤーIdを挿入
	//memcpy(&aSendData[byte], &plyid , sizeof(int));
	//byte += sizeof(int);

	//// 送信
	//m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// 警察の追跡終了送信
//===================================================
void CNetWork::SendPdChaseEnd(int nId)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int)] = {};	// 送信用
	//int nProt = NetWork::COMMAND_PD_CHASEEND;
	//int byte = 0;

	//// protocolを挿入
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// Idを挿入
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// 送信
	//m_pClient->SetData(&aSendData[0], byte);
}

//===================================================
// 追加警察の追跡終了送信
//===================================================
void CNetWork::SendAddPdChaseEnd(int nId)
{
	//if (!GetActive()) { return; }

	//char aSendData[sizeof(int) + sizeof(int)] = {};	// 送信用
	//int nProt = NetWork::COMMAND_ADDPD_CHASEEND;
	//int byte = 0;

	//// protocolを挿入
	//memcpy(&aSendData[byte], &nProt, sizeof(int));
	//byte += sizeof(int);

	//// Idを挿入
	//memcpy(&aSendData[byte], &nId, sizeof(int));
	//byte += sizeof(int);

	//// 送信
	//m_pClient->SetData(&aSendData[0], byte);
}