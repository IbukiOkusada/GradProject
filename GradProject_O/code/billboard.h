//==========================================================
//
// ビルボードの処理全般 [billboard.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

#include "main.h"		//main.hで定義しているものが必要なためinclude
#include "object.h"

//**********************************************************
// ビルボードクラスの定義
//**********************************************************
class CObjectBillboard : public CObject
{
public:

	// 合成方法列挙型
	enum FUSION
	{
		FUSION_ADD = 0,	// 加算合成
		FUSION_MINUS,		// 減算合成
		FUSION_NORMAL,	// 合成しない
		FUSION_MAX
	};

public:	// 誰でもアクセス可能

	CObjectBillboard(int nPriority = 3);	// コンストラクタ
	~CObjectBillboard();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void RotFusionDraw(void);
	static CObjectBillboard *Create(D3DXVECTOR3 pos, const int nPriority = 3);
	void BindTexture(int nIdx);

	// メンバ関数(設定)
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRotation(const D3DXVECTOR3 rot) { m_rot = rot; }
	void SetVtx(void);
	void SetVtx(D3DXCOLOR col);
	void SetMtx(void);
	void SetVtx(const int nPatternAnim, const int nTexWidth, const int nTexHeight);
	void SetVtx(const float fTexU, const float fTexV);
	void SetCol(const D3DXCOLOR col);
	void SetSize(float fWidth, float fHeight);
	float GetHeight(void) { return m_fHeight; }
	float GetWidth(void) { return m_fWidth; }
	virtual CEnemy *GetEnemy(void) { return NULL; }
	void SetFusion(FUSION fusion) { m_fusion = fusion; }
	void SetAlphaText(bool bTest) { m_bAlphatest = bTest; }
	void SetZTest(bool bTest) { m_bZtest = bTest; }
	void SetLighting(bool bTest) { m_bLighting = bTest; }
	void SetCurrent(D3DXMATRIX *Current) { m_pCurrent = Current; }

	// メンバ関数(取得)
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	CObject2D *GetObject2D(void) { return NULL; }
	D3DXMATRIX *GetMtx(void) { return &m_mtxWorld; }
	D3DXCOLOR GetCol(void) { return m_col; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	//頂点バッファへのポインタ
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	D3DXMATRIX *m_pCurrent;    //　親
	D3DXVECTOR3 m_pos;		//位置
	D3DXVECTOR3 m_rot;		//向き
	D3DXCOLOR m_col;		// 色
	int m_nIdexTexture;	// テクスチャ番号
	float m_fWidth;		// 幅
	float m_fHeight;	// 高さ
	FUSION m_fusion;	// 合成方法
	bool m_bZtest;	// Zテストをオンにするか
	bool m_bLighting;	// ライティングをオフにするか
	bool m_bAlphatest;	// αテストをオンにするか
};


#endif
