//==========================================================
//
// 道マネージャー [road_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _ROADMANAGER_H_		// このマクロが定義されていない場合
#define _ROADMANAGER_H_		// 二重インクルード防止用マクロを定義

#include "list_manager.h"
#include "list.h"
#include "road.h"

// 前方宣言

//==========================================================
// サンプルのクラス定義
//==========================================================
class CRoadManager : public CListManager
{
private:

	CRoadManager();		// コンストラクタ
	~CRoadManager();	// デストラクタ

public:	// 誰でもアクセス可能

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CRoadManager* GetInstance(void);
	static void Release(void);
	void ListIn(CRoad* pRoad);
	void ListOut(CRoad* pRoad);
	bool Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage);
	Clist<CRoad::SInfo*>* GetInfoList() { return &m_InfoList; }
	Clist<CRoad*>* GetList() { if (m_pList == nullptr) { m_pList = m_pList->Create(); }return m_pList; }	// リスト取得
	void ListRelease() { if (m_pList != nullptr) { delete m_pList; m_pList = nullptr; } }			// リスト解放

	// 道連結関数
	void AllConnect(void);
	void SearchRoadConnect(void);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void SideConnectCheck(CRoad* pRoad, CRoad* pCheckRoad);		// 横連結判定
	void VerticalConnectCheck(CRoad* pRoad, CRoad* pCheckRoad);	// 縦連結判定

	// メンバ変数
	Clist<CRoad*>* m_pList;
	static CRoadManager* m_pInstance;	// インスタンス
	Clist<CRoad::SInfo*> m_InfoList;
	
};

#endif

