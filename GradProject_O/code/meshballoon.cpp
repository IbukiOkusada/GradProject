//==========================================================
//
// サンプル [sample.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "meshballoon.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"

// マクロ定義

//==========================================================
// コンストラクタ
//==========================================================
CMeshBalloon::CMeshBalloon(int nPriOrity) : CObjectMesh(nPriOrity)
{

}

//==========================================================
// デストラクタ
//==========================================================
CMeshBalloon::~CMeshBalloon()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CMeshBalloon::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CMeshBalloon::Uninit(void)
{
	CObjectMesh::Uninit();
}

//==========================================================
// 更新処理
//==========================================================
void CMeshBalloon::Update(void)
{

}

//==========================================================
// 描画処理
//==========================================================
void CMeshBalloon::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice;		//デバイスへのポインタ

	//デバイスの取得
	pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	// 描画
	CObjectMesh::Draw();

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

//==========================================================
// 生成
//==========================================================
CMeshBalloon *CMeshBalloon::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, float fLength, const int nPriority, const int nWidth, const int nHeight)
{
	CMeshBalloon *pMesh = nullptr;

	pMesh = new CMeshBalloon;

	if (pMesh != NULL)
	{
		// 初期化処理
		pMesh->Init();

		// 座標設定
		pMesh->SetPosition(pos);

		// 向き設定
		pMesh->SetRotation(rot);

		// 頂点生成
		pMesh->CObjectMesh::Create(nWidth, nHeight);

		// 一頂点辺りの角度を求める
		pMesh->m_fRot = (D3DX_PI * 2) / nWidth;

		// 縦方向の角度を求める
		pMesh->m_fHeightRot = (D3DX_PI * 0.5f) / (nHeight + 2);

		// サイズ設定
		pMesh->SetSize(fLength);

		// テクスチャ設定
		pMesh->BindTexture(-1);
	}

	return pMesh;
}

//==========================================================
// 頂点情報設定
//==========================================================
void CMeshBalloon::SetVtxInfo(void)
{
	int nVertex = GetVertex();			// 頂点数を取得
	int nNumWidth = GetNumWidth();		// 幅枚数を取得
	int nNumHeight = GetNumHeight();	// 高さ枚数を取得
	D3DXVECTOR3 pos = GetPosition();	// 座標
	D3DXVECTOR3 vecDir;				// 設定変更用ベクトル

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

		m_pVtx[nCntVtx].tex = D3DXVECTOR2((1.0f / nNumWidth) * (nCntVtx % (nNumWidth + 1)), 1.0f - ((1.0f / nNumHeight) * (nCntVtx / (nNumWidth + 1))));
	}

	// 頂点設定
	SetVtx();
}

//==========================================================
// 幅設定
//==========================================================
void CMeshBalloon::SetSize(float fLength)
{
	// サイズ設定
	m_fLength = fLength;

	// 頂点情報設定
	SetVtxInfo();
}