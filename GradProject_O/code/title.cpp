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
#include "input_gamepad.h"
#include "input_keyboard.h"
#include "object2D.h"
#include "map_manager.h"
#include "meshfield.h"
#include "player.h"
#include "goal.h"

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
	m_nCounter = 0;
	m_bPush = false;
	m_pFileLoad = nullptr;
	m_pEnter = nullptr;
	m_bDisplay = false;
	m_eState = STATE::STATE_TEAMLOGO;
	m_bCol = false;

	for (int nCnt = 0; nCnt < OBJ2D_MAX; nCnt++)
	{//配列の初期化
		m_pObject2D[nCnt] = nullptr;
	}

	m_bIniting = false;

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
	//CCamera* pCamera = CManager::GetInstance()->GetCamera();
	//pCamera->SetPositionV(D3DXVECTOR3(-874.3f, 0.0f, 1717.2f));
	//pCamera->SetPositionR(D3DXVECTOR3(80.0f, 95.0f, 220.0f));
	//pCamera->SetLength(350.0f);
	//pCamera->SetRotation(D3DXVECTOR3(0.0f, -2.1f, 1.79f));
	//pCamera->SetActive(false);

	//カメラ初期状態
	CCamera* pCam = CManager::GetInstance()->GetCamera();
	pCam->SetPositionV(D3DXVECTOR3(-874.3f, 0.0f, 1717.2f));
	pCam->SetPositionR(D3DXVECTOR3(80.0f, 95.0f, 220.0f));
	pCam->SetLength(350.0f);
	pCam->SetRotation(D3DXVECTOR3(0.0f, -2.1f, 1.79f));
	pCam->SetActive(false);

	// 遷移タイマー設定
	m_nCounterTutorial = MOVE_TUTORIAL;
	m_nCounterRanking = AUTOMOVE_RANKING;

	// サウンド再生
	CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_TITLE);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo] = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), VECTOR3_ZERO);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetSize(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);
	m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\logo_thaw.jpg"));

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CTitle::Uninit(void)
{
	CManager::GetInstance()->GetSound()->Stop();
}

//===============================================
// 更新処理
//===============================================
void CTitle::Update(void)
{
	//ステートごとに処理分け
	switch (m_eState)
	{
		//チームロゴステートだった場合
	case STATE::STATE_TEAMLOGO:

		//キーボード入力かパッド入力があれば
		if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) ||
			CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) ||
			CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0)) {
			m_bPush = true;
		}

		//押された場合
		if (m_bPush)
		{
			StateLogo();
		}
		else
		{
			m_nCounterTutorial--;
			if (m_nCounterTutorial <= 0)
			{
				StateLogo();
			}
		}

		break;

		//プレスエンターステートだった場合
	case STATE::STATE_PRESSENTER:

		InitingP_E();
		StateP_E();

		break;

		//プレスエンターを押した後のステートだった場合
	case STATE::STATE_CHASING:

		break;

		//アイスを投げ入れるステートだった場合
	case STATE::STATE_ICETHROW:

		break;

		//それ以外だった場合
	default:

		break;
		
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
//===============================================
// 描画処理
//===============================================
void CTitle::ColChange(void)
{
	// 表示されていなければ
	if (!m_bDisplay)
	{

		// チームロゴの色情報を取得
		D3DXCOLOR TeamCol = m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->GetCol();

		TeamCol.a -= 1.0f / 30;	//透明に近づけていく

		if (TeamCol.a <= 0.0f)
		{//完全に透明になった場合
			TeamCol.a = 0.0f;	//透明度を透明に

			if (m_pObject2D[OBJ2D::OBJ2D_TeamLogo] != NULL)
			{
				m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetDraw(false);
			}
		}
		m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetCol(TeamCol);
	}
}
//===============================================
// チームロゴステートでの動き
//===============================================
void CTitle::StateLogo(void)
{
	
	// チームロゴの色情報を取得
	D3DXCOLOR TeamCol = m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->GetCol();

	//ステートがチームロゴだった場合
	if (m_eState == STATE::STATE_TEAMLOGO)
	{
		if(!m_bCol)
		{
			TeamCol.a -= 1.0f / 30;	//透明に近づけていく

			if (TeamCol.a <= 0.0f)
			{//完全に透明になった場合
				TeamCol.a = 0.0f;	//透明度を透明に

				m_nCounter++;

				if (m_nCounter >= 100)
				{
					m_bCol = true;

					m_nCounter = 0;
				}
				
			}
		}
		else
		{
			TeamCol.a += 1.0f / 30;	//透明に近づけていく

			if (TeamCol.a >= 1.0f)
			{//完全に透明になった場合
				TeamCol.a = 1.0f;	//透明度を透明に

				m_eState = STATE::STATE_PRESSENTER;
				m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetDraw(false);

			}
		}
		m_pObject2D[OBJ2D::OBJ2D_TeamLogo]->SetCol(TeamCol);
	}

}
//===============================================
//プレスエンターステートでの動き
//===============================================
void CTitle::StateP_E(void)
{
	CCamera* pCam = CManager::GetInstance()->GetCamera();


	pCam->SetActive(true);


	//キーボード入力かパッド入力があれば
	if (CInputKeyboard::GetInstance()->GetTrigger(DIK_RETURN) || 
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_START, 0) || 
		CInputPad::GetInstance()->GetTrigger(CInputPad::BUTTON_A, 0)) {
		m_bPush = true;
	}

	if (m_bPush)
	{
		CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);
	}
}
//===============================================
//プレスエンターステートでの動き
//===============================================
void CTitle::InitingP_E(void)
{
	//初期化済みではなければ
	if (!m_bIniting)
	{
		//押下情報を初期化
		m_bPush = false;

		//プレスエンター文字
		m_pObject2D[OBJ2D::OBJ2D_PressEnter] = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), VECTOR3_ZERO,5);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetSize(640.0f, 320.0f);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->SetDraw(true);
		m_pObject2D[OBJ2D::OBJ2D_PressEnter]->BindTexture(CManager::GetInstance()->GetTexture()->Regist("data\\TEXTURE\\T_PressEnter000.png"));


		//マップと地面もろもろの読み込み
		CMapManager::GetInstance()->Load();
		CMeshField::Create(D3DXVECTOR3(0.0f, -10.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1000.0f, 1000.0f, "data\\TEXTURE\\field000.jpg", 30, 30);


		m_bIniting = true;
	}
}