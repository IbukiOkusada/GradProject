//==========================================================
//
// メーター [Container.h]
// Author : 丹野竜之介
//
//==========================================================
#ifndef _Container_H_		// このマクロが定義されていない場合
#define _Container_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
#include "number.h"
#include "object2D.h"
#include "player.h"

namespace
{
	const int ICE_NUM = (4);
	const int LOVE_NUM = (2);
	const D3DXVECTOR3 CONTAINER_POS = (D3DXVECTOR3(1100.0f, 580.0f, 0.0f));
	const D3DXVECTOR3 ICE_POS = (D3DXVECTOR3(-60.0f, -35.0f, 0.0f));
	const D3DXVECTOR3 OFFSET_ICE = (D3DXVECTOR3(120.0f, 70.0f, 0.0f));
	const D3DXVECTOR3 OFFSET_LOVE = (D3DXVECTOR3(0.0f, 80.0f, 0.0f));
}
//==========================================================
// サンプルのクラス定義
//==========================================================
class CContainer : public CTask
{

public:	// 誰でもアクセス可能

	CContainer();	// コンストラクタ(オーバーロード)
	~CContainer();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CContainer* Create(void);
	void BootAnimation();

	// メンバ関数(取得)

	// メンバ関数(設定)

	struct SIce
	{
		D3DXVECTOR3 pos;
		float fTime;
		CObject2D* pIce;
	};
private:	// 自分だけがアクセス可能

	// メンバ関数
	enum STATE
	{
		STATE_NONE = 0,
		STATE_BOOT,
		STATE_NORMAL,
		STATE_MAX
	};
	// メンバ変数
	D3DXVECTOR3 m_pos;


	CObject2D* m_pContainer;
	CObject2D* m_pLove[2];
	SIce* m_pIce[ICE_NUM];
	
	STATE m_state;
	int m_nStateCount;
	float m_fScroll;
};

#endif