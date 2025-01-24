//===============================================
//
// パーティクルの処理 [particle.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "particle3D.h"
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
CParticle3D::CParticle3D()
{

}

//===============================================
// デストラクタ
//===============================================
CParticle3D::~CParticle3D()
{

}

//===============================================
// 生成
//===============================================
void CParticle3D::Create(const D3DXVECTOR3& pos, CEffect3D::TYPE type)
{
	// オブジェクトの種類の設定
	Set(pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), type);
}

//===============================================
// 頂点情報設定
//===============================================
void CParticle3D::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& move, CEffect3D::TYPE type)
{
	// オブジェクトの種類の設定
	Set(pos, move, type);
}

//===============================================
// 設定
//===============================================
void CParticle3D::Set(const D3DXVECTOR3& Defpos, const D3DXVECTOR3& Defmove, CEffect3D::TYPE type)
{
	D3DXVECTOR3 pos = {};
	D3DXVECTOR3 move = {};	// 移動量
	D3DXCOLOR col = {};	// 色
	float fRadius = 0.0f;
	float fLife = 0;
	D3DXVECTOR3 nor;

	// 移動ベクトルを求める
	D3DXVec3Normalize(&nor, &Defmove);	// ベクトルを正規化する

	switch (type)
	{
	case CEffect3D::TYPE_NONE:	// 何もない

		break;

	case CEffect3D::TYPE_SMAKE:	// 煙

		for (int nCnt = 0; nCnt < 2; nCnt++)
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
			fRadius = 50.0f;

			//寿命の設定
			fLife = 100.0f;

			CEffect3D::Create(Defpos + move, move, col, fRadius, fLife, type);
		}

		break;

	case CEffect3D::TYPE_SPLASH:	// エフェクト

		for (int nCnt = 0; nCnt < 1; nCnt++)
		{
			// 座標の設定
			pos = Defpos;

			float angle = static_cast<float>(rand() % 629 - 314) * 0.01f;
			float speed = static_cast<float>(rand() % 100) * 0.11f;

			//移動量の設定
			move.x = sinf(angle) * speed;
			move.y = static_cast<float>(rand() % 20) + 25.0f;
			move.z = cosf(angle) * speed;

			float colrand = static_cast<float>(rand() % 40) * 0.01f;

			//色の設定
			col = D3DXCOLOR(colrand, colrand, 0.7f, 1.0f);

			//半径の設定
			fRadius = 30.0f;

			//寿命の設定
			fLife = 90.0f;

			CEffect3D::Create(Defpos + move, move, col, fRadius, fLife, type);
		}

		break;

	case CEffect3D::TYPE_LASER:	// エフェクト

		for (int nCnt = 0; nCnt < 1; nCnt++)
		{
			// 座標の設定
			pos = Defpos;

			float speed = 5.0f;

			//移動量の設定
			move.x = nor.x * speed;
			move.y = 0.0f;
			move.z = nor.z * speed;

			float colrand = 0.4f;

			//色の設定
			col = D3DXCOLOR(colrand + 0.6f, colrand, colrand, 1.0f);

			//半径の設定
			fRadius = 30.0f;

			//寿命の設定
			fLife = 1.0f;

			CEffect3D::Create(Defpos + move, move, col, fRadius, fLife, type);
		}

		break;
	}
}