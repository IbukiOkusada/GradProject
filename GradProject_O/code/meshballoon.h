//==========================================================
//
// シャボン玉の処理 [meshballoon.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MESH_BALLOON_H_		// このマクロが定義されていない場合
#define _MESH_BALLOON_H_		// 二重インクルード防止用マクロを定義

#include "mesh.h"

//==========================================================
// サンプルのクラス定義
//==========================================================
class CMeshBalloon : public CObjectMesh
{
public:	// 誰でもアクセス可能

	//CMeshBalloon();	// コンストラクタ
	CMeshBalloon(int nPriOrity = 4);	// コンストラクタ(オーバーロード)
	~CMeshBalloon();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtxInfo(void);
	void SetColor(float fDiff);
	static CMeshBalloon *Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, float fLength, const int nPriority = 3, const int nWidth = 1, const int nHeight = 1);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void SetSize(float fLength);

	// メンバ変数
	float m_fLength;	// 距離
	float m_fRot;		// 一頂点の角度
	float m_fHeightRot;	// y軸角度
};

#endif

