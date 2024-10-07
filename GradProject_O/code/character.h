//==========================================================
//
// 階層構造の処理全般 [character.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _CHARACTER_H_	// このマクロが定義されていない場合
#define _CHARACTER_H_	// 二重インクルード防止用マクロを定義

#include "main.h"		//main.hで定義しているものが必要なためinclude

// 前方宣言
class CModel;
class CMotion;

//**********************************************************
// キャラクタークラスの定義
//**********************************************************
class CCharacter
{
private:	// 自分だけがアクセス可能な定義

public:	// 誰でもアクセス可能
	CCharacter();	// コンストラクタ
	~CCharacter();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	HRESULT Init(const char* pFileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void OpenFile(const char *pFileData);
	static CCharacter *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, const char* pFileName);
	static CCharacter *Create(const char* pFileName);

	// 設定
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRotation(const D3DXVECTOR3 rot);
	void SetParent(D3DXMATRIX *pMtx) { m_ParentMtx = pMtx; }
	void SetDraw(bool bDraw = true);
	void SetShadow(bool bShadow = false);
	void SetCharaMtx(void);
	void SetChangeMatCol(const D3DXCOLOR& col);
	void SetChangeMat(const bool bChange = false);

	// メンバ関数(取得)
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXMATRIX *GetMtxWorld(void) { return &m_mtxWorld; }
	CModel *GetParts(int nIdx);
	int GetNumParts(void) { return m_nNumParts; }
	CMotion *GetMotion(void) { return m_pMotion; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	void LoadFile(FILE *pFile);
	void LoadXFile(FILE *pFile);
	void LoadSetData(FILE *pFile);

	// メンバ変数
	CMotion *m_pMotion;		// モーション情報のポインタ
	CModel **m_ppParts;		// パーツの情報
	int m_nNumParts;			// パーツ数
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	D3DXMATRIX *m_ParentMtx;	// 親のマトリックス
	D3DXVECTOR3 m_pos;		// 座標
	D3DXVECTOR3 m_rot;		// 向き
};

#endif