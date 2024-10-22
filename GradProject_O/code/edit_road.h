//==========================================================
//
// エディット道 [edit_road.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_ROAD_H_		// このマクロが定義されていない場合
#define _EDIT_ROAD_H_		// 二重インクルード防止用マクロを定義

#include "edit.h"

// 前方宣言
class CRoad;

//==========================================================
// サンプルのクラス定義
//==========================================================
class CEdit_Road : public CEdit
{
public:	// 誰でもアクセス可能

	CEdit_Road();	// コンストラクタ(オーバーロード)
	~CEdit_Road();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void) override;
	virtual void Uninit(void);
	virtual void Update(void) override;

	// メンバ関数(取得)

	// メンバ関数(設定)

private:	// 自分だけがアクセス可能

	// メンバ関数
	void ClickCheck();
	bool CursorCollision(CRoad* pRoad);
	bool TriangleCollision(const D3DXVECTOR3& rayOrigin, const D3DXVECTOR3& rayDir,
		const D3DXVECTOR3& pos0, const D3DXVECTOR3& pos1, const D3DXVECTOR3& pos2);

	// メンバ変数
	CRoad* m_pSelectRoad;
};

#endif


