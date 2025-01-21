//==========================================================
//
// ギミック [Gimmick.h]
// Author : 丹野竜之介
//
//==========================================================
#ifndef _Gimmick_H_		// このマクロが定義されていない場合
#define _Gimmick_H_		// 二重インクルード防止用マクロを定義

#include "task.h"
#include "list.h"
//#include "task.h"	// これでファイルインクルードできます

class CBridge;

//==========================================================
// ギミックのクラス定義
//==========================================================
class CGimmick : public CTask
{
public:

	// 種類情報
	enum TYPE
	{
		TYPE_FIREHYDRANT = 0,	// 消火栓
		TYPE_GUARDRAIL,			// ガードレール
		TYPE_POLICESTATION,		// 警察署
		TYPE_BRIDGE,			// はね橋
		TYPE_SIGNBOARD,			// 看板
		TYPE_TEASHCAN,			// ゴミ箱
		TYPE_MAX
	};

	// 基本情報
	struct SInfo
	{
		D3DXVECTOR3 pos;	// 位置
		D3DXVECTOR3 rot;	// 向き
		D3DXVECTOR3 scale;	// スケール
		D3DXVECTOR3 CollisionMax;	// 最大値
		D3DXVECTOR3 CollisionMin;	// 最小値
		TYPE type;

		// コンストラクタ
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), scale(VECTOR3_ONE), CollisionMax(VECTOR3_ZERO), CollisionMin(VECTOR3_ZERO){}
	};

public:	// 誰でもアクセス可能

	CGimmick();	// コンストラクタ(オーバーロード)
	virtual ~CGimmick();	// デストラクタ

	// メンバ関数
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void);
	virtual void Update(void)= 0;
	// メンバ関数
	D3DXVECTOR3 GetPos(){ return m_Info.pos; }
	D3DXVECTOR3 GetRot() { return m_Info.rot; }
	D3DXVECTOR3 GetScale() { return m_Info.scale; }
	D3DXVECTOR3 GetVtxMax();
	D3DXVECTOR3 GetVtxMin();
	void SetPos(D3DXVECTOR3 pos) { m_Info.pos = pos; SetObjPos(pos); }
	void SetRot(D3DXVECTOR3 rot) { m_Info.rot = rot; SetObjRot(rot); }
	void SetScale(D3DXVECTOR3 scale) { m_Info.scale = scale; SetObjScale(scale); }
	static Clist<CGimmick*>* GetList() { return &m_List; }
	SInfo& GetInfo() { return m_Info; }
	static CGimmick* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale, const TYPE type);
	TYPE GetType() { return m_Info.type; }
	void SetType(const TYPE type) { m_Info.type = type; }
	int GetId() { return m_nId; }
	virtual CBridge* GetBridge() { return nullptr; }

	// 衝突時の判定
	virtual void SetColMulti(const D3DXCOLOR& col) = 0;

	// オンライン用
	virtual void Hit(const D3DXVECTOR3& HitPos, const float fSpeed) {}

protected:

	void SetVtxMax(const D3DXVECTOR3& max) { m_Info.CollisionMax = max; }
	void SetVtxMin(const D3DXVECTOR3& min) { m_Info.CollisionMin = min; }
	virtual void SetObjPos(const D3DXVECTOR3& pos) = 0;
	virtual void SetObjRot(const D3DXVECTOR3& pos) = 0;
	virtual void SetObjScale(const D3DXVECTOR3& pos) = 0;

private:	// 自分だけがアクセス可能

	// メンバ関数
	static Clist<CGimmick*> m_List;

	// メンバ変数
	SInfo m_Info;
	int m_nId;
};

#endif

