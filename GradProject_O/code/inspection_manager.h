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

//==========================================================
// ゴール管理のクラス定義
//==========================================================
class CInspectionManager
{
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

	static void Release();

	// 取得系
	static CInspectionManager* GetInstance(void);

private:

	// メンバ関数
	void Uninit(void);   // 終了処理

	// メンバ変数
	Cmaplist<CInspection*> m_List;
	static CInspectionManager* m_pInstance;  // 自身のポインタ
};

#endif
