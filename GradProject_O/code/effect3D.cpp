//===============================================
//
// エフェクトの処理 [effect.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "effect3D.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "texture.h"
#include "deltatime.h"
#include "game.h"
#include "meshfield.h"
#include "billboard.h"
#include "deltatime.h"

//===============================================
// 無名名前空間
//===============================================
namespace {
	const float DELTATIME_MULTI = (100.0f);
	const float LIFE = (30.0f); // 寿命
	const float GUNCHARGE = (0.1f);

	const D3DXCOLOR COLINFO[CEffect3D::TYPE_MAX] = {	// 種類別初期色の設定
		{1.0f, 1.0f, 1.0f, 1.0f},
		{ 1.0f, 1.0f, 0.0f, 1.0f },
	};

	const float RADIUSINFO[CEffect3D::TYPE_MAX] = {	// 種類別半径の設定
		100.0f,
		100.0f,
	};

	const CTexture::TYPE TEXIDX[CEffect3D::TYPE::TYPE_MAX] = {
		CTexture::TYPE_EFFECT,
		CTexture::TYPE_EFFECT,
		CTexture::TYPE_WATER,
		CTexture::TYPE_EFFECT,
	};
}

//===============================================
// コンストラクタ
//===============================================
CEffect3D::CEffect3D()
{
	// 値のクリア
	m_Info.col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_Info.fLife = 0;
	m_Info.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Info.fRadius = 0.0f;
	m_Info.Type = TYPE_NONE;
}

//===============================================
// デストラクタ
//===============================================
CEffect3D::~CEffect3D()
{

}

