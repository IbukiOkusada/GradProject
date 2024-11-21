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
		int nBackTime;
		float fSpeed;
		float fSpeedDest;
		bool bBreak;
		bool bBack;
	};

public:	// 誰でもアクセス可能

	CCar();	// コンストラクタ(オーバーロード)
	virtual ~CCar();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	static CCar*Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move);

	// メンバ関数(取得)
	D3DXVECTOR3 GetMove(void) { return m_Info.move; }
	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3 GetRotation(void) { return m_Info.rot; }
	D3DXVECTOR3 GetOldPosition(void) { return m_Info.posOld; }
	float GetSpeed(void) { return m_Info.fSpeed; }
	float GetSpeedDest(void) { return m_Info.fSpeedDest; }
	CRoad* GetRoadStart(void) { return m_Info.pRoadStart; }
	CRoad* GetRoadTarget(void) { return m_Info.pRoadTarget; }
	int GetModelIndex(void) { return m_pObj->GetIdx(); }

	// メンバ関数(設定)
	void SetMove(const D3DXVECTOR3& move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);
	void SetPosTarget(const D3DXVECTOR3& pos) { m_Info.posTarget = pos; }
	void SetSpeed(const float speed) { m_Info.fSpeed = speed; }
	void SetSpeedDest(const float speedDest) { m_Info.fSpeedDest = speedDest; }
	void SetRoadStart(CRoad* RoadStart) { m_Info.pRoadStart = RoadStart; }
	void SetRoadTarget(CRoad* RoadTarget) { m_Info.pRoadTarget = RoadTarget; }

protected:	// 派生クラスからもアクセス可能

	// メンバ関数
	virtual void MoveRoad();
	virtual void SearchRoad();
	virtual void ReachRoad();
	void TailLamp();
	virtual void Break();
	void Set();

	// メンバ変数
	CObjectX* m_pObj;

private:	// 自分だけがアクセス可能

	// メンバ関数
	virtual void Move();
	void Rot();
	bool Collision();

	// メンバ変数
	SInfo m_Info;				// 自分自身の情報
	CEffekseer::CEffectData* m_pTailLamp;
};

#endif
