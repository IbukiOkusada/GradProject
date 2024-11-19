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

//==========================================================
// コンストラクタ
//==========================================================
CXFile::CXFile()
{
	m_List.clear();
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
int CXFile::Regist(const char* pFileName)
{
	int nIdx = -1;

	// 読み込まれているか確認
	for (int i = 0; i < m_List.size(); i++)
	{
		// 同じファイルか確認
		if (strstr(pFileName, m_List[i]->name.c_str()) != nullptr)
		{// 同じものが存在している場合
			nIdx = i;
			return nIdx;	// ファイル情報を返す
		}
	}

	// 存在していない場合メモリを確保する
	SFileInfo* pInfo = DEBUG_NEW SFileInfo;	// メモリの確保
	*pInfo = SFileInfo();

	// ファイル名の取得
	pInfo->name = pFileName;

	nIdx = m_List.size();
	m_List.push_back(pInfo);

	// Xファイルの読み込み
	if (SUCCEEDED(FileLoad(nIdx)))
	{// 成功した場合
		return nIdx;	// ファイル情報を返す
	}
	else {	// 失敗
		m_List.pop_back();
		delete pInfo;
		pInfo = nullptr;
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
	if (FAILED(D3DXLoadMeshFromX(m_List[nIdx]->name.c_str(),
		D3DXMESH_SYSTEMMEM,
		pDevice,
		nullptr,
		&m_List[nIdx]->data.pBuffMat,
		nullptr,
		&m_List[nIdx]->data.dwNumMat,
		&m_List[nIdx]->data.pMesh)))
	{// 読み込みに失敗した場合
		return E_FAIL;	// 失敗を返す
	}

	// テクスチャポインタの確保
	if ((int)m_List[nIdx]->data.dwNumMat > 0)
	{// マテリアルを使用している場合
		if (m_List[nIdx]->data.pIdexTexture == nullptr)
		{// テクスチャが使用されていない場合
			// マテリアル数分確保
			m_List[nIdx]->data.pIdexTexture = DEBUG_NEW int[(int)m_List[nIdx]->data.dwNumMat];
		}
	}

	//マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_List[nIdx]->data.pBuffMat->GetBufferPointer();

	for (int iMat = 0; iMat < (int)m_List[nIdx]->data.dwNumMat; iMat++)
	{
		if (pMat[iMat].pTextureFilename != nullptr)
		{//テクスチャファイルが存在している
			//テクスチャの読み込み
			m_List[nIdx]->data.pIdexTexture[iMat] = pTexture->Regist(pMat[iMat].pTextureFilename);
		}
		else
		{
			m_List[nIdx]->data.pIdexTexture[iMat] = -1;
		}
	}

	//頂点数を取得
	nNumVtx = m_List[nIdx]->data.pMesh->GetNumVertices();

	//頂点フォーマットのサイズを取得
	dwSizeFVF = D3DXGetFVFVertexSize(m_List[nIdx]->data.pMesh->GetFVF());

	//頂点バッファをロック
	m_List[nIdx]->data.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	for (int iVtx = 0; iVtx < nNumVtx; iVtx++)
	{
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;	//頂点座標の代入

		// X座標
		if (vtx.x < m_List[nIdx]->data.vtxMin.x)
		{//最小値よりも値が小さい場合
			m_List[nIdx]->data.vtxMin.x = vtx.x;
		}
		else if (vtx.x > m_List[nIdx]->data.vtxMax.x)
		{//最大値よりも値が大きい場合
			m_List[nIdx]->data.vtxMax.x = vtx.x;
		}

		// Z座標
		if (vtx.z < m_List[nIdx]->data.vtxMin.z)
		{//最小値よりも値が小さい場合
			m_List[nIdx]->data.vtxMin.z = vtx.z;
		}
		else if (vtx.z > m_List[nIdx]->data.vtxMax.z)
		{//最大値よりも値が大きい場合
			m_List[nIdx]->data.vtxMax.z = vtx.z;
		}

		// Y座標
		if (vtx.y < m_List[nIdx]->data.vtxMin.y)
		{//最小値よりも値が小さい場合
			m_List[nIdx]->data.vtxMin.y = vtx.y;
		}
		else if (vtx.y > m_List[nIdx]->data.vtxMax.y)
		{//最大値よりも値が大きい場合
			m_List[nIdx]->data.vtxMax.y = vtx.y;
		}

		pVtxBuff += dwSizeFVF;	//頂点フォーマットのサイズ分ポインタを進める
	}

	//頂点バッファをアンロック
	m_List[nIdx]->data.pMesh->UnlockVertexBuffer();

	return S_OK;	// 成功を返す
}

//==========================================================
// Xファイル情報廃棄
//==========================================================
void CXFile::Unload(void)
{
	for (int i = 0; i < m_List.size(); i++)
	{
		auto texture = m_List[i];

		//メッシュの廃棄
		if (texture->data.pMesh != nullptr)
		{
			texture->data.pMesh->Release();
			texture->data.pMesh = nullptr;
		}

		//マテリアルの廃棄
		if (texture->data.pBuffMat != nullptr)
		{
			texture->data.pBuffMat->Release();
			texture->data.pBuffMat = nullptr;
		}

		// テクスチャ番号の廃棄
		delete[] texture->data.pIdexTexture;	// テクスチャポインタの開放

		// 外す
		m_List.erase(m_List.begin() + i);
		delete texture;	// メモリの開放
		i--;

	}
}

//==========================================================
// 指定された番号の情報取得
//==========================================================
CXFile::SFileData *CXFile::SetAddress(int nIdx)
{
	if (nIdx > m_List.size() || nIdx < 0)
	{// 読み込み範囲外の場合
		return nullptr;
	}

	return &m_List[nIdx]->data;
}