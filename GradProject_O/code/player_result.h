//<=================================================
//タイトルでのプレイヤ―の動き処理 [player_result.h]
//
//Author : Kenta Hashimoto
//<=================================================
#ifndef _PLAYER_RESULT_H_
#define _PLAYER_RESULT_H_

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
class CPlayerResult : public CPlayer
{
public:

	CPlayerResult();
	~CPlayerResult();

	// メンバ関数
	HRESULT Init(void);
	HRESULT Init(const char* pBodyName, const char* pLegName);	// オーバーロード
	void Uninit(void);
	void Update(void);
	static CPlayerResult* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move, const char* pBodyName, const char* pLegName);

	void Moving();

private:


};

#endif
