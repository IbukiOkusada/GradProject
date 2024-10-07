//===============================================
//
// ゲーム画面の管理処理 [game.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "game.h"
#include "manager.h"
#include "renderer.h"
#include "sound.h"
#include "camera.h"
#include "light.h"
#include "texture.h"
#include "Xfile.h"
#include "input.h"
#include "fade.h"
#include "result.h"
#include "debugproc.h"
#include "time.h"
#include "tcp_client.h"
#include <thread>
#include "protocol_online.h"
#include "object2D.h"
#include <assert.h>
#include "ranking.h"

// 無名名前空間を定義
namespace {
    const D3DXVECTOR3 STARTDOORPOS = { -1160.0f, 0.0f, 950.0f };	// スタート地点ドア基本座標
	const D3DXVECTOR3 PLAYERSTARTPOS = { 0.0f, 0.0f, -5500.0f };  // プレイヤーのスタート位置
    const D3DXVECTOR3 LEVERPOS[4] =
    {
        D3DXVECTOR3(130.0f, 100.0f, -5130.0f),
        D3DXVECTOR3(-1000.0f, 100.0f, -4440.0f),
        D3DXVECTOR3(470.0f, 100.0f, -560.0f),
        D3DXVECTOR3(360.0f, 100.0f, -1900.0f),
    };

    const D3DXVECTOR3 LEVERROT[4] =
    {
        D3DXVECTOR3(0.0f, D3DX_PI, 0.0f),
        D3DXVECTOR3(0.0f, D3DX_PI, 0.0f),
        D3DXVECTOR3(0.0f, 0.0f, 0.0f),
        D3DXVECTOR3(0.0f, -D3DX_PI * 0.5f, 0.0f),
    };


	const char* TEXPASS[4] =
	{
		"data\\TEXTURE\\opendoor00.png",
		"data\\TEXTURE\\opendoor01.png",
		"data\\TEXTURE\\opendoor02.png",
		"data\\TEXTURE\\opendoor03.png",
	};

	const D3DXVECTOR3 OPEN_SETPOS = { SCREEN_WIDTH * 1.3f, SCREEN_HEIGHT * 0.5f, 0.0f };  // スタートドア開いたときのUIの生成位置
	const D3DXVECTOR3 OPEN_SETROT = { 0.0f, 0.0f, D3DX_PI * 0.0f };                       // 向き
	const D3DXVECTOR2 OPEN_SIZE = { 470.0f, 150.0f };                                     // サイズ
	const float OPEN_MOVESPEED = (-3.3f);                                                 // 移動スピード
	const float OPEN_MOVESIN = (0.05f);
	const float OPEN_MOVESIZE = (30.0f);
    const D3DXVECTOR2 QUATAUI_SIZE = { 100.0f, 50.0f };	// ノルマのUIのサイズ
    const D3DXVECTOR2 SCORE_SIZE = { 14.0f, 18.0f };	// スコアのサイズ
    const float DOOR_SPACE = (-20.0f);			// 各スタート地点ドアの間
    const char* FILEPASS = "data\\TXT\\player";	// ファイルのパス
    const char* FILEEXT = ".txt";				// ファイルの拡張子
    const int FILEPASS_SIZE = (200);			// ファイルのパスサイズ
    const int START_TIMER = (210);				// 開始制限時間
    const int START_WAITCNT = (430);            // スタート時の走ってる時間
	const int PLAYER_MOVESTART = (180);
	const int CAMERA_ROTATESTART = (240);
	const D3DXVECTOR3 START_CAMERAROT = {0.0f, D3DX_PI * 0.0f, D3DX_PI * 0.38f};
    const int SCORE = (15000);                   // 初期のスコア
    const int UNINITCOUNT = (120);              // ノルマのUIが消えるまでの時間
	const int PLAYER_SPWANSTART = (240);
    const int MAX_STRING = (2048);
    const int DEF_PORT = (22333);
    const char* ADDRESSFILE	= "data\\TXT\\address.txt";
}

//===============================================
// 静的メンバ変数
//===============================================
CGame::STATE CGame::m_state = CGame::STATE_LOCAL;	// 状態
int CGame::m_nNumPlayer = 0;

//===============================================
// コンストラクタ
//===============================================
CGame::CGame()
{
    // 値のクリア
    m_ppCamera = nullptr;
    m_ppPlayer = nullptr;
    m_pFileLoad = nullptr;
    m_pMeshDome = nullptr;
    m_pClient = nullptr;
    m_pTimer = nullptr;
    m_nSledCnt = 0;
    m_bEnd = false;
	m_fOpenDoorUISin = 0.0f;
    m_bPause = false;
    m_pPause = nullptr;
}

