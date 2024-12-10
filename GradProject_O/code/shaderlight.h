//============================================================
//
//	シェーダー用ライト情報[shaderlight.h]
//	Author：丹野竜之介
//
//============================================================
//************************************************************
//	二重インクルード防止
//************************************************************
#ifndef _ShaderLight_
#define _ShaderLight_

//************************************************************
//	インクルードファイル
//************************************************************
#include "list.h"
#define MAX_LIGHT (8)
//************************************************************
//	クラス定義
//************************************************************
// トゥーンシェーダークラス
class CShaderLight
{
public:

	// コンストラクタ
	CShaderLight();

	// デストラクタ
	~CShaderLight();

	struct SLight
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 direction;
		D3DXVECTOR3 color;
		float intensity;
		float range;
		float spotAngle;
	};

	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);		// 終了

	

	// 静的メンバ関数
	static CShaderLight* Create(void);		// 生成
	static CShaderLight* GetInstance(void);	// 取得
	static void Release(void);				// 破棄

private:


	// 静的メンバ変数
	static CShaderLight* m_pShaderLight;	// シェーダー情報

	// メンバ変数
	static Clist<SLight> * m_List;
};

#endif	// _ShaderLight_#pragma once
