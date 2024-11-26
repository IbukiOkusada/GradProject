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
#include "effekseerControl.h"
#include "road.h"

//#include "task.h"	// これでファイルインクルードできます
//前方宣言
class CMeshCylinder;
class CCharacter;
class CBaggage;

//==========================================================
// ゴールのクラス定義
//==========================================================
class CGoal : public CTask
{

public:	// 誰でもアクセス可能

	CGoal();	// コンストラクタ(オーバーロード)
	~CGoal();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CGoal* Create(D3DXVECTOR3 pos,float fRange,float fLimit);

	CRoad* GetRoad() { return m_pRoad; }
	D3DXVECTOR3 GetPos() { return m_pos; }
	void SetEnd(int nId);
	static Clist<CGoal*>* GetInstance() { if (pList == nullptr) { pList = pList->Create(); }return pList; }		// リスト取得
	static void ListRelease() { if (pList != nullptr) { delete pList; pList = nullptr; } }					// リスト解放
	
	bool GetEnd() { return m_bEnd; }
private:	// 自分だけがアクセス可能

	// メンバ関数
	bool CheckRange(int nId);
	bool CheckSpeed(int nId);
	void ScreenEffect();

	// メンバ変数
	int m_nId;
	D3DXVECTOR3 m_pos;				// 座標
	float m_fRange;					// ゴールの範囲
	float m_fLimit;					// 速度制限
	bool m_bEnd;					// 終了地点
	CCharacter* m_pPeople;			// 人
	CRoad * m_pRoad;
	CBaggage* m_pBaggage;
	CEffekseer::CEffectData* pEffect;
	static Clist<CGoal*>* pList;	// 自分のリスト*GetInstance()経由でアクセスする事*
};

#endif