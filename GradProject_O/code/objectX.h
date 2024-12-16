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
#include "list.h"

//**********************************************************
// Xファイルモデルクラスの定義
//**********************************************************
class CObjectX : public CObject
{
public:	// 自分だけがアクセス可能

	enum TYPE
	{
		TYPE_NORMAL = 0,	// 回転
		TYPE_QUATERNION,	// クォータニオン
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
	virtual void DrawOnShader(void);
	void BindFile(int nIdx);
	void BindFile(const char* file);

	static CObjectX *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, const char *pFileName, const int nPriority = 4);
	void SetRotSize(D3DXVECTOR3 &SetMax, D3DXVECTOR3 &SetMin, D3DXVECTOR3 vtxMax, D3DXVECTOR3 vtxMin, float fRot);

	// メンバ関数(設定)
	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);
	void SetScale(const D3DXVECTOR3& scale);
	void SetEnableCollision(const bool bEnable) { m_bEnableCollision = bEnable; }
	void SetRotateType(const TYPE& type) { m_Type = type; }
	void SetHit(bool bHit) { m_bHit = bHit; }
	void SetShadowHeight(const float fHeight) { m_fShadowHeight = fHeight; }
	void SetEnableShadow(const bool bUse) { m_bShadow = bUse; }

	// メンバ関数(取得)
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3* GetPos(void) { return &m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetScale(void) { return m_scale; }
	CObject2D *GetObject2D(void) { return NULL; }
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
	bool GetHit() { return m_bHit; }
	bool GetHitOld() { return m_bHitOld; }
	bool GetEnableCollision() { return m_bEnableCollision; }
	float GetShadowHeight() { return m_fShadowHeight; }
	static Clist<CObjectX*>* GetList() { return &m_List; }

	void CalWorldMtx();
	void Quaternion();

protected:
	void DrawOnry();
	void DrawOnryShader();
	void DrawShadow();
	void DrawShadowonShader();

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	static Clist<CObjectX*> m_List;	// リスト
	D3DXVECTOR3 m_pos;	// 位置
	D3DXVECTOR3 m_rot;	// 向き
	D3DXVECTOR3 m_scale;	// 拡大縮小
	D3DXMATRIX m_mtxWorld;	//ワールドマトリックス
	D3DXMATRIX m_mtxscale;
	D3DXCOLOR m_ColMulti;
	D3DXCOLOR m_AddCol;
	TYPE m_Type;		// 回転種類
	int m_nIdxModel;	// モデル番号
	float m_fShadowHeight;	// 影の高さ
	bool m_bEnableCollision;	//当たり判定の有効・無効
	bool m_bHit;		// 衝突した
	bool m_bHitOld;		// 前回の衝突判定
	bool m_bShadow;
};

#endif
