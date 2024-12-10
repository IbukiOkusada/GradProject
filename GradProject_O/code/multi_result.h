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
class CScrollText2D;

//===============================================
// リザルトクラスの定義(派生クラス)
//===============================================
class CMultiResult : public CScene
{
private:

	// プレイヤー情報
	struct SPlayerInfo
	{
		bool bActive;	// 使用されているか否か
		int nScore;	// スコア
		int nId;	// ID
		CScrollText2D* pString;	// 文字

		// コンストラクタ
		SPlayerInfo(bool _active = false,int _score = 0, int _id = 0, CScrollText2D* _string = nullptr) :bActive(_active), nScore(_score), nId(_id), pString(_string) {}
	};

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

	// メンバ関数
	void Sort();

	// メンバ変数
	CMultiResultManager* m_pMgr;	// マネージャー
	SPlayerInfo* m_pInfo;			// 情報
	int m_nNowScrPlayer;

};

#endif
