//===============================================
//
// ランキング画面の管理処理 [ranking.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _RANKING_H_		// このマクロが定義されていない場合
#define _RANKING_H_		// 二重インクルード防止用マクロを定義

#include "manager.h"

class CScore;
class CFileLoad;
class CMeshDome;
class CTime;
class CCharacter;

#define NUM_RANK	(5)		// ランキングの順位数

//===============================================
// リザルトクラスの定義(派生クラス)
//===============================================
class CRanking : public CScene
{
public:

	// メンバ関数
	CRanking();	// コンストラクタ
	~CRanking();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static void SetScore(int nNum) { m_nScore = nNum; }

private:

	// メンバ関数
	void Save(int* pScore);
	void Load(int* pScore);
	void Sort(int* pScore);
	void RankIn(int* pScore, int nRanking);

	// メンバ変数
	CFileLoad* m_pFileLoad;	// ファイル読み込みのポインタ
	CMeshDome* m_pMeshSky;		// 空用
	CTime* m_pTime;			// タイマー
	CTime* m_apScore[NUM_RANK];	// スコアのポインタ(順位分)
	int m_nRank;				// 今回のランク
	int m_nTimer;				// 遷移タイマー
	static int m_nScore;		// 今回のスコア
};

#endif