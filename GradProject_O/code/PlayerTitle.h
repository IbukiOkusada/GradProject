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
class CTitleBaggage;
class CTitleGoal;
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
	void BaggageMove(void);

	//到着情報に関するSet・Get関数
	void SetReached(bool bReach) { m_bReached = bReach; }
	bool GetReached(void) {return m_bReached;}

	//ゴール情報を取得
	CTitleGoal* GetTitleGoal(void) { return m_pTitleGoal; }

private:

	float m_fBDustValue;				//煙の大きさの値
	bool m_bNextMove;					//次の動きに移行するときになったら
	STATE m_eState;						//デバッグ用のステート
	bool m_bReached;					//着いたかどうか
	CTitleBaggage* m_pTitleBaggage;		//タイトル用の荷物
	CTitleGoal *m_pTitleGoal;			//タイトル用のゴール


};

#endif
