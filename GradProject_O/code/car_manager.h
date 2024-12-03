//==========================================================
//
// 車マネージャー [car_manager.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _CARMANAGER_H_		// このマクロが定義されていない場合
#define _CARMANAGER_H_		// 二重インクルード防止用マクロを定義

#include "list_manager.h"
#include "list.h"
#include "map_list.h"

// 前方宣言
class CCar;

//==========================================================
// サンプルのクラス定義
//==========================================================
class CCarManager : public CListManager
{
private:

	CCarManager();		// コンストラクタ
	~CCarManager();	// デストラクタ

public:	// 誰でもアクセス可能

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CCarManager* GetInstance(void);
	static void Release(void);
	Clist<CCar*>* GetList() { if (m_pList == nullptr) { m_pList = m_pList->Create(); }return m_pList; }	// リスト取得
	Cmaplist<CCar*>* GetMapList() { return &m_List; }
	void ListRelease() { if (m_pList != nullptr) { delete m_pList; m_pList = nullptr; } }			// リスト解放
	void ListIn(CCar* pCar);
	void ListOut(CCar* pCar);
	bool Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void IdListIn(CCar* pCar);
	void IdListOut(CCar* pCar);

	// メンバ変数
	Clist<CCar*>* m_pList;
	Cmaplist<CCar*> m_List;
	int m_nNum;
	static CCarManager* m_pInstance;	// インスタンス
};

#endif
