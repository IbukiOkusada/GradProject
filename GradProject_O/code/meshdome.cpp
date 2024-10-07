//==========================================================
//
// メッシュドームの処理 [meshdome.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "meshdome.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"

// マクロ定義
#define TEXTUREFILE_DATA	"data\\TEXTURE\\sky001.png"		//テクスチャデータ

//==========================================================
// コンストラクタ
//==========================================================
CMeshDome::CMeshDome()
{

}

//==========================================================
// デストラクタ
//==========================================================
CMeshDome::~CMeshDome()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CMeshDome::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CMeshDome::Uninit(void)
{
	//頂点バッファの廃棄
	CObjectMesh::Uninit();
}

//==========================================================
// 更新処理
//==========================================================
void CMeshDome::Update(void)
{
	
}

//==========================================================
// 描画処理
//==========================================================
void CMeshDome::Draw(void)
{
	D3DXVECTOR3 rot = GetRotation();
	rot.y += 0.0001f;

	SetRotation(rot);

	LPDIRECT3DDEVICE9 pDevice;		//デバイスへのポインタ

	//デバイスの取得
	pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	//ライティングをオフにする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 描画
	CObjectMesh::Draw();

	//ライティングをオンにする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

}

//==========================================================
// 頂点情報設定
//==========================================================
void CMeshDome::SetVtxInfo(void)
{
	int nVertex = GetVertex();			// 頂点数を取得
	int nNumWidth = GetNumWidth();		// 幅枚数を取得
	int nNumHeight = GetNumHeight();	// 高さ枚数を取得
	D3DXVECTOR3 pos = GetPosition();	// 座標
	D3DXVECTOR3 vecDir;					// 設定変更用ベクトル

	// 頂点座標の設定(左奥から右手前に向かって頂点情報を設定する
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

		float fHeightRot = m_fHeightRot * (nCntVtx / (nNumWidth + 1));

		if (fHeightRot > D3DX_PI || fHeightRot < -D3DX_PI)
		{//-3.14～3.14の範囲外の場合
			if (fHeightRot > D3DX_PI)
			{
				fHeightRot += (-D3DX_PI * 2);
			}
			else if (fHeightRot < -D3DX_PI)
			{
				fHeightRot += (D3DX_PI * 2);
			}
		}

		//頂点座標
		m_pVtx[nCntVtx].pos.x = (cosf(fHeightRot) * cosf(fRot)) * m_fLength;
		m_pVtx[nCntVtx].pos.y = sinf(fHeightRot) * m_fLength;
		m_pVtx[nCntVtx].pos.z = (cosf(fHeightRot) * sinf(fRot)) * m_fLength;

		// ベクトルを求める
		vecDir = m_pVtx[nCntVtx].pos - pos;

		//ベクトルを正規化する
		D3DXVec3Normalize(&vecDir, &vecDir);

		//法線
		m_pVtx[nCntVtx].nor = vecDir;

		//色
		m_pVtx[nCntVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		m_pVtx[nCntVtx].tex = D3DXVECTOR2((1.0f / nNumWidth) * (nCntVtx % (nNumWidth + 1)), 1.0f -((1.0f / nNumHeight) * (nCntVtx / (nNumWidth + 1))));
	}

	// 頂点設定
	SetVtx();
}

//==========================================================
// 生成
//==========================================================
CMeshDome *CMeshDome::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot,
	float fLength, float fHeight, const int nPriority, const int nWidth, const int nHeight)
{
	CMeshDome *pMeshWall = NULL;	// メッシュフィールドのポインタ
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ

													// メモリの確保
	pMeshWall = new CMeshDome;

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

		// 縦方向の角度を求める
		pMeshWall->m_fHeightRot = (D3DX_PI * 0.5f) / (nHeight + 2);

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
void CMeshDome::SetSize(float fLength, float fHeight)
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
void CMeshDome::SetColor(float fDiff)
{
	int nVertex = GetVertex();			// 頂点数を取得
	D3DXVECTOR3 pos = GetPosition();	// 座標
	D3DXVECTOR3 vecDir;					// 設定変更用ベクトル

	// 頂点座標の設定(左奥から右手前に向かって頂点情報を設定する
	for (int nCntVtx = 0; nCntVtx < nVertex; nCntVtx++)
	{
		if (fDiff >= 0.0f && fDiff < 0.5f)
		{
			//色
			m_pVtx[nCntVtx].col = D3DXCOLOR(0.1f + fDiff * 2, 0.1f + fDiff * 2, 0.1f + fDiff * 2, 1.0f);
		}
		else if (fDiff >= 0.75f && fDiff < 0.875f)
		{
			//色
			m_pVtx[nCntVtx].col = D3DXCOLOR(1.0f - (fDiff - 0.75f) * 2.0f, 1.0f - (fDiff - 0.75f) * 4, 1.0f - (fDiff - 0.75f) * 4, 1.0f);
		}
		else if (fDiff >= 0.875f && fDiff < 1.0f)
		{
			//色
			m_pVtx[nCntVtx].col = D3DXCOLOR(0.75f - (fDiff - 0.875f) * 7.0f, 1.0f - (fDiff - 0.75f) * 4, 1.0f - (fDiff - 0.75f) * 4, 1.0f);
		}
	}

	// 頂点設定
	SetVtx();
}
