//==========================================================
//
// 跳ね橋 [Bridge.h]
// Author : 丹野竜之介
//
//==========================================================
#ifndef _Bridge_H_		// このマクロが定義されていない場合
#define _Bridge_H_		// 二重インクルード防止用マクロを定義

#include "gimmick.h"
#include "objectX.h"
//定数
namespace
{
	const int BRIDGE_NUM = (2);//橋のパーツ数(両側一つずつなので当然2)
};
//==========================================================
// 跳ね橋のクラス定義
//==========================================================

class CBridge : public CGimmick
{

public:	// 誰でもアクセス可能

	CBridge();	// コンストラクタ(オーバーロード)
	~CBridge();	// デストラクタ

	// メンバ関数
	HRESULT Init(void)		override;
	void	Uninit(void)	override;
	void	Update(void)	override;
	void MoveBridge();
	static CBridge* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fdown,float fup);
private:	// 自分だけがアクセス可能

	// メンバ関数
	float m_fUpTime;//閉じてる時間
	float m_fDownTime;//空いてる時間
	float m_fTimeCount;//経過時間
	bool m_bPass;//通行可能か
	// メンバ変数
	CObjectX* m_pBridge[BRIDGE_NUM];
};

#endif
