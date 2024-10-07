//==========================================================
//
// メッシュの管理 [mesh.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MESH_H_		// このマクロが定義されていない場合
#define _MESH_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "object.h"

//**********************************************************
// メッシュクラスの定義(派生クラス)
//**********************************************************
class CObjectMesh : public CObject
{
public:	// 誰でもアクセス可能な定義

	// メッシュ種類
	enum TYPE
	{
		TYPE_NONE = 0,	// 何もない
		TYPE_FIELD,		// 床
		TYPE_WALL,		// 壁
		TYPE_DOME,		// ドーム
		TYPE_CYLINDER,	// シリンダー
		TYPE_MAX
	};

public:		// 誰でもアクセス可能

	//CMesh();	
	CObjectMesh(int nPriority = 3);	// コンストラクタ
	virtual ~CObjectMesh();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void);
	virtual void Update(void) = 0;
	virtual void Draw(void);
	void NotMtxDraw(void);
	virtual void SetVtxInfo(void) = 0;
	void BindTexture(int nIdx);
	void Create(int nWidth, int nHeight);

	// メンバ関数(設定)
	void SetPosition(const D3DXVECTOR3 pos);
	void SetRotation(const D3DXVECTOR3 rot);
	void SetMtx(D3DXMATRIX MtxWorld) { m_mtxWorld = MtxWorld; }

	// メンバ関数(取得)
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXMATRIX *GetMtx(void) { return &m_mtxWorld; }
	int GetVertex(void) { return m_nVertex; }
	int GetIndex(void) { return m_nIndex; }
	int GetNumWidth(void) { return m_nNumWidth; }
	int GetNumHeight(void) { return m_nNumHeight; }

protected:	// 派生クラスからもアクセス可能

	// メンバ関数
	void SetVtx(void);

	// メンバ変数
	VERTEX_3D *m_pVtx;	// 頂点情報

private:	// 自分だけがアクセス可能

	// メンバ関数
	void CreateVertex(void);
	void CreateIndex(void);
	void SetIndex(void);
	void SetSlowCol(void);

	// メンバ変数
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	//頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdexBuff;	//インデックスバッファへのポインタ
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	D3DXVECTOR3 m_pos;	// 位置
	D3DXVECTOR3 m_rot;	// 向き
	int m_nIdxTexture;	// テクスチャ番号
	int m_nNumHeight;	// 高さ枚数
	int m_nNumWidth;	// 幅枚数
	int m_nVertex;		// 頂点数
	int m_nIndex;		// 描画番号数
	TYPE m_Type;		// 種類
};

#endif