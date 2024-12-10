//==========================================================
//
// 検問管理の処理 [inspection_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _INSPECTION_MANAGER_H_		// このマクロが定義されていない場合
#define _INSPECTION_MANAGER_H_		// 二重インクルード防止用マクロを定義

#include "map_list.h"

// 前方宣言
class CInspection;
class CRoad;

//==========================================================
// ゴール管理のクラス定義
//==========================================================
class CInspectionManager
{
private:

	struct SNextInfo
	{
		bool bActive;
		D3DXVECTOR3 pos;
		D3DXVECTOR3 rot;
		CRoad* pRoad;
		int nId;
		int nStartpdid;

		// コンストラクタ
		SNextInfo() : bActive(false), pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), pRoad(nullptr), nId(-1), nStartpdid(-1) {}
	};

public:
	CInspectionManager();      // コンストラクタ
	~CInspectionManager();     // デストラクタ

	HRESULT Init(void);  // 初期化処理
	void Update(void);   // 更新処理

	// リスト管理
	bool ListIn(CInspection* pInspection);
	bool ListOut(CInspection* pInspection);
	CInspection* Get(int nIdx);
	int GetCreateCnt() { return m_List.GetInCnt(); }
	void SetNextInspection(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, CRoad* pRoad, int nId, int startpdid);

	static void Release();

	// 取得系
	static CInspectionManager* GetInstance(void);

private:

	// メンバ関数
	void Uninit(void);   // 終了処理

	// メンバ変数
	SNextInfo m_NextInfo;
	Cmaplist<CInspection*> m_List;
	static CInspectionManager* m_pInstance;  // 自身のポインタ
};

#endif
