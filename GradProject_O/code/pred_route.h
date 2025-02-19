//==========================================================
//
// 次の道予測処理 [pred_route.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PRED_ROUTE_H_		// このマクロが定義されていない場合
#define _PRED_ROUTE_H_		// 二重インクルード防止用マクロを定義

#include "task.h"

// 前方宣言
class CPlayer;
class CRoad;

//==========================================================
// 予測処理のクラス定義
//==========================================================
class CPredRoute : public CTask
{
public:

	// 方向
	enum VEC
	{
		VEC_STRAIGHT = 0,	// 直進
		VEC_TURNLEFT,		// 左曲がり
		VEC_TURNRIGHT,		// 右曲がり
		VEC_TURNING,		// 転回
		VEC_OVER,			// 道を出た
		VEC_STOP,			// 停止している
		VEC_MAX
	};

public:	// 誰でもアクセス可能

	CPredRoute();	// コンストラクタ(オーバーロード)
	~CPredRoute() override;	// デストラクタ

	// メンバ関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	static CPredRoute* Create(CPlayer* pPlayer);

	// メンバ関数(取得)
	float GetNextRot(const VEC& vec);
	CRoad* GetPredRoad() { return m_pPredRoad; }
	CRoad* GetPredPrevRoad() { return m_pPredPrevRoad; }
	VEC GetSelectRankVec(const int nRank = 0);

	// メンバ関数(設定)

private:	// 自分だけがアクセス可能

	// メンバ関数
	void Interp();
	void Pred();
	void TurnCheck();
	float GetRot(const VEC& vec);

	// メンバ変数
	CRoad* m_pOldRoad;				// 前回の道
	std::vector<CRoad*> m_PassRoad;	// 通過した道
	CPlayer* m_pPlayer;				// 確認するプレイヤー
	int m_aTurnCount[VEC_MAX];		// ターン数カウント
	float m_fStopCount;				// 停止カウント
	VEC m_vecOld;					// 前回の回転方向
	CRoad* m_pPredRoad;				// 予測地点
	CRoad* m_pPredPrevRoad;			// 予測地点のひとつ前
};

#endif