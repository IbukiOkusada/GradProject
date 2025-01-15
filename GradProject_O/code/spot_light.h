//==========================================================
//
// スポットライトの処理 [spot_light.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _SPOT_LIGHT_H_		// このマクロが定義されていない場合
#define _SPOT_LIGHT_H_		// 二重インクルード防止用マクロを定義

//**********************************************************
// ライトクラスの定義
//**********************************************************
class CSpotLight
{
public:	// 誰でもアクセス可能
	CSpotLight();	// コンストラクタ
	~CSpotLight();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	static CSpotLight* Create();
	void Uninit();
	void Update();

	//==========================
	// 設定関数
	//==========================
	// 座標設定
	void SetPositon(const D3DXVECTOR3& pos);
	// 方向設定
	void SetDirection(const D3DXVECTOR3& dic);
	// 拡散光設定
	void SetDiffuse(const D3DXCOLOR& dif);
	// 範囲設定
	void SetRange(const float fRange);

	/**
	@brief	スポットライトのパラメータ設定
	@param	fOutRadian	[in]	外側のコーンの角度
	@param	fInRadian	[in]	内側のコーンの角度
	@param	fFallOff	[in]	内側と外側のコーンの減衰率
	@return	void
	*/
	void SetParameter(float fOutRadian, float fInRadian, float fFallOff = 1.0f);
	
	/**
	@brief	スポットライトの減衰設定
	@param	fOutRadian	[in]	定数減衰
	@param	fInRadian	[in]	線形減衰
	@param	fFallOff	[in]	二次減衰
	@return	void
	*/
	void SetAttenuation(float fConstant, float fLinear, float fSecondary);

private:	// 自分だけがアクセス可能

	D3DLIGHT9 m_Light;	// ライト
	int m_nId;
};

#endif

