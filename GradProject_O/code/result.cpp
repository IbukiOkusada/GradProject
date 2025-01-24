//===============================================
//
// リザルト画面の管理処理 [result.cpp]
// Author : Kenta Hashimoto
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
#include "meshfield.h"
#include "edit_manager.h"
#include "input_keyboard.h"
#include "motion.h"
#include "character.h"

//==========================================================
// 定数定義
//==========================================================
namespace OBJ
{
	const char* CLEAR_TEX_PATH = "data\\TEXTURE\\result\\result_clear.png";
	const char* FAIL_TEX_PATH = "data\\TEXTURE\\result\\result_fail.png";
	const float CLEAR_WIDTH = 250.0f;		// 横幅
	const float CLEAR_HEIGHT = 70.0f;		// 高さ
	const D3DXVECTOR3 CLEAR_POS = D3DXVECTOR3(300.0f, 100.0f, 0.0f);

	const char* DELI_TEX_PATH = "data\\TEXTURE\\result\\result_deli.png";
	const char* DELI_TEX_PATH_MAX = "data\\TEXTURE\\result\\result_deli_max.png";	// 最大評価の時のテクスチャ
	const float DELI_WIDTH = 120.0f;		// 横幅

	const char* TIME_TEX_PATH = "data\\TEXTURE\\result\\result_time.png";
	const char* TIME_TEX_PATH_MAX = "data\\TEXTURE\\result\\result_time_max.png";	// 最大評価の時のテクスチャ
	const float TIME_WIDTH = 150.0f;		// 横幅
	const float TIME_POS_ADJUST = 30.0f;	// 位置の調整

	const char* LIFE_TEX_PATH = "data\\TEXTURE\\result\\result_life.png";
	const char* LIFE_TEX_PATH_MAX = "data\\TEXTURE\\result\\result_life_max.png";	// 最大評価の時のテクスチャ
	const float LIFE_WIDTH = 120.0f;		// 横幅

	const char* EVAL_TEX_PATH = "data\\TEXTURE\\result\\result_eval.png";
	const char* EVAL_TEX_PATH_MAX = "data\\TEXTURE\\result\\result_eval_max.png";	// 最大評価の時のテクスチャ
	const float EVAL_WIDTH = 90.0f;			// 横幅
	const float EVAL_HEIGHT = 40.0f;		// 高さ
	const D3DXVECTOR3 EVAL_POS = D3DXVECTOR3(110.0f, 600.0f, 0.0f);
	const float EVAL_STAR_POS_ADJUST = 130.0f;	// 位置の調整

	const char* STAR_TEX_PATH = "data\\TEXTURE\\result\\star_dot.png";
	const char* STAR_FREAM_TEX_PATH = "data\\TEXTURE\\result\\star_fream_dot.png";
	const float STAR_INTERVAL_X = 75.0f;							// 星の間隔
	const float STAR_INTERVAL_Y = 105.0f;							// 星の間隔
	const D3DXVECTOR3 STAR_POS = D3DXVECTOR3(800.0f, 660.0f, 0.0f);	// 初期位置

	const char* RANKING_TEX_PATH = "data\\TEXTURE\\result\\result_ranking.png";
	const float RANKING_WIDTH = 250.0f;		// 横幅
	const float RANKING_HEIGHT = 70.0f;		// 高さ
	const D3DXVECTOR3 RANKING_POS = D3DXVECTOR3(1000.0f, 100.0f, 0.0f);	// 初期位置

	const float HEIGHT_DECPOINT = 4.0f;  // 高さ
	const float WIDTH_DECPOINT = 4.0f;   // 横幅
	const char* TEX_PATH_DECPOINT = "data\\TEXTURE\\result\\Point_dot.png";

	const float HEIGHT = 40.0f;			// 高さ
	const float INTERVAL_Y = 100.0f;	// 縦の間隔
	const D3DXVECTOR3 POS = D3DXVECTOR3(250.0f, 240.0f, 0.0f);	// 初期位置
}

namespace NUMBER
{
	const float HEIGHT = 50.0f;			// 高さ
	const float WIDTH = 25.0f;			// 横幅
	const float INTERVAL = 50.0f;		// 数字の間隔
	const float INTERVAL_DEC = 65.0f;	// 小数の間隔
	const float INTERVAL_OBJ = 60.0f;	// 文字のオブジェクトとの間隔
	const int TIME_OBJ = 3;				// タイムのオブジェクトの数
	const int LIFE_OBJ = 3;				// 体力のオブジェクトの数
	const int EVAL_OBJ = 2;				// 総合評価のオブジェクトの数
	const int RANKING_OBJ = 2;			// ランキングのオブジェクトの数
	const char* TEX_PATH = "data\\TEXTURE\\result\\Number.png";
}

