//==========================================================
//
// 消火栓ギミック [gimmick_firehydrant.h]
// Author : Ibuki Okusada
// 
//==========================================================
#ifndef _FIREHYDRANT_H_		// このマクロが定義されていない場合
#define _FIREHYDRANT_H_		// 二重インクルード防止用マクロを定義

#include "gimmick.h"

// 前方宣言
class CObjectX;

//==========================================================
// 消火栓のクラス定義
//==========================================================
class CFireHydrant : public CGimmick
{
private:

public:	// 誰でもアクセス可能

	CFireHydrant();	// コンストラクタ(オーバーロード)
	~CFireHydrant() override;	// デストラクタ

	// メンバ関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	static CFireHydrant* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale);

private:	// 自分だけがアクセス可能

	// メンバ変数
	CObjectX* m_pObj;
};

#endif

