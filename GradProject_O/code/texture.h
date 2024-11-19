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

private:	// 自分だけがアクセス可能な定義
	
	// ファイル情報
	struct File
	{
		LPDIRECT3DTEXTURE9 pTexture;	// テクスチャのポインタ
		std::string filename;				// ファイル名

		// コンストラクタ 
		File() : pTexture(nullptr), filename("") {}
	};

public:	// 誰でもアクセス可能

	CTexture();		// コンストラクタ
	~CTexture();	// デストラクタ

	// メンバ関数
	HRESULT Load(void);
	void Unload(void);
	int Regist(const char* pFileName);
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
