//==========================================================
//
// 追加パトカー処理 [add_police.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _ADD_POLICE_H_		// このマクロが定義されていない場合
#define _ADD_POLICE_H_		// 二重インクルード防止用マクロを定義

#include "main.h"
#include "task.h"
#include "police.h"
#include "player.h"
#include "list.h"

// 前方宣言
class CRoad;

//==========================================================
// 追加警察のクラス定義
//==========================================================
class CAddPolice : public CPolice
{
public:	// 誰でもアクセス可能

	CAddPolice(int nId);	// コンストラクタ(オーバーロード)
	~CAddPolice() override;	// デストラクタ

	// メンバ関数
	static CAddPolice* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& move, int nId);
	static Clist<CAddPolice*>* GetList() { return &m_List; }

	// 設定関数
	void SetNavi(std::vector<CRoad::SSearch*>& navi) { m_Path = navi; }

private:	// 自分だけがアクセス可能

	//=============================
	// メンバ関数
	//=============================
	// 状態関数
	void StateNormal() override;	// 通常
	void StateSearch() override;	// 警戒
	void StateFadeOut() override;	// フェードアウト

	void SearchRoad() override;
	void ReachRoad() override;
	void SendPosition() override;

	// 状態設定関数
	virtual void SetStateNormal() override;		// 通常
	virtual void SetStateSearch() override;		// 警戒
	virtual void SetStateFadeOut() override;	// フェードアウト

	// メンバ関数
	D3DXVECTOR3 m_SpawnPos;
	std::vector<CRoad::SSearch*> m_Path;
	int m_nNowRoad;
	static Clist<CAddPolice*> m_List;
};

#endif