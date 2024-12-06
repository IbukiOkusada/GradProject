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
	const char* CLEAR_TEX_PATH = "data\\TEXTURE\\result_clear.png";
	const float CLEAR_WIDTH = 250.0f;		// 横幅
	const float CLEAR_HEIGHT = 70.0f;		// 高さ
	const D3DXVECTOR3 CLEAR_POS = D3DXVECTOR3(300.0f, 100.0f, 0.0f);

	const char* DELI_TEX_PATH = "data\\TEXTURE\\result_deli.png";
	const char* DELI_TEX_PATH_MAX = "data\\TEXTURE\\result_deli_max.png";	// 最大評価の時のテクスチャ
	const float DELI_WIDTH = 120.0f;		// 横幅

	const char* TIME_TEX_PATH = "data\\TEXTURE\\result_time.png";
	const char* TIME_TEX_PATH_MAX = "data\\TEXTURE\\result_time_max.png";	// 最大評価の時のテクスチャ
	const float TIME_WIDTH = 150.0f;		// 横幅
	const float TIME_POS_ADJUST = 30.0f;	// 位置の調整

	const char* LIFE_TEX_PATH = "data\\TEXTURE\\result_life.png";
	const char* LIFE_TEX_PATH_MAX = "data\\TEXTURE\\result_life_max.png";	// 最大評価の時のテクスチャ
	const float LIFE_WIDTH = 120.0f;		// 横幅

	const char* EVAL_TEX_PATH = "data\\TEXTURE\\result_eval.png";
	const char* EVAL_TEX_PATH_MAX = "data\\TEXTURE\\result_eval_max.png";	// 最大評価の時のテクスチャ
	const float EVAL_WIDTH = 80.0f;			// 横幅
	const float EVAL_HEIGHT = 40.0f;		// 高さ
	const D3DXVECTOR3 EVAL_POS = D3DXVECTOR3(300.0f, 600.0f, 0.0f);

	const float HEIGHT = 40.0f;			// 高さ
	const float INTERVAL_Y = 100.0f;	// 間隔
	const D3DXVECTOR3 POS = D3DXVECTOR3(300.0f, 240.0f, 0.0f);
}

namespace NUMBER
{
	const float HEIGHT = 50.0f;			// 高さ
	const float WIDTH = 25.0f;			// 横幅
	const float INTERVAL = 50.0f;		// 数字の間隔
	const float INTERVAL_OBJ = 60.0f;	// 文字のオブジェクトとの間隔
	const int TIME_OBJ = 3;				// タイムのオブジェクトの数
	const int LIFE_OBJ = 3;				// 体力のオブジェクトの数
	const int EVAL_OBJ = 2;				// 総合評価のオブジェクトの数
}

namespace PLAYER
{
	const D3DXVECTOR3 POS = D3DXVECTOR3(0.0f, 40.0f, 0.0f);
}

