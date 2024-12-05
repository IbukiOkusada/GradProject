//==========================================================
//
// フォグ処理 [fog.cpp]
// Author : 丹野 竜之介
//
//==========================================================
#include "fog.h"
#include "debugproc.h"
#include"manager.h"
#include "renderer.h"

CFog::CFog()
{
}

CFog::~CFog()
{
}
//======================================
//初期化処理
//======================================
void CFog::Init()
{
	//数値のリセット
	m_FogMode = D3DFOG_NONE;
	m_FogStart = 0.0f;
	m_FogEnd = 0.0f;
	m_FogDensity = 0.0f;
	m_FogColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
}

//======================================
//終了処理
//======================================
void CFog::Uninit()
{
	CRenderer * pRenderer = CManager::GetInstance()->GetRenderer();
	LPDIRECT3DDEVICE9 pDevice; //デバイスのポインタ
	pDevice = pRenderer->GetDevice();
	pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE); //フォグの無効化
}

//======================================
//更新処理
//======================================
void CFog::Update()
{
	//CDebugProc * pDeb = CManager::GetInstance()->GetRenderer()->GetDevice();
	//if (m_FogMode != D3DFOG_NONE)
	//{//フォグがNONEではない時
	//	//デバッグ表示
	//	switch (m_FogMode)
	//	{
	//	case  D3DFOG_EXP:
	//		pDeb->Print("現在のフォグモード：EXP\n");
	//		break;
	//	case  D3DFOG_EXP2:
	//		pDeb->Print("現在のフォグモード：EXP2\n");
	//		break;
	//	case  D3DFOG_LINEAR:
	//		pDeb->Print("現在のフォグモード：LINEAR\n");
	//		break;

	//	default:
	//		break;
	//	}

	//	pDeb->Print("フォグの色：R:%f G:%f B:%f A:%f\n", m_FogColor.r, m_FogColor.g, m_FogColor.b, m_FogColor.a);

	//	if (m_FogMode == D3DFOG_LINEAR)
	//	{
	//		pDeb->Print("フォグの開始地点：%f\n",m_FogStart);
	//		pDeb->Print("フォグの終了地点：%f\n", m_FogEnd);
	//	}
	//	else
	//	{
	//		pDeb->Print("フォグの密度：%f\n", m_FogDensity);
	//	}
	//}
	//else
	//{
	//	pDeb->Print("現在のフォグモード：NONE");
	//}
}

//======================================
//描画処理
//======================================
void CFog::Draw()
{
	CRenderer* pRenderer = CManager::GetInstance()->GetRenderer();
	LPDIRECT3DDEVICE9 pDevice; //デバイスのポインタ
	pDevice = pRenderer->GetDevice();
	
	
		if (m_FogMode != D3DFOG_NONE)
		{//フォグモードが有効な時
			pDevice->SetRenderState(D3DRS_FOGCOLOR, m_FogColor);					//色の設定
			pDevice->SetRenderState(D3DRS_FOGTABLEMODE, m_FogMode);					//モードの設定
			pDevice->SetRenderState(D3DRS_FOGSTART, *((LPDWORD)(&m_FogStart)));		//開始地点の設定
			pDevice->SetRenderState(D3DRS_FOGEND, *((LPDWORD)(&m_FogEnd)));			//終了地点の設定
			pDevice->SetRenderState(D3DRS_FOGDENSITY, *((LPDWORD)(&m_FogDensity)));	//フォグ密度の設定
			pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);								//フォグの有効化
		}
}

//======================================
//設定処理
//======================================
void CFog::Set(D3DFOGMODE FogMode, D3DXCOLOR FogColor, float FogStart, float FogEnd, float FogDensity)
{
	CRenderer* pRenderer = CManager::GetInstance()->GetRenderer();
	LPDIRECT3DDEVICE9 pDevice; //デバイスのポインタ
	pDevice = pRenderer->GetDevice();
	m_FogMode = FogMode;        //モードの設定
	m_FogColor = FogColor;		//色の設定
	m_FogStart = FogStart;		//開始地点の設定		*フォグモードがD3DFOG_LINEARの時に参照される*
	m_FogEnd = FogEnd;			//終了地点の設定		*フォグモードがD3DFOG_LINEARの時に参照される*
	m_FogDensity = FogDensity;	//フォグ密度の設定		0.0f～1.0fの間の数値を入力	*フォグモードがD3DFOG_LINEARでは無い時に参照される*
	pDevice->SetRenderState(D3DRS_FOGCOLOR, m_FogColor);					//色の設定
	pDevice->SetRenderState(D3DRS_FOGTABLEMODE, m_FogMode);					//モードの設定
	pDevice->SetRenderState(D3DRS_FOGSTART, *((LPDWORD)(&m_FogStart)));		//開始地点の設定
	pDevice->SetRenderState(D3DRS_FOGEND, *((LPDWORD)(&m_FogEnd)));			//終了地点の設定
	pDevice->SetRenderState(D3DRS_FOGDENSITY, *((LPDWORD)(&m_FogDensity)));	//フォグ密度の設定
	pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);								//フォグの有効化
}