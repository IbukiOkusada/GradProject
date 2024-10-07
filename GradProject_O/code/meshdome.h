//==========================================================
//
// メッシュドームの処理 [meshcylinder.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MESHDOME_H_	// このマクロが定義されていない場合
#define _MESHDOME_H_	// 二重インクルード防止用マクロを定義

#include "main.h"
#include "mesh.h"

//**********************************************************
// メッシュドームクラスの定義(派生クラス)
//**********************************************************
class CMeshDome : public CObjectMesh
{
public:		// 誰でもアクセス可能

	CMeshDome();	// コンストラクタ
	~CMeshDome();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtxInfo(void);
	void SetColor(float fDiff);
	static CMeshDome *Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, float fLength, float fHeight, const int nPriority = 3, const int nWidth = 1, const int nHeight = 1);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void SetSize(float fLength, float fHeight);

	// メンバ関数
	float m_fHeight;	// 高さ
	float m_fLength;	// 距離
	float m_fRot;		// 一頂点の角度
	float m_fHeightRot;	// y軸角度
};

#endif