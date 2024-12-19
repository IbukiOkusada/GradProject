//===============================================
//
// 数字全般の処理 [number.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _NUNBER_H_		// このマクロが定義されていない場合
#define _NUNBER_H_		// 二重インクルード防止用マクロを定義

#include "main.h"

// 前方宣言
class CObject2D;

//===============================================
// ナンバークラスの定義
//===============================================
class CNumber
{
public:	// 誰でもアクセス可能

	CNumber();	// コンストラクタ
	~CNumber();	// デストラクタ

	// メンバ関数
	HRESULT Init(D3DXVECTOR3 pos, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static CNumber *Create(void);
	static CNumber *Create(D3DXVECTOR3 pos, float fWidth, float fHeight);
	CObject2D *GetObject2D(void) { return m_pObject2D; }
	void PolygonDelete(void);
	void SetPosition(const D3DXVECTOR3& pos);
	
	void BindTexture(LPDIRECT3DTEXTURE9 m_Texture);  //テクスチャをバインド

	// メンバ関数(設定)
	void SetIdx(const int nIdx);

	// メンバ関数(取得)
	int GetIdx(void) { return m_nIdx; }

private:	// 自分だけがアクセス可能
	
	// メンバ変数
	LPDIRECT3DTEXTURE9 m_pTexture;        //テクスチャへのポインタ
	CObject2D *m_pObject2D;	// 2Dオブジェクトのポインタ
	int m_nIdx;	// 数字の番号
	int m_nIdxTexture;
};

#endif

