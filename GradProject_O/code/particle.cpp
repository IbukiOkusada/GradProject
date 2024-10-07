//===============================================
//
// パーティクルの処理 [particle.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "particle.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "texture.h"
#include "billboard.h"

//===============================================
// マクロ定義
//===============================================
#define LIFE	(30)		// 寿命

//===============================================
// コンストラクタ
//===============================================
CParticle::CParticle()
{

}

//===============================================
// デストラクタ
//===============================================
CParticle::~CParticle()
{

}

//===============================================
// 生成
//===============================================
void CParticle::Create(const D3DXVECTOR3& pos, CEffect::TYPE type)
{
	// オブジェクトの種類の設定
	Set(pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), type);
}

//===============================================
// 頂点情報設定
//===============================================
void CParticle::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& move, CEffect::TYPE type)
{
	// オブジェクトの種類の設定
	Set(pos, move, type);
}

//===============================================
// 設定
//===============================================
void CParticle::Set(const D3DXVECTOR3& Defpos, const D3DXVECTOR3& Defmove, CEffect::TYPE type)
{
	D3DXVECTOR3 pos = {};
	D3DXVECTOR3 move = {};	// 移動量
	D3DXCOLOR col = {};	// 色
	CEffect::TYPE ParType = type;
	float fRadius = 0.0f;
	float fLife = 0;
	D3DXVECTOR3 nor;
	float fCol = (float)(rand() % 3) / 10.0f + 0.3f;

	// 移動ベクトルを求める
	D3DXVec3Normalize(&nor, &Defmove);	// ベクトルを正規化する

	switch (type)
	{
	case CEffect::TYPE_NONE:	// 何もない

		break;

	case CEffect::TYPE_SMAKE:	// 煙

		for (int nCnt = 0; nCnt < 20; nCnt++)
		{
			// 座標の設定
			pos = Defpos;

			//移動量の設定
			move.x = sinf((float)(rand() % 629 - 314) * 0.01f) * ((float)(rand() % 100)) * 0.09f;
			move.y = ((float)(rand() % 50)) * 0.15f;
			move.z = cosf((float)(rand() % 629 - 314) * 0.01f) * ((float)(rand() % 100)) * 0.09f;

			//色の設定
			col = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);

			//半径の設定
			fRadius = 100.0f;

			//寿命の設定
			fLife = 500.0f;

			CEffect::Create(Defpos + move, move, col, fRadius, fLife, type);
		}

		break;
	}
}