namespace PLAYER
{
	const D3DXVECTOR3 POS = D3DXVECTOR3(0.0f, 40.0f, 0.0f);
}

namespace
{
	const int SCORE_OBJ_NUM = 4;	// スコアのオブジェクトの個数
	const int RANKING_OBJ_NUM = 5;	// ランキングのオブジェクトの個数
	const int STAR_OBJ_NUM = 5;		// 星のオブジェクトの個数
	const int EVAL_STAR_OBJ_NUM = 5;// 星のオブジェクトの個数
	const float ALPHA_ADD = 0.025f;
	const D3DXCOLOR INIT_COL = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	const D3DXCOLOR DISPLAY_COL = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}
// 静的メンバ変数
int CResult::m_nDeli = 0;
float CResult::m_fTime = 0.0f;
float CResult::m_fLife = 0.0f;
float CResult::m_nScore = 0.0f;
bool CResult::m_bAuto = false;

//===============================================
// コンストラクタ
//===============================================
CResult::CResult()
{
	m_pMeshSky = NULL;
	m_pTime = NULL;
	m_pObjClear = nullptr;
	m_Display = 0;
	m_DisplayRank = 0;
	m_Timehid = 0;
	m_Lifehid = 0;
	m_bAllDisp = false;

	for (int Cnt = 0; Cnt < NUMBER::TIME_OBJ; Cnt++)
	{
		m_TimeObj[Cnt] = 0;
	}

	for (int Cnt = 0; Cnt < NUMBER::LIFE_OBJ; Cnt++)
	{
		m_LifeObj[Cnt] = 0;
	}

	for (int Cnt = 0; Cnt < NUMBER::EVAL_OBJ; Cnt++)
	{
		m_EvalObj[Cnt] = 0;
	}

	for (int CntFst = 0; CntFst < RANKING_OBJ_NUM; CntFst++)
	{
		for (int CntSec = 0; CntSec < NUMBER::RANKING_OBJ; CntSec++)
		{
			m_RankingObj[CntSec + CntFst * 2] = 0;
		}
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
	m_bSuccess = CManager::GetInstance()->GetSuccess();

	// 今回のスコアを取得
	m_nDeli = CManager::GetInstance()->GetDeliveryStatus();
	m_fTime = CTimer::GetTime();
	m_fLife = CManager::GetInstance()->GetLife();

	if (m_fLife <= 0.0f)
	{
		m_fTime = 0.0f;
	}

	// 総合スコアの計算
	m_nScore = ((float)m_nDeli + m_fTime / 50.0f + m_fLife / 100.0f) * 10.0f;

	if (m_nScore >= 50.0f)
	{
		m_nScore = 50.0f;
	}

	// 失敗or成功のオブジェクト生成
	m_pObj = CObject2D::Create(D3DXVECTOR3(OBJ::CLEAR_POS.x, OBJ::CLEAR_POS.y, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		7);

	m_pObj->SetSize(OBJ::CLEAR_WIDTH, OBJ::CLEAR_HEIGHT);
	if (m_bSuccess == true)
	{// 成功
		m_pObj->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::CLEAR_TEX_PATH));

		if (m_pPlayer != nullptr)
		{
			m_pPlayer->GetCharacteter()->GetMotion()->BlendSet(CPlayer::MOTION::MOTION_CLEAR);
		}

		CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_RESULT_SUC);
	}
	else if (m_bSuccess == false)
	{// 失敗
		m_pObj->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::FAIL_TEX_PATH));

		if (m_pPlayer != nullptr)
		{
			m_pPlayer->GetCharacteter()->GetMotion()->BlendSet(CPlayer::MOTION::MOTION_FAILED);
		}

		CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_RESULT_FAI);
	}

	// ランキングのオブジェクト生成
	m_pRankingObj = CObject2D::Create(D3DXVECTOR3(OBJ::RANKING_POS.x, OBJ::RANKING_POS.y, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		7);
	m_pRankingObj->SetSize(OBJ::RANKING_WIDTH, OBJ::RANKING_HEIGHT);
	m_pRankingObj->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::RANKING_TEX_PATH));
	m_pRankingObj->SetCol(INIT_COL);

	// 今回のスコアのオブジェクトを生成
	ScoreObjCreat();

	// ランキングの読み込み等の処理
	RoadScore();
	SortScore();
	SaveScore();

	// ランキングのオブジェクトを生成
	RankObjCreat();

	// 右側
	CMeshField::Create(D3DXVECTOR3(27250.0f, -10.0f, 3000.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1000.0f, 1000.0f, "data\\TEXTURE\\field000.jpg", 13, 16);

	// 左側
	CMeshField::Create(D3DXVECTOR3(-750.0f, -10.0f, 3000.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 500.0f, 500.0f, "data\\TEXTURE\\field001.jpg", 26, 32);


	//CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_RESULT);

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CResult::Uninit(void)
{
	CManager::GetInstance()->GetSound()->Stop();

	// 失敗or成功のオブジェクト
	if (m_pObj != NULL)
	{
		m_pObj->Uninit();
		m_pObj = NULL;
	}

	for (int Cnt = 0; Cnt < SCORE_OBJ_NUM; Cnt++)
	{
		// 総合スコアの数字
		if (m_pScoreObj[Cnt] != NULL)
		{
			m_pScoreObj[Cnt]->Uninit();
			m_pScoreObj[Cnt] = NULL;
		}
	}

	// 届けた数の数字
	if (m_pDeliNumber != NULL)
	{
		m_pDeliNumber->Uninit();
		m_pDeliNumber = NULL;
	}

	for (int Cnt = 0; Cnt < NUMBER::TIME_OBJ - m_Timehid; Cnt++)
	{
		// 残りタイムの数字
		if (m_pTimeNumber[Cnt] != NULL)
		{
			m_pTimeNumber[Cnt]->Uninit();
			m_pTimeNumber[Cnt] = NULL;
		}
	}

	for (int Cnt = 0; Cnt < NUMBER::LIFE_OBJ - m_Lifehid; Cnt++)
	{
		// 残り体力の数字
		if (m_pLifeNumber[Cnt] != NULL)
		{
			m_pLifeNumber[Cnt]->Uninit();
			m_pLifeNumber[Cnt] = NULL;
		}
	}

	for (int Cnt = 0; Cnt < NUMBER::EVAL_OBJ; Cnt++)
	{
		// 総合スコアの数字
		if (m_pEvalNumber[Cnt] != NULL)
		{
			m_pEvalNumber[Cnt]->Uninit();
			m_pEvalNumber[Cnt] = NULL;
		}
	}

	// 総合スコアの小数点
	if (m_pDecPointEval != NULL)
	{
		m_pDecPointEval->Uninit();
		m_pDecPointEval = NULL;
	}

	for (int Cnt = 0; Cnt < EVAL_STAR_OBJ_NUM; Cnt++)
	{
		// 総合スコアの星
		if (m_pEvalStarObj[Cnt] != NULL)
		{
			m_pEvalStarObj[Cnt]->Uninit();
			m_pEvalStarObj[Cnt] = NULL;
		}
	}

	for (int Cnt = 0; Cnt < EVAL_STAR_OBJ_NUM; Cnt++)
	{
		// 総合スコアの星の枠
		if (m_pEvalStarFreamObj[Cnt] != NULL)
		{
			m_pEvalStarFreamObj[Cnt]->Uninit();
			m_pEvalStarFreamObj[Cnt] = NULL;
		}
	}

	for (int Cnt = 0; Cnt < RANKING_OBJ_NUM; Cnt++)
	{
		// ランキングの小数点
		if (m_pDecPointRank[Cnt] != NULL)
		{
			m_pDecPointRank[Cnt]->Uninit();
			m_pDecPointRank[Cnt] = NULL;
		}
	}

	for (int CntFst = 0; CntFst < RANKING_OBJ_NUM; CntFst++)
	{
		for (int CntSec = 0; CntSec < NUMBER::RANKING_OBJ; CntSec++)
		{
			// ランキングの数字
			if (m_pRankingNumber[CntSec + CntFst * 2] != nullptr)
			{
				m_pRankingNumber[CntSec + CntFst * 2]->Uninit();
				m_pRankingNumber[CntSec + CntFst * 2] = nullptr;
			}
		}
	}

	for (int CntFst = 0; CntFst < RANKING_OBJ_NUM; CntFst++)
	{
		for (int CntSec = 0; CntSec < STAR_OBJ_NUM; CntSec++)
		{
			// ランキングの星
			if (m_pStarObj[CntSec + CntFst * STAR_OBJ_NUM] != nullptr)
			{
				m_pStarObj[CntSec + CntFst * STAR_OBJ_NUM]->Uninit();
				m_pStarObj[CntSec + CntFst * STAR_OBJ_NUM] = nullptr;
			}
		}
	}

	for (int CntFst = 0; CntFst < RANKING_OBJ_NUM; CntFst++)
	{
		for (int CntSec = 0; CntSec < STAR_OBJ_NUM; CntSec++)
		{
			// ランキングの星の枠
			if (m_pStarFreamObj[CntSec + CntFst * STAR_OBJ_NUM] != nullptr)
			{
				m_pStarFreamObj[CntSec + CntFst * STAR_OBJ_NUM]->Uninit();
				m_pStarFreamObj[CntSec + CntFst * STAR_OBJ_NUM] = nullptr;
			}
		}
	}

	// マップ情報廃棄
	CMapManager::Release();
	CRanking::SetScore(static_cast<int>(m_nScore));
	CManager::GetInstance()->GetCamera()->SetActive(true);
	CManager::GetInstance()->GetSound()->Stop();
	m_bAuto = false;
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
		if (m_bAllDisp == true)
		{
			CManager::GetInstance()->GetFade()->Set(CScene::MODE_TITLE);
		}

		else if (m_bAllDisp == false)
		{
			AllDisplay();
			m_bAllDisp = true;
		}
	}

	if (CManager::GetInstance()->GetFade()->GetState() == CFade::STATE_NONE)
	{
		if (m_bAllDisp == false)
		{
			Display();
		}
	}

	// エディター関連
#if _DEBUG

	CEditManager* pMgr = CEditManager::GetInstance();
	// エディター生成
	if (pKey->GetTrigger(DIK_F4) && CEditManager::GetInstance() == nullptr)
	{
		pMgr = CEditManager::Create();
	}

	// エディター更新
	if (pMgr != nullptr) { pMgr->Update(); }

#endif

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
// スコアオブジェクトの初期化処理
//===============================================
void CResult::ScoreObjCreat()
{
	float nStarTex = 0.0f;

	for (int nCnt = 0; nCnt < SCORE_OBJ_NUM; nCnt++)
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
			m_pDeliNumber->GetObject2D()->BindTexture(CManager::GetInstance()->GetTexture()->Regist(NUMBER::TEX_PATH));

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

			for (int Cnt = 0; Cnt < NUMBER::TIME_OBJ; Cnt++)
			{
				Calculation(&m_TimeObj[Cnt], m_fTime, Cnt, NUMBER::TIME_OBJ, CResult::TYPE_TIME);
			}

			for (int Cnt = 0; Cnt < NUMBER::TIME_OBJ; Cnt++)
			{
				if (Cnt == 0 && m_TimeObj[Cnt] == 0)
				{
					m_Timehid++;
				}
				else if (m_TimeObj[Cnt - 1] == 0 && m_TimeObj[Cnt] == 0 && Cnt != NUMBER::TIME_OBJ - 1)
				{
					m_Timehid++;
				}
				else
				{
					break;
				}
			}

			// 残りタイムのオブジェクト生成
			for (int Cnt = 0; Cnt < NUMBER::TIME_OBJ - m_Timehid; Cnt++)
			{
				m_pTimeNumber[Cnt] = CNumber::Create(D3DXVECTOR3(
					m_pScoreObj[nCnt]->GetPosition().x + OBJ::TIME_WIDTH + NUMBER::INTERVAL_OBJ + NUMBER::INTERVAL * Cnt,
					m_pScoreObj[nCnt]->GetPosition().y,
					0.0f),
					NUMBER::WIDTH,
					NUMBER::HEIGHT);

				m_pTimeNumber[Cnt]->SetIdx(m_TimeObj[Cnt + m_Timehid]);
				m_pTimeNumber[Cnt]->GetObject2D()->SetCol(INIT_COL);
				m_pTimeNumber[Cnt]->GetObject2D()->BindTexture(CManager::GetInstance()->GetTexture()->Regist(NUMBER::TEX_PATH));
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

			for (int Cnt = 0; Cnt < NUMBER::TIME_OBJ; Cnt++)
			{
				Calculation(&m_LifeObj[Cnt], m_fLife, Cnt, NUMBER::LIFE_OBJ, CResult::TYPE_LIFE);
			}

			for (int Cnt = 0; Cnt < NUMBER::TIME_OBJ; Cnt++)
			{
				if (Cnt == 0 && m_LifeObj[Cnt] == 0)
				{
					m_Lifehid++;
				}
				else if (m_LifeObj[Cnt - 1] == 0 && m_LifeObj[Cnt] == 0 && Cnt != NUMBER::TIME_OBJ - 1)
				{
					m_Lifehid++;
				}
				else
				{
					break;
				}
			}

			// 残り体力のオブジェクト生成
			for (int Cnt = 0; Cnt < NUMBER::LIFE_OBJ - m_Lifehid; Cnt++)
			{
				m_pLifeNumber[Cnt] = CNumber::Create(D3DXVECTOR3(
					m_pScoreObj[nCnt]->GetPosition().x + OBJ::LIFE_WIDTH + NUMBER::INTERVAL_OBJ + NUMBER::INTERVAL * Cnt,
					m_pScoreObj[nCnt]->GetPosition().y,
					0.0f),
					NUMBER::WIDTH,
					NUMBER::HEIGHT);

				m_pLifeNumber[Cnt]->SetIdx(m_LifeObj[Cnt + m_Lifehid]);
				m_pLifeNumber[Cnt]->GetObject2D()->SetCol(INIT_COL);
				m_pLifeNumber[Cnt]->GetObject2D()->BindTexture(CManager::GetInstance()->GetTexture()->Regist(NUMBER::TEX_PATH));
			}

			break;

		case 3:

			// 総合評価のオブジェクト生成
			m_pScoreObj[nCnt] = CObject2D::Create(D3DXVECTOR3(OBJ::EVAL_POS.x, OBJ::EVAL_POS.y, 0.0f),
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),
				7);

			if (m_nScore >= 50.0f)
			{
				m_pScoreObj[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::EVAL_TEX_PATH_MAX));
			}
			else
			{
				m_pScoreObj[nCnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::EVAL_TEX_PATH));
			}
			m_pScoreObj[nCnt]->SetSize(OBJ::EVAL_WIDTH, OBJ::EVAL_HEIGHT);
			m_pScoreObj[nCnt]->SetCol(INIT_COL);

			// 星のオブジェクト生成
			for (int Cnt = 0; Cnt < EVAL_STAR_OBJ_NUM; Cnt++)
			{
				nStarTex = (m_nScore / 10) - Cnt;

				if (nStarTex >= 1.0f)
				{
					nStarTex = 1.0f;
				}
				else if (nStarTex <= 0.0f)
				{
					nStarTex = 0.0f;
				}

				// 星
				m_pEvalStarObj[Cnt] = CObject2D::Create(D3DXVECTOR3(
					OBJ::EVAL_POS.x + OBJ::EVAL_STAR_POS_ADJUST + OBJ::STAR_INTERVAL_X * Cnt,
					OBJ::EVAL_POS.y,
					0.0f),
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					7);

				m_pEvalStarObj[Cnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::STAR_TEX_PATH));
				m_pEvalStarObj[Cnt]->GetObject2D()->SetCol(INIT_COL);

				m_pEvalStarObj[Cnt]->SetVtxRatio(0.0f, 0.0f, 35.0f, 35.0f, nStarTex, 1.0f);
				m_pEvalStarObj[Cnt]->SetTex(0.0f, 0.0f, nStarTex, 1.0f);

				// 枠
				m_pEvalStarFreamObj[Cnt] = CObject2D::Create(D3DXVECTOR3(
					m_pEvalStarObj[Cnt]->GetPosition().x,
					m_pEvalStarObj[Cnt]->GetPosition().y,
					0.0f),
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),
					7);

				m_pEvalStarFreamObj[Cnt]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::STAR_FREAM_TEX_PATH));
				m_pEvalStarFreamObj[Cnt]->GetObject2D()->SetCol(INIT_COL);
				m_pEvalStarFreamObj[Cnt]->SetVtxRatio(0.0f, 0.0f, 35.0f, 35.0f, 1.0f, 1.0f);

			}

			// 総合評価のオブジェクト生成
			for (int Cnt = 0; Cnt < NUMBER::EVAL_OBJ; Cnt++)
			{
				m_pEvalNumber[Cnt] = CNumber::Create(D3DXVECTOR3(
					m_pEvalStarObj[EVAL_STAR_OBJ_NUM - 1]->GetPosition().x + NUMBER::INTERVAL_OBJ + NUMBER::INTERVAL_DEC * Cnt,
					m_pEvalStarObj[EVAL_STAR_OBJ_NUM - 1]->GetPosition().y,
					0.0f),
					NUMBER::WIDTH,
					NUMBER::HEIGHT);

				Calculation(&m_EvalObj[Cnt], m_nScore, Cnt, NUMBER::EVAL_OBJ, CResult::TYPE_EVAL);
				m_pEvalNumber[Cnt]->SetIdx(m_EvalObj[Cnt]);
				m_pEvalNumber[Cnt]->GetObject2D()->SetCol(INIT_COL);
				m_pEvalNumber[Cnt]->GetObject2D()->BindTexture(CManager::GetInstance()->GetTexture()->Regist(NUMBER::TEX_PATH));
			}

			m_pDecPointEval = CObject2D::Create(7);
			m_pDecPointEval->SetPosition(D3DXVECTOR3(m_pEvalNumber[0]->GetObject2D()->GetPosition().x + (m_pEvalNumber[1]->GetObject2D()->GetPosition().x - m_pEvalNumber[0]->GetObject2D()->GetPosition().x) * 0.55f,
				m_pEvalNumber[0]->GetObject2D()->GetPosition().y + (NUMBER::HEIGHT - OBJ::HEIGHT_DECPOINT) * 0.5f,
				0.0f));
			m_pDecPointEval->SetSize(OBJ::WIDTH_DECPOINT, OBJ::HEIGHT_DECPOINT);
			m_pDecPointEval->SetCol(INIT_COL);
			m_pDecPointEval->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::TEX_PATH_DECPOINT));
			m_pDecPointEval->SetDraw();

			break;

		default:
			break;
		}
	}

}

