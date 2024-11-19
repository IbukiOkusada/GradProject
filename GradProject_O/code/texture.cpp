//===============================================
//
// テクスチャの管理全般 [texture.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "texture.h"
#include "manager.h"
#include "renderer.h"
#include <string.h>

// ファイル名
const char *CTexture::m_apDefFileName[TYPE_MAX] =
{
	"data\\TEXTURE\\effect\\effect000.jpg",
	"data\\TEXTURE\\effect\\water.png",
};

//===============================================
// コンストラクタ
//===============================================
CTexture::CTexture()
{
	m_List.Clear();
}

//===============================================
// デストラクタ
//===============================================
CTexture::~CTexture()
{
	
}

//===============================================
// 読み込み
//===============================================
HRESULT CTexture::Load(void)
{
	LPDIRECT3DDEVICE9 pDevice;		//デバイスへのポインタ

	//デバイスの取得
	pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// 総数分読み込み
	for (int i = 0; i < TYPE_MAX; i++)
	{
		File* pFile = DEBUG_NEW File;

		if (FAILED(D3DXCreateTextureFromFile(pDevice,
			m_apDefFileName[i],
			&pFile->pTexture)))
		{
			delete pFile;
			return -1;
		}

		m_List.Regist(pFile);
		pFile->filename = m_apDefFileName[i];

	}

	return S_OK;
}

//===============================================
// 廃棄
//===============================================
void CTexture::Unload(void)
{
	for (int i = 0; i < m_List.GetNum(); i++)
	{
		File* pFile = m_List.Get(i);

		// テクスチャの廃棄
		if (pFile->pTexture != nullptr)
		{// 使用されている場合
			pFile->pTexture->Release();
			pFile->pTexture = nullptr;
		}

		m_List.Delete(pFile);
		delete pFile;
		i--;
	}
}

//===============================================
// 指定ファイル読み込み
//===============================================
int CTexture::Regist(const char* pFileName)
{
	CRenderer* pRenderer = CManager::GetInstance()->GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		//デバイスへのポインタを取得
	int nIdx = -1;	// テクスチャ番号

	for (int i = 0; i < m_List.GetNum(); i++)
	{
		if (strstr(&m_List.Get(i)->filename[0], pFileName) != nullptr)
		{// ファイル名が一致している場合
			nIdx = i;	// テクスチャ番号を設定
			return nIdx;
		}
	}

	nIdx = m_List.GetNum();
	File* pFile = DEBUG_NEW File;
	pFile->pTexture = nullptr;

	if(FAILED(D3DXCreateTextureFromFile(pDevice,
		pFileName,
		&pFile->pTexture)))
	{
		delete pFile;
		return -1;
	}

	m_List.Regist(pFile);

	pFile->filename = pFileName;

	return nIdx;	// テクスチャ番号を返す
}

//===============================================
// 指定アドレスのテクスチャを取得
//===============================================
LPDIRECT3DTEXTURE9 CTexture::SetAddress(int nIdx)
{
	if (nIdx > m_List.GetNum() || nIdx < 0)
	{// 読み込み範囲外の場合
		return nullptr;
	}

	return m_List.Get(nIdx)->pTexture;
}

//===============================================
// テクスチャ名を取得
//===============================================
const char *CTexture::GetFileName(int nIdx)
{
	if (nIdx >= 0 && nIdx < TYPE_MAX)
	{// 範囲内
		return m_apDefFileName[nIdx];
	}

	return nullptr;
}