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
class CObjectX;

//==========================================================
// 追加警察のクラス定義
//==========================================================
class CInstpection : public CTask
{
private:


	// 基本情報構造体
	struct SInfo
	{
		D3DXVECTOR3 pos;	// 座標
		D3DXVECTOR3 rot;	// 向き

		// コンストラクタ
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO) {}
	};

	// fence用情報
	struct SGuardInfo
	{
		float fTimer;	// 遷移タイマー
		D3DXVECTOR3 targetpos;	// 目標座標
		D3DXVECTOR3 targetrot;	// 目標向き
		D3DXVECTOR3 startpos;	// 開始座標
		D3DXVECTOR3 startrot;	// 開始向き
		CObjectX* pObj;	// 描画オブジェクト

		// コンストラクタ
		SGuardInfo() : fTimer(0.0f), targetpos(VECTOR3_ZERO), 
			targetrot(VECTOR3_ZERO), startpos(VECTOR3_ZERO), startrot(VECTOR3_ZERO), pObj(nullptr) {}
	};

public:	// 誰でもアクセス可能

	CInstpection();	// コンストラクタ(オーバーロード)
	~CInstpection() override;	// デストラクタ

	// メンバ関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;

	// 静的メンバ関数
	static CInstpection* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);

	// メンバ関数(取得)
	D3DXVECTOR3& GetPosition() { return m_Info.pos; }
	D3DXVECTOR3& GetRotation() { return m_Info.rot; }

	// メンバ関数(設定)
	void SetPosition(const D3DXVECTOR3& pos) { m_Info.pos = pos; }
	void SetRotation(const D3DXVECTOR3& rot) { m_Info.rot = rot; }

private:	// 自分だけがアクセス可能

	// メンバ関数
	void Away();

	// メンバ変数
	CAddPolice* m_pPolice;	// 検問場所にいる警察
	SGuardInfo m_Guard;		// フェンス用
	SInfo m_Info;			// 基本情報
	bool m_bHit;			// 一度でも衝突した
};

#endif