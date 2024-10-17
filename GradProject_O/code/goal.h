//==========================================================
//
// ゴール [goal.h]
// Author : 丹野竜之介
//
//==========================================================
#ifndef _GOAL_H_		// このマクロが定義されていない場合
#define _GOAL_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
#include "list.h"
//#include "task.h"	// これでファイルインクルードできます
//前方宣言
class CMeshCylinder;
//==========================================================
// サンプルのクラス定義
//==========================================================
class CGole : public CTask
{

public:	// 誰でもアクセス可能

	CGole();	// コンストラクタ(オーバーロード)
	~CGole();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CGole* Create(D3DXVECTOR3 pos,float fRange,float fLimit);

	
	static Clist<CGole*>* GetInstance() { if (pList == nullptr) { pList = pList->Create(); }return pList; }		// リスト取得
	static void ListRelease() { if (pList != nullptr) { delete pList; pList = nullptr; } }					// リスト解放
private:	// 自分だけがアクセス可能

	// メンバ関数
	bool CheckRange();
	bool CheckSpeed();
	// メンバ変数
	D3DXVECTOR3 m_pos;				// 座標
	float m_fRange;					// ゴールの範囲
	float m_fLimit;					// 速度制限
	CMeshCylinder* pMesh;

	static Clist<CGole*>* pList;	// 自分のリスト*GetInstance()経由でアクセスする事*
};

#endif