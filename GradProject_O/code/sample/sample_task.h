//==========================================================
//
// サンプルタスク [sample_task.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _SAMPLETASK_H_		// このマクロが定義されていない場合
#define _SAMPLETASK_H_		// 二重インクルード防止用マクロを定義

#include "../task.h"
//#include "task.h"	// これでファイルインクルードできます

//==========================================================
// サンプルのクラス定義
//==========================================================
class CSampleTask : public CTask
{

public:	// 誰でもアクセス可能

	CSampleTask();	// コンストラクタ(オーバーロード)
	~CSampleTask();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CSampleTask *Create(void);

	// メンバ関数(取得)

	// メンバ関数(設定)

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
};

#endif
