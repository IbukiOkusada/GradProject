//==========================================================
//
// ガードレールギミック [gimmick_guardrail.h]
// Author : Ibuki Okusada
// 
//==========================================================
#ifndef _GUARDRAIL_H_		// このマクロが定義されていない場合
#define _GUARDRAIL_H_		// 二重インクルード防止用マクロを定義

#include "gimmick.h"

// 前方宣言
class CObjectX;

//==========================================================
// 消火栓のクラス定義
//==========================================================
class CGimmickGuardRail : public CGimmick
{
private:

public:	// 誰でもアクセス可能

	CGimmickGuardRail();	// コンストラクタ(オーバーロード)
	~CGimmickGuardRail() override;	// デストラクタ

	// メンバ関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	static CGimmickGuardRail* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale);
	
	// 衝突時の判定
	void Hit(const D3DXVECTOR3& HitPos);
	void SetColMulti(const D3DXCOLOR& col) override;

private:	// 自分だけがアクセス可能

	// メンバ関数
	virtual void SetObjPos(const D3DXVECTOR3& pos) override;
	virtual void SetObjRot(const D3DXVECTOR3& rot) override;
	virtual void SetObjScale(const D3DXVECTOR3& scale) override;
	void Away();

	// メンバ変数
	D3DXVECTOR3 m_TargetRot;	// 吹っ飛び後向き
	D3DXVECTOR3 m_TargetPos;	// 吹っ飛び後座標
	CObjectX* m_pObj;			// 描画オブジェクト
	bool m_bHit;				// 衝突した
};

#endif

