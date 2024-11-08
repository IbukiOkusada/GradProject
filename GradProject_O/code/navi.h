//==========================================================
//
// ナビ [navi.h]
// Author : 丹野竜之介
//
//==========================================================
#ifndef _Navi_H_		// このマクロが定義されていない場合
#define _Navi_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
#include "camera.h"
//#include "task.h"	// これでファイルインクルードできます

//==========================================================
// サンプルのクラス定義
//==========================================================
class CNavi : public CTask
{

public:	// 誰でもアクセス可能

	CNavi();	// コンストラクタ(オーバーロード)
	~CNavi();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CNavi* Create(void);

	// メンバ関数(取得)

	// メンバ関数(設定)

private:	// 自分だけがアクセス可能

	// メンバ関数
	
	// メンバ変数
	CMultiCamera* m_pCamera;
};

#endif
