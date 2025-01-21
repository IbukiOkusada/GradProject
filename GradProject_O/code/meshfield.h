//==========================================================
//
// メッシュフィールドの処理 [meshfield.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MESHFIELD_H_	// このマクロが定義されていない場合
#define _MESHFIELD_H_	// 二重インクルード防止用マクロを定義

#include "main.h"
#include "mesh.h"
#include "list.h"

//**********************************************************
// メッシュフィールドクラスの定義(派生クラス)
//**********************************************************
class CMeshField : public CObjectMesh
{
public:		// 誰でもアクセス可能

	CMeshField();	// コンストラクタ
	~CMeshField();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtxInfo(void);
	static CMeshField *Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, float fWidth, float fHeight,
		const char *pFileName, const int nWidth = 1, const int nHeight = 1, const int nPriority = 3);
	void UpDownLoad(const char *pFileName);

	// メンバ関数(取得)
	float GetWidth(void) { return m_fWidth; }
	float GetHeight(void) { return m_fHeight; }
	static Clist<CMeshField*>* GetList() { return m_List; }
	bool IsCollision() { return m_bCollision; }

	// メンバ関数(設定)
	void SetSize(float fWidth, float fHeight);
	void SetTexMove(D3DXVECTOR2 texmove) { m_texmove = texmove; }
	void SetCollision(bool bHit = true) { m_bCollision = bHit; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	void SetTex(void);

	// メンバ変数
	D3DXVECTOR2 m_texmove;	// テクスチャ移動量
	D3DXVECTOR2 m_tex;	// テクスチャ座標
	float m_fWidth;			// 幅
	float m_fHeight;		// 高さ
	bool m_bCollision;			// 暑い状態か否か
	static Clist<CMeshField*>* m_List;
};

#endif
