//==========================================================
//
// ナビ [navi.h]
// Author : 丹野竜之介
//
//==========================================================
#ifndef _Navi_H_		// このマクロが定義されていない場合
#define _Navi_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
#include "camera.h"
#include "road.h"
#include "effekseerControl.h"
#include "list.h"
#include "goal.h"
#include "scrollText2D.h"
#include "objectsound.h"
//#include "task.h"	// これでファイルインクルードできます

//==========================================================
// サンプルのクラス定義
//==========================================================
class CNavi : public CTask
{

public:	// 誰でもアクセス可能

	CNavi();	// コンストラクタ(オーバーロード)
	~CNavi();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CNavi* Create(void);

	// メンバ関数(取得)

	// メンバ関数(設定)
	struct SEffect
	{
		CRoad::SSearch* pTarget;
		CEffekseer::CEffectData* pLine;
	
		SEffect():pTarget(nullptr), pLine(nullptr){}
	};
private:	// 自分だけがアクセス可能

	// メンバ関数
	void StartNavigation();
	void UpdateNavigation();
	void Reach();
	void CreateEffect();
	void clear();
	// メンバ変数
	CGoal* m_pGole;
	int m_IdxPath;
	int m_nTime;
	std::vector<CRoad::SSearch*> m_Path;
	Clist<SEffect*> m_Effects;
	CScrollText2D* m_pFont;
	CMasterSound::CObjectSound* m_pSound;
	int m_nFontTimeCount;
};

#endif
