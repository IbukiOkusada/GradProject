//==========================================================
//
// ビルボードの処理全般 [billboard.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "billboard.h"
#include "input.h"
#include "camera.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//==========================================================
//コンストラクタ
//==========================================================
CObjectBillboard::CObjectBillboard(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	m_nIdexTexture = -1;
	m_pVtxBuff = NULL;
	m_fusion = FUSION_ADD;
	m_bLighting = true;
	m_bAlphatest = true;
	m_bZtest = true;
	m_pCurrent = nullptr;
}

//==========================================================
//デストラクタ
//==========================================================
CObjectBillboard::~CObjectBillboard()
{

}

//==========================================================
//ビルボードの初期化処理
//==========================================================
HRESULT CObjectBillboard::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice;		//デバイスへのポインタ

	//デバイスの取得
	pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(
		sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点情報設定
	SetVtx();

	return S_OK;
}

//==========================================================
//ビルボードの終了処理
//==========================================================
void CObjectBillboard::Uninit(void)
{

	//頂点バッファの廃棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// 廃棄
	Release();
}

//==========================================================
//ビルボードの更新処理
//==========================================================
void CObjectBillboard::Update(void)
{
	
}

//==========================================================
//ビルボードの描画処理
//==========================================================
void CObjectBillboard::Draw(void)
{
	if (!GetDraw())
	{
		return;
	}

	if (m_pVtxBuff == nullptr)
	{
		return;
	}

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//デバイスへのポインタを取得
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;	//計算用マトリックス
	D3DXMATRIX mtxView;		//ビューマトリックス取得用

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	
		//ビューマトリックスを取得
		pDevice->GetTransform(D3DTS_VIEW, &mtxView);

		//ポリゴンをカメラに対して正面に向ける
		D3DXMatrixInverse(&m_mtxWorld, NULL, &mtxView);
		m_mtxWorld._41 = 0.0f;
		m_mtxWorld._42 = 0.0f;
		m_mtxWorld._43 = 0.0f;
	
	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	if (m_pCurrent != nullptr)
	{
		// マトリックスと親のマトリックスをかけ合わせる
		D3DXMatrixMultiply(&m_mtxWorld,
			&m_mtxWorld, m_pCurrent);
	}

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(
		0,
		m_pVtxBuff,
		0,
		sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, pTexture->SetAddress(m_nIdexTexture));

	if (m_bLighting) {
		//ライティングをオフにする
		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	}

	if (m_bZtest) {
		//Zテストを無効化する
		pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}

	if (m_bAlphatest) {
		//アルファテストを有効にする
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	}

	if (m_fusion == FUSION_ADD)
	{
		//αブレンディングを加算合成に設定
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}
	else if (m_fusion == FUSION_MINUS)
	{
		//減算合成の設定
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

	//ポリゴンの描画
	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,	//プリミティブの種類
		0,
		2	//頂点情報構造体のサイズ
	);

	if (m_bLighting) {
		//ライティングをオンにする
		pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	}

	if (m_bZtest) {
		//Zテストを有効にする
		pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}

	if (m_bAlphatest) {
		//アルファテストを無効にする
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
		pDevice->SetRenderState(D3DRS_ALPHAREF, 255);
	}

	if (m_fusion == FUSION_ADD)
	{
		//αブレンディングを元に戻す
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	else if (m_fusion == FUSION_MINUS)
	{
		//αブレンディングを元に戻す
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
}

//==========================================================
// 向きを反映させた描画処理
//==========================================================
void CObjectBillboard::RotFusionDraw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//デバイスへのポインタを取得
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;	//計算用マトリックス
	D3DXMATRIX mtxView;		//ビューマトリックス取得用

	if (m_bLighting) {
		//ライティングをオフにする
		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	}

	if (m_bZtest) {
		//Zテストを無効化する
		pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}

	if (m_bAlphatest) {
		//アルファテストを有効にする
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	}

	if (m_fusion == FUSION_ADD)
	{
		//αブレンディングを加算合成に設定
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}
	else if (m_fusion == FUSION_MINUS)
	{
		//減算合成の設定
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	//ポリゴンをカメラに対して正面に向ける
	D3DXMatrixInverse(&m_mtxWorld, NULL, &mtxView);
	m_mtxWorld._41 = 0.0f;
	m_mtxWorld._42 = 0.0f;
	m_mtxWorld._43 = 0.0f;

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(
		0,
		m_pVtxBuff,
		0,
		sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, pTexture->SetAddress(m_nIdexTexture));

	//ポリゴンの描画
	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,	//プリミティブの種類
		0,
		2	//頂点情報構造体のサイズ
	);

	if (m_bLighting) {
		//ライティングをオンにする
		pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	}

	if (m_bZtest) {
		//Zテストを有効にする
		pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}

	if (m_bAlphatest) {
		//アルファテストを無効にする
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
		pDevice->SetRenderState(D3DRS_ALPHAREF, 255);
	}

	if (m_fusion == FUSION_ADD)
	{
		//αブレンディングを元に戻す
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	else if (m_fusion == FUSION_MINUS)
	{
		//αブレンディングを元に戻す
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
}

//==========================================================
//生成処理
//==========================================================
CObjectBillboard *CObjectBillboard::Create(D3DXVECTOR3 pos, const int nPriority)
{
	CObjectBillboard *pObjBillboard = NULL;

	// オブジェクト2Dの生成
	pObjBillboard = new CObjectBillboard(nPriority);

	if (pObjBillboard != NULL)
	{// 生成できた場合

		// 初期化処理
		pObjBillboard->Init();

		// 座標設定
		pObjBillboard->SetPosition(pos);

		// 種類設定
		pObjBillboard->SetType(TYPE_NONE);
	}
	else
	{// 生成に失敗した場合
		return NULL;
	}

	return pObjBillboard;
}

//==========================================================
// 頂点情報設定
//==========================================================
void CObjectBillboard::SetVtx(void)
{
	VERTEX_3D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0);

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-5.0f, +5.0f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(+5.0f, +5.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-5.0f, -5.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(+5.0f, -5.0f, 0.0f);

	//法線ベクトルの設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//==========================================================
//テクスチャ座標設定
//==========================================================
void CObjectBillboard::SetVtx(const int nPatternAnim, const int nTexWidth, const int nTexHeight)
{
	VERTEX_3D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0
	);

	//頂点座標の設定
	pVtx[0].tex = D3DXVECTOR2(1.0f / nTexWidth * (float)nPatternAnim, (1.0f / nTexHeight * (float)(nPatternAnim / nTexWidth)));
	pVtx[1].tex = D3DXVECTOR2(1.0f / nTexWidth * (float)(nPatternAnim + 1), (1.0f / nTexHeight * (float)(nPatternAnim / nTexWidth)));
	pVtx[2].tex = D3DXVECTOR2(1.0f / nTexWidth * (float)nPatternAnim, (1.0f / nTexHeight * (float)(nPatternAnim / nTexWidth + 1)));
	pVtx[3].tex = D3DXVECTOR2(1.0f / nTexWidth * (float)(nPatternAnim + 1), (1.0f / nTexHeight * (float)(nPatternAnim / nTexWidth + 1)));

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//===============================================
// 頂点情報設定(テクスチャアニメーション)
//===============================================
void CObjectBillboard::SetVtx(const float fTexU, const float fTexV)
{
	VERTEX_3D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0
	);

	float ftexU = 0.3f + fTexU;

	if (ftexU > 1.0f)
	{
		ftexU = 1.0f;
	}

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(fTexU, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(0.3f + fTexU, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(fTexU, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(0.3f + fTexU, 1.0f);

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//==========================================================
// 色設定
//==========================================================
void CObjectBillboard::SetVtx(D3DXCOLOR col)
{
	VERTEX_3D *pVtx;
	m_col = col;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0);

	//頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//==========================================================
//テクスチャ設定
//==========================================================
void CObjectBillboard::BindTexture(int nIdx)
{
	m_nIdexTexture = nIdx;
}

//==========================================================
//サイズ設定
//==========================================================
void CObjectBillboard::SetSize(float fWidth, float fHeight)
{
	VERTEX_3D *pVtx;

	m_fWidth = fWidth;
	m_fHeight = fHeight;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0);

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(sinf(m_rot.z + (-D3DX_PI * 0.25f)) * m_fWidth, cosf(m_rot.z + (-D3DX_PI * 0.25f)) * m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(sinf(m_rot.z + (D3DX_PI * 0.25f)) * m_fWidth, cosf(m_rot.z + (D3DX_PI * 0.25f)) * m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(sinf(m_rot.z + (-D3DX_PI * 0.75f)) * m_fWidth, cosf(m_rot.z + (-D3DX_PI * 0.75f)) * m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(sinf(m_rot.z + (D3DX_PI * 0.75f)) * m_fWidth, cosf(m_rot.z + (D3DX_PI * 0.75f)) * m_fHeight, 0.0f);

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//==========================================================
// 色設定
//==========================================================
void CObjectBillboard::SetCol(D3DXCOLOR col)
{
	VERTEX_3D *pVtx;
	m_col = col;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0);

	//頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//==========================================================
// マトリックス設定
//==========================================================
void CObjectBillboard::SetMtx(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//デバイスへのポインタを取得
	D3DXMATRIX mtxTrans;	//計算用マトリックス
	D3DXMATRIX mtxView;		//ビューマトリックス取得用

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	//ポリゴンをカメラに対して正面に向ける
	D3DXMatrixInverse(&m_mtxWorld, NULL, &mtxView);
	m_mtxWorld._41 = 0.0f;
	m_mtxWorld._42 = 0.0f;
	m_mtxWorld._43 = 0.0f;

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);
}