//===============================================
//
// タイトル画面の管理処理 [title.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "title.h"
#include "manager.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "camera.h"
#include "texture.h"

//===============================================
// 無名名前空間
//===============================================
namespace {
	const D3DXVECTOR3 ENTERPOS = { 0.0f, 0.0f, 0.0f };	// ENTER 座標
	const D3DXVECTOR3 ENTERROT = { 0.0f, 0.0f, 0.0f };	// ENTER 向き
	const int AUTOMOVE_RANKING = 640;	// ランキング自動遷移時間
	const int MOVE_TUTORIAL = 120;		// チュートリアルに遷移するまでの時間
	const int PLAYER_WALLKICKTIMER = 40;
	const int ENEMY_NUM = 3;			//演出用敵出現数
	const char* FILEPASS = "data\\TXT\\player";	// ファイルのパス
	const char* FILEEXT = ".txt";				// ファイルの拡張子
	const int FILEPASS_SIZE = (200);			// ファイルのパスサイズ
}

//===============================================
// コンストラクタ
//===============================================
CTitle::CTitle()
{
	m_nCounterRanking = 0;
	m_nCounterTutorial = 0;
	m_nCounterRanking = 0;
	m_bPush = false;
	m_pFileLoad = nullptr;
	m_pEnter = nullptr;
}

//===============================================
// デストラクタ
//===============================================
CTitle::~CTitle()
{

}

//===============================================
// 初期化処理
//===============================================
HRESULT CTitle::Init(void)
{

	// カメラの初期位置設定
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	pCamera->SetPositionV(D3DXVECTOR3(-874.3f, 1124.15f, 1717.2f));
	pCamera->SetPositionR(D3DXVECTOR3(80.0f, 95.0f, 220.0f));
	pCamera->SetLength(350.0f);
	pCamera->SetRotation(D3DXVECTOR3(0.0f, -2.1f, 1.79f));
	pCamera->SetActive(false);

	// 遷移タイマー設定
	m_nCounterTutorial = MOVE_TUTORIAL;
	m_nCounterRanking = AUTOMOVE_RANKING;

	// サウンド再生
	CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_TITLE);

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CTitle::Uninit(void)
{
	CManager::GetInstance()->GetSound()->Stop();
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	pCamera->SetActive(true);
}

//===============================================
// 更新処理
//===============================================
void CTitle::Update(void)
{
	CInputPad *pInputPad = CManager::GetInstance()->GetInputPad();
	CInputKeyboard *pInputKey = CManager::GetInstance()->GetInputKeyboard();

	if (pInputKey->GetTrigger(DIK_RETURN) || pInputPad->GetTrigger(CInputPad::BUTTON_START, 0)) {
		m_bPush = true;
	}

	if (m_bPush) {
		m_nCounterTutorial--;
		if (m_nCounterTutorial <= 0) {
			CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);
		}
	}
	else {
		m_nCounterRanking--;
		if (m_nCounterRanking <= 0) {
			CManager::GetInstance()->GetFade()->Set(CScene::MODE_RANKING);
		}
	}

	CScene::Update();
}

//===============================================
// 描画処理
//===============================================
void CTitle::Draw(void)
{
	CScene::Draw();
}
