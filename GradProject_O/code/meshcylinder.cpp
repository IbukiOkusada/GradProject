//==========================================================
//
// メッシュシリンダーの処理 [meshcylinder.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "meshcylinder.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"
#include "slow.h"

// マクロ定義
#define TEXTUREFILE_DATA	"data\\TEXTURE\\city000.png"		//テクスチャデータ

//==========================================================
// コンストラクタ
//==========================================================
CMeshCylinder::CMeshCylinder(int nPriority) : CObjectMesh(nPriority)
{
	
}

//==========================================================
// デストラクタ
//==========================================================
CMeshCylinder::~CMeshCylinder()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CMeshCylinder::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CMeshCylinder::Uninit(void)
{
	// 頂点バッファの廃棄
	CObjectMesh::Uninit();
}

//==========================================================
// 更新処理
//==========================================================
void CMeshCylinder::Update(void)
{
	
}

//==========================================================
// 描画処理
//==========================================================
void CMeshCylinder::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice;	//デバイスへのポインタ

	//デバイスの取得
	pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	//ライティングをオフにする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// カリング設定
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 描画
	CObjectMesh::Draw();

	//ライティングをオンにする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// カリング設定直す
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//==========================================================
// 頂点情報設定
//==========================================================
void CMeshCylinder::SetVtxInfo(void)
{
	int nVertex = GetVertex();			// 頂点数を取得
	int nNumWidth = GetNumWidth();		// 幅枚数を取得
	int nNumHeight = GetNumHeight();	// 高さ枚数を取得
	D3DXVECTOR3 pos = GetPosition();	// 座標
	D3DXVECTOR3 vecDir;	//設定変更用ベクトル

	//頂点座標の設定(左奥から右手前に向かって頂点情報を設定する
	for (int nCntVtx = 0; nCntVtx < nVertex; nCntVtx++)
	{
		float fRot = m_fRot * (nCntVtx % (nNumWidth + 1));

		if (fRot > D3DX_PI || fRot < -D3DX_PI)
		{//-3.14～3.14の範囲外の場合
			if (fRot > D3DX_PI)
			{
				fRot += (-D3DX_PI * 2);
			}
			else if (fRot < -D3DX_PI)
			{
				fRot += (D3DX_PI * 2);
			}
		}

		//頂点座標
		m_pVtx[nCntVtx].pos.x = sinf(fRot) * m_fLength;
		m_pVtx[nCntVtx].pos.y = ((m_fHeight * 2) * nNumHeight) + ((nCntVtx / (nNumWidth + 1) * (-m_fHeight * 2)));
		m_pVtx[nCntVtx].pos.z = cosf(fRot) * m_fLength;

		vecDir = (m_pVtx[nCntVtx].pos - pos);

		//ベクトルを正規化する
		D3DXVec3Normalize(&vecDir, &vecDir);

		//法線
		m_pVtx[nCntVtx].nor = vecDir;

		//色
		m_pVtx[nCntVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		m_pVtx[nCntVtx].tex = D3DXVECTOR2((5.0f / nNumWidth) * (nCntVtx % (nNumWidth + 1)), (1.0f / nNumHeight) * (nCntVtx / (nNumWidth + 1)));
	}

	// 頂点設定
	SetVtx();
}

//==========================================================
// 生成
//==========================================================
CMeshCylinder *CMeshCylinder::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot,
	float fLength, float fHeight, const int nPriority, const int nWidth, const int nHeight)
{
	CMeshCylinder *pMeshWall = NULL;	// メッシュフィールドのポインタ
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ

	// メモリの確保
	pMeshWall = new CMeshCylinder(nPriority);

	if (pMeshWall != NULL)
	{// 確保できた場合

		// 初期化
		pMeshWall->Init();

		// 座標設定
		pMeshWall->SetPosition(pos);

		// 向き設定
		pMeshWall->SetRotation(rot);

		// 頂点生成
		pMeshWall->CObjectMesh::Create(nWidth, nHeight);

		// 一頂点辺りの角度を求める
		pMeshWall->m_fRot = (D3DX_PI * 2) / nWidth;

		// サイズ設定
		pMeshWall->SetSize(fLength, fHeight);

		// テクスチャ設定
		pMeshWall->BindTexture(pTexture->Regist(TEXTUREFILE_DATA));
	}

	return pMeshWall;
}

//==========================================================
// 幅設定
//==========================================================
void CMeshCylinder::SetSize(float fLength, float fHeight)
{
	// サイズ設定
	m_fLength = fLength;
	m_fHeight = fHeight;

	// 頂点情報設定
	SetVtxInfo();
}

//==========================================================
// 距離を設定
//==========================================================
void CMeshCylinder::SetLength(const float fLength)
{
	// 距離を設定
	m_fLength = fLength;

	// 頂点情報設定
	SetVtxInfo();
}

