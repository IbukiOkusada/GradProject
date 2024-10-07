//==========================================================
//
// Xファイル管理 [modelfile.h]
// Author : Ibuki Okusada
//
//==========================================================
#include "Xfile.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include <string.h>

// 静的メンバ変数宣言
int CXFile::m_nNumAll = 0;	// 読み込み総数

//==========================================================
// コンストラクタ
//==========================================================
CXFile::CXFile()
{
	for(int nCnt = 0; nCnt < MAX_FILE; nCnt++)
	{
		m_apModelFileData[nCnt] = NULL;
	}
}

//==========================================================
// デストラクタ
//==========================================================
CXFile::~CXFile()
{

}

//==========================================================
// 読み込み確認
//==========================================================
int CXFile::Regist(const char *pFileName)
{
	int nIdx = -1;
	// 読み込まれているか確認
	for (int nCnt = 0; nCnt < MAX_FILE; nCnt++)
	{
		if (m_apModelFileData[nCnt] == NULL)
		{// 使われていない場合
			continue;
		}

		// 同じファイルか確認
		if (strstr(pFileName, &m_apModelFileData[nCnt]->aFileName[0]) != NULL)
		{// 同じものが存在している場合
			nIdx = nCnt;
			return nIdx;	// ファイル情報を返す
		}
	}

	// 最後まで存在していない場合
	for (int nCnt = 0; nCnt < MAX_FILE; nCnt++)
	{
		if (m_apModelFileData[nCnt] == NULL)
		{// 使われていない場合
			m_apModelFileData[nCnt] = new FileInfo;	// メモリの確保

			if (m_apModelFileData[nCnt] != NULL)
			{// メモリを確保できた場合

				// メモリのクリア
				memset(m_apModelFileData[nCnt], NULL, sizeof(FileInfo));

				// サイズの最低値を代入
				m_apModelFileData[nCnt]->filedata.vtxMax = D3DXVECTOR3(-100000.0f, -100000.0f, -100000.0f);
				m_apModelFileData[nCnt]->filedata.vtxMin = D3DXVECTOR3(100000.0f, 100000.0f, 100000.0f);

				// ファイル名の取得
				strcpy(&m_apModelFileData[nCnt]->aFileName[0], pFileName);

				// Xファイルの読み込み
				if (SUCCEEDED(FileLoad(nCnt)))
				{// 成功した場合

					m_nNumAll++;	// 読み込み数カウントアップ
					nIdx = nCnt;
					return nIdx;	// ファイル情報を返す
				}
				else {
					// 値をクリアする
					memset(m_apModelFileData[nCnt]->aFileName, '\0', sizeof(m_apModelFileData[nCnt]->aFileName));

					delete m_apModelFileData[nCnt];
					m_apModelFileData[nCnt] = nullptr;
				}

				break;
			}
		}
	}

	return -1;
}

