//==========================================================
//
// 警察マネージャー [police_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _POLICEMANAGER_H_		// このマクロが定義されていない場合
#define _POLICEMANAGER_H_		// 二重インクルード防止用マクロを定義

#include "list_manager.h"
#include "list.h"
#include "map_list.h"

// 前方宣言
class CPolice;

//==========================================================
// マネージャーのクラス定義
//==========================================================
class CPoliceManager : public CListManager
{
private:

	struct SInspInfo
	{
		float fInterval;	// インターバル
		float fTime;		// インターバル時間
		int nCnt;			// カウント

		// コンストラクタ
		SInspInfo() : fInterval(0.0f), nCnt(0), fTime(0.0f) {}
	};

private:

	CPoliceManager();		// コンストラクタ
	~CPoliceManager();	// デストラクタ

public:	// 誰でもアクセス可能

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CPoliceManager* GetInstance(void);
	static void Release(void);
	Clist<CPolice*>* GetList() { if (m_pList == nullptr) { m_pList = m_pList->Create(); }return m_pList; }	// リスト取得
	void ListRelease() { if (m_pList != nullptr) { delete m_pList; m_pList = nullptr; } }			// リスト解放
	void ListIn(CPolice* pPolice);
	void ListOut(CPolice* pPolice);
	void Warning(CPolice* pPolice);
	void SetInspection();
	void ResetInterVal() { m_InspInfo.fInterval = 0.0f; }
	bool Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage);

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	Clist<CPolice*>* m_pList;
	int m_nNum;
	static CPoliceManager* m_pInstance;	// インスタンス
	SInspInfo m_InspInfo;
};

#endif
