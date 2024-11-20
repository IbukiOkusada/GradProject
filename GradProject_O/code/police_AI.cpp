//==========================================================
//
// パトカーAI処理 [police_AI.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "police_AI.h"
#include "car.h"
#include "car_manager.h"
#include "road.h"
#include "road_manager.h"
#include "manager.h"
#include "debugproc.h"
#include "collision.h"
#include "player.h"
#include "player_manager.h"
#include "debugproc.h"
#include "police_manager.h"
#include "deltatime.h"
#include "a_star.h"

// マクロ定義

// 無名名前空間を定義
namespace
{

}

//==========================================================
// コンストラクタ
//==========================================================
CPoliceAI::CPoliceAI()
{
	// 値のクリア
}

//==========================================================
// デストラクタ
//==========================================================
CPoliceAI::~CPoliceAI()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CPoliceAI::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CPoliceAI::Uninit(void)
{

}

//==========================================================
// 更新処理
//==========================================================
void CPoliceAI::Update(void)
{

}

//==========================================================
// 生成
//==========================================================
CPoliceAI *CPoliceAI::Create()
{
	CPoliceAI *pPoliceAI = nullptr;

	pPoliceAI = DEBUG_NEW CPoliceAI;

	if (pPoliceAI != nullptr)
	{
		// 初期化処理
		pPoliceAI->Init();
	}

	return pPoliceAI;
}