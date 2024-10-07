//===============================================
//
// ゲーム画面の管理処理 [game.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _TUTORIAL_H_		// このマクロが定義されていない場合
#define _TUTORIAL_H_		// 二重インクルード防止用マクロを定義

#include "manager.h"

// 前方宣言
class CFileLoad;
class CPlayer;
class CEntryIcon;
class CObject3D;
class CEnemy;

// マクロ定義
#define NUM_PORI   (5)  // 説明のテクスチャ
#define NUM_ENEMY  (2)  // 出す敵の数
#define NUM_PLAYER (4)  // プレイヤーの数

//===============================================
// ゲームクラスの定義(派生クラス)
//===============================================
class CTutorial : public CScene
{
public:

	// メンバ関数
	CTutorial();	// コンストラクタ
	~CTutorial();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	// メンバ関数(ポインタ)
	CFileLoad *GetFileLoad(void);

private:

	CPlayer **m_ppPlayer;			    // プレイヤーのポインタ
	CFileLoad *m_pFileLoad;			    // ファイル読み込みのポインタ
	CEntryIcon *m_apObject[NUM_PLAYER]; // オブジェクト2Dのポインタ
	CObject3D *m_pObject3D[NUM_PORI];   // オブジェクト3Dのポインタ
	CEnemy*m_apEnemy[NUM_ENEMY];        // 敵のポインタ
	int m_nCntRespawn;                  // 敵が再び出るまでのカウント
	int m_nEnemyId;                     // 敵の番号
	bool m_bEnd;
};

#endif