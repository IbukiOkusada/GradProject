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

//==========================================================
// 定数定義
//==========================================================
namespace TEXT
{
	const D3DXVECTOR3 SETPOS = D3DXVECTOR3(100.0f, SCREEN_CENTER.y, 0.0f);
}

//===============================================
// コンストラクタ
//===============================================
CMultiResult::CMultiResult() :
m_pMgr(nullptr),
m_pInfo(nullptr),
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
		m_pInfo[i].nId = pInfo[i].nId;
		m_pInfo[i].nScore = pInfo[i].nNumDelv;
		m_pInfo[i].bActive = pInfo[i].bActive;
	}

	// ソートする
	Sort();

	// 文字設定
	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
	{
		if (!m_pInfo[i].bActive) { continue; }

		D3DXVECTOR3 pos = TEXT::SETPOS;
		pos.x += 200.0f * m_pInfo[i].nId;
		m_pInfo[i].pString = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, pos, 0.25f, 50.0f, 50.0f, XALIGN_CENTER, YALIGN_CENTER, VECTOR3_ZERO, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
		
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
		str += "スコア : " + score.str();

		// 文字挿入
		m_pInfo[i].pString->PushBackString(str);
	}

	// 文字再生
	m_pInfo[m_nNowScrPlayer].pString->SetEnableScroll(true);

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