//===============================================
// コンストラクタ(人数設定)
//===============================================
CGame::CGame(int nNumPlayer)
{
    // 値のクリア
    m_ppCamera = nullptr;
    m_ppPlayer = nullptr;
    m_pFileLoad = nullptr;
    m_pMeshDome = nullptr;
    m_pClient = nullptr;
    m_pTimer = nullptr;
    m_nSledCnt = 0;
    m_bEnd = false;
	m_fOpenDoorUISin = 0.0f;
    m_bPause = false;
    m_pPause = nullptr;

    // 人数設定
    m_nNumPlayer = nNumPlayer;
}

//===============================================
// デストラクタ
//===============================================
CGame::~CGame()
{
   
}

//===============================================
// 初期化処理
//===============================================
HRESULT CGame::Init(void)
{
    memset(&m_aAddress[0], '\0', sizeof(m_aAddress));
    int nErr = WSAStartup(WINSOCK_VERSION, &m_wsaData);	// winsockの初期化関数

    if (nErr != 0)
    {// 初期化に失敗した場合
        m_state = STATE_LOCAL;
    }

    // 外部ファイル読み込みの生成
    if (nullptr == m_pFileLoad)
    {// 使用していない場合

    }

    switch (m_state)
    {
    case STATE_LOCAL:
    {// ローカルの場合
        if (m_nNumPlayer <= 0)
        {// 人数が指定されていない
            m_nNumPlayer = 1;
        }

        char aBodyPass[FILEPASS_SIZE] = "";		// 胴体パス
        char aLegPass[FILEPASS_SIZE] = "";		// 下半身パス

        sprintf(&aBodyPass[0], "%s\\motion_body%s", FILEPASS, FILEEXT);
        sprintf(&aLegPass[0], "%s\\motion_leg%s", FILEPASS, FILEEXT);
    }
        break;

    case STATE_ONLINE:
    {// オンライン通信の場合
        m_pClient = new CClient;
        AddressLoad(&m_aAddress[0]);

        if (m_pClient->Init(&m_aAddress[0], DEF_PORT))
        {// 初期接続成功
            // オンライン関数をマルチスレッド
            std::thread th(&CGame::Online, this);
            th.detach();
        }
    }
        break;

    default:

        break;
    }

    //CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_GAME);

    return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CGame::Uninit(void)
{
   CManager::GetInstance()->GetSound()->Stop();

    m_bEnd = true;

    while (1)
    {
        OnlineEnd();
        if (m_nSledCnt <= 0)
        {
            break;
        }
    }

    // クライアント
    if (m_pClient != nullptr)
    {
        m_pClient->Uninit();
        delete m_pClient;
        m_pClient = nullptr;
    }

    // defaultカメラオン
    CManager::GetInstance()->GetCamera()->SetDraw(true);

    //Winsock終了処理
    WSACleanup();	// WSACleanup関数 : winsockの終了処理

    m_state = STATE_LOCAL;
}

//===============================================
// 更新処理
//===============================================
void CGame::Update(void)
{
	CInputPad *pInputPad = CManager::GetInstance()->GetInputPad();
	CInputKeyboard *pInputKey = CManager::GetInstance()->GetInputKeyboard();

	if (pInputKey->GetTrigger(DIK_P) == true || pInputPad->GetTrigger(CInputPad::BUTTON_START, 0))
	{//ポーズキー(Pキー)が押された
		m_bPause = m_bPause ? false : true;
	}

    CScene::Update();
}

//===============================================
// 描画処理
//===============================================
void CGame::Draw(void)
{
    CScene::Draw();
}

//===================================================
// プレイヤーの取得
//===================================================
CPlayer *CGame::GetPlayer(void)
{
    return *m_ppPlayer;
}

//===================================================
// ファイル読み込みの取得
//===================================================
CFileLoad *CGame::GetFileLoad(void)
{
    return m_pFileLoad;
}

//===================================================
// オンライン通信
//===================================================
void CGame::Online(void)
{
    m_nSledCnt = 1;

    while (1)
    {
        if (m_ppPlayer == NULL || m_bEnd == true)
        {
            break;
        }

        char aRecvData[MAX_STRING] = {};	// 受信用

        // 受信
        int nRecvByte = m_pClient->Recv(&aRecvData[0], MAX_STRING);

        // マルチスレッド
        std::thread th(&CGame::ByteCheck, this, &aRecvData[0], nRecvByte);
        th.detach();
    }

    m_nSledCnt--;
}

//===================================================
// 受信確認
//===================================================
void CGame::ByteCheck(char *pRecvData, int nRecvByte)
{
    m_mutex.lock();

    //m_nSledCnt++;
    //D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    //D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    //int nType = COMMAND_TYPE_NONE;	// コマンド番号
    //int nId = -1;		// ID
    //int nByte = 0;	// バイト数
    //int nDamage = 0;	// ダメージ数
    //CPlayer *pPlayer = NULL;		// 先頭を取得
    //CPlayer *pPlayerNext = NULL;	// 次を保持

    //if (nRecvByte <= 0)
    //{
    //    m_nSledCnt--;
    //    m_mutex.unlock();
    //    return;
    //}

    //pPlayer = CPlayerManager::GetInstance()->GetTop();	// 先頭を取得

    //// 終端文字まで確認する
    //while (nByte < nRecvByte)
    //{
    //    bool bIn = false;	// 既にいるかどうか
    //    memcpy(&nId, &pRecvData[nByte], sizeof(int));
    //    nByte += sizeof(int);

    //    // コマンド取得
    //    memcpy(&nType, &pRecvData[nByte], sizeof(int));
    //    nByte += sizeof(int);

    //    if (nId != -1)
    //    {
    //        // コマンドごとに分ける
    //        switch (nType)
    //        {

    //        case COMMAND_TYPE_SETPOSITION:

    //            memcpy(&pos, &pRecvData[nByte], sizeof(D3DXVECTOR3));
    //            nByte += sizeof(D3DXVECTOR3);
    //            break;

    //        case COMMAND_TYPE_SETROTATION:

    //            memcpy(&pos, &pRecvData[nByte], sizeof(D3DXVECTOR3));
    //            nByte += sizeof(D3DXVECTOR3);
    //            break;

    //        case COMMAND_TYPE_SETLIFE:

    //            memcpy(&nDamage, &pRecvData[nByte], sizeof(int));
    //            nByte += sizeof(int);
    //            break;

    //        case COMMAND_TYPE_START_OK:

    //            break;

    //        case COMMAND_TYPE_CREATE:
    //            break;

    //        case COMMAND_TYPE_DAMAGE:

    //            memcpy(&nDamage, &pRecvData[nByte], sizeof(int));
    //            nByte += sizeof(int);
    //            break;

    //        case COMMAND_TYPE_DELETE:
    //            break;

    //        case COMMAND_TYPE_GETID:
    //            break;
    //        }
    //    }

    //    if (m_ppPlayer == NULL)
    //    {
    //        m_nSledCnt--;
    //        m_mutex.unlock();
    //        return;
    //    }

    //    if (nId != -1 && (*m_ppPlayer)->GetId() != -1)
    //    {// IDを受信できた

    //        pPlayer = CPlayerManager::GetInstance()->GetTop();	// 先頭を取得

    //        while (pPlayer != NULL)
    //        {// 使用されている間繰り返し
    //            pPlayerNext = pPlayer->GetNext();	// 次を保持

    //            if (nId == pPlayer->GetId() && (*m_ppPlayer)->GetId() != nId)
    //            {// 自分以外かつ操作していない

    //             // コマンドごとに分ける
    //                switch (nType)
    //                {
    //                case COMMAND_TYPE_SETPOSITION:

    //                    pPlayer->SetPosition(pos);
    //                    break;

    //                case COMMAND_TYPE_SETROTATION:

    //                    pPlayer->SetRotation(pos);
    //                    break;

    //                case COMMAND_TYPE_SETLIFE:

    //                    pPlayer->SetLife(nDamage);
    //                    break;

    //                case COMMAND_TYPE_START_OK:

    //                    break;

    //                case COMMAND_TYPE_CREATE:

    //                    break;

    //                case COMMAND_TYPE_DAMAGE:

    //                    pPlayer->Damage(nDamage);
    //                    break;

    //                case COMMAND_TYPE_GOAL:

    //                    break;

    //                case COMMAND_TYPE_DELETE:

    //                    pPlayer->Uninit();
    //                    break;

    //                case COMMAND_TYPE_GETID:

    //                    break;
    //                }

    //                bIn = true;	// いる状態にする
    //                break;
    //            }
    //            else if (nId == pPlayer->GetId() && (*m_ppPlayer)->GetId() == nId)
    //            {// 自分以外かつ操作キャラ

    //                bIn = true;	// いる状態にする
    //                break;
    //            }

    //            pPlayer = pPlayerNext;	// 次に移動
    //        }

    //        if (bIn == false && (*m_ppPlayer)->GetId() != -1 && nType > COMMAND_TYPE_NONE && nType < COMMAND_TYPE_MAX)
    //        {// まだ存在していない場合
    //            pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), NULL, NULL);
    //            pPlayer->BindId(nId);
    //            pPlayer->SetType(CPlayer::TYPE_NONE);
    //        }
    //    }
    //    else if (nId == -1 && (*m_ppPlayer)->GetId() == -1)
    //    {// IDが受信できていないかつ自分自身のIDも存在していない
    //        nId = nType;

    //        // 自分のIDを設定
    //        (*m_ppPlayer)->BindId(nId);

    //        break;
    //    }
    //}

    //m_nSledCnt--;
    m_mutex.unlock();
}

//===================================================
// 座標送信
//===================================================
void CGame::SendPosition(D3DXVECTOR3& pos)
{
    if (m_pClient != nullptr)
    {
        char aSendData[MAX_STRING] = {};	// 送信用
        int nProt = COMMAND_TYPE_SETPOSITION;

        // protocolを挿入
        memcpy(&aSendData[0], &nProt, sizeof(int));

        // 座標を挿入
        memcpy(&aSendData[sizeof(int)], &pos, sizeof(D3DXVECTOR3));

        // 送信
        m_pClient->Send(&aSendData[0], sizeof(int) + sizeof(D3DXVECTOR3));
    }
}

//===================================================
// 向き送信
//===================================================
void CGame::SendRotation(D3DXVECTOR3& rot)
{
    if (m_pClient != nullptr)
    {
        char aSendData[MAX_STRING] = {};	// 送信用
        int nProt = COMMAND_TYPE_SETROTATION;

        // protocolを挿入
        memcpy(&aSendData[0], &nProt, sizeof(int));

        // 座標を挿入
        memcpy(&aSendData[sizeof(int)], &rot, sizeof(D3DXVECTOR3));

        // 送信
        m_pClient->Send(&aSendData[0], sizeof(int) + sizeof(D3DXVECTOR3));
    }
}

//===================================================
// ダメージ送信
//===================================================
void CGame::SendDamage(int nDamage)
{
    if (m_pClient != nullptr)
    {
        char aSendData[MAX_STRING] = {};	// 送信用
        int nProt = COMMAND_TYPE_DAMAGE;

        // protocolを挿入
        memcpy(&aSendData[0], &nProt, sizeof(int));

        // ダメージを挿入
        memcpy(&aSendData[sizeof(int)], &nDamage, sizeof(int));

        // 送信
        m_pClient->Send(&aSendData[0], sizeof(int) + sizeof(int));
    }
}

//===================================================
// 体力送信
//===================================================
void CGame::SendLife(int nLife)
{
    if (m_pClient != nullptr)
    {
        char aSendData[MAX_STRING] = {};	// 送信用
        int nProt = COMMAND_TYPE_SETLIFE;

        // protocolを挿入
        memcpy(&aSendData[0], &nProt, sizeof(int));

        // ダメージを挿入
        memcpy(&aSendData[sizeof(int)], &nLife, sizeof(int));

        // 送信
        m_pClient->Send(&aSendData[0], sizeof(int) + sizeof(int));
    }
}

//===================================================
// 終了送信
//===================================================
void CGame::OnlineEnd(void)
{
    if (m_pClient != nullptr)
    {
        char aSendData[MAX_STRING] = {};	// 送信用
        int nProt = COMMAND_TYPE_DELETE;

        // protocolを挿入
        memcpy(&aSendData[0], &nProt, sizeof(int));

        // 送信
        m_pClient->Send(&aSendData[0], sizeof(int));
    }
}

//===============================================
// 体力設定
//===============================================
void CGame::SendSetUp(void)
{
    if (m_pClient != nullptr)
    {
        char aSendData[MAX_STRING] = {};	// 送信用
        int nProt = COMMAND_TYPE_START_OK;

        // protocolを挿入
        memcpy(&aSendData[0], &nProt, sizeof(int));

        // 送信
        m_pClient->Send(&aSendData[0], sizeof(int));
    }
}

//===============================================
// ゴール送信
//===============================================
void CGame::SendGoal(void)
{
    if (m_pClient != nullptr)
    {
        char aSendData[MAX_STRING] = {};	// 送信用
        int nProt = COMMAND_TYPE_GOAL;

        // protocolを挿入
        memcpy(&aSendData[0], &nProt, sizeof(int));


        // 送信
        m_pClient->Send(&aSendData[0], sizeof(int));
    }
}

//===================================================
// アドレス読み込み
//===================================================
void CGame::AddressLoad(char *pAddrss)
{
    FILE *pFile;	// ファイルへのポインタ

    pFile = fopen(ADDRESSFILE, "r");

    if (pFile != NULL)
    {//ファイルが開けた場合
        
        //テキスト読み込み
        fscanf(pFile, "%s", pAddrss);

        //ファイルを閉じる
        fclose(pFile);
    }
    else
    {//ファイルが開けなかった場合
        return;
    }
}

//===================================================
// 開始演出
//===================================================
bool CGame::StartDirection(void)
{
	return false;
}