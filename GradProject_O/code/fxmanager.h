//============================================================
//
//	トゥーンシャドウヘッダー [FXManager.h]
//	Author：丹野竜之介
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _TOON_SHADOW_H_
#define _TOON_SHADOW_H_

//************************************************************
//	インクルードファイル
//************************************************************
#include"texture.h"

//************************************************************
//	クラス定義
//************************************************************
// トゥーンシェーダークラス
class CFXManager 
{
public:

	// コンストラクタ
	CFXManager();

	// デストラクタ
	~CFXManager();

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);		// 終了

	// メンバ関数
	void SetMaterial(const D3DMATERIAL9& rMaterial);		// マテリアル設定
	void SetDiffuse(const D3DXCOLOR& rDiffuse);				// 拡散光設定
	void SetMatrix(const D3DXMATRIX& rmtxWorld);
	void SetTechnique(const D3DXHANDLE pTechnique);
	// 静的メンバ関数
	static CFXManager* Create(void);		// 生成
	static CFXManager* GetInstance(void);	// 取得
	static void Release(void);				// 破棄

private:
	
	
	// 静的メンバ変数
	static CFXManager* m_pShader;	// シェーダー情報

	// メンバ変数
	LPD3DXEFFECT m_pEffect;						// エフェクト

	D3DXHANDLE m_pTechnique;					// テクニック

	D3DXHANDLE m_hDirectLight;					// 平行光源の方向ベクトル
	D3DXHANDLE m_hLightDiffuse;					// 拡散光色
	D3DXHANDLE m_hMatDiffuse;					// マテリアル拡散光色
	D3DXHANDLE m_hUseTex;						// テクスチャの使用可否

	D3DXHANDLE m_hWorldMat;						// ワールド変換行列ハンドル
	D3DXHANDLE m_hViewMat;						// ビュー変換行列ハンドル
	D3DXHANDLE m_hProjMat;						// 射影変換行列ハンドル

	D3DXMATRIX m_matWorld;						// ワールド変換行列
	D3DXMATRIX m_matView;						// ビュー変換行列
	D3DXMATRIX m_matProj;						// 射影変換行列
};

#endif	// _TOON_SHADOW_H_
