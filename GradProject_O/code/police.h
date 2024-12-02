//==========================================================
//
// パトカー処理 [police.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _POLICE_H_		// このマクロが定義されていない場合
#define _POLICE_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "object.h"
#include "objectX.h"
#include "task.h"
#include "car.h"
#include "player.h"
#include <list>

// 前方宣言
class CRoad;
class CPoliceAI;

//==========================================================
// 警察のクラス定義
//==========================================================
class CPolice : public CCar
{
public:

	// 状態
	enum STATE
	{
		STATE_NORMAL = 0,
		STATE_CHASE,
		STATE_SEARCH,
		STATE_FADEOUT,
		STATE_STOP,
		STATE_MAX
	};

private:	// 自分だけがアクセス可能

	// 情報構造体
	struct SInfo
	{
		CPlayer* pPlayer;
		int nChaseCount;
		bool bChase;

		// コンストラクタ
		SInfo() : pPlayer(nullptr), nChaseCount(0), bChase(false) {}
	};

protected:

	// 状態管理構造体
	struct SState
	{
		STATE state;
		float fTimer;
		float fTimerOrigin;

		// コンストラクタ
		SState() : state(STATE::STATE_NORMAL), fTimer(0.0f) {}
	};

public:	// 誰でもアクセス可能

	CPolice();	// コンストラクタ(オーバーロード)
	virtual ~CPolice() override;	// デストラクタ

	// メンバ関数
	HRESULT Init(void) override;
	void Uninit(void);
	void Update(void);
	static CPolice*Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move);

	// メンバ関数(取得)
	STATE GetState() { return m_stateInfo.state; }
	CPlayer* GetPlayer() { return m_Info.pPlayer; }
	bool GetChase() { return m_Info.bChase; }
	int GetChaseCount() { return m_Info.nChaseCount; }

	// メンバ関数(設定)
	void SetState(const STATE state);
	void SetPlayer(CPlayer* pPlayer) { m_Info.pPlayer = pPlayer; }
	void SetChase(bool bChase) { m_Info.bChase = bChase; }
	void SetChaseCount(int bChaseCount) { m_Info.nChaseCount = bChaseCount; }

protected:

	//=============================
	// メンバ関数
	//=============================
	// 状態関数
	virtual void UpdateState();		// 状態更新
	virtual void StateNormal();		// なし
	virtual void StateChase();		// 追跡
	virtual void StateSearch();		// 警戒
	virtual void StateFadeOut();	// フェードアウト
	virtual void StateStop();		// 停止

	// 状態設定関数
	virtual void SetStateNormal();		// なし
	virtual void SetStateChase();		// 追跡
	virtual void SetStateSearch();		// 警戒
	virtual void SetStateFadeOut();		// フェードアウト
	virtual void SetStateStop();		// 停止
	
	// メンバ関数
	void SetStateTimer(const float fTimer) { // 状態タイマー設定
		m_stateInfo.fTimer = fTimer; 
		m_stateInfo.fTimerOrigin = fTimer; 
	}

	// メンバ変数
	SState m_stateInfo;	// 状態管理情報

private:	// 自分だけがアクセス可能

	//=============================
	// 関数リスト
	//=============================
	typedef void(CPolice::* STATE_FUNC)();
	static STATE_FUNC m_StateFunc[];
	typedef void(CPolice::* SETSTATE_FUNC)();
	static SETSTATE_FUNC m_SetStateFunc[];

	// メンバ関数
	void MoveRoad() override;
	void ReachRoad() override;
	void ChasePlayer();
	void SearchPlayer();
	void Collision();
	void Hit() override;
	void Break() override;

	// メンバ変数
	SInfo m_Info;					// 自分自身の情報

	CMasterSound::CObjectSound* m_pSiren;
	CPoliceAI* m_pPoliceAI;
	CEffekseer::CEffectData* m_pPatrolLamp;
};

#endif
