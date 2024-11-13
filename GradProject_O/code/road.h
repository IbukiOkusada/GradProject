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
public:	// 誰でもアクセス可能な定義

	// 道連結情報
	enum DIRECTION
	{
		DIC_UP = 0,	// 上
		DIC_DOWN,	// 下
		DIC_LEFT,	// 左
		DIC_RIGHT,	// 右
		DIC_MAX
	};

	// 道種類
	enum TYPE
	{
		TYPE_STOP = 0,		// 停止中
		TYPE_NONE,			// まっすぐ道
		TYPE_CURVE,			// カーブ
		TYPE_T_JUNCTION,	// T字路
		TYPE_CROSSING,		// 交差点
		TYPE_MAX
	};

	// 基本情報構造体
	struct SInfo
	{
		D3DXVECTOR3 pos;	// 座標
		D3DXVECTOR3 rot;	// 向き
		D3DXVECTOR2 size;	// 幅
		D3DXVECTOR2 sizeOrigin;	// 幅

		// コンストラクタ
		SInfo() : pos({ 0.0f, 0.0f, 0.0f }), rot({ 0.0f, 0.0f, 0.0f }), size({ 0.0f, 0.0f }), sizeOrigin(size) {}
		SInfo(const D3DXVECTOR3& _pos, const D3DXVECTOR3& _rot, const D3DXVECTOR2& _size) : pos(_pos), rot(_rot), size(_size), sizeOrigin(size) {}
	};

	// 経路探索用
	struct SSearch
	{
		D3DXVECTOR3 pos;	// 座標
		CRoad* pRoad;		// 確認する道
		bool bActive;		// 使用されている情報かどうか
		float fFCost;		//最終コスト
		float fGCost;		//移動コスト
		float fHCost;		//予想コスト
		SSearch* pParent;
		// コンストラクタ
		SSearch() : pos(VECTOR3_ZERO), fFCost(0.0f), fGCost(0.0f), fHCost(0.0f), pRoad(nullptr), pParent(nullptr), bActive(false) {}
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
	void BindTexture();

	// メンバ関数(取得)
	CObject3D* GetObj(void) { return m_pObj; }	// 描画オブジェ
	SInfo* GetInfo(void) { return &m_Info; }		// 基本情報
	D3DXVECTOR3& GetPosition(void) { return m_Info.pos; }	// 座標
	D3DXVECTOR2& GetSize(void) { return m_Info.size; }		// 幅高さ
	CRoad* GetConnectRoad(const DIRECTION& dic) // 連結道路
	{ return m_apConnectRoad[dic]; }
	float GetConnectLength(const DIRECTION& dic) // 連結道路への距離
	{ return m_apConnectLength[dic]; }
	TYPE GetType(void) { return m_Type; }
	SSearch* GetSearchRoad(const DIRECTION& dic)
	{ return &m_aSearchRoad[dic]; }
	SSearch* GetSearchSelf()
	{ return &m_Searchself; }
	D3DXVECTOR3* GetVtxPos() { return &m_aVtxPos[0]; }
	// メンバ関数(設定)
	void Connect(CRoad* pRoad, const DIRECTION dic);
	void SearchConnect(CRoad* pRoad, const DIRECTION dic);
	void SetPosition(const D3DXVECTOR3& pos);
	void SetSize(const D3DXVECTOR2& size);

private:	// 自分だけがアクセス可能

	// メンバ関数
	void Rotation(TYPE type);

	// メンバ変数
	SInfo m_Info;	// 基本情報
	D3DXVECTOR3 m_aVtxPos[4];
	CObject3D* m_pObj;
	int m_nIdx;
	CRoad* m_apConnectRoad[DIRECTION::DIC_MAX];		// 連結した道
	float m_apConnectLength[DIRECTION::DIC_MAX];	// 連結した道への距離
	SSearch m_aSearchRoad[DIRECTION::DIC_MAX];		// 経路探索用情報
	SSearch m_Searchself;
	TYPE m_Type;
};

#endif