//===============================================
// スコアオブジェクトの初期化処理
//===============================================
void CResult::RankObjCreat()
{
	float nStarTex = 0.0f;

	for (int CntFst = 0; CntFst < RANKING_OBJ_NUM; CntFst++)
	{
		// 星のオブジェクト生成
		for (int CntSec = 0; CntSec < STAR_OBJ_NUM; CntSec++)
		{
			nStarTex = (m_RankingScore[CntFst] / 10) - CntSec;

			if (nStarTex >= 1.0f)
			{
				nStarTex = 1.0f;
			}
			else if (nStarTex <= 0.0f)
			{
				nStarTex = 0.0f;
			}

			// 星
			m_pStarObj[CntSec + CntFst * STAR_OBJ_NUM] = CObject2D::Create(D3DXVECTOR3(
				OBJ::STAR_POS.x + OBJ::STAR_INTERVAL_X * CntSec,
				OBJ::STAR_POS.y - OBJ::STAR_INTERVAL_Y * CntFst,
				0.0f),
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),
				7);

			m_pStarObj[CntSec + CntFst * STAR_OBJ_NUM]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::STAR_TEX_PATH));
			m_pStarObj[CntSec + CntFst * STAR_OBJ_NUM]->GetObject2D()->SetCol(INIT_COL);

			m_pStarObj[CntSec + CntFst * STAR_OBJ_NUM]->SetVtxRatio(0.0f, 0.0f, 35.0f, 35.0f, nStarTex, 1.0f);
			m_pStarObj[CntSec + CntFst * STAR_OBJ_NUM]->SetTex(0.0f, 0.0f, nStarTex, 1.0f);

			// 枠
			m_pStarFreamObj[CntSec + CntFst * STAR_OBJ_NUM] = CObject2D::Create(D3DXVECTOR3(
				m_pStarObj[CntSec + CntFst * STAR_OBJ_NUM]->GetPosition().x,
				m_pStarObj[CntSec + CntFst * STAR_OBJ_NUM]->GetPosition().y,
				0.0f),
				D3DXVECTOR3(0.0f, 0.0f, 0.0f),
				7);

			m_pStarFreamObj[CntSec + CntFst * STAR_OBJ_NUM]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::STAR_FREAM_TEX_PATH));
			m_pStarFreamObj[CntSec + CntFst * STAR_OBJ_NUM]->GetObject2D()->SetCol(INIT_COL);
			m_pStarFreamObj[CntSec + CntFst * STAR_OBJ_NUM]->SetVtxRatio(0.0f, 0.0f, 35.0f, 35.0f, 1.0f, 1.0f);

		}

		// ランキングのオブジェクト生成
		for (int CntSec = 0; CntSec < NUMBER::RANKING_OBJ; CntSec++)
		{
			m_pRankingNumber[CntSec + CntFst * NUMBER::RANKING_OBJ] = CNumber::Create(D3DXVECTOR3(
				m_pStarObj[(CntFst + 1) * STAR_OBJ_NUM - 1]->GetPosition().x + NUMBER::INTERVAL_OBJ + NUMBER::INTERVAL_DEC * CntSec,
				m_pStarObj[(CntFst + 1) * STAR_OBJ_NUM - 1]->GetPosition().y,
				0.0f),
				NUMBER::WIDTH,
				NUMBER::HEIGHT);

			Calculation(&m_RankingObj[CntSec + CntFst * NUMBER::RANKING_OBJ], m_RankingScore[CntFst], CntSec, NUMBER::RANKING_OBJ, CResult::TYPE_RANKING);
			m_pRankingNumber[CntSec + CntFst * NUMBER::RANKING_OBJ]->SetIdx(m_RankingObj[CntSec + CntFst * 2]);
			m_pRankingNumber[CntSec + CntFst * NUMBER::RANKING_OBJ]->GetObject2D()->SetCol(INIT_COL);
			m_pRankingNumber[CntSec + CntFst * NUMBER::RANKING_OBJ]->GetObject2D()->BindTexture(CManager::GetInstance()->GetTexture()->Regist(NUMBER::TEX_PATH));
		}

		m_pDecPointRank[CntFst] = CObject2D::Create(7);
		m_pDecPointRank[CntFst]->SetPosition(D3DXVECTOR3(m_pRankingNumber[CntFst * NUMBER::RANKING_OBJ]->GetObject2D()->GetPosition().x + (m_pRankingNumber[CntFst * NUMBER::RANKING_OBJ + 1]->GetObject2D()->GetPosition().x - m_pRankingNumber[CntFst * NUMBER::RANKING_OBJ]->GetObject2D()->GetPosition().x) * 0.55f,
			m_pRankingNumber[CntFst * NUMBER::RANKING_OBJ]->GetObject2D()->GetPosition().y + (NUMBER::HEIGHT - OBJ::HEIGHT_DECPOINT) * 0.5f,
			0.0f));
		m_pDecPointRank[CntFst]->SetSize(OBJ::WIDTH_DECPOINT, OBJ::HEIGHT_DECPOINT);
		m_pDecPointRank[CntFst]->SetCol(INIT_COL);
		m_pDecPointRank[CntFst]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(OBJ::TEX_PATH_DECPOINT));
		m_pDecPointRank[CntFst]->SetDraw();


	}
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

	case CResult::TYPE_RANKING:
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
// アルファ値の判定
//===============================================
void CResult::RankAlphaJudge(float Alpha)
{
	if (Alpha >= 1.0f)
	{
		m_DisplayRank++;

		if (m_DisplayRank >= RANKING_OBJ_NUM)
		{
			m_Display++;
		}
	}
}

