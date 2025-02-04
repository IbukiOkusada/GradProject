//============================================================
//
//	シェーダー管理[Postprocess.h]
//	Author：丹野竜之介
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _postprocess_H_
#define _postprocess_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include"texture.h"
#include "shaderlight.h"
#include "object2D.h"
//************************************************************
//	クラス定義
//************************************************************
// シェーダークラス
class CPostprocess
{
public:

	// コンストラクタ
	CPostprocess();

	// デストラクタ
	~CPostprocess();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);		// 終了

	void Draw();
	void Rendering(LPDIRECT3DTEXTURE9 pTex, D3DXCOLOR col);

	// 静的メンバ関数
	static CPostprocess* Create(void);		// 生成
	static CPostprocess* GetInstance(void);	// 取得
	static void Release(void);				// 破棄
	HRESULT Loadshader(char* path, LPD3DXEFFECT* pEffect);
private:
	// 静的メンバ変数
	static CPostprocess* m_pShader;	// シェーダー情報

	// メンバ変数
	LPD3DXEFFECT m_pBright;						// エフェクト
	LPD3DXEFFECT m_pGauss;						// エフェクト
	LPD3DXEFFECT m_pAberration;						// エフェクト

	LPDIRECT3DTEXTURE9 m_pGaussTex;
	LPDIRECT3DSURFACE9 m_pGaussSurface;
	ID3DXSprite* m_pSprite;
	float time;
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;	// 頂点バッファ
};

#endif	// _postprocess_H_
