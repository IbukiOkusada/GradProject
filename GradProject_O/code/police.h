//==========================================================
//
// パトカー処理 [police.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _POLICE_H_		// このマクロが定義されていない場合
#define _POLICE_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "object.h"
#include "objectX.h"
#include "task.h"
#include "car.h"
#include "player.h"
#include <list>

// 前方宣言
class CRoad;

//==========================================================
// サンプルのクラス定義
//==========================================================
class CPolice : public CCar
{
private:	// 自分だけがアクセス可能

	// 情報構造体
	struct SInfo
	{
		CPlayer* pPlayer;
		int nChaseCount;
		bool bChase;
	};

public:	// 誰でもアクセス可能

	CPolice();	// コンストラクタ(オーバーロード)
	~CPolice();	// デストラクタ

	// メンバ関数
	HRESULT Init(void) override;
	void Uninit(void);
	void Update(void);
	static CPolice*Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move);

	// メンバ関数(取得)

	// メンバ関数(設定)

private:	// 自分だけがアクセス可能

	// メンバ関数
	void MoveRoad() override;
	void ReachRoad() override;
	void ChasePlayer();
	void SearchPlayer();
	void Collision();

	// メンバ変数
	SInfo m_Info;					// 自分自身の情報
	std::list<CRoad*> listRoad;		// 追跡用リスト
	std::list<CRoad*> listChase;	// 追跡用リスト

	CEffekseer::CEffectData* m_pPatrolLamp;
};

#endif
