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
#include "police_AI.h"

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
		STATE_NORMAL = 0,	// 通常
		STATE_CHASE,		// 追跡
		STATE_SEARCH,		// 探索
		STATE_FADEOUT,		// フェードアウト
		STATE_STOP,			// 停止
		STATE_MAX
	};

	// 次の追跡状態列挙
	enum CHASE
	{
		CHASE_BEGIN = 0,
		CHASE_END,
		CHASE_MAX
	};

private:	// 自分だけがアクセス可能

	// 情報構造体
	struct SInfo
	{
		CPlayer* pPlayer;
		D3DXVECTOR3 offsetLane;
		int nChaseCount;
		int nLaneCount;
		int nLaneTime;
		bool bChase;

		// コンストラクタ
		SInfo() : pPlayer(nullptr), nChaseCount(0), bChase(false) {}
	};

protected:

	// 状態管理構造体
	struct SState
	{
		CPlayer* pNextPlayer;
		CHASE chasenext;
		STATE state;
		float fTimer;
		float fTimerOrigin;

		// コンストラクタ
		SState() : state(STATE::STATE_NORMAL), chasenext(CHASE::CHASE_MAX), pNextPlayer(nullptr), fTimer(0.0f) {}
	};

public:	// 誰でもアクセス可能

	CPolice(int nId);	// コンストラクタ(オーバーロード)
	virtual ~CPolice() override;	// デストラクタ

	// メンバ関数
	HRESULT Init(void) override;
	void Uninit(void);
	void Update(void);
	static CPolice* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move, int nId);

	// 通信用
	virtual void SendChase();
	virtual void SendChaseEnd();

	// メンバ関数(取得)
	STATE GetState() { return m_stateInfo.state; }
	CPlayer* GetPlayer() { return m_Info.pPlayer; }
	bool GetChase() { return m_Info.bChase; }
	int GetChaseCount() { return m_Info.nChaseCount; }
	CPoliceAI* GetAi() { return m_pPoliceAI; }
	CMasterSound::CObjectSound* GetSound() { return m_pSiren; }

	// メンバ関数(設定)
	void SetState(const STATE state);
	void SetChaseNext(const CHASE chase) { m_stateInfo.chasenext = chase; }
	void SetPlayer(CPlayer* pPlayer) { m_Info.pPlayer = pPlayer; }
	void SetNextPlayer(CPlayer* pPlayer) { m_stateInfo.pNextPlayer = pPlayer; }
	void SetChase(bool bChase) { m_Info.bChase = bChase; }
	void SetChaseCount(int bChaseCount) { m_Info.nChaseCount = bChaseCount; }
	void SetLaneCount(int nLaneCount) { m_Info.nLaneCount = nLaneCount; }
	void SetLaneTime(int nLaneTime) { m_Info.nLaneTime = nLaneTime; }
	void SetTypeAI(CPoliceAI::TYPE type);		// AIのタイプ変更
	void SetSound(CMasterSound::CObjectSound* pSound) { m_pSiren = pSound; }

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

	// 通信用
	virtual void SendPosition() override;

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
	void ChasePlayer();
	void SearchPlayer();
	void LanePlayer();
	bool Collision() override;
	bool CollisionObjX() override;
	void Hit() override;
	void Break() override;
	void RecvTypeSet() override;

	// メンバ変数
	SInfo m_Info;					// 自分自身の情報
		//シェーダーライト
	CShaderLight::SLight* m_pShaderLight;
	CMasterSound::CObjectSound* m_pSiren;
	CPoliceAI* m_pPoliceAI;
	CEffekseer::CEffectData* m_pPatrolLamp;
};

#endif
