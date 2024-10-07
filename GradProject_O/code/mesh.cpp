//==========================================================
//
// メッシュの管理 [mesh.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "mesh.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "slow.h"
#include "debugproc.h"

//==========================================================
// コンストラクタ
//==========================================================
CObjectMesh::CObjectMesh(int nPriority) : CObject(nPriority)
{
	// 値をクリアする
	m_nIndex = 0;
	m_nVertex = 0;
	m_nNumWidth = 0;
	m_nIdxTexture = -1;
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pVtx = NULL;
	m_Type = TYPE_NONE;
}

//==========================================================
// デストラクタ
//==========================================================
CObjectMesh::~CObjectMesh()
{

}

#if 0
//==========================================================
// 初期化処理
//==========================================================
HRESULT CMesh::Init(void)
{
	return S_OK;
}
#endif

//==========================================================
// 終了処理
//==========================================================
void CObjectMesh::Uninit(void)
{
	//頂点バッファの廃棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	//インデックスバッファの廃棄
	if (m_pIdexBuff != NULL)
	{
		m_pIdexBuff->Release();
		m_pIdexBuff = NULL;
	}

	// 頂点情報の廃棄
	if (m_pVtx != NULL)
	{
		delete[] m_pVtx;
		m_pVtx = NULL;
	}

	// 廃棄
	Release();
}

#if 0
//==========================================================
// 更新処理
//==========================================================
void CMesh::Update(void)
{

}
#endif

//==========================================================
// 描画処理
//==========================================================
void CObjectMesh::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//デバイスへのポインタを取得
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;	//計算用マトリックス

	if (m_pVtxBuff != NULL)
	{
		// 色設定
		//SetSlowCol();

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

		//インデックスバッファをデータストリームに設定
		pDevice->SetIndices(m_pIdexBuff);

		//頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		//テクスチャの設定
		pDevice->SetTexture(0, pTexture->SetAddress(m_nIdxTexture));
		
		//ポリゴンの描画
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
			0,
			0,
			m_nVertex,		//用意した頂点数
			0,
			m_nIndex - 2	//描画するプリミティブ数
		);
	}
}

//==========================================================
// 頂点情報生成
//==========================================================
void CObjectMesh::CreateVertex(void)
{
	if (m_nVertex <= 0)
	{// 設定されていない場合
		return;
	}

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(
		sizeof(VERTEX_3D) * m_nVertex,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);
}

//==========================================================
// インデックス情報生成
//==========================================================
void CObjectMesh::CreateIndex(void)
{
	if (m_nIndex <= 0)
	{// 設定されていない場合
		return;
	}

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//デバイスへのポインタ

	// インデックスバッファの生成
	pDevice->CreateIndexBuffer(sizeof(WORD) * m_nIndex,		// 必要なインデックス数
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdexBuff,
		NULL);
}

//==========================================================
// インデックス情報の設定
//==========================================================
void CObjectMesh::SetIndex(void)
{
	WORD *pIdx;	//インデックス情報のポインタ

	//インデックスバッファをロックし頂点番号データへのポインタを取得
	m_pIdexBuff->Lock(0, 0, (void**)&pIdx, 0);

	for (int nCntIdx = 0; nCntIdx < m_nIndex / 2; nCntIdx++)
	{
		pIdx[nCntIdx * 2] = (nCntIdx + m_nNumWidth + 1)
			- (nCntIdx % (m_nNumWidth + 2) / (m_nNumWidth + 1) * (m_nNumWidth + 2))
			- (nCntIdx / (m_nNumWidth + 2));
		pIdx[nCntIdx * 2 + 1] = nCntIdx
			+ (nCntIdx % (m_nNumWidth + 2) / (m_nNumWidth + 1) * (m_nNumWidth + 1))
			- (nCntIdx / (m_nNumWidth + 2));

		pIdx[nCntIdx * 2] = (nCntIdx + m_nNumWidth + 1)
			- (nCntIdx % (m_nNumWidth + 2) / (m_nNumWidth + 1) * (m_nNumWidth + 2))
			- (nCntIdx / (m_nNumWidth + 2));
		pIdx[nCntIdx * 2 + 1] = nCntIdx
			+ (nCntIdx % (m_nNumWidth + 2) / (m_nNumWidth + 1) * (m_nNumWidth + 1))
			- (nCntIdx / (m_nNumWidth + 2));
	}

	//インデックスバッファをアンロックする
	m_pIdexBuff->Unlock();
}

