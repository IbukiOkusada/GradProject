//==========================================================
//
// サンプルマネージャー [sample_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _SAMPLEMANAGER_H_		// このマクロが定義されていない場合
#define _SAMPLEMANAGER_H_		// 二重インクルード防止用マクロを定義

// 前方宣言
class CSample;

#include "../list_manager.h"

//==========================================================
// サンプルマネージャーのクラス定義
//==========================================================
class CSampleManager : public CListManager
{
private:

	CSampleManager();		// コンストラクタ
	~CSampleManager();	// デストラクタ

public:	// 誰でもアクセス可能

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CSampleManager* GetInstance(void);
	static void Release(void);
	CSample* GetTop(void) { return m_pTop; }
	CSample* GetCur(void) { return m_pCur; }
	void ListIn(CSample* pSample);
	void ListOut(CSample* pSample);
	int GetNum(void) { return m_nNum; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	

	// メンバ変数
	CSample* m_pTop;	// 先頭
	CSample* m_pCur;	// 最後尾
	int m_nNum;
	static CSampleManager* m_pInstance;	// インスタンス
};

#endif

