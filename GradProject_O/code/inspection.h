//==========================================================
//
// 検問処理 [instpection.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _INSTPECTION_H_		// このマクロが定義されていない場合
#define _INSTPECTION_H_		// 二重インクルード防止用マクロを定義

#include "task.h"

// 前方宣言
class CAddPolice;
class CGimmickPoliceStation;
class CRoad;

//==========================================================
// 追加警察のクラス定義
//==========================================================
class CInstpection : public CTask
{
private:

	// 出動警察種類
	enum TYPE
	{
		TYPE_SEARCH_L = 0,	// 左側監視
		TYPE_SEARCH_R,		// 右側監視
		TYPE_MAX
	};

	// 基本情報構造体
	struct SInfo
	{
		D3DXVECTOR3 pos;	// 座標
		D3DXVECTOR3 rot;	// 向き

		// コンストラクタ
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO) {}
	};

	// 出勤パトカー情報
	struct SPoliceInfo
	{
		CAddPolice* pPolice;	// 警察クラスのポインタ
		D3DXVECTOR3 goalpos;	// ゴール地点の座標
		D3DXVECTOR3 goalrot;	// ゴール地点の座標
		D3DXVECTOR3 startpos;	// 開始地点の座標
		float fTimer;			// 移動タイマー

		// コンストラクタ
		SPoliceInfo() : pPolice(nullptr), goalpos(VECTOR3_ZERO), goalrot(VECTOR3_ZERO), startpos(VECTOR3_ZERO), fTimer(0.0f) {}
	};

	// 検問線情報
	struct SLagerInfo
	{
		float fTimer;			// タイマー
		float fRotateTimer;		// 回転タイマー
		bool bHit;				// 衝突判定

		// コンストラクタ
		SLagerInfo() : fTimer(0.0f), fRotateTimer(0.0f), bHit(false) {}
	};


public:	// 誰でもアクセス可能

	CInstpection();	// コンストラクタ(オーバーロード)
	~CInstpection() override;	// デストラクタ

	// メンバ関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;

	// 静的メンバ関数
	static CInstpection* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, CRoad* pRoad);

	// メンバ関数(取得)
	D3DXVECTOR3& GetPosition() { return m_Info.pos; }
	D3DXVECTOR3& GetRotation() { return m_Info.rot; }

	// メンバ関数(設定)
	void SetPosition(const D3DXVECTOR3& pos) { m_Info.pos = pos; }
	void SetRotation(const D3DXVECTOR3& rot) { m_Info.rot = rot; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	void Away();
	void LagerSet();
	void LagerSetRotation();

	// メンバ変数
	SInfo m_Info;			// 基本情報
	CRoad* m_pRoad;			// 設置する道
	CGimmickPoliceStation* m_pNearStation;	// 近い警察署
	SPoliceInfo m_aPoliceInfo[TYPE_MAX];	// 警察情報
	SLagerInfo m_LagerInfo;			// 検問線情報
};

#endif