//===============================================
// 表示処理
//===============================================
void CResult::Display()
{
	D3DXCOLOR col = INIT_COL;

	switch (m_Display)
	{
	case 0:		// 配達の数
		col = m_pScoreObj[m_Display]->GetCol();
		col.a += ALPHA_ADD;

		m_pScoreObj[m_Display]->SetCol(col);
		m_pDeliNumber->GetObject2D()->SetCol(col);

		break;

	case 1:		// 残りタイム
		col = m_pScoreObj[m_Display]->GetCol();
		col.a += ALPHA_ADD;

		m_pScoreObj[m_Display]->SetCol(col);
		for (int Cnt = 0; Cnt < NUMBER::TIME_OBJ - m_Timehid; Cnt++)
		{
			m_pTimeNumber[Cnt]->GetObject2D()->SetCol(col);
		}

		break;

	case 2:		// 残り体力
		col = m_pScoreObj[m_Display]->GetCol();
		col.a += ALPHA_ADD;

		m_pScoreObj[m_Display]->SetCol(col);
		for (int Cnt = 0; Cnt < NUMBER::LIFE_OBJ - m_Lifehid; Cnt++)
		{
			m_pLifeNumber[Cnt]->GetObject2D()->SetCol(col);
		}

		break;

	case 3:		// 総合スコア
		col = m_pScoreObj[m_Display]->GetCol();
		col.a += ALPHA_ADD;

		m_pScoreObj[m_Display]->SetCol(col);
		for (int Cnt = 0; Cnt < EVAL_STAR_OBJ_NUM; Cnt++)
		{
			m_pEvalStarObj[Cnt]->GetObject2D()->SetCol(col);
			m_pEvalStarFreamObj[Cnt]->GetObject2D()->SetCol(col);
		}
		for (int Cnt = 0; Cnt < NUMBER::EVAL_OBJ; Cnt++)
		{
			m_pEvalNumber[Cnt]->GetObject2D()->SetCol(col);
		}
		m_pDecPointEval->SetCol(col);

		break;

	case 4:		// ランキング
		col = m_pRankingObj->GetCol();
		col.a += ALPHA_ADD;

		m_pRankingObj->SetCol(col);

		break;

	case 5:		// ランキングの数字等の評価
		DisplayRanking();

		break;

	default:
		m_bAllDisp = true;

		break;
	}

	AlphaJudge(col.a);
}

