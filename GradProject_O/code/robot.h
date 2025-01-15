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

// 前方宣言
class CObjectX;

//==========================================================
// ロボットのクラス定義
//==========================================================
class CRobot : public CTask
{

public:	// 誰でもアクセス可能

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

	// メンバ関数(設定)
	void SetMove(const D3DXVECTOR3& move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);

	void SetNext(CRobot* pNext) { m_pNext = pNext; }
	void SetPrev(CRobot* pPrev) { m_pPrev = pPrev; }

protected:	// 派生クラスからもアクセス可能

	// メンバ関数
	void Set();

	// メンバ変数
	CObjectX* m_pObj;

private:	// 自分だけがアクセス可能

	// メンバ関数
	void MoveControl();
	void SetPosTerget(const float& Distance);
	bool TergetReach();

	// 情報構造体
	struct SInfo
	{
		D3DXVECTOR3 pos;		// 位置
		D3DXVECTOR3 rot;		// 向き
		D3DXVECTOR3 rotDest;	// 目標向き
		D3DXVECTOR3 move;		// 移動量
		D3DXVECTOR3 posOld;		// 設定位置
		int nTargetID;
		int nId;

		// コンストラクタ
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), rotDest(VECTOR3_ZERO),
			move(VECTOR3_ZERO), posOld(VECTOR3_ZERO),
			nTargetID(0), nId(-1) {}
	};

	// メンバ変数
	SInfo m_Info;				// 自分自身の情報
	CRobot* m_pPrev;			// 前のオブジェクトへのポインタ
	CRobot* m_pNext;			// 次のオブジェクトへのポインタ

	D3DXVECTOR3 m_PosTarget[2];

};

#endif

