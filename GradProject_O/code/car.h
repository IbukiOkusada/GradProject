//==========================================================
//
// 車処理 [car.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _CAR_H_		// このマクロが定義されていない場合
#define _CAR_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "object.h"
#include "objectX.h"
#include "task.h"
#include "effekseerControl.h"
// 前方宣言
class CWaist;
class CCharacter;
class CObjectX;
class CRoad;

//==========================================================
// サンプルのクラス定義
//==========================================================
class CCar : public CTask
{
public:

	// 種類列挙
	enum TYPE
	{
		TYPE_RECV = 0,	// 受信のみ
		TYPE_SEND,		// 送信のみ
		TYPE_ACTIVE,	// 動く
		TYPE_MAX
	};

private:	// 自分だけがアクセス可能

	// 情報構造体
	struct SInfo
	{
		D3DXVECTOR3 pos;		// 位置
		D3DXVECTOR3 rot;		// 向き
		D3DXVECTOR3 rotDest;	// 目標向き
		D3DXVECTOR3 move;		// 移動量
		D3DXVECTOR3 posOld;		// 設定位置
		D3DXVECTOR3 posTarget;	// 目標位置
		CRoad* pRoadStart;		// 移動開始地点
		CRoad* pRoadTarget;		// 目標地点
		TYPE type;				// 状態
		TYPE typeNext;			// 次の状態
		int nBackTime;
		float fSpeed;
		float fSpeedDest;
		float fRotMulti;
		bool bBreak;
		bool bBack;
		int nId;

		// コンストラクタ
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), rotDest(VECTOR3_ZERO),
			move(VECTOR3_ZERO), posOld(VECTOR3_ZERO), posTarget(VECTOR3_ZERO), pRoadStart(nullptr), pRoadTarget(nullptr),
			type(TYPE::TYPE_SEND), typeNext(TYPE::TYPE_MAX), nBackTime(0), fSpeed(0.0f), fSpeedDest(0.0f), fRotMulti(0.0f), bBreak(false), bBack(0.0f),
			nId(-1) {}
	};

	// 受信用情報構造体
	struct SRecvInfo
	{
		D3DXVECTOR3 pos;		// 位置
		D3DXVECTOR3 rot;		// 向き

		// コンストラクタ
		SRecvInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO) {}
	};

public:	// 誰でもアクセス可能

	CCar(int nId);	// コンストラクタ(オーバーロード)
	virtual ~CCar();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	static CCar*Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move, int nID);

	// メンバ関数(取得)
	D3DXVECTOR3& GetMove(void) { return m_Info.move; }
	D3DXVECTOR3& GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3& GetRotation(void) { return m_Info.rot; }
	D3DXVECTOR3& GetOldPosition(void) { return m_Info.posOld; }
	float GetSpeed(void) { return m_Info.fSpeed; }
	float GetSpeedDest(void) { return m_Info.fSpeedDest; }
	CRoad* GetRoadStart(void) { return m_Info.pRoadStart; }
	CRoad* GetRoadTarget(void) { return m_Info.pRoadTarget; }
	int GetModelIndex(void) { return m_pObj->GetIdx(); }
	bool GetBack(void) { return m_Info.bBack; }
	TYPE GetType() { return m_Info.type; }
	TYPE GetTypeNext() { return m_Info.typeNext; }
	virtual int GetId() { return m_Info.nId; }

	// 自分で動くときはtrue 動かないときはfalse
	bool IsActive() {
		if (m_Info.type == TYPE::TYPE_RECV) return false;
		return true;
	}

	// メンバ関数(設定)
	void SetMove(const D3DXVECTOR3& move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);
	void SetPosTarget(const D3DXVECTOR3& pos) { m_Info.posTarget = pos; }
	void SetSpeed(const float speed) { m_Info.fSpeed = speed; }
	void SetSpeedDest(const float speedDest) { m_Info.fSpeedDest = speedDest; }
	void SetRotMulti(const float rotMulti) { m_Info.fRotMulti = rotMulti; }
	void SetRoadStart(CRoad* RoadStart) { m_Info.pRoadStart = RoadStart; }
	void SetRoadTarget(CRoad* RoadTarget) { m_Info.pRoadTarget = RoadTarget; }
	void SetBack(bool bBack) { m_Info.bBack = bBack; }
	void SetBackTime(int nBackTime) { m_Info.nBackTime = nBackTime; }
	void SetType(TYPE type) { m_Info.type = type; }
	void SetTypeNext(TYPE type) { m_Info.typeNext = type; }
	void BinfId(int nId) { m_Info.nId = nId; }

	// 受信用情報
	void SetRecvPosition(const D3DXVECTOR3& pos) { m_RecvInfo.pos = pos; }
	void SetRecvRotation(const D3DXVECTOR3& rot) { m_RecvInfo.rot = rot; }

protected:	// 派生クラスからもアクセス可能

	// メンバ関数
	virtual void MoveRoad();
	virtual void SearchRoad();
	virtual void ReachRoad();
	virtual void Hit();
	void TailLamp();
	virtual void Break();
	void Set();
	virtual void SendPosition();
	virtual void RecvTypeSet() {
		if (m_Info.typeNext != TYPE::TYPE_MAX)
		{
			m_Info.type = m_Info.typeNext;
			m_Info.typeNext = TYPE::TYPE_MAX;
		}
	}

	// メンバ変数
	CObjectX* m_pObj;

private:	// 自分だけがアクセス可能

	// メンバ関数
	virtual void Move();
	void Rot();
	bool Collision();

	// 受信用メンバ関数
	void RecvInerSet();

	// メンバ変数
	SInfo m_Info;				// 自分自身の情報
	CEffekseer::CEffectData* m_pTailLamp;

	// オンライン関連
	SRecvInfo m_RecvInfo;		// 受信情報
};

#endif
