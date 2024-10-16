//==========================================================
//
// サンプルタスク(リスト管理タスク) [sample.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _LIST_MANAGER_H_		// このマクロが定義されていない場合
#define _LIST_MANAGER_H_		// 二重インクルード防止用マクロを定義

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

	// メンバ関数(取得)
	CListManager* GetNext(void) { return m_pNext; }
	CListManager* GetPrev(void) { return m_pPrev; }

	// メンバ関数(設定)
	void SetNext(CListManager* pNext) { m_pNext = pNext; }
	void SetPrev(CListManager* pPrev) { m_pPrev = pPrev; }

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	CListManager* m_pPrev;			// 前のオブジェクトへのポインタ
	CListManager* m_pNext;			// 次のオブジェクトへのポインタ
};

#endif


