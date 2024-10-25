//==========================================================
//
// サンプルタスク(リスト管理タスク) [sample.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _LIST_MANAGER_H_		// このマクロが定義されていない場合
#define _LIST_MANAGER_H_		// 二重インクルード防止用マクロを定義

#include "list.h"

//==========================================================
// サンプルのクラス定義
//==========================================================
class CListManager
{

public:	// 誰でもアクセス可能

	CListManager();	// コンストラクタ(オーバーロード)
	~CListManager();	// デストラクタ

	// メンバ関数
	virtual void Uninit(void);

	static void Release();

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	static Clist<CListManager*>* m_pList;
};

#endif


