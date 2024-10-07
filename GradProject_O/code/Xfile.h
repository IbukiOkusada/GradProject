//==========================================================
//
// Xファイル管理 [modelfile.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MODELFILE_H_		// このマクロが定義されていない場合
#define _MODELFILE_H_		// 二重インクルード防止用マクロを定義

#include "main.h"

//==========================================================
// Xファイル情報のクラス定義
//==========================================================
class CXFile
{
private:	// 自分だけがアクセス可能な定数定義

	const static int MAX_FILE = 256;	// モデルファイルの最大数
	const static int MAX_NAME = 256;	// ファイル名文字数

public:	// 誰でもアクセス可能な定義

	// Xファイル情報
	struct FileData
	{
		LPD3DXMESH pMesh;		//メッシュ(頂点情報)へのポインタ
		LPD3DXBUFFER pBuffMat;	//マテリアルへのポインタ
		int *pIdexTexture;	// テクスチャ番号
		DWORD dwNumMat;		//マテリアルの数
		D3DXVECTOR3 vtxMin;	// 最小
		D3DXVECTOR3 vtxMax;	// 最大
	};

private:	// 自分だけがアクセス可能な定義

	// ファイル読み込み情報
	struct FileInfo
	{
		char aFileName[MAX_NAME];	// ファイル名
		FileData filedata;			// ファイル情報
	};

public:	// 誰でもアクセス可能

	CXFile();	// コンストラクタ
	~CXFile();	// デストラクタ

	// メンバ関数
	 int Regist(const char *pFileName);
	 void Unload(void);
	 void SetSizeVtxMax(int nIdx, D3DXVECTOR3 vtxMax) { m_apModelFileData[nIdx]->filedata.vtxMax = vtxMax; }
	 void SetSizeVtxMin(int nIdx, D3DXVECTOR3 vtxMin) { m_apModelFileData[nIdx]->filedata.vtxMin = vtxMin; }

	 // メンバ関数(取得)
	 FileData *SetAddress(int nIdx);
	 static int GetNumAll(void) { return m_nNumAll; }
	 D3DXVECTOR3 GetMax(int nIdx) { return m_apModelFileData[nIdx]->filedata.vtxMax; }
	 D3DXVECTOR3 GetMin(int nIdx) { return m_apModelFileData[nIdx]->filedata.vtxMin; }
	 char *GetFileName(int nIdx) { return &m_apModelFileData[nIdx]->aFileName[0]; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	 HRESULT FileLoad(int nIdx);

	// メンバ変数
	FileInfo *m_apModelFileData[MAX_FILE];	// モデルのファイル情報のポインタ
	static int m_nNumAll;	// 読み込み総数
};

#endif

