//<=================================================
//タイトルでのプレイヤ―の動き処理 [PlayerTitle.h]
//
//Author : Kazuki Watanabe
//<=================================================
#ifndef _PLAYERTITLE_H_
#define _PLAYERTITLE_H_

#include "player.h"

//<***************************************
//タイトル関連で使う関数
//<***************************************
namespace Function
{
	//目的地に着いた時の判定
	bool BoolToDest(const D3DXVECTOR3 Pos, const D3DXVECTOR3 DestPos, const float Distance, bool bZuse = true);
}

//<========================================
//プレイヤークラスを継承して定義
//<========================================
class CPlayerTitle: public CPlayer
{
public:

	//ステート
	enum STATE
	{

		STATE_NONE=0,	//通常の動き
		STATE_DEBUG,	//位置調整などのデバッグ時
		STATE_MAX
	};

	//目的地ナンバー
	enum DEST
	{
		DEST_FIRST = 0,	//一番目
		DEST_SECOND,	//二番目
		DEST_THIRD,		//三番目
		DEST_FOURTH,	//四番目
		DEST_MAX,

	};


	CPlayerTitle();
	~CPlayerTitle();

	// メンバ関数
	HRESULT Init(void);
	HRESULT Init(const char* pBodyName, const char* pLegName);	// オーバーロード
	void Uninit(void);
	void Update(void);
	static CPlayerTitle* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move, const char* pBodyName, const char* pLegName);

	void Moving(const int nNum);

	//到着情報に関するSet・Get関数
	bool SetReached(bool bReach) { m_bReached = bReach; }
	bool GetReached(void) {return m_bReached;}

private:

	STATE m_eState;				//デバッグ用のステート
	bool m_bReached;			//着いたかどうか


};

#endif