//==========================================================
// Xファイル読み込み
//==========================================================
HRESULT CXFile::FileLoad(int nIdx)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();;		//デバイスへのポインタ
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// テクスチャへのポインタ
	D3DXMATERIAL *pMat;	//マテリアルデータへのポインタ
	int nNumVtx;		//頂点数
	DWORD dwSizeFVF;	//頂点フォーマットのサイズ
	BYTE *pVtxBuff;		//頂点バッファのポインタ

	//Xファイルの読み込み
	if (FAILED(D3DXLoadMeshFromX(&m_apModelFileData[nIdx]->aFileName[0],
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&m_apModelFileData[nIdx]->filedata.pBuffMat,
		NULL,
		&m_apModelFileData[nIdx]->filedata.dwNumMat,
		&m_apModelFileData[nIdx]->filedata.pMesh)))
	{// 読み込みに失敗した場合
		return E_FAIL;	// 失敗を返す
	}

	// テクスチャポインタの確保
	if ((int)m_apModelFileData[nIdx]->filedata.dwNumMat > 0)
	{// マテリアルを使用している場合
		if (m_apModelFileData[nIdx]->filedata.pIdexTexture == NULL)
		{// テクスチャが使用されていない場合
			// マテリアル数分確保
			m_apModelFileData[nIdx]->filedata.pIdexTexture = new int[(int)m_apModelFileData[nIdx]->filedata.dwNumMat];
		}
	}

	//マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_apModelFileData[nIdx]->filedata.pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)m_apModelFileData[nIdx]->filedata.dwNumMat; nCntMat++)
	{
		if (pMat[nCntMat].pTextureFilename != NULL)
		{//テクスチャファイルが存在している
			//テクスチャの読み込み
			m_apModelFileData[nIdx]->filedata.pIdexTexture[nCntMat] = pTexture->Regist(pMat[nCntMat].pTextureFilename);
		}
		else
		{
			m_apModelFileData[nIdx]->filedata.pIdexTexture[nCntMat] = -1;
		}
	}

	//頂点数を取得
	nNumVtx = m_apModelFileData[nIdx]->filedata.pMesh->GetNumVertices();

	//頂点フォーマットのサイズを取得
	dwSizeFVF = D3DXGetFVFVertexSize(m_apModelFileData[nIdx]->filedata.pMesh->GetFVF());

	//頂点バッファをロック
	m_apModelFileData[nIdx]->filedata.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;	//頂点座標の代入

		// X座標
		if (vtx.x < m_apModelFileData[nIdx]->filedata.vtxMin.x)
		{//最小値よりも値が小さい場合
			m_apModelFileData[nIdx]->filedata.vtxMin.x = vtx.x;
		}
		else if (vtx.x > m_apModelFileData[nIdx]->filedata.vtxMax.x)
		{//最大値よりも値が大きい場合
			m_apModelFileData[nIdx]->filedata.vtxMax.x = vtx.x;
		}

		// Z座標
		if (vtx.z < m_apModelFileData[nIdx]->filedata.vtxMin.z)
		{//最小値よりも値が小さい場合
			m_apModelFileData[nIdx]->filedata.vtxMin.z = vtx.z;
		}
		else if (vtx.z > m_apModelFileData[nIdx]->filedata.vtxMax.z)
		{//最大値よりも値が大きい場合
			m_apModelFileData[nIdx]->filedata.vtxMax.z = vtx.z;
		}

		// Y座標
		if (vtx.y < m_apModelFileData[nIdx]->filedata.vtxMin.y)
		{//最小値よりも値が小さい場合
			m_apModelFileData[nIdx]->filedata.vtxMin.y = vtx.y;
		}
		else if (vtx.y > m_apModelFileData[nIdx]->filedata.vtxMax.y)
		{//最大値よりも値が大きい場合
			m_apModelFileData[nIdx]->filedata.vtxMax.y = vtx.y;
		}

		pVtxBuff += dwSizeFVF;	//頂点フォーマットのサイズ分ポインタを進める
	}

	//頂点バッファをアンロック
	m_apModelFileData[nIdx]->filedata.pMesh->UnlockVertexBuffer();

	return S_OK;	// 成功を返す
}

//==========================================================
// Xファイル情報廃棄
//==========================================================
void CXFile::Unload(void)
{
	// 読み込まれているか確認
	for (int nCnt = 0; nCnt < MAX_FILE; nCnt++)
	{
		if (m_apModelFileData[nCnt] == NULL)
		{// 使用されていない場合
			continue;	// やり直し
		}
		
		// 値をクリアする
		memset(m_apModelFileData[nCnt]->aFileName, '\0', sizeof(m_apModelFileData[nCnt]->aFileName));

		//メッシュの廃棄
		if (m_apModelFileData[nCnt]->filedata.pMesh != NULL)
		{
			m_apModelFileData[nCnt]->filedata.pMesh->Release();
			m_apModelFileData[nCnt]->filedata.pMesh = NULL;
		}

		//マテリアルの廃棄
		if (m_apModelFileData[nCnt]->filedata.pBuffMat != NULL)
		{
			m_apModelFileData[nCnt]->filedata.pBuffMat->Release();
			m_apModelFileData[nCnt]->filedata.pBuffMat = NULL;
		}

		// テクスチャ番号の廃棄
		delete[] m_apModelFileData[nCnt]->filedata.pIdexTexture;	// テクスチャポインタの開放

		delete m_apModelFileData[nCnt];	// メモリの開放
		m_apModelFileData[nCnt] = NULL;	// 使用していない状態にする
		m_nNumAll--;	// 読み込み数カウントダウン
	}
}

//==========================================================
// Xファイル情報廃棄
//==========================================================
CXFile::FileData *CXFile::SetAddress(int nIdx)
{
	if (nIdx > m_nNumAll || nIdx < 0)
	{// 読み込み範囲外の場合
		return NULL;
	}

	return &m_apModelFileData[nIdx]->filedata;
}