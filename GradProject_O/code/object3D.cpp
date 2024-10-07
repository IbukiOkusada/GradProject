//==========================================================
//
//ポリゴン描画処理
//Author 奥定伊吹
//
//==========================================================
#include "main.h"
#include "object3D.h"
#include "input.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "debugproc.h"
#include "player.h"
#include "particle.h"
#include "sound.h"

//==========================================================
//マクロ定義
//==========================================================
#define TEXTUREFILE_DATA	"data\\TEXTURE\\bg0000.png"		//テクスチャデータ

//==========================================================
//コンストラクタ
//==========================================================
CObject3D::CObject3D(int nPriority) : CObject(nPriority)
{
	m_nIdxTexture = -1;
	m_bLighting = false;

	//各種変数の初期化
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

//==========================================================
//デストラクタ
//==========================================================
CObject3D::~CObject3D()
{

}

//==========================================================
//ポリゴンの初期化処理
//==========================================================
HRESULT CObject3D::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//デバイスへのポインタを取得
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ

	// テクスチャの割り当て
	m_nIdxTexture = pTexture->Regist(TEXTUREFILE_DATA);

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(
		sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_3D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0);

	D3DXVECTOR3 vecLine, vecline2, vec;	//判定用変数
	D3DXVECTOR3 vecOldPos;	//前回の座標ベクトル

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-300.0f, 200.0f, +300.0f);
	pVtx[1].pos = D3DXVECTOR3(+300.0f, 100.0f, +300.0f);
	pVtx[2].pos = D3DXVECTOR3(-300.0f, 100.0f, -300.0f);
	pVtx[3].pos = D3DXVECTOR3(+300.0f, 0.0f, -300.0f);

	//Pos0からのベクトルを求める
	vecLine = pVtx[1].pos - pVtx[0].pos;
	vecline2 = pVtx[2].pos - pVtx[0].pos;

	D3DXVec3Cross(&vec, &vecLine, &vecline2);

	D3DXVec3Normalize(&vec, &vec);	// ベクトルを正規化する

	//現在の位置との面積を求める
	pVtx[0].nor = vec;

	//壁のベクトルを求める
	vecLine = pVtx[2].pos - pVtx[3].pos;
	vecline2 = pVtx[1].pos - pVtx[3].pos;

	//現在の位置との面積を求める
	D3DXVec3Cross(&vec, &vecLine, &vecline2);

	D3DXVec3Normalize(&vec, &vec);	// ベクトルを正規化する

	pVtx[3].nor = vec;

	pVtx[1].nor = (pVtx[0].nor + pVtx[3].nor) / 2;
	pVtx[2].nor = (pVtx[0].nor + pVtx[3].nor) / 2;

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

	return S_OK;
}

//==========================================================
//ポリゴンの終了処理
//==========================================================
void CObject3D::Uninit(void)
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
//ポリゴンの更新処理
//==========================================================
void CObject3D::Update(void)
{

}

//==========================================================
//ポリゴンの描画処理
//==========================================================
void CObject3D::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//デバイスへのポインタを取得
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ

	D3DXMATRIX mtxRot, mtxTrans;	//計算用マトリックス

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

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
	pDevice->SetTexture(0, pTexture->SetAddress(m_nIdxTexture));

	if (m_bLighting) {
		//ライティングをオンにする
		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	}

	// カリング設定
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//ポリゴンの描画
	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,	//プリミティブの種類
		0,
		2	//頂点情報構造体のサイズ
		);

	// カリング設定直す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	if (m_bLighting) {
		//ライティングをオンにする
		pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	}
}

//==========================================================
//生成処理
//==========================================================
CObject3D *CObject3D::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CObject3D *pObject3D = NULL;

	// オブジェクト2Dの生成
	pObject3D = new CObject3D;

	if (pObject3D != NULL)
	{// 生成できた場合
		// 初期化処理
		pObject3D->Init();

		pObject3D->SetPosition(pos);

		// 種類設定
		pObject3D->SetType(TYPE_NONE);
	}
	else
	{// 生成に失敗した場合
		return NULL;
	}

	return pObject3D;
}

//==========================================================
// 頂点情報設定
//==========================================================
void CObject3D::SetpVtx(float fWidth, float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	VERTEX_3D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0);

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-m_fWidth, 0.0f, +m_fHeight);
	pVtx[1].pos = D3DXVECTOR3(+m_fWidth, 0.0f, +m_fHeight);
	pVtx[2].pos = D3DXVECTOR3(-m_fWidth, 0.0f, -m_fHeight);
	pVtx[3].pos = D3DXVECTOR3(+m_fWidth, 0.0f, -m_fHeight);

	//法線ベクトルの設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

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
// テクスチャ設定
//==========================================================
void CObject3D::BindTexture(int nIdx)
{
	m_nIdxTexture = nIdx;
}

