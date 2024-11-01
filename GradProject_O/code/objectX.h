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
	void BindFile(const char* file);

	static CObjectX *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, const char *pFileName, const int nPriority = 4);
	static bool Collision(D3DXVECTOR3& pos, D3DXVECTOR3& posOld, D3DXVECTOR3& move, D3DXVECTOR3 vtxMin, D3DXVECTOR3 vtxMax, COLLISION_AXIS& Axis);
	static D3DXVECTOR3 Collision(D3DXVECTOR3& pos, D3DXVECTOR3& posOld, D3DXVECTOR3& move, D3DXVECTOR3 vtxMin, D3DXVECTOR3 vtxMax, D3DXVECTOR3 vtxMinOld, D3DXVECTOR3 vtxMaxOld, COLLISION_AXIS& Axis);
	static bool Touch(D3DXVECTOR3& pos, D3DXVECTOR3& posOld, D3DXVECTOR3& move, D3DXVECTOR3 vtxMin, D3DXVECTOR3 vtxMax);
	static bool CollisionCloss(D3DXVECTOR3 &pos, D3DXVECTOR3 &posOld,D3DXVECTOR3* posCollisioned = nullptr);
	static void CollisionLand(D3DXVECTOR3 &pos);
	void SetRotSize(D3DXVECTOR3 &SetMax, D3DXVECTOR3 &SetMin, D3DXVECTOR3 vtxMax, D3DXVECTOR3 vtxMin, float fRot);

	// メンバ関数(設定)
	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);
	void SetScale(const D3DXVECTOR3& scale);
	void SetEnableCollision(const bool bEnable) { m_bEnableCollision = bEnable; }

	// メンバ関数(取得)
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetScale(void) { return m_scale; }
	CObject2D *GetObject2D(void) { return NULL; }
	static CObjectX *GetTop(void) { return m_pTop; }
	CObjectX *GetNext(void) { return m_pNext; }
	virtual D3DXMATRIX *GetMtx(void) { return &m_mtxWorld; }
	int GetIdx(void) { return m_nIdxModel; }
	void ListOut(void);
	int GetModelType(void) { return m_nIdxModel; }
	D3DXVECTOR3& GetVtxMax(void);
	D3DXVECTOR3& GetVtxMin(void);
	D3DXCOLOR& GetColMuliti() { return m_ColMulti; }
	void SetColMulti(const D3DXCOLOR& col) { m_ColMulti = col; }
	D3DXCOLOR& GetColAdd() { return m_AddCol; }
	void SetColAdd(const D3DXCOLOR& col) { m_AddCol = col; }

protected:
	void Quaternion();
	void DrawOnry();
	void CalWorldMtx();

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数c
	static CObjectX *m_pTop;	// 先頭のオブジェクトへのポインタ
	static CObjectX *m_pCur;	// 最後尾のオブジェクトへのポインタ
	CObjectX *m_pPrev;	// 前のオブジェクトへのポインタ
	CObjectX *m_pNext;	// 次のオブジェクトへのポインタ
	D3DXVECTOR3 m_pos;	// 位置
	D3DXVECTOR3 m_rot;	// 向き
	D3DXVECTOR3 m_scale;	// 拡大縮小
	D3DXMATRIX m_mtxWorld;	//ワールドマトリックス
	D3DXCOLOR m_ColMulti;
	D3DXCOLOR m_AddCol;
	int m_nIdxModel;		// モデル番号
	bool m_bEnableCollision;	//当たり判定の有効・無効
};

#endif
