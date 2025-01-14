//==========================================================
//
// マップ障害物基底 [map_obstacle.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MAP_OBSTACLE_H_		// このマクロが定義されていない場合
#define _MAP_OBSTACLE_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
#include "list.h"

//前方宣言
class CObjectX;

//==========================================================
// 障害物の基底クラス定義
//==========================================================
class CMapObstacle : public CTask
{
public:	// 誰でもアクセス可能な定義

	// 基本情報構造体
	struct SInfo
	{
		D3DXVECTOR3 pos;	// 座標
		D3DXVECTOR3 rot;	// 向き
		D3DXVECTOR3 scale;	// 幅
		int fileidx;		// ファイル番号

		// コンストラクタ
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), scale(VECTOR3_ZERO), fileidx(-1) {}
		SInfo(const D3DXVECTOR3& _pos, const D3DXVECTOR3& _rot, const D3DXVECTOR3& _scale) : pos(_pos), rot(_rot), scale(scale), fileidx(-1) {}
		SInfo(const SInfo& _info) : pos(_info.pos), rot(_info.rot), scale(_info.scale), fileidx(_info.fileidx) {}
	};

public:	// 誰でもアクセス可能

	CMapObstacle(const SInfo& info = SInfo());	// コンストラクタ(オーバーロード)
	~CMapObstacle();	// デストラクタ

	// メンバ関数
	HRESULT Init(void) override;
	HRESULT Init(const std::string& filename);
	void Uninit(void) override;
	void Update(void) override;
	
	// 取得関数
	D3DXVECTOR3& GetPosition() { return m_Info.pos; }
	D3DXVECTOR3& GetRotation() { return m_Info.rot; }
	D3DXVECTOR3& GetScale() { return m_Info.scale; }
	CObjectX* GetObj() { return m_pObj; }
	SInfo& GetInfo() { return m_Info; }

	// 設定関数
	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);
	void SetScale(const D3DXVECTOR3& scale);
	void BindModel(const int& idx);

	// 静的
	static CMapObstacle* Create(const SInfo& info);
	static Clist<CMapObstacle*>* GetList() { return m_pList; }
	static Clist<CMapObstacle*>* GetInstance() // リスト取得
	{ if (m_pList == nullptr) { m_pList = m_pList->Create(); }return m_pList; }
	static void ListRelease() // リスト解放
	{ if (m_pList != nullptr) { delete m_pList; m_pList = nullptr; } }

private:	// 自分だけがアクセス可能

	// メンバ関数
	void DrawCheck();

	// メンバ変数
	CObjectX* m_pObj;	// 障害物
	SInfo m_Info;		// 基本情報
	static Clist<CMapObstacle*>* m_pList;	// リスト
};

#endif