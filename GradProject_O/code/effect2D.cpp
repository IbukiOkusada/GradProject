//===============================================
//
// エフェクトの処理 [effect.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "effect2D.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "texture.h"
#include "slow.h"
#include "game.h"
#include "meshfield.h"
#include "billboard.h"

//===============================================
// 無名名前空間
//===============================================
namespace {
	const float LIFE = (30.0f); // 寿命
	const float GUNCHARGE = (0.1f);

	const D3DXCOLOR COLINFO[CEffect2D::TYPE_MAX] = {	// 種類別初期色の設定
		{1.0f, 1.0f, 1.0f, 1.0f},
		{ 1.0f, 1.0f, 0.0f, 1.0f },
	};

	const float RADIUSINFO[CEffect2D::TYPE_MAX] = {	// 種類別半径の設定
		100.0f,
		100.0f,
	};
}

//===============================================
// コンストラクタ
//===============================================
CEffect2D::CEffect2D()
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
CEffect2D::~CEffect2D()
{

}

//===============================================
// 初期化処理
//===============================================
HRESULT CEffect2D::Init(void)
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
void CEffect2D::Uninit(void)
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
void CEffect2D::Update(void)
{
	m_Info.fLife -= CManager::GetInstance()->GetSlow()->Get();
	D3DXVECTOR3 nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 posOld = m_Info.pos;
	float fHeight = CMeshField::GetHeight(m_Info.pos, posOld);

	if (m_Info.fLife < 0)
	{// 寿命がなくなった場合

		// 終了する
		Uninit();
		return;
	}

	D3DXVECTOR3 pos = GetPosition();	// 座標

	pos += m_Info.move * CManager::GetInstance()->GetSlow()->Get();

	// 座標
	SetPosition(pos);

	switch (m_Info.Type)
	{
	case TYPE_NONE:
		m_Info.col.a -= 0.05f * CManager::GetInstance()->GetSlow()->Get();
		m_Info.fRadius += 0.1f * CManager::GetInstance()->GetSlow()->Get();
		break;

	case TYPE_SMAKE:	// 煙

		m_Info.col.a -= 0.05f * CManager::GetInstance()->GetSlow()->Get();
		m_Info.fRadius += 0.1f * CManager::GetInstance()->GetSlow()->Get();

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
CEffect2D* CEffect2D::Create(D3DXVECTOR3 pos, TYPE type)
{
	CEffect2D* pEffect2D = NULL;
	CTexture* pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ

	// エフェクトの生成
	pEffect2D = DEBUG_NEW CEffect2D;

	if (pEffect2D != NULL)
	{// 生成できた場合

		// 座標設定
		pEffect2D->SetPosition(pos);

		// 種類の設定
		pEffect2D->SetType(type);

		// 半径設定
		pEffect2D->RadiusSet();

		// 色の設定
		pEffect2D->ColorSet();

		pEffect2D->InfoSet();

		// 初期化処理
		pEffect2D->Init();

		// テクスチャの割り当て
	}
	else
	{// 生成に失敗した場合
		return NULL;
	}

	return pEffect2D;
}

//===============================================
// 生成
//===============================================
CEffect2D* CEffect2D::Create(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fRadius, float fLife, TYPE type)
{
	CEffect2D* pEffect2D = NULL;
	CTexture* pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ

	// エフェクトの生成
	pEffect2D = DEBUG_NEW CEffect2D();

	if (pEffect2D != NULL)
	{// 生成できた場合

		// 座標設定
		pEffect2D->SetPosition(pos);

		// エフェクトの設定
		pEffect2D->SetType(type);

		// 半径設定
		pEffect2D->m_Info.fRadius = fRadius;

		// 移動量設定
		pEffect2D->SetMove(move);

		// サイズの設定
		//pEffect2D->SetSize(pEffect2D->m_Info.fRadius, pEffect2D->m_Info.fRadius);

		// 色の設定
		pEffect2D->m_Info.col = col;

		// 初期化処理
		pEffect2D->Init();

		// 寿命設定
		pEffect2D->m_Info.fLife = fLife;

		pEffect2D->InfoSet();

		// 描画方法設定
		pEffect2D->DrawSet();
	}
	else
	{// 生成に失敗した場合
		return NULL;
	}

	return pEffect2D;
}

//===============================================
// 色設定
//===============================================
void CEffect2D::ColorSet(void)
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
void CEffect2D::RadiusSet(void)
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
void CEffect2D::SetMove(D3DXVECTOR3 move)
{
	m_Info.move = move;
}

//===============================================
// 情報基本設定
//===============================================
void CEffect2D::InfoSet(void)
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
CTexture::TYPE CEffect2D::SetTex(TYPE type)
{
	switch (type)
	{
	case TYPE_NONE:
	{

	}
	break;

	case TYPE_SMAKE:
	{
		return CTexture::TYPE_EFFECT;

	}
	break;
	}

	return CTexture::TYPE();
}

//===============================================
// 描画設定
//===============================================
void CEffect2D::DrawSet(void)
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

	}
}

//===============================================
// 半径取得
//===============================================
float CEffect2D::GetRange(void) const
{
	if (m_pObjectBilBoard == nullptr) {
		return 0.0f;
	}

	return m_pObjectBilBoard->GetWidth();
}

//===============================================
// 色取得
//===============================================
D3DXCOLOR CEffect2D::GetCol(void) const
{
	if (m_pObjectBilBoard == nullptr) {
		return D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	}

	return m_pObjectBilBoard->GetCol();
}