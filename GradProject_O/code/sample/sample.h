//==========================================================
//
// サンプルタスク(リスト管理タスク) [sample.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _SAMPLE_H_		// このマクロが定義されていない場合
#define _SAMPLE_H_		// 二重インクルード防止用マクロを定義

#include "../task.h"
//#include "task.h"	// これでファイルインクルードできます

//==========================================================
// サンプルのクラス定義
//==========================================================
class CSample : public CTask
{

public:	// 誰でもアクセス可能

	CSample();	// コンストラクタ(オーバーロード)
	~CSample();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CSample* Create(void);

	// メンバ関数(取得)
	CSample* GetNext(void) { return m_pNext; }
	CSample* GetPrev(void) { return m_pPrev; }

	// メンバ関数(設定)
	void SetNext(CSample* pNext) { m_pNext = pNext; }
	void SetPrev(CSample* pPrev) { m_pPrev = pPrev; }


private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	CSample* m_pPrev;			// 前のオブジェクトへのポインタ
	CSample* m_pNext;			// 次のオブジェクトへのポインタ
};

#endif

