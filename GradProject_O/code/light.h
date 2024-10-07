//==========================================================
//
// ライトの処理全般 [light.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _LIGHT_H_		// このマクロが定義されていない場合
#define _LIGHT_H_		// 二重インクルード防止用マクロを定義

#include "main.h"

//==========================================================
// マクロ定義
//==========================================================
#define NUM_LIGHT	(4)	// ライトの総数

//**********************************************************
// ライトクラスの定義
//**********************************************************
class CLight
{
public:	// 誰でもアクセス可能
	CLight();	// コンストラクタ
	~CLight();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void SetLight(float fDiff);
	void EnablePointLight(bool bEnable);
	void SetPositonPointLight(D3DXVECTOR3 pos);

private:	// 自分だけがアクセス可能
	D3DLIGHT9 m_aLight[NUM_LIGHT];		// ライトの情報
};

#endif
