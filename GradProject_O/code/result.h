//===============================================
//
// リザルト画面の管理処理 [result.h]
// Author : Ibuki Okusada
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
		TYPE_NUM
	};

	void Calculation(int* Obj, float Score, int Cnt, int ObjMax, TYPE_OBJ Type);
	void AlphaJudge(float Alpha);

	// メンバ変数
	CFileLoad *m_pFileLoad;		// ファイル読み込みのポインタ
	CMeshDome *m_pMeshSky;		// 空用
	CTime *m_pTime;				// タイマー
	CObject2D *m_pObjClear;		// clearしたかどうか
	CPlayerResult* m_pPlayer;	// リザルトプレイヤーのポインタ

	CObject2D* m_pObj;			// オブジェクト
	CObject2D* m_pScoreObj[3];	// 各スコアの文字のオブジェクト
	CNumber* m_pDeliNumber;		// 届けた数のオブジェクト
	CNumber* m_pTimeNumber[3];	// 残りタイムのオブジェクト
	CNumber* m_pLifeNumber[3];	// 残り体力のオブジェクト
	CNumber* m_pEvalNumber[2];	// 総合評価のオブジェクト

	static int m_nDeli;
	static float m_fTime;
	static float m_fLife;
	static float m_nScore;

	int m_TimeObj[3];
	int m_LifeObj[3];
	int m_EvalObj[2];
	int m_Display;
};

#endif