//==========================================================
//
// エディット矢印 [edit_arrow.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_ARROW_H_		// このマクロが定義されていない場合
#define _EDIT_ARROW_H_		// 二重インクルード防止用マクロを定義

#include "edit_handle.h"

//==========================================================
// サンプルのクラス定義
//==========================================================
class CEdit_Arrow : public CEdit_Handle
{
public:	// 誰でもアクセス可能

	CEdit_Arrow();	// コンストラクタ(オーバーロード)
	~CEdit_Arrow() override;	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void) override;
	static CEdit_Arrow* Create(const D3DXVECTOR3& pos);

private:	// 自分だけがアクセス可能

	void Move() override;
};

#endif
