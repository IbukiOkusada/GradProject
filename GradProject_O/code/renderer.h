//==========================================================
//
// DirectXのレンダラー処理 [renderer.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _RENDERER_H_		// このマクロが定義されていない場合
#define _RENDERER_H_		// 二重インクルード防止用マクロを定義

#include "main.h"

// レンダラークラスの定義
class CRenderer
{
public:	// 誰でもアクセス可能

	CRenderer();	// コンストラクタ
	~CRenderer();	// デストラクタ

	// メンバ関数
	HRESULT Init(HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	LPDIRECT3DDEVICE9 GetDevice(void);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void SetWire(bool bWire);

	// メンバ変数
	LPDIRECT3D9 m_pD3D;				//Direct3Dオブジェクトのポインタ
	LPDIRECT3DDEVICE9 m_pD3DDevice;	//Direct3Dデバイスへのポインタ
	bool m_bWire;					// ワイヤーフレームにするか否か
	LPDIRECT3DTEXTURE9 pTexture;
	LPDIRECT3DSURFACE9 m_pRenderTextureSurface;
	LPDIRECT3DSURFACE9 m_pZSurface;
	LPDIRECT3DSURFACE9 m_pOrgSurface;
	LPDIRECT3DSURFACE9 m_pOrgZBuffer;
};

#endif