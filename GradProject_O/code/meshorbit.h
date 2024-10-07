//==========================================================
//
//メッシュ構造の軌跡処理 [meshorbit.h]
//Author Ibuki Okusada
//
//==========================================================
#ifndef _MESHORBIT_H_
#define _MESHORBIT_H_

#include "mesh.h"

// マクロ定義
#define ORBIT_EDGE	(2)	// 用意する端の数

// 軌跡クラスの定義(派生クラス)
class CMeshOrbit : public CObjectMesh
{
public:		// 誰でもアクセス可能

	// 軌跡タイプ
	enum TYPE
	{
		TYPE_NONE = 0,		// 何もない
		TYPE_PLAYER,		// プレイヤー
		TYPE_BULLET,		// 弾
		TYPE_MAX
	};

	CMeshOrbit();	// コンストラクタ
	~CMeshOrbit();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);	// 初期化処理
	void Uninit(void);	// 終了処理
	void Update(void);	// 更新処理
	void Draw(void);	// 描画処理
	static CMeshOrbit *Create(D3DXMATRIX *pMtx, D3DXVECTOR3 UpSet, D3DXVECTOR3 DownSet, TYPE type = TYPE_NONE);

	// メンバ関数(設定)
	void SetVtxInfo(void);
	void SetParent(D3DXMATRIX *pMtx) { m_pMtxParent = pMtx; }
	void SetOffSet(D3DXVECTOR3 Up, D3DXVECTOR3 Down) { m_aOffSet[0] = Up; m_aOffSet[1] = Down; }
	static int Get(void) { return m_nNumAll; }
	void Reset(void);
	void SetNor(D3DXVECTOR3 nor) { m_nor = nor; }
	void SetCol(D3DXCOLOR col);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void SetPlayer(void);
	void SetBullet(void);
	void SetNone(void);

	// メンバ変数
	D3DXMATRIX *m_pMtxParent;			// 親のマトリックスのポインタ
	D3DXVECTOR3 m_aOffSet[ORBIT_EDGE];	// 両端のオフセット
	D3DXCOLOR m_aCol[ORBIT_EDGE];		// 両端の基準色
	D3DXVECTOR3 *m_pPos;				// 位置
	D3DXCOLOR *m_pCol;					// 色
	D3DXVECTOR3 m_rot;					// 向き
	D3DXVECTOR3 m_nor;					// 移動方向
	float m_fTimer;						// タイマー
	static int m_nNumAll;				// 総数
	float m_fRadius;					// 半径
	TYPE m_type;						// 種類
	D3DXMATRIX m_aMtxWorldPoint[ORBIT_EDGE];	// 両端のワールドマトリックス
};

#endif
