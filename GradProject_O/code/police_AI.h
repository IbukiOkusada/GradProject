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
		TYPE_MAX
	};

	CPoliceAI();	// コンストラクタ
	~CPoliceAI();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Search(void);
	void Chase(void);

	static CPoliceAI* Create(CPolice* pPolice, TYPE type = TYPE_NORMAL);

	// メンバ関数(取得)
	CRoad::SSearch* GetSearchRoad() { return m_pSearchTarget; }

	// メンバ関数(設定)

protected:
	
	// メンバ関数
	virtual void SelectRoad(void);
	virtual void ReachRoad(void);

	// メンバ変数
	CPolice* m_pPolice;
	CRoad* m_pRoadStart;		// 移動開始地点
	CRoad* m_pRoadTarget;		// 目標地点

private:	// 自分だけがアクセス可能

	// メンバ関数
	void CheckSpeed(CPlayer* pPlayer);
	void CheckTurn(CPlayer* pPlayer);
	void CheckDamage(CPlayer* pPlayer);
	void CheckSmoke(CPlayer* pPlayer);
	void CheckCollision(CPlayer* pPlayer);

	// メンバ変数
	CRoad::SSearch* m_pSearchTarget;
	float m_fSearchTimer;
	vector<CRoad::SSearch*> m_searchRoad;
	TYPE m_type;
};

class CPoliceAINomal : public CPoliceAI
{
	void SelectRoad(void) override;
};

class CPoliceAIElite : public CPoliceAI
{
	void SelectRoad(void) override;
};

#endif