//===============================================
// 初期化処理
//===============================================
HRESULT CEffect3D::Init(void)
{
	// オブジェクトの初期化処理
	m_pObjectBilBoard = CObjectBillboard::Create(m_Info.pos, 6);
	m_pObjectBilBoard->BindTexture(CManager::GetInstance()->GetTexture()->Regist(CManager::GetInstance()->GetTexture()->GetFileName(SetTex(m_Info.Type))));
	m_pObjectBilBoard->SetAlphaText(true);
	m_pObjectBilBoard->SetZTest(true);
	m_pObjectBilBoard->SetLighting(true);
	m_pObjectBilBoard->SetFusion(CObjectBillboard::FUSION_ADD);

	m_Info.fLife = LIFE;	// 体力の初期化

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CEffect3D::Uninit(void)
{
	// オブジェクトの終了処理
	if (m_pObjectBilBoard != nullptr) {
		m_pObjectBilBoard->Uninit();
		m_pObjectBilBoard = nullptr;
	}

	Release();
}

//===============================================
// 更新処理
//===============================================
void CEffect3D::Update(void)
{
	float slow = CDeltaTime::GetInstance()->GetSlow();
	m_Info.fLife -= slow;
	D3DXVECTOR3 nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 posOld = m_Info.pos;
	//float fHeight = CMeshField::GetHeight(m_Info.pos, posOld);

	if (m_Info.fLife < 0)
	{// 寿命がなくなった場合

		// 終了する
		Uninit();
		return;
	}

	D3DXVECTOR3 pos = GetPosition();	// 座標

	pos += m_Info.move * slow;

	// 座標
	SetPosition(pos);

	switch (m_Info.Type)
	{
	case TYPE_NONE:
		m_Info.col.a -= 0.05f * slow;
		m_Info.fRadius += 0.1f * slow;
		break;

	case TYPE_SMAKE:	// 煙

		m_Info.col.a -= 0.05f * slow;
		m_Info.fRadius += 0.1f * slow;

		break;

	case TYPE_SPLASH:	// 煙

		m_Info.col.a -= 0.01f * slow;
		m_Info.move.y += -1.0f;

		break;

	case TYPE_LASER:	// 煙

		m_Info.col.a -= 0.01f * slow;

		break;
	}

	if (m_Info.col.a < 0.0f || m_Info.fRadius < 0.0f)
	{// 値がひっくり返った

		
		Uninit();
		return;
	}

	InfoSet();
}

//===============================================
// 生成
//===============================================
CEffect3D *CEffect3D::Create(D3DXVECTOR3 pos, TYPE type)
{
	CEffect3D *pEffect3D = NULL;

	// エフェクトの生成
	pEffect3D = DEBUG_NEW CEffect3D;

	if (pEffect3D != NULL)
	{// 生成できた場合

		// 座標設定
		pEffect3D->SetPosition(pos);

		// 種類の設定
		pEffect3D->SetType(type);

		// 半径設定
		pEffect3D->RadiusSet();

		// 色の設定
		pEffect3D->ColorSet();

		pEffect3D->InfoSet();

		// 初期化処理
		pEffect3D->Init();

		// テクスチャの割り当て
	}
	else
	{// 生成に失敗した場合
		return NULL;
	}

	return pEffect3D;
}

//===============================================
// 生成
//===============================================
CEffect3D *CEffect3D::Create(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fRadius, float fLife, TYPE type)
{
	CEffect3D *pEffect3D = NULL;

	// エフェクトの生成
	pEffect3D = DEBUG_NEW CEffect3D();

	if (pEffect3D != NULL)
	{// 生成できた場合

		// 座標設定
		pEffect3D->SetPosition(pos);

		// エフェクトの設定
		pEffect3D->SetType(type);

		// 半径設定
		pEffect3D->m_Info.fRadius = fRadius;

		// 移動量設定
		pEffect3D->SetMove(move);

		// サイズの設定
		//pEffect3D->SetSize(pEffect3D->m_Info.fRadius, pEffect3D->m_Info.fRadius);

		// 色の設定
		pEffect3D->m_Info.col = col;

		// 初期化処理
		pEffect3D->Init();

		// 寿命設定
		pEffect3D->m_Info.fLife = fLife;

		pEffect3D->InfoSet();

		// 描画方法設定
		pEffect3D->DrawSet();
	}
	else
	{// 生成に失敗した場合
		return NULL;
	}

	return pEffect3D;
}

//===============================================
// 色設定
//===============================================
void CEffect3D::ColorSet(void)
{
	// 種類ごとに色の設定
	m_Info.col = COLINFO[m_Info.Type];
	

	if (m_pObjectBilBoard == nullptr) {
		return;
	}

	m_pObjectBilBoard->SetCol(m_Info.col);
}

//===============================================
// サイズの設定
//===============================================
void CEffect3D::RadiusSet(void)
{
	// 半径の設定
	m_Info.fRadius = RADIUSINFO[m_Info.Type];

	if (m_pObjectBilBoard == nullptr) {
		return;
	}

	m_pObjectBilBoard->SetSize(m_Info.fRadius, m_Info.fRadius);
}

//===============================================
// 移動量の設定
//===============================================
void CEffect3D::SetMove(D3DXVECTOR3 move)
{
	m_Info.move = move;
}

//===============================================
// 情報基本設定
//===============================================
void CEffect3D::InfoSet(void)
{
	if (m_pObjectBilBoard == nullptr) {
		return;
	}

	m_pObjectBilBoard->SetPosition(m_Info.pos);
	m_pObjectBilBoard->SetCol(m_Info.col);
	m_pObjectBilBoard->SetSize(m_Info.fRadius, m_Info.fRadius);
}

//===============================================
// タイプ別テクスチャ
//===============================================
CTexture::TYPE CEffect3D::SetTex(TYPE type)
{
	return TEXIDX[type];
}

//===============================================
// 描画設定
//===============================================
void CEffect3D::DrawSet(void)
{
	if (m_pObjectBilBoard == nullptr) {
		return;
	}

	switch (m_Info.Type)
	{
	case TYPE_NONE:
	{

	}
	break;

	case TYPE_SMAKE:
	{
		m_pObjectBilBoard->SetAlphaText(true);
		m_pObjectBilBoard->SetZTest(true);
		m_pObjectBilBoard->SetLighting(true);
		m_pObjectBilBoard->SetFusion(CObjectBillboard::FUSION_ADD);
	}
	break;

	case TYPE_SPLASH:
	{
		m_pObjectBilBoard->SetAlphaText(true);
		m_pObjectBilBoard->SetZTest(false);
		m_pObjectBilBoard->SetLighting(true);
		m_pObjectBilBoard->SetFusion(CObjectBillboard::FUSION_ADD);
	}
	break;

	case TYPE_LASER:
	{
		m_pObjectBilBoard->SetAlphaText(true);
		m_pObjectBilBoard->SetZTest(false);
		m_pObjectBilBoard->SetLighting(true);
		m_pObjectBilBoard->SetFusion(CObjectBillboard::FUSION_ADD);
	}
	break;

	}
}

//===============================================
// 半径取得
//===============================================
float CEffect3D::GetRange(void) const
{
	if (m_pObjectBilBoard == nullptr) {
		return 0.0f;
	}

	return m_pObjectBilBoard->GetWidth();
}

//===============================================
// 色取得
//===============================================
D3DXCOLOR CEffect3D::GetCol(void) const
{
	if (m_pObjectBilBoard == nullptr) {
		return D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	}

	return m_pObjectBilBoard->GetCol();
}