//===============================================
//
// マルチリザルト画面の管理処理 [multi_result.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _MULTI_RESULT_H_		// このマクロが定義されていない場合
#define _MULTI_RESULT_H_		// 二重インクルード防止用マクロを定義

#include "manager.h"

// 前方宣言
class CMultiResultManager;

//===============================================
// リザルトクラスの定義(派生クラス)
//===============================================
class CMultiResult : public CScene
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
	CMultiResult();	// コンストラクタ
	~CMultiResult();	// デストラクタ

	// メンバ関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:

	enum TYPE_OBJ
	{
		TYPE_TIME = 0,
		TYPE_LIFE,
		TYPE_EVAL,
		TYPE_RANKING,
		TYPE_NUM
	};

	// メンバ変数
	CFileLoad* m_pFileLoad;			// ファイル読み込みのポインタ
	CTime* m_pTime;					// タイマー
	CMultiResultManager* m_pMgr;	// マネージャー
};

#endif
