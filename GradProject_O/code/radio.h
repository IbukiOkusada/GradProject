//==========================================================
//
// ラジオ [Radio.h]
// Author : 丹野竜之介
//
//==========================================================
#ifndef _Radio_H_		// このマクロが定義されていない場合
#define _Radio_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "objectsound.h"
#include "list.h"
#include "convenience.h"
//==========================================================
// サンプルのクラス定義
//==========================================================
class CRadio
{

public:	// 誰でもアクセス可能

	CRadio();	// コンストラクタ(オーバーロード)
	~CRadio();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CRadio* Create(void);
	void VolumeChange();
	void ChannelChange();
	enum CHANNEL
	{
		CHANNEL_RELAX = 0,
		CHANNEL_METAL,
		CHANNEL_TECHNO,
		CHANNEL_POPS,
		CHANNEL_SHINOBI,
		CHANNEL_MAX
	};
	struct radio
	{
		Clist<CMasterSound::CObjectSound*>* m_pList;
		int nCurrent;
	};
private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	float m_fVolume;
	int m_nChannel;
	bool m_bSwitch;
	radio m_pRadio[CHANNEL_MAX];
	CMasterSound::CObjectSound* m_pSE;
	float m_fFade;
};

#endif