namespace
{
	const int SCORE_OBJ_MAX = 4;
	const float ALPHA_ADD = 0.015f;
	const D3DXCOLOR INIT_COL = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
}
// 静的メンバ変数
int CResult::m_nDeli = 0;
float CResult::m_fTime = 0.0f;
float CResult::m_fLife = 0.0f;
float CResult::m_nScore = 0.0f;

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

	for (int Cnt = 0; Cnt < NUMBER::TIME_OBJ; Cnt++)
	{
		m_TimeObj[Cnt] = 0.0f;
	}

	for (int Cnt = 0; Cnt < NUMBER::LIFE_OBJ; Cnt++)
	{
		m_LifeObj[Cnt] = 0.0f;
	}

	for (int Cnt = 0; Cnt < NUMBER::EVAL_OBJ; Cnt++)
	{
		m_EvalObj[Cnt] = 0.0f;
	}

	for (int Cnt = 0; Cnt < 5; Cnt++)
	{
		m_RankinScore[Cnt] = 0.0f;
	}
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

	// マップ読み込み
	CMapManager::GetInstance()->Load();

	m_pPlayer = CPlayerResult::Create(PLAYER::POS, D3DXVECTOR3(0.0f, 0.0f, 0.0f), VECTOR3_ZERO, nullptr, nullptr);

	// 今回のスコアを取得
	m_nDeli = CManager::GetInstance()->GetDeliveryStatus();
	m_fTime = CTimer::GetTime();
	m_fLife = CManager::GetInstance()->GetLife();

	m_nScore = ((float)m_nDeli + m_fTime / 100.0f + m_fLife / 100.0f) * 10.0f;

	if (m_nScore >= 50.0f)
	{
		m_nScore = 50.0f;
	}


	for (int nCnt = 0; nCnt < SCORE_OBJ_MAX; nCnt++)
	{
		switch (nCnt)
		{
		case 0:
			// 届けた数の文字のオブジェクト生成
			m_pScoreObj[nCnt] = CObject2D::Create(D3DXVECTOR3(OBJ::POS.x, OBJ::POS.y, 0.0f),
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),
				7);

			if (m_nDeli >= 3)
			{
				m_pScoreObj[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::DELI_TEX_PATH_MAX));
			}
			else
			{
				m_pScoreObj[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::DELI_TEX_PATH));
			}
			m_pScoreObj[nCnt]->SetSize(OBJ::DELI_WIDTH, OBJ::HEIGHT);
			m_pScoreObj[nCnt]->SetCol(INIT_COL);

			// 届けた数のオブジェクト生成
			m_pDeliNumber = CNumber::Create(D3DXVECTOR3(
				m_pScoreObj[nCnt]->GetPosition().x + OBJ::DELI_WIDTH + NUMBER::INTERVAL_OBJ,
				m_pScoreObj[nCnt]->GetPosition().y,
				0.0f),
				NUMBER::WIDTH,
				NUMBER::HEIGHT);

			m_pDeliNumber->SetIdx(m_nDeli);
			m_pDeliNumber->GetObject2D()->SetCol(INIT_COL);

			break;

		case 1:
			// 残りタイムの文字のオブジェクト生成
			m_pScoreObj[nCnt] = CObject2D::Create(D3DXVECTOR3(OBJ::POS.x + OBJ::TIME_POS_ADJUST,
				OBJ::POS.y + OBJ::INTERVAL_Y * nCnt,
				0.0f),
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),
				7);

			if (m_fTime >= 100)
			{
				m_pScoreObj[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::TIME_TEX_PATH_MAX));
			}
			else
			{
				m_pScoreObj[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::TIME_TEX_PATH));
			}
			m_pScoreObj[nCnt]->SetSize(OBJ::TIME_WIDTH, OBJ::HEIGHT);
			m_pScoreObj[nCnt]->SetCol(INIT_COL);

			// 残りタイムのオブジェクト生成
			for (int Cnt = 0; Cnt < NUMBER::TIME_OBJ; Cnt++)
			{
				m_pTimeNumber[Cnt] = CNumber::Create(D3DXVECTOR3(
					m_pScoreObj[nCnt]->GetPosition().x + OBJ::TIME_WIDTH + NUMBER::INTERVAL_OBJ + NUMBER::INTERVAL * Cnt,
					m_pScoreObj[nCnt]->GetPosition().y,
					0.0f),
					NUMBER::WIDTH,
					NUMBER::HEIGHT);

				Calculation(&m_TimeObj[Cnt], m_fTime, Cnt, NUMBER::TIME_OBJ, CResult::TYPE_TIME);
				m_pTimeNumber[Cnt]->SetIdx(m_TimeObj[Cnt]);
				m_pTimeNumber[Cnt]->GetObject2D()->SetCol(INIT_COL);
			}

			break;

		case 2:
			// 残り体力の文字のオブジェクト生成
			m_pScoreObj[nCnt] = CObject2D::Create(D3DXVECTOR3(OBJ::POS.x,
				OBJ::POS.y + OBJ::INTERVAL_Y * nCnt,
				0.0f),
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),
				7);

			if (m_fLife >= 100)
			{
				m_pScoreObj[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::LIFE_TEX_PATH_MAX));
			}
			else
			{
				m_pScoreObj[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::LIFE_TEX_PATH));
			}
			m_pScoreObj[nCnt]->SetSize(OBJ::LIFE_WIDTH, OBJ::HEIGHT);
			m_pScoreObj[nCnt]->SetCol(INIT_COL);

			// 残り体力のオブジェクト生成
			for (int Cnt = 0; Cnt < NUMBER::LIFE_OBJ; Cnt++)
			{
				m_pLifeNumber[Cnt] = CNumber::Create(D3DXVECTOR3(
					m_pScoreObj[nCnt]->GetPosition().x + OBJ::LIFE_WIDTH + NUMBER::INTERVAL_OBJ + NUMBER::INTERVAL * Cnt,
					m_pScoreObj[nCnt]->GetPosition().y,
					0.0f),
					NUMBER::WIDTH,
					NUMBER::HEIGHT);
				Calculation(&m_LifeObj[Cnt], m_fLife, Cnt, NUMBER::LIFE_OBJ, CResult::TYPE_LIFE);
				m_pLifeNumber[Cnt]->SetIdx(m_LifeObj[Cnt]);
				m_pLifeNumber[Cnt]->GetObject2D()->SetCol(INIT_COL);
			}

			break;

		case 3:

			// 残り体力の文字のオブジェクト生成
			m_pScoreObj[nCnt] = CObject2D::Create(D3DXVECTOR3(OBJ::EVAL_POS.x, OBJ::EVAL_POS.y, 0.0f),
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),
				7);

			if (m_nScore >= 5.0f)
			{
				m_pScoreObj[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::EVAL_TEX_PATH_MAX));
			}
			else
			{
				m_pScoreObj[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::EVAL_TEX_PATH));
			}
			m_pScoreObj[nCnt]->SetSize(OBJ::EVAL_WIDTH, OBJ::EVAL_HEIGHT);
			m_pScoreObj[nCnt]->SetCol(INIT_COL);

			// 総合評価のオブジェクト生成
			for (int Cnt = 0; Cnt < NUMBER::EVAL_OBJ; Cnt++)
			{
				m_pEvalNumber[Cnt] = CNumber::Create(D3DXVECTOR3(
					m_pScoreObj[nCnt]->GetPosition().x + NUMBER::INTERVAL_OBJ * 2.5f + NUMBER::INTERVAL * Cnt,
					m_pScoreObj[nCnt]->GetPosition().y,
					0.0f),
					NUMBER::WIDTH,
					NUMBER::HEIGHT);

				Calculation(&m_EvalObj[Cnt], m_nScore, Cnt, NUMBER::EVAL_OBJ, CResult::TYPE_EVAL);
				m_pEvalNumber[Cnt]->SetIdx(m_EvalObj[Cnt]);
				m_pEvalNumber[Cnt]->GetObject2D()->SetCol(INIT_COL);
			}

			break;

		default:
			break;
		}
	}

	// 失敗or成功のオブジェクト生成
	m_pObj = CObject2D::Create(D3DXVECTOR3(OBJ::CLEAR_POS.x, OBJ::CLEAR_POS.y, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		7);

	m_pObj->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::CLEAR_TEX_PATH));
	m_pObj->SetSize(OBJ::CLEAR_WIDTH, OBJ::CLEAR_HEIGHT);

	//SaveScore();
	RoadScore();

	//CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_RESULT);

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CResult::Uninit(void)
{
	if (m_pObj != NULL)
	{
		m_pObj->Uninit();
		m_pObj = NULL;
	}

	for (int Cnt = 0; Cnt < SCORE_OBJ_MAX; Cnt++)
	{
		if (m_pScoreObj[Cnt] != NULL)
		{
			m_pScoreObj[Cnt]->Uninit();
			m_pScoreObj[Cnt] = NULL;
		}
	}

	if (m_pDeliNumber != NULL)
	{
		m_pDeliNumber->Uninit();
		m_pDeliNumber = NULL;
	}

	for (int Cnt = 0; Cnt < NUMBER::TIME_OBJ; Cnt++)
	{
		if (m_pTimeNumber[Cnt] != NULL)
		{
			m_pTimeNumber[Cnt]->Uninit();
			m_pTimeNumber[Cnt] = NULL;
		}
	}

	for (int Cnt = 0; Cnt < NUMBER::LIFE_OBJ; Cnt++)
	{
		if (m_pLifeNumber[Cnt] != NULL)
		{
			m_pLifeNumber[Cnt]->Uninit();
			m_pLifeNumber[Cnt] = NULL;
		}
	}

	for (int Cnt = 0; Cnt < NUMBER::EVAL_OBJ; Cnt++)
	{
		if (m_pEvalNumber[Cnt] != NULL)
		{
			m_pEvalNumber[Cnt]->Uninit();
			m_pEvalNumber[Cnt] = NULL;
		}
	}

	// マップ情報廃棄
	CMapManager::Release();
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
		CManager::GetInstance()->GetFade()->Set(CScene::MODE_TITLE);
	}

	if (CManager::GetInstance()->GetFade()->GetState() == CFade::STATE_NONE)
	{
		Display(m_Display);
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
void CResult::Calculation(int* Obj, float Score, int Cnt, int ObjMax, TYPE_OBJ Type)
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

	switch (Type)
	{
	case CResult::TYPE_TIME:
		switch (Cnt)
		{
		case 0:
			*Obj = ((int)Score) / nCal;
			break;

		case NUMBER::TIME_OBJ - 1:
			*Obj = ((int)Score) % nCal;
			break;

		default:
			*Obj = ((int)Score) % nCal / (nCal / 10);
			break;
		}
		break;


	case CResult::TYPE_LIFE:
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

		break;

	case CResult::TYPE_EVAL:
		switch (Cnt)
		{
		case 0:
			*Obj = ((int)Score) / nCal;
			break;

		case NUMBER::EVAL_OBJ - 1:
			*Obj = ((int)Score) % nCal;
			break;

		default:
			*Obj = ((int)Score) % nCal / (nCal / 10);
			break;
		}

		break;

	default:
		break;
	}
}

