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
#include "car.h"
#include "police.h"

// 前方宣言

//==========================================================
// サンプルのクラス定義
//==========================================================
class CCarManager : public CListManager
{
public:

	// 受信用構造体
	struct NextCreateInfo
	{
		int nChaseId;		// 追跡しているプレイヤーID
		CCar::CAR_TYPE type;	// 種類
		D3DXVECTOR3 pos;	// 位置
		D3DXVECTOR3 rot;	// 向き
		D3DXVECTOR3 move;	// 移動量
		CPolice::CHASE chase;

		// コンストラクタ
		NextCreateInfo() : nChaseId(-1), type(CCar::CAR_TYPE::CAR_TYPE_MAX),
			pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), move(VECTOR3_ZERO), chase(CPolice::CHASE::CHASE_MAX) {}
	};

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
	void CreateListIn(NextCreateInfo& info, int nId);
	void CreateListOut(NextCreateInfo& info, int nId);
	NextCreateInfo* CreateGet(int nId);
	bool Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void IdListIn(CCar* pCar);
	void IdListOut(CCar* pCar);

	// メンバ変数
	Clist<CCar*>* m_pList;
	Cmaplist<CCar*> m_List;
	Cmaplist<NextCreateInfo*> m_NextList;
	Cmaplist<NextCreateInfo*> m_NextTempList;
	int m_nNum;		// 総数
	bool m_bSet;	// 設定中かどうか
	static CCarManager* m_pInstance;	// インスタンス
};

#endif
