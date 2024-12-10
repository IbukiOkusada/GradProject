//===============================================
//
// マルチリザルト画面の管理処理 [multi_result.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "multi_result.h"
#include "multi_result_manager.h"
#include "map_manager.h"
#include "manager.h"
#include "sound.h"
#include "camera.h"
#include "network.h"
#include "scrollText2D.h"
#include "player.h"
#include "objectX.h"

//==========================================================
// 定数定義
//==========================================================
namespace TEXT
{
	const D3DXVECTOR3 TITLE_POS = D3DXVECTOR3(SCREEN_CENTER.x, 100.0f, 0.0f);	// タイトル座標
	const D3DXVECTOR3 RESULT_POS = D3DXVECTOR3(SCREEN_CENTER.x, SCREEN_HEIGHT - 150.0f, 0.0f);	// 結果座標
	const D3DXVECTOR3 SETPOS = D3DXVECTOR3(170.0f, SCREEN_CENTER.y * 0.9f, 0.0f);		// プレイヤーごとの基本座標

	const float STR_SPACE = 300.0f;	// プレイヤーの文字間
	const float STR_SIZE = 80.0f;	// プレイヤー使用の文字サイズ
	const float TITLE_SIZE = 150.0f;	// タイトルの文字サイズ
	const float RESULT_SIZE = 100.0f;	// タイトルの文字サイズ
}

namespace CAMERA
{
	const D3DXVECTOR3 POSV = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	const D3DXVECTOR3 ROT = D3DXVECTOR3(0.0f, D3DX_PI * 1.0f, D3DX_PI * 0.3f);
	const float LENGTH = 1700.0f;
}

namespace
{
	const float PLAYER_INER = 0.075f;			// プレイヤー移動量慣性
	const float PLAYER_TARGET_POSZ = -800.0f;	// プレイヤーランク表示時目標Z座標
	const float PLAYER_SPACE = 275.0f;
}

// using定義
using namespace TEXT;

//===============================================
// コンストラクタ
//===============================================
CMultiResult::CMultiResult() :
m_pMgr(nullptr),
m_pInfo(nullptr),
m_pTitleStr(nullptr),
m_pEndStr(nullptr),
m_nNowScrPlayer(0)
{

}

//===============================================
// デストラクタ
//===============================================
CMultiResult::~CMultiResult()
{

}

//===============================================
// 初期化処理
//===============================================
HRESULT CMultiResult::Init(void)
{
	// マップ読み込み
	CMapManager::GetInstance()->Load();

	// マネージャー取得
	m_pMgr = CMultiResultManager::GetInstance();

	// 情報を格納
	CMultiResultManager::SInfo* pInfo = m_pMgr->GetInfo();
	m_pInfo = DEBUG_NEW SPlayerInfo[NetWork::MAX_CONNECT]();

	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		// 各情報を保存
		m_pInfo[i].nId = pInfo[i].nId;
		m_pInfo[i].nScore = pInfo[i].nNumDelv;
		m_pInfo[i].bActive = pInfo[i].bActive;

		// アクティブではない
		if (!m_pInfo[i].bActive) { m_pInfo[i].nScore = 100; continue; }

		// プレイヤーの座標設定
		D3DXVECTOR3 pos = TEXT::SETPOS;
		pos.z -= PLAYER_SPACE * 0.5f;
		pos.z += PLAYER_SPACE * m_pMgr->GetNumPlayer() * 0.5f;
		pos.z -= PLAYER_SPACE * m_pInfo[i].nId;
		pos.x = -PLAYER_TARGET_POSZ * 2;

		// 向き設定
		D3DXVECTOR3 rot = VECTOR3_ZERO;
		rot.y += D3DX_PI;

		// プレイヤー生成
		m_pInfo[i].pPlayer = CPlayer::Create(pos, rot, VECTOR3_ZERO, m_pInfo[i].nId);
		m_pInfo[i].pPlayer->SetType(CPlayer::TYPE::TYPE_NONE);
	}

	// ソートする
	Sort();

	// プレイヤーごとの文字設定
	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		if (!m_pInfo[i].bActive) { continue; }

		// 座標設定
		D3DXVECTOR3 pos = SETPOS;
		pos.x += STR_SPACE * m_pInfo[i].nId;

		// 生成
		m_pInfo[i].pString = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, pos, 
			0.1f, STR_SIZE, STR_SIZE, XALIGN_CENTER, YALIGN_CENTER, VECTOR3_ZERO, m_pInfo[i].pPlayer->GetObj()->GetColMuliti());
		
		// 文字設定
		std::string str;
		str.clear();

		// ID
		std::ostringstream id;
		id << m_pInfo[i].nId + 1;
		str += id.str() + "P ";

		// 順位
		std::ostringstream rank;
		rank << m_pMgr->GetNumPlayer() - i;
		str += rank.str() + "位";

		// 文字挿入
		m_pInfo[i].pString->PushBackString(str);

		// スコア
		str.clear();
		std::ostringstream score;
		score << m_pInfo[i].nScore;
		str += "配達数 : " + score.str();

		// 文字挿入
		m_pInfo[i].pString->PushBackString(str);
	}

	// 結果発表の文字生成
	m_pTitleStr = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, TITLE_POS,
		0.4f, TITLE_SIZE, TITLE_SIZE, XALIGN_CENTER, YALIGN_CENTER, VECTOR3_ZERO, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	if (m_pTitleStr != nullptr)
	{
		// 文字設定
		m_pTitleStr->PushBackString("<<< 配達報告 >>>");
		m_pTitleStr->SetEnableScroll(true);
	}

	// 勝敗の文字生成
	int topid = m_pMgr->GetNumPlayer() - 1;	// 1位のID
	int myId = -1;
	if (m_pMgr != nullptr) { myId = m_pMgr->GetMyId(); }

	m_pEndStr = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, RESULT_POS,
		0.1f, RESULT_SIZE, RESULT_SIZE, XALIGN_CENTER, YALIGN_CENTER, VECTOR3_ZERO, m_pInfo[topid].pPlayer->GetObj()->GetColMuliti());
	if (m_pEndStr != nullptr)
	{
		// 文字設定
		std::string str;
		str.clear();

		// ID
		std::ostringstream id;
		id.clear();
		id << m_pInfo[topid].nId + 1;
		str += "*** 最優秀賞 : " + id.str() + "P ***";

		m_pEndStr->PushBackString(str);

		if (myId == m_pInfo[topid].nId)
		{
			m_pEndStr->PushBackString("あなたです おめでとうございます");
		}
		else
		{
			m_pEndStr->PushBackString("他の方です また頑張りましょう");
		}
	}

	// カメラ初期設定
	InitCameraSet();

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CMultiResult::Uninit(void)
{
	// マネージャー廃棄
	if(m_pMgr != nullptr)
	{
		m_pMgr->Release();
		m_pMgr = nullptr;
	}

	// データ廃棄
	if (m_pInfo != nullptr)
	{
		delete[] m_pInfo;
		m_pInfo = nullptr;
	}

	// マップ情報廃棄
	CMapManager::Release();

	// カメラ操作可能に
	CManager::GetInstance()->GetCamera()->SetActive(true);

	// サウンド停止
	CManager::GetInstance()->GetSound()->Stop();
}

