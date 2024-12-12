//===============================================
//
// リザルト画面の管理処理 [result.h]
// Author : Kenta Hashimoto
//
//===============================================
#ifndef _RESULT_H_		// このマクロが定義されていない場合
#define _RESULT_H_		// 二重インクルード防止用マクロを定義

#include "manager.h"

class CScore;
class CFileLoad;
class CMeshDome;
class CTime;
class CCharacter;
class CObject2D;
class CNumber;
class CObjectX;
class CPlayerResult;

#define MAX_RANK	(4)		// ランキングの順位数

//===============================================
// リザルトクラスの定義(派生クラス)
//===============================================
class CResult : public CScene
{
public:
	
	// 種類列挙型
	enum TYPE
	{
		TYPE_MULTI_WIN,		// マルチ勝利
		TYPE_MULTI_LOSE,	// マルチ敗北
		TYPE_MAX			// 通常
	};

public:

	// メンバ関数
	CResult();	// コンストラクタ
	~CResult();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static void SetScore(const int nScore) { m_nScore = nScore; }

private:

	enum TYPE_OBJ
	{
		TYPE_TIME = 0,
		TYPE_LIFE,
		TYPE_EVAL,
		TYPE_RANKING,
		TYPE_NUM
	};
	void ScoreObjInit();

	void Calculation(int* Obj, float Score, int Cnt, int ObjMax, TYPE_OBJ Type);
	void AlphaJudge(float Alpha);		// アルファ値の判定
	void RankAlphaJudge(float Alpha);	// アルファ値の判定
	void Display();						// 表示処理
	void DisplayRanking();				// ランキングの表示処理
	void RoadScore();
	void SaveScore();
	void SortScore();


	// メンバ変数
	CFileLoad *m_pFileLoad;			// ファイル読み込みのポインタ
	CMeshDome *m_pMeshSky;			// 空用
	CTime *m_pTime;					// タイマー
	CObject2D *m_pObjClear;			// clearしたかどうか
	CPlayerResult* m_pPlayer;		// リザルトプレイヤーのポインタ

	CObject2D* m_pObj;				// 失敗or成功のオブジェクト
	CObject2D* m_pScoreObj[4];		// 各スコアの文字のオブジェクト
	CObject2D* m_pStarObj[25];		// 星のオブジェクト
	CObject2D* m_pStarFreamObj[25];	// 星の枠のオブジェクト
	CNumber* m_pDeliNumber;			// 届けた数のオブジェクト
	CNumber* m_pTimeNumber[3];		// 残りタイムのオブジェクト
	CNumber* m_pLifeNumber[3];		// 残り体力のオブジェクト
	CNumber* m_pEvalNumber[2];		// 総合評価のオブジェクト
	CNumber* m_pRankingNumber[10];	// ランキングのオブジェクト

	static int m_nDeli;
	static float m_fTime;
	static float m_fLife;
	static float m_nScore;
	float m_RankingScore[5];

	int m_TimeObj[3];
	int m_LifeObj[3];
	int m_EvalObj[2];
	int m_RankingObj[10];
	int m_Display;
	int m_DisplayRank;

};

#endif