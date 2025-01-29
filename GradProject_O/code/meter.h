//==========================================================
//
// メーター [meter.h]
// Author : 丹野竜之介
//
//==========================================================
#ifndef _Meter_H_		// このマクロが定義されていない場合
#define _Meter_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
#include "number.h"
#include "object2D.h"
#include "player.h"
#include "objectsound.h"

namespace
{
	const int NUMBER_NUM = (5);
	const int METER_NUM = (29);
	const int ANIM_COUNT = (180);
	const float ONE_METRE = (160.0f);
	D3DXVECTOR3 OFFSET_NUMBER = (D3DXVECTOR3(-60.0f, -14.0f, 0.0f));
	D3DXVECTOR3 INTERVAL_NUMBER = (D3DXVECTOR3(30.0f, 0.0f, 0.0f));
	D3DXVECTOR3 OFFSET_METER = (D3DXVECTOR3(0.0f, -120.0f, 0.0f));
	
}
//==========================================================
// サンプルのクラス定義
//==========================================================
class CMeter : public CTask
{

public:	// 誰でもアクセス可能

	CMeter();	// コンストラクタ(オーバーロード)
	~CMeter();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CMeter* Create(void);
	void BootAnimation();
	void Measure();;
	void Gage();
	// メンバ関数(取得)

	// メンバ関数(設定)

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

	CNumber*m_pNumber[NUMBER_NUM];
	CObject2D* m_pMeter[METER_NUM];
	CObject2D* m_pFrame;

	CObject2D* m_pNitroGage;
	CObject2D* m_pCircle;
	CObject2D* m_pInnerCircle;
	CMasterSound::CObjectSound * m_pSound;
	STATE m_state;
	int m_nStateCount;
};

#endif