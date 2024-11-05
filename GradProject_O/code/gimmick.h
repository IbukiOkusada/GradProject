//==========================================================
//
// ギミック [Gimmick.h]
// Author : 丹野竜之介
//
//==========================================================
#ifndef _Gimmick_H_		// このマクロが定義されていない場合
#define _Gimmick_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
//#include "task.h"	// これでファイルインクルードできます

//==========================================================
// サンプルのクラス定義
//==========================================================
class CGimmick : public CTask
{

public:	// 誰でもアクセス可能

	CGimmick();	// コンストラクタ(オーバーロード)
	~CGimmick();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void)= 0;
	virtual void Update(void)= 0;

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_rot;
	D3DXVECTOR3 m_scale;

};

#endif

