//==========================================================
//
// エディット回転 [edit_arrow.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_ROTATE_H_		// このマクロが定義されていない場合
#define _EDIT_ROTATE_H_		// 二重インクルード防止用マクロを定義

#include "edit_handle.h"

//==========================================================
// サンプルのクラス定義
//==========================================================
class CEdit_Rotate : public CEdit_Handle
{
public:	// 誰でもアクセス可能

	CEdit_Rotate();	// コンストラクタ(オーバーロード)
	~CEdit_Rotate() override;	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void) override;
	static CEdit_Rotate* Create(const D3DXVECTOR3& pos);

private:	// 自分だけがアクセス可能

	void Rotation() override;
};

#endif