//==========================================================
// 頂点情報の反映
//==========================================================
void CObjectMesh::SetVtx(void)
{
	VERTEX_3D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0);

	for (int nCntVtx = 0; nCntVtx < m_nVertex; nCntVtx++)
	{
		pVtx[nCntVtx].pos = m_pVtx[nCntVtx].pos;
		pVtx[nCntVtx].col = m_pVtx[nCntVtx].col;
		pVtx[nCntVtx].nor = m_pVtx[nCntVtx].nor;
		pVtx[nCntVtx].tex = m_pVtx[nCntVtx].tex;
	}

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//==========================================================
// 生成
//==========================================================
void CObjectMesh::Create(int nWidth, int nHeight)
{
	// 縦横の枚数設定
	m_nNumHeight = nHeight;
	m_nNumWidth = nWidth;

	// 頂点数の設定
	m_nVertex = (m_nNumWidth + 1) * (m_nNumHeight + 1);

	// インデックス数の設定
	m_nIndex = (m_nNumWidth + 1) * (m_nNumHeight) * 2 + (m_nNumHeight - 1) * 2;

	// 頂点バッファの生成
	CreateVertex();

	// インデックスバッファの生成
	CreateIndex();

	// インデックスの設定
	SetIndex();

	// 頂点情報の生成
	m_pVtx = new VERTEX_3D[m_nVertex];
}

//==========================================================
// テクスチャ番号設定
//==========================================================
void CObjectMesh::BindTexture(int nIdx)
{
	m_nIdxTexture = nIdx;
}

//===============================================
// 座標設定
//===============================================
void CObjectMesh::SetPosition(const D3DXVECTOR3 pos)
{
	m_pos = pos;
}

//===============================================
// 向き設定
//===============================================
void CObjectMesh::SetRotation(const D3DXVECTOR3 rot)
{
	m_rot = rot;

	if (m_rot.x < -D3DX_PI)
	{// z座標角度限界
		m_rot.x += D3DX_PI * 2;
	}
	else if (m_rot.x > D3DX_PI)
	{// z座標角度限界
		m_rot.x += -D3DX_PI * 2;
	}

	if (m_rot.y < -D3DX_PI)
	{// z座標角度限界
		m_rot.y += D3DX_PI * 2;
	}
	else if (m_rot.y > D3DX_PI)
	{// z座標角度限界
		m_rot.y += -D3DX_PI * 2;
	}

	if (m_rot.z < -D3DX_PI)
	{// z座標角度限界
		m_rot.z += D3DX_PI * 2;
	}
	else if (m_rot.z > D3DX_PI)
	{// z座標角度限界
		m_rot.z += -D3DX_PI * 2;
	}
}

//==========================================================
// マトリックス設定無し描画
//==========================================================
void CObjectMesh::NotMtxDraw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//デバイスへのポインタを取得
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ

	if (m_pVtxBuff != NULL)
	{
		// 色設定
		//SetSlowCol();

		//ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

		//頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(
			0,
			m_pVtxBuff,
			0,
			sizeof(VERTEX_3D));

		//インデックスバッファをデータストリームに設定
		pDevice->SetIndices(m_pIdexBuff);

		//頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		//テクスチャの設定
		pDevice->SetTexture(0, pTexture->SetAddress(m_nIdxTexture));

		//ポリゴンの描画
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
			0,
			0,
			m_nVertex,		//用意した頂点数
			0,
			m_nIndex - 2	//描画するプリミティブ数
		);
	}
}

//==========================================================
// スロー状態色設定
//==========================================================
void CObjectMesh::SetSlowCol(void)
{
	CSlow *pSlow = CManager::GetInstance()->GetSlow();

	if (pSlow == NULL)
	{
		return;
	}

	if (pSlow->Get() == pSlow->GetOld() || pSlow->Get() > 1.0f)
	{
		return;
	}

	float GetSlowMul = pSlow->Get();

	VERTEX_3D *pVtx;

	//頂点バッファをロックし頂点情報へのポインタを取得
	m_pVtxBuff->Lock(
		0,
		0,
		(void**)&pVtx,
		0);

	for (int nCntVtx = 0; nCntVtx < m_nVertex; nCntVtx++)
	{
		D3DXCOLOR col = m_pVtx[nCntVtx].col;
		m_pVtx[nCntVtx].col = D3DXCOLOR(GetSlowMul, GetSlowMul, GetSlowMul, col.a);
		pVtx[nCntVtx].col = (D3DXCOLOR)m_pVtx[nCntVtx].col;
	}

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}