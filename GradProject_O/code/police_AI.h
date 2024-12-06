//==========================================================
//
// パトカーAI処理 [police_AI.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _POLICE_AI_H_		// このマクロが定義されていない場合
#define _POLICE_AI_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "road.h"
#include "road_manager.h"
#include <list>

// 前方宣言
class CRoad;
class CPolice;
class CPlayer;

//==========================================================
// 警察AIのクラス定義
//==========================================================
class CPoliceAI
{
public:	// 誰でもアクセス可能

	// AIタイプ列挙型
	enum TYPE
	{
		TYPE_NORMAL = 0,	// 通常
		TYPE_ELITE,			// 回り込み型
		TYPE_GENTLE,		// 緩やか型
		TYPE_MAX
	};

	CPoliceAI();	// コンストラクタ
	~CPoliceAI();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void);
	void Uninit(void);
	void Search(void);
	void Chase(void);

	void BeginChase(CPlayer* pPlayer);
	void EndChase(void);

	static CPoliceAI* Create(CPolice* pPolice, TYPE type = TYPE_NORMAL);

	// メンバ関数(取得)
	CRoad::SInfoSearch* GetSearchRoad() { return m_pSearchTarget; }

	// メンバ関数(設定)

protected:
	
	// メンバ関数
	virtual void SelectRoad(void);
	virtual void ReachRoad(void);
	virtual void ChaseAStar(void);

	// メンバ変数
	CPolice* m_pPolice;
	CRoad* m_pRoadStart;		// 移動開始地点
	CRoad* m_pRoadTarget;		// 目標地点
	CRoad::SInfoSearch* m_pSearchTarget;
	vector<CRoad::SInfoSearch*> m_searchRoad;
	int m_nCntThread;

private:	// 自分だけがアクセス可能

	// メンバ関数
	void CheckSpeed(CPlayer* pPlayer);
	void CheckTurn(CPlayer* pPlayer);
	void CheckDamage(CPlayer* pPlayer);
	void CheckSmoke(CPlayer* pPlayer);
	void CheckCollision(CPlayer* pPlayer);

	// メンバ変数
	float m_fSearchTimer;
	TYPE m_type;
};

//==========================================================
// 警察AIのクラス定義
//==========================================================
class CPoliceAINomal : public CPoliceAI
{
	// メンバ関数
	HRESULT Init(void) override;
	void SelectRoad(void) override;
};

//==========================================================
// 警察AIのクラス定義
//==========================================================
class CPoliceAIElite : public CPoliceAI
{
	// メンバ関数
	HRESULT Init(void) override;
	void SelectRoad(void) override;
	void ChaseAStar(void) override;

	// メンバ変数
	CRoad* m_pRoadRelay;		// 目標地点
};

//==========================================================
// 警察AIのクラス定義
//==========================================================
class CPoliceAIGentle : public CPoliceAI
{
	// メンバ関数
	HRESULT Init(void) override;
	void SelectRoad(void) override;
};

#endif
