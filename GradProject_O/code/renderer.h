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
private:

	// マルチターゲットレンダリング用情報
	struct MultiTargetInfo
	{
		float fStartColAlpha;	// 開始透明度
		float fStartMulti;		// 開始サイズ倍率
		float fAddTimer;		// タイマー増加数
		float fColAlpha;		// 透明度
		float fTimer;			// 遷移タイマー
		float fMulti;			// サイズ倍率
		bool bActive;			// 稼働中

		// コンストラクタ
		MultiTargetInfo() : fStartColAlpha(0.0f), fStartMulti(0.0f), fAddTimer(0.0f), 
			fColAlpha(0.0f), fMulti(0.0f), fTimer(0.0f), bActive(false){ }
	};

	// マルチターゲットレンダリング用生成データ
	struct MultiTarget
	{
		LPDIRECT3DTEXTURE9 pTextureMT[2];	// レンダリングターゲット用テクスチャ
		LPDIRECT3DSURFACE9 pRenderMT[2];	// テクスチャレンダリング用インターフェース
		LPDIRECT3DSURFACE9 pZBuffMT;		// テクスチャレンダリング用Zバッファ
		D3DVIEWPORT9 viewportMT;			// テクスチャレンダリング用ビューポート
		LPDIRECT3DVERTEXBUFFER9 pVtxBuff;	// 頂点バッファ
	};

public:	// 誰でもアクセス可能

	CRenderer();	// コンストラクタ
	~CRenderer();	// デストラクタ

	// メンバ関数
	HRESULT Init(HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	LPDIRECT3DDEVICE9 GetDevice(void);

	// マルチターゲット関連
	void GetDefaultRenderTarget(LPDIRECT3DSURFACE9* pRender, LPDIRECT3DSURFACE9* pZBuff, D3DXMATRIX* viewport, D3DXMATRIX* projection);	// デフォルトのレンダーターゲット取得
	void ChangeRendertarget(LPDIRECT3DSURFACE9 pRender, LPDIRECT3DSURFACE9 pZBuff, D3DXMATRIX viewport, D3DXMATRIX projection);	// ターゲット切明
	void ChangeTarget(const D3DXVECTOR3& posV, const D3DXVECTOR3& posR, const D3DXVECTOR3& vecU);
	LPDIRECT3DTEXTURE9 GetTextureMT(int idx) { return m_Multitarget.pTextureMT[idx]; }	// レンダリングターゲット用テクスチャ取得
	float GetGoalAlpha() { return m_MultitargetInfo.fColAlpha; }
	/**
	@brief	マルチターゲット画面の描画判定
	@param	fGoalAlpha	[in]	目標透明色
	@param	fGoalMulti	[in]	目標画面倍率
	@param	fTimer		[in]	目標までの時間
	@return	void
	*/
	void SetEnableDrawMultiScreen(float fGoalAlpha, float fGoalMulti, float fTimer);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void SetWire(bool bWire);

	// マルチターゲットレンダラーの初期化
	void InitMTRender();
	void SetMultiTarget();
	void DrawMultiTargetScreen(int texIdx, const D3DXCOLOR& col, const D3DXVECTOR2& size);	// マルチターゲット画面の描画

	// メンバ変数
	LPDIRECT3D9 m_pD3D;				//Direct3Dオブジェクトのポインタ
	LPDIRECT3DDEVICE9 m_pD3DDevice;	//Direct3Dデバイスへのポインタ
	bool m_bWire;					// ワイヤーフレームにするか否か
	LPDIRECT3DTEXTURE9 pTexture;
	LPDIRECT3DSURFACE9 m_pRenderTextureSurface;
	LPDIRECT3DSURFACE9 m_pZSurface;
	LPDIRECT3DSURFACE9 m_pOrgSurface;
	LPDIRECT3DSURFACE9 m_pOrgZBuffer;
	MultiTargetInfo m_MultitargetInfo;
	MultiTarget m_Multitarget;	// マルチターゲット用
};

#endif