//<=================================================
//タイトルでのプレイヤ―の動き処理 [PlayerTitle.h]
//
//Author : Kazuki Watanabe
//<=================================================
#ifndef _PLAYERTITLE_H_
#define _PLAYERTITLE_H_

#include "player.h"

//<========================================
//プレイヤークラスを継承して定義
//<========================================
class CPlayerTitle: public CPlayer
{
public:

	//
	enum STATE
	{

		STATE_NONE=0,
		STATE_DEBUG,
		STATE_MAX
	};


	CPlayerTitle();
	~CPlayerTitle();

	// メンバ関数
	HRESULT Init(void);
	HRESULT Init(const char* pBodyName, const char* pLegName);	// オーバーロード
	void Uninit(void);
	void Update(void);
	static CPlayerTitle* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move, const char* pBodyName, const char* pLegName);

private:

	void Moving(void);

	STATE m_eState;				//デバッグ用のステート
	bool m_bReached;			//着いたかどうか

};

#endif