//==========================================================
// 頂点情報設定
//==========================================================
float CObject3D::GetHeight(D3DXVECTOR3 pos, D3DXVECTOR3 &normal)
{
	float fHeight = 0.0f;	// 高さ
	D3DXVECTOR3 Pos0, Pos1, Pos2, Pos3;
	D3DXVECTOR3 vecToPos;	//判定用変数
	D3DXVECTOR3 vec1, vec2;	//判定用変数
	D3DXVECTOR3 nor0, nor3;
	float fRate, fRate2;	//判定用変数
	float fMaxField;		//判定用
	float fField, fField2;
	VERTEX_3D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0);

	Pos0 = pVtx[0].pos;
	nor0 = pVtx[0].nor;
	Pos1 = pVtx[1].pos;
	Pos2 = pVtx[2].pos;
	Pos3 = pVtx[3].pos;
	nor3 = pVtx[3].nor;

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	// Pos0からのベクトルを求める
	vec1 = Pos1 - Pos0;
	vec2 = Pos2 - Pos0;

	// 現在の座標のベクトルを求める
	vecToPos = D3DXVECTOR3(pos.x - (m_pos.x + Pos0.x),
		pos.y - (m_pos.y + Pos0.y),
		pos.z - (m_pos.z + Pos0.z));

	// 面積を求める
	fMaxField = (vec1.x * vec2.z) - (vec1.z * vec2.x);

	// 現在の位置との面積を求める
	fField = (vecToPos.x * vec2.z) - (vecToPos.z * vec2.x);
	fField2 = (vecToPos.z * vec1.x) - (vecToPos.x * vec1.z);

	// 交点の割合を求める
	fRate = fField / fMaxField;
	fRate2 = fField2 / fMaxField;

	// 範囲内判定
	if (nor0.y != 0.0f)
	{
		if (fRate >= 0.0f && fRate <= 1.0f && fRate2 >= 0.0f && fRate2 <= 1.0f && (fRate + fRate2) <= 1.0f)
		{// 三角ポリゴンの中にいる

			fHeight = -((pos.x - (m_pos.x + Pos0.x)) * nor0.x / nor0.y) +
				-((pos.z - (m_pos.z + Pos0.z)) * nor0.z / nor0.y) + (m_pos.y + Pos0.y);

			normal = nor0;

			return fHeight;
		}
	}

	// Pos3からのベクトルを求める
	vec1 = Pos1 - Pos3;  
	vec2 = Pos2 - Pos3;

	// 現在の座標のベクトルを求める
	vecToPos = D3DXVECTOR3(pos.x - (m_pos.x + Pos3.x),
		pos.y - (m_pos.y + Pos3.y),
		pos.z - (m_pos.z + Pos3.z));

	// 面積を求める
	fMaxField = (vec1.x * vec2.z) - (vec1.z * vec2.x);

	// 現在の位置との面積を求める
	fField = (vecToPos.x * vec2.z) - (vecToPos.z * vec2.x);
	fField2 = (vecToPos.z * vec1.x) - (vecToPos.x * vec1.z);

	// 交点の割合を求める
	fRate = fField / fMaxField;
	fRate2 = fField2 / fMaxField;

	// 範囲内判定
	if (nor3.y != 0.0f)
	{
		if (fRate >= 0.0f && fRate <= 1.0f && fRate2 >= 0.0f && fRate2 <= 1.0f && (fRate + fRate2) <= 1.0f)
		{// 三角ポリゴンの中にいる
			fHeight = -((pos.x - (m_pos.x + Pos3.x)) * nor3.x / nor3.y) +
				-((pos.z - (m_pos.z + Pos3.z)) * nor3.z / nor3.y) + (m_pos.y + Pos3.y);

			normal = nor3;

			return fHeight;
		}
	}

	return pos.y;
}

//==========================================================
//サイズ設定
//==========================================================
void CObject3D::SetSize(float fWidth, float fHeight)
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
	pVtx[0].pos = D3DXVECTOR3(-m_fWidth, +m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(+m_fWidth, +m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_fWidth, -m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(+m_fWidth, -m_fHeight, 0.0f);

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//==========================================================
// 色設定
//==========================================================
void CObject3D::SetCol(D3DXCOLOR col)
{
	VERTEX_3D *pVtx;

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
// 色設定
//==========================================================
void CObject3D::SetTextureVtx(float fWidth, float fHeight)
{
	VERTEX_3D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0);

	//テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(fWidth, fHeight);
	pVtx[1].tex = D3DXVECTOR2(fWidth + 1.0f, fHeight);
	pVtx[2].tex = D3DXVECTOR2(fWidth, fHeight + 1.0f);
	pVtx[3].tex = D3DXVECTOR2(fWidth + 1.0f, fHeight + 1.0f);

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//==========================================================
// マトリックス設定
//==========================================================
void CObject3D::SetMtx(void)
{
	D3DXMATRIX mtxRot, mtxTrans;	//計算用マトリックス

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);
}