//==========================================================
//
// オブジェクト管理の処理 [object_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _OBJECT_MANAGER_H_	// このマクロが定義されていない場合
#define _OBJECT_MANAGER_H_	// 二重インクルード防止用マクロを定義

// マクロ定義
#define NUM_PRIORITY	(8)		// 優先順位管理数

// 前方宣言
class CObject;

//==========================================================
// サンプルのクラス定義
//==========================================================
class CObjectManager
{
private:
	CObjectManager();	// コンストラクタ
	~CObjectManager();	// デストラクタ

public:
	// リスト管理メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Draw(void);
	void DrawAll(void);
	void ListIn(CObject *pObject);
	CObject *GetTop(const int nPriority) { return m_apTop[nPriority]; }
	int GetNumAll(void) { return m_nNumAll; }
	int GetPriNumAll(int nPriority) { return m_aPriNumAll[nPriority]; }

	// シングルトン
	static CObjectManager* GetInstance(void);
	static void Release(void);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void ReleaseAll(void);
	void DeathCheck(void);

	// メンバ変数
	CObject *m_apTop[NUM_PRIORITY];	// 先頭のオブジェクトへのポインタ
	CObject *m_apCur[NUM_PRIORITY];	// 最後尾のオブジェクトへのポインタ
	static CObjectManager *m_pInstance;
	int m_aPriNumAll[NUM_PRIORITY];	// 各優先順位ごとの総数
	int m_nNumAll;	// オブジェクト総数
};

#endif
