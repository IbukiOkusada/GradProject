//===============================================
//
// リザルト画面の管理処理 [result.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "result.h"
#include "object2D.h"
#include "texture.h"
#include "manager.h"
#include "main.h"
#include "input.h"
#include "fade.h"
#include "ranking.h"
#include "sound.h"
#include "result.h"
#include "camera.h"
#include "ranking.h"
#include "object_manager.h"
#include "time.h"
#include "input_gamepad.h"
#include "input_keyboard.h"
#include "timer.h"
#include "player_manager.h"
#include "player.h"
#include "objectX.h"
#include "map_manager.h"
#include "player_result.h"


//==========================================================
// 定数定義
//==========================================================
namespace OBJ
{
	const char* DELI_TEX_PATH = "data\\TEXTURE\\result_deli.png";
	const float DELI_WIDTH = 120.0f;		// 横幅

	const char* TIME_TEX_PATH = "data\\TEXTURE\\result_time.png";
	const float TIME_WIDTH = 150.0f;		// 横幅
	const float TIME_POS_ADJUST = 30.0f;	// 位置の調整

	const char* LIFE_TEX_PATH = "data\\TEXTURE\\result_life.png";
	const float LIFE_WIDTH = 120.0f;		// 横幅

	const float HEIGHT = 50.0f;			// 高さ
	const float INTERVAL_Y = 110.0f;	// 間隔
	const D3DXVECTOR3 POS = D3DXVECTOR3(300.0f, 220.0f, 0.0f);
}

namespace NUMBER
{
	const float HEIGHT = 50.0f;			// 高さ
	const float WIDTH = 25.0f;			// 横幅
	const float INTERVAL = 50.0f;		// 数字の間隔
	const float INTERVAL_OBJ = 60.0f;	// 文字のオブジェクトとの間隔
	const int TIME_OBJ = 3;				// タイムのオブジェクトの数
	const int LIFE_OBJ = 3;				// 体力のオブジェクトの数
}

namespace PLAYER
{
	const D3DXVECTOR3 POS = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

namespace
{
	const float ALPHA_ADD = 0.015f;
	const D3DXCOLOR INIT_COL = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
}
// 静的メンバ変数
int CResult::m_nScore = 0;
int CResult::m_nDeli = 0;
float CResult::m_fTime = 0.0f;
float CResult::m_fLife = 0.0f;

//===============================================
// コンストラクタ
//===============================================
CResult::CResult()
{
	m_pMeshSky = NULL;
	m_pTime = NULL;
	m_pObjClear = nullptr;
	m_TimeObj[0] = {};
	m_LifeObj[0] = {};
	m_Display = 0;
}

//===============================================
// デストラクタ
//===============================================
CResult::~CResult()
{

}

//===============================================
// 初期化処理
//===============================================
HRESULT CResult::Init(void)
{

	//カメラ初期化
	{
		CManager::GetInstance()->GetCamera()->SetLength(300.0f);
		CManager::GetInstance()->GetCamera()->SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		CManager::GetInstance()->GetCamera()->SetPositionR(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		D3DVIEWPORT9 viewport;

		//プレイヤー追従カメラの画面位置設定
		viewport.X = 0;
		viewport.Y = 0;
		viewport.Width = (DWORD)(SCREEN_WIDTH * 1.0f);
		viewport.Height = (DWORD)(SCREEN_HEIGHT * 1.0f);
		viewport.MinZ = 0.0f;
		viewport.MaxZ = 1.0f;
		CManager::GetInstance()->GetCamera()->SetViewPort(viewport);
	}

	// マップ読み込み
	CMapManager::GetInstance()->Load();

	m_pPlayer = CPlayerResult::Create(PLAYER::POS, D3DXVECTOR3(0.0f, 0.0f, 0.0f), VECTOR3_ZERO, nullptr, nullptr);

	// 今回のスコアを取得
	m_nDeli = CManager::GetInstance()->GetDeliveryStatus();
	m_fTime = CTimer::GetTime();
	m_fLife = CManager::GetInstance()->GetLife();

	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		switch (nCnt)
		{
		case 0:
			// 届けた数の文字のオブジェクト生成
			m_pObj[nCnt] = CObject2D::Create(D3DXVECTOR3(OBJ::POS.x, OBJ::POS.y, 0.0f),
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),
				7);

			m_pObj[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::DELI_TEX_PATH));
			m_pObj[nCnt]->SetSize(OBJ::DELI_WIDTH, OBJ::HEIGHT);
			m_pObj[nCnt]->SetCol(INIT_COL);

			// 届けた数のオブジェクト生成
			m_pDeliObject2D = CNumber::Create(D3DXVECTOR3(
				m_pObj[nCnt]->GetPosition().x + OBJ::DELI_WIDTH + NUMBER::INTERVAL_OBJ,
				m_pObj[nCnt]->GetPosition().y,
				0.0f),
				NUMBER::WIDTH,
				NUMBER::HEIGHT);

			m_pDeliObject2D->SetIdx(m_nDeli);
			m_pDeliObject2D->GetObject2D()->SetCol(INIT_COL);

			break;

		case 1:
			// 残りタイムの文字のオブジェクト生成
			m_pObj[nCnt] = CObject2D::Create(D3DXVECTOR3(OBJ::POS.x + OBJ::TIME_POS_ADJUST,
				OBJ::POS.y + OBJ::INTERVAL_Y * nCnt,
				0.0f),
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),
				7);

