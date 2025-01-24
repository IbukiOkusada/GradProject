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
	static void SetScore(const int nScore) { m_nScore = static_cast<float>(nScore); }

private:

	enum TYPE_OBJ
	{
		TYPE_TIME = 0,
		TYPE_LIFE,
		TYPE_EVAL,
		TYPE_RANKING,
		TYPE_NUM
	};
	void ScoreObjCreat();	// 今回のスコアのオブジェクトを生成
	void RankObjCreat();	// ランキングのオブジェクトを生成

	void Calculation(int* Obj, float Score, int Cnt, int ObjMax, TYPE_OBJ Type);
	void AlphaJudge(float Alpha);		// アルファ値の判定
	void RankAlphaJudge(float Alpha);	// アルファ値の判定
	void Display();						// 表示処理
	void DisplayRanking();				// ランキングの表示処理
	void AllDisplay();					// 全て表示処理
	void RoadScore();					// ランキングのスコアをロード
	void SaveScore();					// ランキングのスコアをセーブ
	void SortScore();					// ランキングのスコアをソート


	// メンバ変数
	CFileLoad *m_pFileLoad;			// ファイル読み込みのポインタ
	CMeshDome *m_pMeshSky;			// 空用
	CTime *m_pTime;					// タイマー
	CObject2D *m_pObjClear;			// clearしたかどうか
	CPlayerResult* m_pPlayer;		// リザルトプレイヤーのポインタ

	CObject2D* m_pObj;					// 失敗or成功のオブジェクト
	CObject2D* m_pRankingObj;			// ランキングのオブジェクト
	CObject2D* m_pScoreObj[4];			// 各スコアの文字のオブジェクト
	CObject2D* m_pEvalStarObj[5];		// 総合スコアの星のオブジェクト
	CObject2D* m_pEvalStarFreamObj[5];	// 総合スコアの星の枠のオブジェクト
	CObject2D* m_pStarObj[25];			// 星のオブジェクト
	CObject2D* m_pStarFreamObj[25];		// 星の枠のオブジェクト
	CObject2D* m_pDecPointEval;			// 総合スコアの小数点のオブジェクト
	CObject2D* m_pDecPointRank[5];		// ランキングの小数点のオブジェクト
	CNumber* m_pDeliNumber;				// 届けた数のオブジェクト
	CNumber* m_pTimeNumber[3];			// 残りタイムのオブジェクト
	CNumber* m_pLifeNumber[3];			// 残り体力のオブジェクト
	CNumber* m_pEvalNumber[2];			// 総合スコアのオブジェクト
	CNumber* m_pRankingNumber[10];		// ランキングのオブジェクト

	static int m_nDeli;			// 配達した数
	static float m_fTime;		// 残り時間
	static float m_fLife;		// 残り体力
	static float m_nScore;		// 総合スコア
	static bool m_bAuto;
	float m_RankingScore[5];	// ランキングスコア

	int m_TimeObj[3];			// タイムのオブジェクトに必要な変数
	int m_LifeObj[3];			// 体力のオブジェクトに必要な変数
	int m_EvalObj[2];			// 総合スコアのオブジェクトに必要な変数
	int m_RankingObj[10];		// ランキングスコアのオブジェクトに必要な変数
	int m_Display;				// 表示させるオブジェクトの変数
	int m_DisplayRank;			// ランキングの表示させるオブジェクトの変数
	int m_Timehid;
	int m_Lifehid;
	bool m_bSuccess;			// クリアしたかどうか
	bool m_bAllDisp;			// すべて表示したかどうか
};

#endif