//===============================================
// ランキングの表示処理
//===============================================
void CResult::DisplayRanking()
{
	D3DXCOLOR col = INIT_COL;

	col = m_pRankingNumber[m_DisplayRank * 2]->GetObject2D()->GetCol();
	col.a += ALPHA_ADD;

	for (int Cnt = 0; Cnt < NUMBER::RANKING_OBJ; Cnt++)
	{
		// ランキングスコアの表示
		m_pRankingNumber[Cnt + m_DisplayRank * NUMBER::RANKING_OBJ]->GetObject2D()->SetCol(col);
	}

	for (int Cnt = 0; Cnt < STAR_OBJ_NUM; Cnt++)
	{
		// 星の表示
		m_pStarObj[Cnt + m_DisplayRank * STAR_OBJ_NUM]->SetCol(col);
		m_pStarFreamObj[Cnt + m_DisplayRank * STAR_OBJ_NUM]->SetCol(col);
	}

	m_pDecPointRank[m_DisplayRank]->SetCol(col);

	RankAlphaJudge(col.a);
}

//===============================================
// ランキングの表示処理
//===============================================
void CResult::AllDisplay()
{
	// スコアオブジェクト
	for (int Cnt = 0; Cnt < SCORE_OBJ_NUM; Cnt++)
	{
		m_pScoreObj[Cnt]->SetCol(DISPLAY_COL);
	}

	// 配達の数字
	m_pDeliNumber->GetObject2D()->SetCol(DISPLAY_COL);

	// タイマーの数字
	for (int Cnt = 0; Cnt < NUMBER::TIME_OBJ - m_Timehid; Cnt++)
	{
		m_pTimeNumber[Cnt]->GetObject2D()->SetCol(DISPLAY_COL);
	}

	// 体力の数字
	for (int Cnt = 0; Cnt < NUMBER::LIFE_OBJ - m_Lifehid; Cnt++)
	{
		m_pLifeNumber[Cnt]->GetObject2D()->SetCol(DISPLAY_COL);
	}

	// タイマーの数字
	for (int Cnt = 0; Cnt < EVAL_STAR_OBJ_NUM; Cnt++)
	{
		m_pEvalStarObj[Cnt]->GetObject2D()->SetCol(DISPLAY_COL);
		m_pEvalStarFreamObj[Cnt]->GetObject2D()->SetCol(DISPLAY_COL);
	}

	// 総合スコアの数字
	for (int Cnt = 0; Cnt < NUMBER::EVAL_OBJ; Cnt++)
	{
		m_pEvalNumber[Cnt]->GetObject2D()->SetCol(DISPLAY_COL);
	}
	// 小数点
	m_pDecPointEval->SetCol(DISPLAY_COL);

	// ランキング
	m_pRankingObj->SetCol(DISPLAY_COL);

	for (int Cnt = 0; Cnt < NUMBER::RANKING_OBJ * RANKING_OBJ_NUM; Cnt++)
	{
		// ランキングスコアの表示
		m_pRankingNumber[Cnt]->GetObject2D()->SetCol(DISPLAY_COL);
	}

	for (int Cnt = 0; Cnt < STAR_OBJ_NUM * RANKING_OBJ_NUM; Cnt++)
	{
		// 星の表示
		m_pStarObj[Cnt]->SetCol(DISPLAY_COL);
		m_pStarFreamObj[Cnt]->SetCol(DISPLAY_COL);
	}

	for (int Cnt = 0; Cnt < RANKING_OBJ_NUM; Cnt++)
	{
		// ランキングの小数点
		m_pDecPointRank[Cnt]->SetCol(DISPLAY_COL);
	}
}