//===============================================
// アルファ値の判定
//===============================================
void CResult::AlphaJudge(float Alpha)
{
	if (Alpha >= 1.0f)
	{
		m_Display++;
	}
}

//===============================================
// 表示処理
//===============================================
void CResult::Display(int nDisplay)
{
	D3DXCOLOR col;

	if (nDisplay >= SCORE_OBJ_MAX)
	{
		return;
	}

	else
	{
		col = m_pScoreObj[nDisplay]->GetCol();

		col.a += ALPHA_ADD;
		m_pScoreObj[nDisplay]->SetCol(col);
	}

	switch (nDisplay)
	{
	case 0:
		m_pDeliNumber->GetObject2D()->SetCol(col);

		break;

	case 1:
		for (int Cnt = 0; Cnt < NUMBER::TIME_OBJ; Cnt++)
		{
			m_pTimeNumber[Cnt]->GetObject2D()->SetCol(col);
		}

		break;

	case 2:
		for (int Cnt = 0; Cnt < NUMBER::LIFE_OBJ; Cnt++)
		{
			m_pLifeNumber[Cnt]->GetObject2D()->SetCol(col);
		}

		break;

	case 3:
		for (int Cnt = 0; Cnt < NUMBER::EVAL_OBJ; Cnt++)
		{
			m_pEvalNumber[Cnt]->GetObject2D()->SetCol(col);
		}

		break;

	default:
		break;
	}


	AlphaJudge(col.a);
}

//===============================================
// ランキングスコアのロード
//===============================================
void CResult::RoadScore()
{
	std::ifstream pFile("data\\FILE\\map\\ranking.bin", std::ios::binary);

	if (!pFile.is_open())
	{
		return;
	}

	if (pFile)
	{
		pFile.read(reinterpret_cast<char*>(&m_RankinScore), sizeof(m_RankinScore) * 5);
	}

	// ファイルを閉じる
	pFile.close();
}

//===============================================
// ランキングスコアのセーブ
//===============================================
void CResult::SaveScore()
{
	// ファイルを開く
	std::ofstream File("data\\FILE\\map\\ranking.bin", std::ios::binary);
	
	if (!File.is_open()) 
	{
		return;
	}

	if (File)
	{
		File.write(reinterpret_cast<char*>(&m_RankinScore), sizeof(m_RankinScore) * 5);
	}

	// ファイルを閉じる
	File.close();
}