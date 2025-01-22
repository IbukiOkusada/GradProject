//==========================================================
//
// 警察AIマネージャー [police_AI_manager.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _POLICEAIMANAGER_H_		// このマクロが定義されていない場合
#define _POLICEAIMANAGER_H_		// 二重インクルード防止用マクロを定義

#include "list_manager.h"
#include "list.h"
#include "map_list.h"

// 前方宣言
class CPoliceAI;

//==========================================================
// マネージャーのクラス定義
//==========================================================
class CPoliceAIManager : public CListManager
{
private:

	CPoliceAIManager();		// コンストラクタ
	~CPoliceAIManager();	// デストラクタ

public:	// 誰でもアクセス可能

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CPoliceAIManager* GetInstance(void);
	static void Release(void);
	Cmaplist<CPoliceAI*>* GetMapList() { return &m_maplist; }
	void ListIn(CPoliceAI* pPoliceAI);
	void ListOut(CPoliceAI* pPoliceAI);

	bool GetCall() { return m_bCall; }
	void SetCall(bool bCall) { m_bCall = bCall; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	void Search(void);

	// メンバ変数
	bool m_bFlag;
	bool m_bCall;
	Cmaplist<CPoliceAI*> m_maplist;
	static CPoliceAIManager* m_pInstance;	// インスタンス
};

#endif