//===============================================
// 更新処理
//===============================================
void CMultiResult::Update(void)
{
	// 文字確認
	StrCheck();

	CScene::Update();
}

//===============================================
// 描画処理
//===============================================
void CMultiResult::Draw(void)
{
	CScene::Draw();
}

//===============================================
// ソート処理
//===============================================
void CMultiResult::Sort()
{
	for (int i = 0; i < NetWork::MAX_CONNECT - 1; i++)
	{
		int temp = i;

		for (int j = i + 1; j < NetWork::MAX_CONNECT; j++)
		{
			// 昇順ソート
			if (m_pInfo[j].nScore < m_pInfo[temp].nScore)
			{
				// 記憶
				temp = j;
			}
		}

		// 変更された
		if (i != temp)
		{
			SPlayerInfo info = m_pInfo[temp];
			m_pInfo[temp] = m_pInfo[i];
			m_pInfo[i] = info;
		}
	}
}

//===============================================
// 文字列確認
//===============================================
void CMultiResult::StrCheck()
{
	// マネージャー無し
	if (m_pMgr == nullptr) { return; }

	// 開始文字無し
	if (m_pTitleStr == nullptr) { return; }

	// 開始文字終了していない
	if (!m_pTitleStr->GetEnd()) { return; }

	// 最後まで見た
	if (m_nNowScrPlayer >= m_pMgr->GetNumPlayer()) {
		EndStr(); 
		return; 
	}

	CScrollText2D* pStr = m_pInfo[m_nNowScrPlayer].pString;

	if (pStr == nullptr) { m_nNowScrPlayer++;  return; }

	// 文字をスクロールしていない
	if (!pStr->IsScroll()) {
		pStr->SetEnableScroll(true);
	}

	// 文字がない
	if (pStr == nullptr) { return; }

	// 文字を最後まで表示していない
	if (!pStr->GetEnd()) {
		// プレイヤー移動
		RankPlayerMove(); 
		return; 
	}

	// 表示していたら次の文字を表示する
	m_nNowScrPlayer++;
}

//===============================================
// 終了文字表示
//===============================================
void CMultiResult::EndStr()
{
	// 文字がない
	if (m_pEndStr == nullptr) { return; }

	// 文字がスクロールされている
	if (m_pEndStr->IsScroll()) { return; }

	m_pEndStr->SetEnableScroll(true);
}

//===============================================
// 初期カメラ設定
//===============================================
void CMultiResult::InitCameraSet()
{
	// カメラ取得
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	if (pCamera == nullptr) { return; }

	// カメラの値設定
	pCamera->SetLength(CAMERA::LENGTH);
	pCamera->SetRotation(CAMERA::ROT);
	pCamera->SetPositionR(CAMERA::POSV);
}

//===============================================
// ランク表示中のプレイヤーの動き制作
//===============================================
void CMultiResult::RankPlayerMove()
{
	// マネージャー無し
	if (m_pMgr == nullptr) { return; }

	// 最後まで見た
	if (m_nNowScrPlayer >= m_pMgr->GetNumPlayer()) { return; }

	// プレイヤー取得
	CPlayer* pPlayer = m_pInfo[m_nNowScrPlayer].pPlayer;
	if (pPlayer == nullptr) { return; }

	// 座標を補正する
	D3DXVECTOR3 pos = pPlayer->GetPosition();
	D3DXVECTOR3 posdest = pos;
	posdest.x = PLAYER_TARGET_POSZ;
	pos += ((posdest - pos) * PLAYER_INER);

	pPlayer->SetPosition(pos);
}