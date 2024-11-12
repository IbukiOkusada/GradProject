//==========================================================
//
// 警察署ギミック [gimmick_policestation.h]
// Author : Ibuki Okusada
// 
//==========================================================
#ifndef _POLICESTATION_H_		// このマクロが定義されていない場合
#define _POLICESTATION_H_		// 二重インクルード防止用マクロを定義

#include "gimmick.h"

// 前方宣言
class CObjectX;
class CRoad;

//==========================================================
// 警察署のクラス定義
//==========================================================
class CGimmickPoliceStation : public CGimmick
{
private:

	struct SInfo
	{
		float fInterVal;	// インターバル
		float fSpawnTime;	// 生成タイミング
		CRoad* pRoad;		// 正面の道

		// コンストラクタ
		SInfo() : fInterVal(0.0f), fSpawnTime(0.0f), pRoad(nullptr) {}
	};

public:	// 誰でもアクセス可能

	CGimmickPoliceStation(); // コンストラクタ(オーバーロード)
	~CGimmickPoliceStation() override;	// デストラクタ

	// メンバ関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	static CGimmickPoliceStation* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale);
	void SetColMulti(const D3DXCOLOR& col) override;

private:	// 自分だけがアクセス可能

	// メンバ関数
	virtual void SetObjPos(const D3DXVECTOR3& pos) override;
	virtual void SetObjRot(const D3DXVECTOR3& rot) override;
	virtual void SetObjScale(const D3DXVECTOR3& scale) override;

	// メンバ変数
	CObjectX* m_pObj;			// 描画オブジェクト
	SInfo m_Info;
};

#endif
