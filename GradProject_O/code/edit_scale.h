//==========================================================
//
// エディットスケール [edit_scale.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_SCALE_H_		// このマクロが定義されていない場合
#define _EDIT_SCALE_H_		// 二重インクルード防止用マクロを定義

#include "edit_handle.h"

//==========================================================
// サンプルのクラス定義
//==========================================================
class CEdit_Scale : public CEdit_Handle
{
public:	// 誰でもアクセス可能

	CEdit_Scale();	// コンストラクタ(オーバーロード)
	~CEdit_Scale() override;	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void) override;

private:	// 自分だけがアクセス可能

	void Scale() override;
};

#endif
