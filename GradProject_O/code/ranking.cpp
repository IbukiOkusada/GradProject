//===============================================
//
// ランキング画面の管理処理 [ranking.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "ranking.h"
#include "object2D.h"
#include "texture.h"
#include "manager.h"
#include "main.h"
#include "input.h"
#include "fade.h"
#include "ranking.h"
#include "sound.h"
#include "result.h"
#include "time.h"
#include "camera.h"

// マクロ定義
#define RANKING_FILE	"data\\FILE\\ranking.bin"	// ランキングファイル
#define MOVE_TIMER	(660)

namespace
{
	const char* FILEPASS = "data\\TXT\\player";	// ファイルのパス
	const char* FILEEXT = ".txt";				// ファイルの拡張子
	const int FILEPASS_SIZE = (200);			// ファイルのパスサイズ
}

int CRanking::m_nScore = 0;

//===============================================
// コンストラクタ
//===============================================
CRanking::CRanking()
{
	m_pMeshSky = NULL;
	m_nTimer = 0;
	m_pTime = NULL;
}

//===============================================
// デストラクタ
//===============================================
CRanking::~CRanking()
{

}

//===============================================
// 初期化処理
//===============================================
HRESULT CRanking::Init(void)
{
	int aScore[NUM_RANK] = {};	// スコア格納用
	m_nRank = -1;	//ランクインしてない状態

	// データの読み込み
	Load(&aScore[0]);

	// データのソート
	Sort(&aScore[0]);

	// ランクイン確認
	RankIn(&aScore[0], m_nScore);

	char aBodyPass[FILEPASS_SIZE] = "";		// 胴体パス
	char aLegPass[FILEPASS_SIZE] = "";		// 下半身パス

	sprintf(&aBodyPass[0], "%s\\motion_body%s", FILEPASS, FILEEXT);
	sprintf(&aLegPass[0], "%s\\motion_leg%s", FILEPASS, FILEEXT);

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CRanking::Uninit(void)
{
	CManager::GetInstance()->GetSound()->Stop();
	CManager::GetInstance()->GetCamera()->SetActive(true);
	m_nScore = 0;
}

//===============================================
// 更新処理
//===============================================
void CRanking::Update(void)
{
	m_nTimer++;

	if (CManager::GetInstance()->GetInputKeyboard()->GetTrigger(DIK_RETURN) || m_nTimer > MOVE_TIMER
		|| CManager::GetInstance()->GetInputPad()->GetTrigger(CInputPad::BUTTON_A, 0) || CManager::GetInstance()->GetInputPad()->GetTrigger(CInputPad::BUTTON_START, 0))
	{
		CManager::GetInstance()->GetFade()->Set(CScene::MODE_TITLE);
	}

	CScene::Update();
}

//===============================================
// 描画処理
//===============================================
void CRanking::Draw(void)
{
	CScene::Draw();
}

//===============================================
// ランキングデータ保存
//===============================================
void CRanking::Save(int* pScore)
{
	FILE* pFile;

	pFile = fopen(RANKING_FILE, "wb");

	if (pFile != NULL)
	{//ファイルが開けた場合

	 //データを読み込む
		fwrite(pScore, sizeof(int), NUM_RANK, pFile);

		//ファイルを閉じる
		fclose(pFile);
	}
}

//===============================================
// ランキングデータ読み込み
//===============================================
void CRanking::Load(int* pScore)
{
	FILE* pFile;

	pFile = fopen(RANKING_FILE, "rb");

	if (pFile != NULL)
	{//ファイルが開けた場合

	 //データを読み込む
		fread(pScore, sizeof(int), NUM_RANK, pFile);

		//ファイルを閉じる
		fclose(pFile);
	}
	else
	{//ファイルが開けなかった場合
	 //要素を入れておく
		for (int nCntRanking = 0; nCntRanking < NUM_RANK; nCntRanking++)
		{
			pScore[nCntRanking] = 6000 * 10 + (6000 * nCntRanking);
		}
	}
}

//===============================================
// ランキングデータソート
//===============================================
void CRanking::Sort(int* pScore)
{
	// 降順ソート
	for (int nCntFst = 0; nCntFst < NUM_RANK - 1; nCntFst++)
	{
		int nTempNum = nCntFst;	// 仮の一番大きい番号

		for (int nCntSec = nCntFst + 1; nCntSec < NUM_RANK; nCntSec++)
		{
			if (pScore[nCntSec] < pScore[nTempNum])
			{// 値が大きい場合
				nTempNum = nCntSec;	// 大きい番号を変更
			}
		}

		if (nTempNum != nCntFst)
		{// 変更する場合
			int nTemp = pScore[nCntFst];
			pScore[nCntFst] = pScore[nTempNum];
			pScore[nTempNum] = nTemp;
		}
	}
}

//===============================================
// ランキングイン確認
//===============================================
void CRanking::RankIn(int* pScore, int nRanking)
{
	if (nRanking < pScore[NUM_RANK - 1] && nRanking != 0)
	{
		pScore[NUM_RANK - 1] = nRanking;

		// ソート処理
		Sort(pScore);

		// 保存処理
		Save(pScore);

		//ランクインした順位を確認する
		for (int nCntRank = 0; nCntRank < NUM_RANK; nCntRank++)
		{
			if (pScore[nCntRank] == nRanking)
			{
				m_nRank = nCntRank;	// ランクインした順位を保存			
				break;
			}
		}
	}
}