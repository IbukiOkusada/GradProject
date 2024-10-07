//==========================================================
//
// オブジェクト(2D)の処理 [object2D.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _OBJECT2D_H_		// このマクロが定義されていない場合
#define _OBJECT2D_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "object.h"

#define DEF_2DPRI	(3)	// 2D基本優先順位

// 前方宣言
class CMultiBg;

//==========================================================
// オブジェクト(2D)のクラス定義(派生クラス)
//==========================================================
class CObject2D : public CObject
{
public:	// 誰でもアクセス可能

	//CObject2D();	// コンストラクタ
	CObject2D(const D3DXVECTOR3 pos);	// コンストラクタ(オーバーロード)
	CObject2D(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const int nPriority = 3);	// コンストラクタ(オーバーロード)
	CObject2D(int nPriority = DEF_2DPRI);	// デフォルト引数コンストラクタ
	virtual ~CObject2D();	// デストラクタ
	
	// メンバ関数
	virtual HRESULT Init(void);
	HRESULT Init(const char *pFileName);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	static CObject2D *Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const int nPriority = 3);
	static CObject2D *Create(const int nPriority = 3);
	void BindTexture(int nIdx);

	// メンバ関数(設定)
	void SetPosition(const D3DXVECTOR3 pos);
	void SetRotation(const D3DXVECTOR3 rot);
	void SetLength(const float fLength);
	void SetAngle(const float fAngle);
	void SetLength(float fWidth, float fHeight);
	void SetSize(float fWidth, float fHeight);
	void SetWidth(float fWidth);
	void SetHeight(float fHeight);
	void SetIdxTex(int nIdx);

	// メンバ関数(取得)
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXCOLOR GetCol(void) { return m_col; }
	float GetLength(void) { return m_fLength; }
	float GetAngle(void) { return m_fAngle; }
	float GetWidth(void) { return m_fWidth; }
	float GetHeight(void) { return m_fHeight; }
	CObject2D *GetObject2D(void) { return this; }
	int GetIdxTex(void) { return m_nIdxTexture; }

	// メンバ関数
	void SetVtx(void);
	void SetVtx(const int nPatternAnim, const int nTexWidth, const int nTexHeight);
	void SetVtx(const float fTexU, const float fTexV, const float fWidth, const float fHeight);
	void SetTex(const float fTexU = 0.0f, const float fTexV = 0.0f, const float fWidth = 1.0f, const float fHeight = 1.0f);
	void SetPlayerVtx(void);
	void SetCol(const D3DXCOLOR col);

protected:	// 派生クラスからもアクセス可能
	LPDIRECT3DVERTEXBUFFER9 GetVtx(void) { return m_pVtxBuff; }

private:	// 自分だけがアクセス可能

	// メンバ変数
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファへのポインタ
	float m_fWidth;		// 幅
	float m_fHeight;		// 高さ
	float m_fLength;		// 対角線の長さ
	float m_fAngle;		// 対角線の角度
	D3DXVECTOR3 m_pos;	// 位置
	D3DXVECTOR3 m_rot;	// 向き
	D3DXCOLOR m_col;		// 色
	int m_nIdxTexture;	// テクスチャ番号
};

#endif