//===============================================
// ランキングスコアのロード
//===============================================
void CResult::RoadScore()
{
	std::ifstream pFile("data\\FILE\\map\\ranking.bin", std::ios::binary);

	if (!pFile.is_open())
	{
		// ファイルを開く
		std::ofstream File("data\\FILE\\map\\ranking.bin", std::ios::binary);

		for (int Cnt = 0; Cnt < RANKING_OBJ_NUM; Cnt++)
		{
			m_RankingScore[Cnt] = 0.0f;
		}
		return;
	}

	if (pFile)
	{
		pFile.read(reinterpret_cast<char*>(&m_RankingScore), sizeof(m_RankingScore) * 5);
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
		File.write(reinterpret_cast<char*>(&m_RankingScore), sizeof(m_RankingScore) * 5);
	}

	// ファイルを閉じる
	File.close();
}

//===============================================
// ランキングスコアのソート
//===============================================
void CResult::SortScore()
{
	float nScore = m_nScore;		// 比較用スコア
	int nTemp = -1;

	for (int nCnt = 0; nCnt < RANKING_OBJ_NUM; nCnt++)
	{
		if (m_RankingScore[nCnt] < nScore)
		{// 値が大きい場合
			nTemp = nCnt;	// 大きい番号を変更
			nScore = m_RankingScore[nCnt];
		}
	}

	if (nTemp != -1)
	{// 変更する場合
		m_RankingScore[nTemp] = m_nScore;
	}

	// 降順ソート
	for (int nCntFst = 0; nCntFst < RANKING_OBJ_NUM - 1; nCntFst++)
	{
		int nTempNum = nCntFst;	// 仮の一番大きい番号

		for (int nCntSec = nCntFst + 1; nCntSec < RANKING_OBJ_NUM; nCntSec++)
		{
			if (m_RankingScore[nCntSec] < m_RankingScore[nTempNum])
			{// 値が大きい場合
				nTempNum = nCntSec;	// 大きい番号を変更
			}
		}

		if (nTempNum != nCntFst)
		{// 変更する場合
			float fTemp = m_RankingScore[nCntFst];
			m_RankingScore[nCntFst] = m_RankingScore[nTempNum];
			m_RankingScore[nTempNum] = fTemp;
		}
	}
}