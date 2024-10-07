//==========================================================
//
// Xファイルモデルの処理全般 [objectX.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _OBJECTX_H_
#define _OBJECTX_H_

#include "main.h"
#include "object.h"

// マクロ定義
#define NUM_TEXTURE	(64)	// テクスチャ最大数

//**********************************************************
// Xファイルモデルクラスの定義
//**********************************************************
class CObjectX : public CObject
{
public:	// 自分だけがアクセス可能
	enum COLLISION_AXIS
	{
		TYPE_X = 0,
		TYPE_Y,
		TYPE_Z,
		TYPE_MAX
	};
public:	// 誰でもアクセス可能

	CObjectX(int nPriority = 0);	// コンストラクタ
	virtual ~CObjectX();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void BindFile(int nIdx);

	static CObjectX *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, const char *pFileName, const int nPriority = 4);
	static bool Collision(D3DXVECTOR3& pos, D3DXVECTOR3& posOld, D3DXVECTOR3& move, D3DXVECTOR3 vtxMin, D3DXVECTOR3 vtxMax, COLLISION_AXIS& Axis);
	static D3DXVECTOR3 Collision(D3DXVECTOR3& pos, D3DXVECTOR3& posOld, D3DXVECTOR3& move, D3DXVECTOR3 vtxMin, D3DXVECTOR3 vtxMax, D3DXVECTOR3 vtxMinOld, D3DXVECTOR3 vtxMaxOld, COLLISION_AXIS& Axis);
	static bool Touch(D3DXVECTOR3& pos, D3DXVECTOR3& posOld, D3DXVECTOR3& move, D3DXVECTOR3 vtxMin, D3DXVECTOR3 vtxMax);
	static bool CollisionCloss(D3DXVECTOR3 &pos, D3DXVECTOR3 &posOld,D3DXVECTOR3* posCollisioned = nullptr);
	static void CollisionLand(D3DXVECTOR3 &pos);
	void SetRotSize(D3DXVECTOR3 &SetMax, D3DXVECTOR3 &SetMin, D3DXVECTOR3 vtxMax, D3DXVECTOR3 vtxMin, float fRot);

	// メンバ関数(設定)
	void SetPosition(const D3DXVECTOR3 pos);
	void SetRotation(const D3DXVECTOR3 rot);
	void SetEnableCollision(const bool bEnable) { m_bEnableCollision = bEnable; }

	// メンバ関数(取得)
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	CObject2D *GetObject2D(void) { return NULL; }
	static CObjectX *GetTop(void) { return m_pTop; }
	CObjectX *GetNext(void) { return m_pNext; }
	virtual D3DXMATRIX *GetMtx(void) { return &m_mtxWorld; }
	int GetIdx(void) { return m_nIdxModel; }
	void ListOut(void);
	int GetModelType(void) { return m_nIdxModel; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	//D3DMATERIAL9 SetSlowCol(D3DMATERIAL9 *pMat);
	bool CollisionCheck(D3DXVECTOR3& pos, D3DXVECTOR3& posOld, D3DXVECTOR3& move, D3DXVECTOR3 vtxMin, D3DXVECTOR3 vtxMax, COLLISION_AXIS axis, const float fRefMulti = 0.5f);
	bool CollisionCheck(D3DXVECTOR3& pos, D3DXVECTOR3& posOld, D3DXVECTOR3& move, D3DXVECTOR3 vtxMin, D3DXVECTOR3 vtxMax, D3DXVECTOR3 vtxMinOld, D3DXVECTOR3 vtxMaxOld, COLLISION_AXIS axis);
	bool CollisionCheckCloss(D3DXVECTOR3& pos, D3DXVECTOR3& posOld, D3DXVECTOR3* posCollisioned = nullptr);

	// メンバ変数c
	static CObjectX *m_pTop;	// 先頭のオブジェクトへのポインタ
	static CObjectX *m_pCur;	// 最後尾のオブジェクトへのポインタ
	CObjectX *m_pPrev;	// 前のオブジェクトへのポインタ
	CObjectX *m_pNext;	// 次のオブジェクトへのポインタ
	D3DXVECTOR3 m_pos;	// 位置
	D3DXVECTOR3 m_rot;	// 向き
	D3DXMATRIX m_mtxWorld;	//ワールドマトリックス
	int m_nIdxModel;		// モデル番号
	bool m_bEnableCollision;	//当たり判定の有効・無効
};

#endif
