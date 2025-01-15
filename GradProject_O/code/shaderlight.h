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
#include "task.h"
#define MAX_LIGHT (20)
//************************************************************
//	クラス定義
//************************************************************
// ライトシェーダークラス
class CShaderLight
{
public:
	// コンストラクタ
	CShaderLight();
	// デストラクタ
	~CShaderLight();
	// オーバーライド関数
	HRESULT Init(void);	// 初期化
	void Uninit(void);		// 終了
	void Update(void);
	
	struct SLight 
	{
		D3DXVECTOR3 position;    // ライトの位置
		float intensity;         // 光の強度

		D3DXVECTOR3 direction;   // スポットライトの方向
		float range;             // ライトの影響範囲

		D3DXVECTOR3 color;       // ライトの色
		float spotAngle;         // スポットライトの角度
	};
	// 静的メンバ関数
	
	static CShaderLight::SLight* Create(D3DXVECTOR3 pos, D3DXVECTOR3 col,float intens,float range,D3DXVECTOR3 vec = VECTOR3_ZERO,float Angle = 0.0f);		// 生成
	static void Delete(CShaderLight::SLight* plight) { if (m_List != nullptr) { m_List->Delete(plight); } }
	static void Release() {SAFE_DELETE(m_List)}

	static Clist<CShaderLight::SLight*>* GetList() { if (m_List != nullptr) { return m_List; } return nullptr; }
private:
	

	// 静的メンバ変数
	static Clist<CShaderLight::SLight*>* m_List;

};

#endif	// _ShaderLight_#pragma once
