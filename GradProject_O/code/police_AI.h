//==========================================================
//
// パトカーAI処理 [police_AI.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _POLICE_AI_H_		// このマクロが定義されていない場合
#define _POLICE_AI_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "road.h"
#include "road_manager.h"
#include <list>

// 前方宣言
class CRoad;
class CPolice;

//==========================================================
// 警察AIのクラス定義
//==========================================================
class CPoliceAI
{
public:	// 誰でもアクセス可能

	CPoliceAI();	// コンストラクタ
	virtual ~CPoliceAI();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CPoliceAI* Create(void);

	// メンバ関数(取得)

	// メンバ関数(設定)

protected:
	
	// メンバ関数

	// メンバ変数

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	CPolice* m_pPolice;
	vector<CRoad::SSearch*> m_searchRoad;

};

#endif