			m_pObj[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::TIME_TEX_PATH));
			m_pObj[nCnt]->SetSize(OBJ::TIME_WIDTH, OBJ::HEIGHT);
			m_pObj[nCnt]->SetCol(INIT_COL);

			// 残りタイムのオブジェクト生成
			for (int Cnt = 0; Cnt < 3; Cnt++)
			{
				m_pTimeObject2D[Cnt] = CNumber::Create(D3DXVECTOR3(
					m_pObj[nCnt]->GetPosition().x + OBJ::TIME_WIDTH + NUMBER::INTERVAL_OBJ + NUMBER::INTERVAL * Cnt,
					m_pObj[nCnt]->GetPosition().y,
					0.0f),
					NUMBER::WIDTH,
					NUMBER::HEIGHT);

				Calculation(&m_TimeObj[Cnt], m_fTime, Cnt, NUMBER::TIME_OBJ);
				m_pTimeObject2D[Cnt]->SetIdx(m_TimeObj[Cnt]);
				m_pTimeObject2D[Cnt]->GetObject2D()->SetCol(INIT_COL);
			}

			break;

		case 2:
			// 残り体力の文字のオブジェクト生成
			m_pObj[nCnt] = CObject2D::Create(D3DXVECTOR3(OBJ::POS.x,
				OBJ::POS.y + OBJ::INTERVAL_Y * nCnt,
				0.0f),
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),
				7);

			m_pObj[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::LIFE_TEX_PATH));
			m_pObj[nCnt]->SetSize(OBJ::LIFE_WIDTH, OBJ::HEIGHT);
			m_pObj[nCnt]->SetCol(INIT_COL);

			// 残り体力のオブジェクト生成
			for (int Cnt = 0; Cnt < 3; Cnt++)
			{
				m_pLifeObject2D[Cnt] = CNumber::Create(D3DXVECTOR3(
					m_pObj[nCnt]->GetPosition().x + OBJ::LIFE_WIDTH + NUMBER::INTERVAL_OBJ + NUMBER::INTERVAL * Cnt,
					m_pObj[nCnt]->GetPosition().y,
					0.0f),
					NUMBER::WIDTH,
					NUMBER::HEIGHT);
				Calculation(&m_LifeObj[Cnt], m_fLife, Cnt, NUMBER::LIFE_OBJ);
				m_pLifeObject2D[Cnt]->SetIdx(m_LifeObj[Cnt]);
				m_pLifeObject2D[Cnt]->GetObject2D()->SetCol(INIT_COL);
			}

			break;

		default:
			break;
		}
	}

	//CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_RESULT);

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CResult::Uninit(void)
{
	for (int Cnt = 0; Cnt < 3; Cnt++)
	{
		if (m_pObj[Cnt] != NULL)
		{
			m_pObj[Cnt]->Uninit();
			m_pObj[Cnt] = NULL;
		}
	}

	if (m_pDeliObject2D != NULL)
	{
		m_pDeliObject2D->Uninit();
		m_pDeliObject2D = NULL;
	}

	for (int Cnt = 0; Cnt < 3; Cnt++)
	{
		if (m_pTimeObject2D[Cnt] != NULL)
		{
			m_pTimeObject2D[Cnt]->Uninit();
			m_pTimeObject2D[Cnt] = NULL;
		}
	}

	for (int Cnt = 0; Cnt < 3; Cnt++)
	{
		if (m_pLifeObject2D[Cnt] != NULL)
		{
			m_pLifeObject2D[Cnt]->Uninit();
			m_pLifeObject2D[Cnt] = NULL;
		}
	}

	CRanking::SetScore(m_nScore);
	m_nScore = 0;
	CManager::GetInstance()->GetCamera()->SetActive(true);
	CManager::GetInstance()->GetSound()->Stop();
}

