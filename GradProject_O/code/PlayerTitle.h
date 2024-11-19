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
	//インライン関数使用・目的地に着いた時の判定
	inline bool MoveToDest(const D3DXVECTOR3 Pos, const D3DXVECTOR3 DestPos, const float Distance,bool bZuse = true)
	{
		//Z軸を使うなら
		if (bZuse)
		{
			//目的の位置についたら
			if (Pos.x >= DestPos.x + Distance && Pos.x <= DestPos.x - Distance
				|| Pos.x <= DestPos.x + Distance && Pos.x >= DestPos.x - Distance
				&& Pos.z >= DestPos.z + Distance && Pos.z <= DestPos.z - Distance
				|| Pos.z <= DestPos.z + Distance && Pos.z >= DestPos.z - Distance)
			{
				return true;
			}

		}
		//Z軸を使わないなら
		else
		{
			//目的の位置についたら
			if (Pos.x >= DestPos.x + Distance && Pos.x <= DestPos.x - Distance
				|| Pos.x <= DestPos.x + Distance && Pos.x >= DestPos.x - Distance)
			{
				return true;
			}

		}



		return false;
	}
}

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

	bool GetReached(void) {return m_bReached;}

private:

	void Moving(void);

	STATE m_eState;				//デバッグ用のステート
	bool m_bReached;			//着いたかどうか


};

#endif
