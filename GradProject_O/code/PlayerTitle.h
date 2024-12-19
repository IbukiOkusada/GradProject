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
	bool BoolDis(const D3DXVECTOR3 Pos, const D3DXVECTOR3 DestPos, const float Distance);
}
//前方宣言
class CTitleBaggage;
class CTitleGoal;
class CCharacter;
//<========================================
//プレイヤークラスを継承して定義
//<========================================
class CPlayerTitle: public CPlayer
{
public:

	//ステート
	const enum STATE
	{
		STATE_NONE=0,	//通常の動き
		STATE_DEBUG,	//位置調整などのデバッグ時
		STATE_MAX
	};


	CPlayerTitle();
	~CPlayerTitle();

	// メンバ関数
	HRESULT Init(void) { CPlayer::Init(); return S_OK; }
	HRESULT Init(const char* pBodyName, const char* pLegName);	// オーバーロード
	void Uninit(void);
	void Update(void);
	static CPlayerTitle* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move, const char* pBodyName, const char* pLegName);

	//動きに関する処理
	void Moving(const int nNum);
	void MovingSelect(void);
	void BaggageMove(void);

	//到着情報に関するSet・Get関数
	inline void SetReached(bool bReach) { m_bReached = bReach; }
	inline bool GetReached(void) {return m_bReached;}
	inline int GetNumDest(void) {}

	//ゴール情報を取得
	inline CTitleGoal* GetTitleGoal(void) { return m_pTitleGoal; }

	//サウンド情報設定
	inline void SetS(const bool bStart)
	{
		//開始なら再生、していないなら止める
		if (bStart) { m_pSound->Start(); }
		else { m_pSound->Stop(); }
	}

private:

	//プレイヤーの向き設定
	void PlayerRotSet(void);

	int m_nNumDest;						//目的地の番号
	float m_fBDustValue;				//煙の大きさの値
	bool m_bNextMove;					//次の動きに移行するときになったら
	bool m_bFirst;						//最初の位置
	bool m_bReached;					//着いたかどうか
	STATE m_eState;						//デバッグ用のステート
	CTitleBaggage* m_pTitleBaggage;		//タイトル用の荷物
	CTitleGoal *m_pTitleGoal;			//タイトル用のゴール
};

#endif
