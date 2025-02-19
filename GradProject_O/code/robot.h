//==========================================================
//
// ロボットの処理 [robot.h]
// Author : Kenta Hashimoto
//
//==========================================================
#ifndef _ROBOT_H_		// このマクロが定義されていない場合
#define _ROBOT_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
#include "objectX.h"
#include "player.h"
#include <list>
#include "task.h"
#include "effekseerControl.h"

// 前方宣言
class CCharacter;

//==========================================================
// ロボットのクラス定義
//==========================================================
class CRobot : public CTask
{
public:	// 誰でもアクセス可能

	// 状態列挙型
	enum STATE
	{
		STATE_NONE = 0, // 何もない
		STATE_WALK, 	// 歩き状態
		STATE_AVOID,	// 回避状態
		STATE_MAX
	};

	// 情報構造体
	struct SInfo
	{
		D3DXVECTOR3 pos;		// 位置
		D3DXVECTOR3 rot;		// 向き
		D3DXVECTOR3 rotDest;	// 目標向き
		D3DXVECTOR3 move;		// 移動量
		D3DXVECTOR3 posOld;		// 設定位置
		STATE state;			// 状態
		CPlayer* pPlayer;
		int nTargetID;
		int nId;
		float fDistance;        // 距離

		// コンストラクタ
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), rotDest(VECTOR3_ZERO),
			move(VECTOR3_ZERO), posOld(VECTOR3_ZERO), pPlayer(nullptr),
			nTargetID(0), nId(-1), fDistance(0.0f) {}
	};

private:

	

public:	// 誰でもアクセス可能
	
	// モーション列挙型
	enum MOTION
	{
		MOTION_WALK = 0,	// 歩きモーション
		MOTION_AVOID,		// 回避モーション
		MOTION_MAX
	};

	CRobot();	// コンストラクタ(オーバーロード)
	~CRobot();	// デストラクタ

	// メンバ関数
	HRESULT Init();
	HRESULT Init(const D3DXVECTOR3& rot);
	void Uninit(void);
	void Update(void);
	static CRobot* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const float& Distance);

	// メンバ関数(取得)
	CRobot* GetNext(void) { return m_pNext; }
	CRobot* GetPrev(void) { return m_pPrev; }
	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3 GetRotation(void) { return m_Info.rot; }
	float GetDistace(void) { return m_Info.fDistance; }
	CRobot::SInfo GetInfo(void) { return m_Info; }

	// メンバ関数(設定)
	void SetMove(const D3DXVECTOR3& move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);
	void SetDistance(const float distance) { m_Info.fDistance = distance; }
	void SetState(STATE state) { m_Info.state = state; }
	void SetPosTerget(const float& Distance);

	void SetNext(CRobot* pNext) { m_pNext = pNext; }
	void SetPrev(CRobot* pPrev) { m_pPrev = pPrev; }

protected:	// 派生クラスからもアクセス可能

	// メンバ関数
	void Set();

	// メンバ変数
	CCharacter* m_pCharacter;	// キャラクター

private:	// 自分だけがアクセス可能

	// メンバ関数
	bool TergetReach();	// 目標位置に到達したかどうか

	void Walk();		// 歩き処理
	void Avoid();		// 回避処理
	void AvoidCollision(D3DXVECTOR3 pos, const float CollisionDistance);	// 回避コリジョン
	bool CollisionObjX();													// オブジェクトとの当たり判定処理
	void SetAvoid(D3DXVECTOR3 pos, D3DXVECTOR3 rot);	// 回避の設定
	void SetWalk();										// 歩きを設定

	// メンバ変数
	SInfo m_Info;				// 自分自身の情報
	CRobot* m_pPrev;			// 前のオブジェクトへのポインタ
	CRobot* m_pNext;			// 次のオブジェクトへのポインタ
	CEffekseer::CEffectData* m_pDust;

	D3DXVECTOR3 m_PosTarget[2];

};

#endif