//===============================================
// 更新処理
//===============================================
void CResult::Update(void)
{
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();
	CInputPad* pPad = CInputPad::GetInstance();

	if (pPad->GetTrigger(CInputPad::BUTTON_A, 0) ||
		pPad->GetTrigger(CInputPad::BUTTON_START, 0) ||
		pKey->GetTrigger(DIK_RETURN))
	{
		CManager::GetInstance()->GetFade()->Set(CScene::MODE_RANKING);
	}

	if (CManager::GetInstance()->GetFade()->GetState() == CFade::STATE_NONE)
	{
		D3DXCOLOR col;

		switch (m_Display)
		{
		case 0:
			col = m_pObj[m_Display]->GetCol();

			col.a += ALPHA_ADD;
			m_pObj[m_Display]->SetCol(col);
			m_pDeliObject2D->GetObject2D()->SetCol(col);

			if (col.a >= 1.0f)
			{
				col.a = 1.0f;
				m_Display++;
			}

			break;

		case 1:
			col = m_pObj[m_Display]->GetCol();

			col.a += ALPHA_ADD;
			m_pObj[m_Display]->SetCol(col);
			for (int Cnt = 0; Cnt < 3; Cnt++)
			{
				m_pTimeObject2D[Cnt]->GetObject2D()->SetCol(col);
			}

			if (col.a >= 1.0f)
			{
				col.a = 1.0f;
				m_Display++;
			}
			break;

		case 2:
			col = m_pObj[m_Display]->GetCol();

			col.a += ALPHA_ADD;
			m_pObj[m_Display]->SetCol(col);
			for (int Cnt = 0; Cnt < 3; Cnt++)
			{
				m_pLifeObject2D[Cnt]->GetObject2D()->SetCol(col);
			}

			if (col.a >= 1.0f)
			{
				col.a = 1.0f;
				m_Display++;
			}
			break;

		default:
			break;
		}
	}

	CScene::Update();
}

//===============================================
// 描画処理
//===============================================
void CResult::Draw(void)
{
	CScene::Draw();
}

//===============================================
// ナンバー表示のための計算
//===============================================
void CResult::Calculation(int* Obj, float Score, int Cnt, int ObjMax)
{
	int nCal = 1;
	bool bFirst = false;

	if (Cnt == 0)
	{
		Cnt++;
		bFirst = true;
	}

	for(int nCnt = Cnt; nCnt < ObjMax; nCnt++)
	{
		nCal = nCal * 10;
	}

	if (bFirst == true)
	{
		Cnt--;
	}

	switch (Cnt)
	{
	case 0:
		*Obj = ((int)Score) / nCal;
		break;

	case NUMBER::LIFE_OBJ - 1:
		*Obj = ((int)Score) % nCal;
		break;

	default:
		*Obj = ((int)Score) % nCal / (nCal / 10);
		break;
	}
}