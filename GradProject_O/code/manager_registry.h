//==========================================================
//
// マネージャー総括マネージャー [manager_registry.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MANAGER_REGISTRY_H_		// このマクロが定義されていない場合
#define _MANAGER_REGISTRY_H_		// 二重インクルード防止用マクロを定義

// 前方宣言
class CListManager;

//==========================================================
// マネージャー総括マネージャーのクラス定義
//==========================================================
class CManagerRegistry
{
private:

	CManagerRegistry();		// コンストラクタ
	~CManagerRegistry();	// デストラクタ

public:	// 誰でもアクセス可能

	static CManagerRegistry* GetInstance(void);
	static void Release(void);
	void ListIn(CListManager* pListManager);
	void ListOut(CListManager* pListManager);
	int GetNum(void) { return m_nNum; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	CListManager* GetTop(void) { return m_pTop; }
	CListManager* GetCur(void) { return m_pCur; }

	// メンバ変数
	CListManager* m_pTop;	// 先頭
	CListManager* m_pCur;	// 最後尾
	int m_nNum;
	static CManagerRegistry* m_pInstance;	// インスタンス
};

#endif