//==========================================================
// 色設定
//==========================================================
void CMeshCylinder::SetCol(D3DXCOLOR col)
{
	int nVertex = GetVertex();			// 頂点数を取得

	//頂点座標の設定(左奥から右手前に向かって頂点情報を設定する
	for (int nCntVtx = 0; nCntVtx < nVertex; nCntVtx++)
	{
		//色
		m_pVtx[nCntVtx].col = col;
	}

	// 頂点設定
	SetVtx();
}

//==========================================================
// コンストラクタ
//==========================================================
CMeshSmake::CMeshSmake(int nPriority) : CMeshCylinder(nPriority)
{
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_tex = D3DXVECTOR2(0.0f, 0.0f);
}

//==========================================================
// デストラクタ
//==========================================================
CMeshSmake::~CMeshSmake()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CMeshSmake::Init(void)
{
	CMeshCylinder::Init();

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CMeshSmake::Uninit(void)
{
	// 頂点バッファの廃棄
	CMeshCylinder::Uninit();
}

//==========================================================
// 更新処理
//==========================================================
void CMeshSmake::Update(void)
{
	CMeshCylinder::Update();

	m_col.a -= 0.005f * CManager::GetInstance()->GetSlow()->Get();

	SetCol(m_col);
	SetHeight(m_fHeight + 0.07f);
	m_tex.x += 0.005f;

	if (m_tex.x > 1.0f)
	{
		m_tex.x -= 1.0f;
	}
	else if (m_tex.x < 0.0f)
	{
		m_tex.x += 1.0f;
	}

	if (m_col.a <= 0.0f)
	{
		Uninit();
	}
}

//==========================================================
// 描画処理
//==========================================================
void CMeshSmake::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice;		//デバイスへのポインタ

	//デバイスの取得
	pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	//アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	//αブレンディングを加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 描画
	CMeshCylinder::Draw();

	//アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 255);

	//αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//==========================================================
// 生成
//==========================================================
CMeshSmake *CMeshSmake::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot,
	float fLength, float fHeight, const int nPriority, const int nWidth, const int nHeight)
{
	CMeshSmake *pMeshWall = NULL;	// メッシュフィールドのポインタ
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ

	// メモリの確保
	pMeshWall = new CMeshSmake(6);

	if (pMeshWall != NULL)
	{// 確保できた場合

		// 初期化
		pMeshWall->Init();

		// 座標設定
		pMeshWall->SetPosition(pos);

		// 向き設定
		pMeshWall->SetRotation(rot);

		// 頂点生成
		pMeshWall->CObjectMesh::Create(nWidth, nHeight);

		// 一頂点辺りの角度を求める
		pMeshWall->m_fRot = (D3DX_PI * 2) / nWidth;

		// サイズ設定
		pMeshWall->SetSize(fLength, fHeight);

		// テクスチャ設定
		pMeshWall->BindTexture(pTexture->Regist(TEXTUREFILE_DATA));
	}

	return pMeshWall;
}

//==========================================================
// 幅設定
//==========================================================
void CMeshSmake::SetSize(float fLength, float fHeight)
{
	// サイズ設定
	m_fLength = fLength;
	m_fHeight = fHeight;

	// 頂点情報設定
	SetVtxInfo();
}

//==========================================================
// 色設定
//==========================================================
void CMeshSmake::SetCol(D3DXCOLOR col)
{
	int nVertex = GetVertex();			// 頂点数を取得

	//頂点座標の設定(左奥から右手前に向かって頂点情報を設定する
	for (int nCntVtx = 0; nCntVtx < nVertex; nCntVtx++)
	{
		//色
		m_pVtx[nCntVtx].col = col;
	}

	// 頂点設定
	SetVtx();
}

//==========================================================
// 高さ設定
//==========================================================
void CMeshSmake::SetHeight(float fHeight)
{
	int nVertex = GetVertex();			// 頂点数を取得
	int nNumWidth = GetNumWidth();		// 幅枚数を取得
	int nNumHeight = GetNumHeight();	// 高さ枚数を取得
	D3DXVECTOR3 pos = GetPosition();	// 座標
	D3DXVECTOR3 vecDir;	//設定変更用ベクトル

	m_fHeight = fHeight;

	//頂点座標の設定(左奥から右手前に向かって頂点情報を設定する
	for (int nCntVtx = 0; nCntVtx < nVertex; nCntVtx++)
	{
		//頂点座標
		m_pVtx[nCntVtx].pos.y = ((m_fHeight * 2) * nNumHeight) + ((nCntVtx / (nNumWidth + 1) * (-m_fHeight * 2)));
		m_pVtx[nCntVtx].tex = D3DXVECTOR2((5.0f / nNumWidth) * (nCntVtx % (nNumWidth + 1)) + m_tex.x, (1.0f / nNumHeight) * (nCntVtx / (nNumWidth + 1)));
	}

	// 頂点設定
	SetVtx();
}
