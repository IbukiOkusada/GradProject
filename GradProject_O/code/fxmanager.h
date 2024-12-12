//============================================================
//
//	シェーダー管理[FXManager.h]
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
#include "shaderlight.h"
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
	void SetLightVec(const D3DXVECTOR4& rVec);
	void SetCameraVec(const D3DXVECTOR3& rVec);
	void SetMatrixWorld(const D3DXMATRIX& rmtxWorld);
	void SetView(const D3DXMATRIX& rmtxView);
	void SetViewpos(const D3DXVECTOR3& rposView);
	void SetProj(const D3DXMATRIX& rmtxPorj);
	void SetScale(const D3DXMATRIX& rmtxScale);
	void SetTechnique(const D3DXHANDLE pTechnique);
	void SetLight();
	void SetParamToEffect();
	// 描画の開始を宣言する
	HRESULT Begin();
	// 描画の終了を宣言する
	HRESULT End();
	// パスの開始を宣言する
	HRESULT BeginPass();
	// パスの終了を宣言する
	HRESULT EndPass();
	LPD3DXEFFECT GetEffect() { return m_pEffect; }
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
	D3DXHANDLE m_hMatAmbient;					// マテリアル環境色
	D3DXHANDLE m_hMatEmissive;					// マテリアル発光色
	D3DXHANDLE m_hMatSpecula;					// マテリアル反射色
	D3DXHANDLE m_hMatPower;					// マテリアル強度
	D3DXHANDLE m_hUseTex;						// テクスチャの使用可否
	D3DXHANDLE m_hViewvec;					//視点ベクトル
	D3DXHANDLE m_hWorldMat;						// ワールド変換行列ハンドル
	D3DXHANDLE m_hViewMat;						// ビュー変換行列ハンドル
	D3DXHANDLE m_hProjMat;						// 射影変換行列ハンドル
	D3DXHANDLE m_hMatScaleReverse;				// スケーリング逆行列
	D3DXHANDLE m_hnumLights;				// ライトの数
	D3DXHANDLE m_hLights;				// ライト構造体
	D3DXHANDLE m_hviewPos;				// 視点

	
	D3DXMATRIX m_matWorld;						// ワールド変換行列
	D3DXMATRIX m_matView;						// ビュー変換行列
	D3DXMATRIX m_matProj;						// 射影変換行列
	D3DXMATRIX m_matScaleReverse;				// スケーリング逆行列
	D3DXVECTOR4 m_DirectLight;					// 平行光源の方向ベクトル
	D3DXVECTOR4 m_viewPos;				// 視点
	D3DXVECTOR4 m_LightDiffuse;					// 拡散光色
	D3DXVECTOR4 m_MatDiffuse;					// マテリアル拡散光色
	D3DXVECTOR4 m_MatAmbient;					// マテリアル環境色
	D3DXVECTOR4 m_MatEmissive;					// マテリアル発光色
	D3DXVECTOR4 m_Viewvec;					//視点ベクトル
	D3DXVECTOR4 m_MatSpecula;					// マテリアル反射色
	float m_MatPower;					// マテリアル強度
	int m_numLights;
	CShaderLight::SLight m_lightArray[MAX_LIGHT];
	bool m_bBegin;
};

#endif	// _TOON_SHADOW_H_
