//==========================================================
//
// 道処理 [road.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _ROAD_H_		// このマクロが定義されていない場合
#define _ROAD_H_		// 二重インクルード防止用マクロを定義

#include "task.h"

// 前方宣言
class CObject3D;

//==========================================================
// 道のクラス定義
//==========================================================
class CRoad : public CTask
{
public:

	// 道連結情報
	enum DIRECTION
	{
		DIC_UP = 0,	// 上
		DIC_DOWN,	// 下
		DIC_LEFT,	// 左
		DIC_RIGHT,	// 右
		DIC_MAX
	};

private:

	// 基本情報構造体
	struct SInfo
	{
		D3DXVECTOR3 pos;	// 座標
		D3DXVECTOR3 rot;	// 向き
		D3DXVECTOR2 size;	// 幅

		// コンストラクタ
		SInfo() : pos({ 0.0f, 0.0f, 0.0f }), rot({ 0.0f, 0.0f, 0.0f }), size({ 0.0f, 0.0f }) {}
		SInfo(const D3DXVECTOR3& _pos, const D3DXVECTOR3& _rot, const D3DXVECTOR2& _size) : pos(_pos), rot(_rot), size(_size) {}
	};

public:	// 誰でもアクセス可能

	//CRoad();	// コンストラクタ(オーバーロード)
	CRoad(const SInfo& info = SInfo());
	~CRoad();	// デストラクタ

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CRoad* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR2& size);

	// メンバ関数(取得)
	CRoad* GetNext(void) { return m_pNext; }
	CRoad* GetPrev(void) { return m_pPrev; }

	// メンバ関数(設定)
	void Connect(CRoad* pRoad, const DIRECTION dic);
	void SetNext(CRoad* pNext) { m_pNext = pNext; }
	void SetPrev(CRoad* pPrev) { m_pPrev = pPrev; }

private:	// 自分だけがアクセス可能

	// メンバ関数

	// メンバ変数
	CRoad* m_pPrev;			// 前のオブジェクトへのポインタ
	CRoad* m_pNext;			// 次のオブジェクトへのポインタ
	SInfo m_Info;	// 基本情報
	CObject3D* m_pObj;
	CRoad* m_apConnectRoad[DIRECTION::DIC_MAX];	// 連結した道
};

#endif
