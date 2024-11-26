//===============================================
//
// エントリー画面の管理処理 [entry.h]
// Author : Ryosuke Oohara
//
//===============================================
#ifndef _ENTRY_H_		// このマクロが定義されていない場合
#define _ENTRY_H_		// 二重インクルード防止用マクロを定義

#include "manager.h"

class CScore;
class CFileLoad;
class CMeshDome;
class CTime;
class CCharacter;
class CObject2D;
class CMultiCamera;

//===============================================
// エントリークラスの定義(派生クラス)
//===============================================
class CEntry : public CScene
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
	CEntry();	// コンストラクタ
	~CEntry();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:

	// メンバ関数
	void AddPlayer(void);  // プレイヤー参加処理

	// メンバ変数
	CPlayer** m_ppPlayer;	// プレイヤーのポインタ
	CMultiCamera** m_ppCamera;   // カメラのポインタ
};

#endif