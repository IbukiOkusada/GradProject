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

	// メンバ変数
	CFileLoad *m_pFileLoad;	// ファイル読み込みのポインタ
	CMeshDome *m_pMeshSky;	// 空用
	CTime *m_pTime;			// タイマー
	CObject2D *m_pObjClear;	// clearしたかどうか
	CPlayer *m_ppPlayer;	// プレイヤーのポインタ
	static int m_nScore;
};

#endif