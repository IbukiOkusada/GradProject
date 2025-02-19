//===================================================
//
//パーティクル処理
//Author 奥定伊吹
//
//===================================================
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "main.h"
#include "effect2D.h"

#define MAX_PARTICLE	(256)	// パーティクルの最大数

// クラスの定義
class CParticle2D
{
public:	// 誰でもアクセス可能

	CParticle2D();	// コンストラクタ
	~CParticle2D();	// デストラクタ

	// メンバ関数
	static void Create(const D3DXVECTOR3& pos, CEffect2D::TYPE type);
	static void Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& move, CEffect2D::TYPE type);

private:	// 自分だけがアクセス可能

	// メンバ関数
	static void Set(const D3DXVECTOR3& Defpos, const D3DXVECTOR3& Defmove, CEffect2D::TYPE type);
};

#endif

