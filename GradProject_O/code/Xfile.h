//==========================================================
//
// Xファイル管理 [modelfile.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MODELFILE_H_		// このマクロが定義されていない場合
#define _MODELFILE_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "list.h"

namespace XFile
{
	const D3DXVECTOR3 START_VTXMAX = D3DXVECTOR3(-100000.0f, -100000.0f, -100000.0f);
	const D3DXVECTOR3 START_VTXMIN = D3DXVECTOR3(100000.0f, 100000.0f, 100000.0f);
}

//==========================================================
// Xファイル情報のクラス定義
//==========================================================
class CXFile
{
public:	// 誰でもアクセス可能な定義

	// Xファイル情報
	struct SFileData
	{
		LPD3DXMESH pMesh;		// メッシュ(頂点情報)へのポインタ
		LPD3DXBUFFER pBuffMat;	// マテリアルへのポインタ
		int *pIdexTexture;		// テクスチャ番号
		DWORD dwNumMat;			// マテリアルの数
		D3DXVECTOR3 vtxMin;		// 最小
		D3DXVECTOR3 vtxMax;		// 最大

		// コンストラクタ
		SFileData() : pMesh(nullptr), pBuffMat(nullptr), pIdexTexture(nullptr), 
			dwNumMat(), vtxMin(XFile::START_VTXMIN), vtxMax(XFile::START_VTXMAX) {}
	};

private:	// 自分だけがアクセス可能な定義

	// ファイル読み込み情報
	struct SFileInfo
	{
		std::string name;		// ファイル名
		SFileData data;			// ファイル情報

		// コンストラクタ
		SFileInfo() : name(""), data(SFileData()) {}
	};

public:	// 誰でもアクセス可能

	CXFile();	// コンストラクタ
	~CXFile();	// デストラクタ

	// メンバ関数
	 int Regist(const char *pFileName);
	 void Unload(void);
	 void SetSizeVtxMax(int nIdx, D3DXVECTOR3 vtxMax) { m_List[nIdx]->data.vtxMax = vtxMax; }
	 void SetSizeVtxMin(int nIdx, D3DXVECTOR3 vtxMin) { m_List[nIdx]->data.vtxMin = vtxMin; }

	 // メンバ関数(取得)
	 SFileData*SetAddress(int nIdx);
	 int GetNumAll(void) { return m_List.size(); }
	 D3DXVECTOR3 GetMax(int nIdx) { return m_List[nIdx]->data.vtxMax; }
	 D3DXVECTOR3 GetMin(int nIdx) { return m_List[nIdx]->data.vtxMin; }
	 const char *GetFileName(int nIdx) { return m_List[nIdx]->name.c_str(); }

private:	// 自分だけがアクセス可能

	// メンバ関数
	 HRESULT FileLoad(int nIdx);

	// メンバ変数
	std::vector<SFileInfo*> m_List;
};

#endif

