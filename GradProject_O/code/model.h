//==========================================================
//
// 階層構造のモデル処理 [model.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MODEL_H_	// このマクロが定義されていない場合
#define _MODEL_H_	// 二重インクルード防止用マクロを定義

#include "main.h"
#include "object.h"

//**********************************************************
// 階層構造モデルクラス
//**********************************************************
class CModel : public CObject
{
public:	// 誰でもアクセス可能

	CModel();	// コンストラクタ
	~CModel();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	HRESULT Init(const char *pFileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawOnShader(void);
	static CModel *Create(const char *pFileName);
	void SetParent(D3DXMATRIX *pMtx);
	void SetCharaParent(D3DXMATRIX *pMtx) { m_pCharacterMtx = pMtx; }
	void BindModelFile(int nIdx);
	void SetRotSize(D3DXVECTOR3 &SetMax, D3DXVECTOR3 &SetMin, D3DXVECTOR3 vtxMax, D3DXVECTOR3 vtxMin, float fRot);

	// メンバ関数(取得)
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetPosOrigin(void) { return m_posOrigin; }
	D3DXVECTOR3 GetCurrentPosition(void) { return m_CurPos; }
	D3DXVECTOR3 GetCurrentRotation(void) { return m_CurRot; }
	D3DXVECTOR3 GetScaleOrigin() { return m_scaleOrigin; }
	D3DXVECTOR3 GetOldPosition(void) { return m_OldPos; }
	D3DXVECTOR3* GetMtxPos() { return &m_mtxpos; }
	D3DXMATRIX *GetMtx(void) { return &m_mtxWorld; }
	D3DXMATRIX *GetMtxParent(void) { return m_pParentMtx; }
	int GetId(void) { return m_nIdxModel; }
	void SetShadowHeight(const float fHeight) { m_fShadowHeight = fHeight; }
	D3DXCOLOR& GetColMuliti() { return m_ColMulti; }
	D3DXCOLOR& GetColAdd() { return m_AddCol; }

	// メンバ関数(設定)
	void SetPosition(const D3DXVECTOR3 pos);
	void SetPosOrigin(const D3DXVECTOR3 pos) { m_posOrigin = pos; m_pos = pos; }
	void SetScaleOrigin(const D3DXVECTOR3& scale) { m_scaleOrigin = scale; m_scale = scale; }
	void SetRotation(const D3DXVECTOR3 rot);
	void SetCurrentPosition(const D3DXVECTOR3 pos);
	void SetCurrentRotation(const D3DXVECTOR3 rot);
	void SetScale(const D3DXVECTOR3& scale) { m_scale = scale; }
	void SetDraw(bool bDraw = true) { m_bDraw = bDraw; }
	void SetShadow(bool bShadow = false) { m_bShadow = bShadow; }
	void SetMaterial(const D3DMATERIAL9& Material) { m_ChangeMat = Material; }
	void ChangeCol(bool bValue = false) { m_bChangeCol = bValue; }
	float GetShadowHeight() { return m_fShadowHeight; }
	void SetColAdd(const D3DXCOLOR& col) { m_AddCol = col; }
	void SetColMulti(const D3DXCOLOR& col) { m_ColMulti = col; }
	void SetEdit(bool bUse = false) { m_bEdit = bUse; }

private:	// 自分だけがアクセス可能

	// メンバ変数
	int m_nIdxModel;			// モデル番号
	bool m_bChangeCol;			// 色変更をするかどうか
	bool m_bDraw;
	bool m_bEdit;				// エディット扱い
	D3DMATERIAL9 m_ChangeMat;
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_posOrigin;	// 初期位置
	D3DXVECTOR3 m_rot;			// 向き
	D3DXVECTOR3 m_scale;		// スケール
	D3DXVECTOR3 m_scaleOrigin;	// 初期スケール
	D3DXVECTOR3 m_OldPos;		// 前回の位置
	D3DXVECTOR3 m_CurPos;		// 現在の位置
	D3DXVECTOR3 m_CurRot;		// 現在の向き
	D3DXVECTOR3 m_mtxpos;		// マトリックス座標
	D3DXCOLOR m_ColMulti;
	D3DXCOLOR m_AddCol;
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	D3DXMATRIX m_mtxscale;
	D3DXMATRIX *m_pParentMtx;	// 親のワールドマトリックス
	D3DXMATRIX *m_pCharacterMtx;	// まとめている親のマトリックス
	bool m_bShadow;		// 影も描画するか否か
	float m_fShadowHeight;	// 影の高さ
};

#endif
