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
		CRoad* pRoadStart;		// 移動開始地点
		CRoad* pRoadTarget;		// 目標位置
		float speed;
		float speedDest;
	};

public:	// 誰でもアクセス可能

	CCar();	// コンストラクタ(オーバーロード)
	~CCar();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CCar*Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move);

	// メンバ関数(取得)
	D3DXVECTOR3 GetMove(void) { return m_Info.move; }
	D3DXVECTOR3 GetPosition(void) { return m_Info.pos; }
	D3DXVECTOR3 GetRotation(void) { return m_Info.rot; }
	D3DXVECTOR3 GetOldPosition(void) { return m_Info.posOld; }
	CCar* GetNext(void) { return m_pNext; }
	CCar* GetPrev(void) { return m_pPrev; }

	// メンバ関数(設定)
	void SetMove(const D3DXVECTOR3 move) { m_Info.move = move; }
	void SetPosition(const D3DXVECTOR3 pos) { m_Info.pos = pos; }
	void SetRotation(const D3DXVECTOR3 rot) { m_Info.rot = rot; }
	void SetNext(CCar* pNext) { m_pNext = pNext; }
	void SetPrev(CCar* pPrev) { m_pPrev = pPrev; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	void Move();
	void Rot();
	void MoveRoad();
	void SearchRoad();
	void ReachRoad();

	// メンバ変数
	CCar* m_pPrev;			// 前のオブジェクトへのポインタ
	CCar* m_pNext;			// 次のオブジェクトへのポインタ
	SInfo m_Info;				// 自分自身の情報
	CObjectX* m_pObj;
};

#endif
