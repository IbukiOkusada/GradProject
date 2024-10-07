//===============================================
//
// タイトル画面の管理処理 [title.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _TITLE_H_		// このマクロが定義されていない場合
#define _TITLE_H_		// 二重インクルード防止用マクロを定義

#include "manager.h"

class CFileLoad;
class CTitleEnter;
class CPlayer;

//===============================================
// タイトルクラスの定義(派生クラス)
//===============================================
class CTitle : public CScene
{
public:

	// メンバ関数
	CTitle();	// コンストラクタ
	~CTitle();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:

	CFileLoad *m_pFileLoad;		// ファイル読み込みのポインタ
	CTitleEnter *m_pEnter;
	CPlayer* m_pPlayer;			// プレイヤーのポインタ
	int m_nCounterTutorial;		// チュートリアル及び人数選択画面遷移タイマー
	int m_nCounterRanking;		// ランキング自動遷移タイマー
	bool m_bPush;				// チュートリアル遷移に必要なボタンが押されているか
};

#endif