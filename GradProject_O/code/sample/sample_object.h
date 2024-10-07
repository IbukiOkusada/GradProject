//==========================================================
//
// サンプルオブジェクト [sample.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _SAMPLEOBJECT_H_		// このマクロが定義されていない場合
#define _SAMPLEOBJECT_H_		// 二重インクルード防止用マクロを定義

#include "../object.h"
//#include "object.h"	// これでファイルインクルードできます

//==========================================================
// サンプルのクラス定義
//==========================================================
class CSampleObject : public CObject
{

public:	// 誰でもアクセス可能

	//CSample();	// コンストラクタ
	CSampleObject(int nPriOrity = 3);	// コンストラクタ(オーバーロード)
	~CSampleObject();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Draw(void);
	static CSampleObject *Create(void);

	// メンバ関数(取得)
	

	// メンバ関数(設定)

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
};

#endif

