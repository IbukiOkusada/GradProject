//==========================================================
//
// メッシュウォールの処理 [meshwall.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MESHWALL_H_	// このマクロが定義されていない場合
#define _MESHWALL_H_	// 二重インクルード防止用マクロを定義

#include "main.h"
#include "mesh.h"
#include "objectX.h"

//**********************************************************
// メッシュウォールクラスの定義(派生クラス)
//**********************************************************
class CMeshWall : public CObjectMesh
{
public:		// 誰でもアクセス可能

	CMeshWall();	// コンストラクタ
	~CMeshWall();	// デストラクタ

	// 壁種類列挙型
	enum TYPE {
		TYPE_NORMAL = 0,	// 通常
		TYPE_DUSH,			// 壁走り可能
		TYPE_MAX
	};

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtxInfo(void);
	void SceltonWall(void);
	static CMeshWall *Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot,
		float fWidth, float fHeight, const char *pFileName, const int nPriority = 4, const int nWidth = 1, const int nHeight = 1);

	// メンバ関数(取得)
	float GetWidth(void) { return m_fWidth; }
	float GetHeight(void) { return m_fHeight; }

	// メンバ関数(設定)
	void SetSize(float fWidth, float fHeight);
	void SetType(const int type);
	static D3DXVECTOR3 Collision(D3DXVECTOR3 &pos, D3DXVECTOR3 &posOld, D3DXVECTOR3& move, const D3DXVECTOR3& vtxMax, const D3DXVECTOR3& vtxMin, CObjectX::COLLISION_AXIS &axis, int &nType);

private:	// 自分だけがアクセス可能

	// メンバ関数
	static CMeshWall *m_pTop;	// 先頭のオブジェクトへのポインタ
	static CMeshWall *m_pCur;	// 最後尾のオブジェクトへのポインタ
	CMeshWall *m_pPrev;	// 前のオブジェクトへのポインタ
	CMeshWall *m_pNext;	// 次のオブジェクトへのポインタ
	float m_fWidth;		// 幅
	float m_fHeight;	// 高さ
	int m_type;		// 種類
};

#endif
