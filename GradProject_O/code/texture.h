//===============================================
//
// テクスチャの管理全般 [texture.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _TEXTURE_H_		// このマクロが定義されていない場合
#define _TEXTURE_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "list.h"

//**********************************************************
// テクスチャクラスの定義
//**********************************************************
class CTexture
{
public:	// 誰でもアクセス可能な定義
	
	// 初期読み込み種類
	enum TYPE
	{
		TYPE_EFFECT = 0,		// エフェクト
		TYPE_WATER,				// しずく
		TYPE_MAX				   
	};
	// テクスチャ生成情報構造体
	struct SInfo
	{
	public:
		// デフォルトコンストラクタ
		SInfo() :
			Width(0),	// テクスチャ横幅
			Height(0),	// テクスチャ縦幅
			MipLevels(0),	// ミップマップレベル
			Usage(0),	// 性質・確保オプション
			Format(D3DFMT_UNKNOWN),	// ピクセルフォーマット
			Pool(D3DPOOL_DEFAULT)	// 格納メモリ
		{}

		// 引数付きコンストラクタ
		SInfo(const UINT in_nWidth, const UINT in_nHeight, const UINT in_nMip, const DWORD in_dwUsage, const D3DFORMAT in_format, const D3DPOOL in_pool) :
			Width(in_nWidth),	// テクスチャ横幅
			Height(in_nHeight),	// テクスチャ縦幅
			MipLevels(in_nMip),		// ミップマップレベル
			Usage(in_dwUsage),	// 性質・確保オプション
			Format(in_format),	// ピクセルフォーマット
			Pool(in_pool)		// 格納メモリ
		{}

		// デストラクタ
		~SInfo() {}

		// メンバ変数
		UINT		Width;		// テクスチャ横幅
		UINT		Height;		// テクスチャ縦幅
		UINT		MipLevels;	// ミップマップレベル
		DWORD		Usage;		// 性質・確保オプション
		D3DFORMAT	Format;		// ピクセルフォーマット
		D3DPOOL		Pool;		// 格納メモリ
	};
private:	// 自分だけがアクセス可能な定義
	
	// ファイル情報
	struct File
	{
		LPDIRECT3DTEXTURE9 pTexture;	// テクスチャのポインタ
		std::string filename;				// ファイル名
		D3DXIMAGE_INFO status;			// テクスチャステータスへのポインタ
		D3DXVECTOR2 aspect;
		// コンストラクタ 
		File() : pTexture(nullptr), filename(""), status({}), aspect(VECTOR2_ZERO) {}
	};

public:	// 誰でもアクセス可能

	CTexture();		// コンストラクタ
	~CTexture();	// デストラクタ

	// メンバ関数
	HRESULT Load(void);
	void Unload(void);
	int Regist(const char* pFileName);
	int Regist(const SInfo info);				// テクスチャ登録 (生成)
	LPDIRECT3DTEXTURE9 SetAddress(int nIdx);

	// メンバ関数(取得)
	int GetNumAll(void) { return m_List.GetNum(); }

	const static char *GetFileName(int nIdx);

private:	// 自分だけがアクセス可能

	// メンバ関数
	Clist<File*> m_List;
	const static char *m_apDefFileName[TYPE_MAX];	// 初期読み込みファイル名
};

#endif
