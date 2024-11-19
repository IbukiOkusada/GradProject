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
//============================================================
//	テクスチャ登録処理 (生成)
//============================================================
int CTexture::Regist(const SInfo info)
{
	int nIdx = m_List.GetNum();	// テクスチャ読込番号
	HRESULT hr;				// 異常終了の確認用
	CTexture::File *tempMapInfo = DEBUG_NEW CTexture::File;	// マップ情報

	// マップ情報のポインタを初期化
	tempMapInfo->pTexture = nullptr;	// テクスチャへのポインタ

	// 空のテクスチャを生成
	hr = D3DXCreateTexture
	( // 引数
		CManager::GetInstance()->GetRenderer()->GetDevice(),		// Direct3Dデバイス
		info.Width,		// テクスチャ横幅
		info.Height,	// テクスチャ縦幅
		info.MipLevels,	// ミップマップレベル
		info.Usage,		// 性質・確保オプション
		info.Format,	// ピクセルフォーマット
		info.Pool,		// 格納メモリ
		&tempMapInfo->pTexture	// テクスチャへのポインタ
	);
	if (FAILED(hr))
	{ // テクスチャの生成に失敗した場合

		assert(false);
		return -1;
	}

	// テクスチャステータスを設定
	D3DXIMAGE_INFO* pStatus = &tempMapInfo->status;	// ステータス情報
	pStatus->Width = info.Width;						// テクスチャ横幅
	pStatus->Height = info.Height;						// テクスチャ縦幅
	pStatus->Depth = 1;								// テクスチャ深度
	pStatus->MipLevels = info.MipLevels;					// ミップマップレベル
	pStatus->Format = info.Format;						// ピクセルフォーマット
	pStatus->ResourceType = D3DRTYPE_TEXTURE;				// リソース種類
	pStatus->ImageFileFormat = (D3DXIMAGE_FILEFORMAT)-1;	// ファイル形式 (作成のため無し)

	// ファイルパス名を保存
	tempMapInfo->filename = -1;	// 読込ではないのでパス無し

	// アスペクト比を計算
	tempMapInfo->aspect.x = (float)info.Width / (float)info.Height;
	tempMapInfo->aspect.y = (float)info.Height / (float)info.Width;

	// テクスチャ情報を保存
	m_List.Regist(tempMapInfo);

	// 読み込んだテクスチャの配列番号を返す
	return nIdx;
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