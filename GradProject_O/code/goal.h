//==========================================================
//
// ゴール [goal.h]
// Author : 丹野竜之介
//
//==========================================================
#ifndef _GOAL_H_		// このマクロが定義されていない場合
#define _GOAL_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
#include "list.h"
//#include "task.h"	// これでファイルインクルードできます

//==========================================================
// サンプルのクラス定義
//==========================================================
class CGole : public CTask
{

public:	// 誰でもアクセス可能

	CGole();	// コンストラクタ(オーバーロード)
	~CGole();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CGole* Create(void);

	
	Clist<CGole *> GetList() {
		if (pList == NULL)
		{
			pList = new Clist<CGole *>;
		}
	}

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	static Clist<CGole *> * pList
};

#endif