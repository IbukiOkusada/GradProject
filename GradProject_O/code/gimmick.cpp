//==========================================================
//
// ギミック [Gimmick.cpp]
// Author : 丹野竜之介
//
//==========================================================
#include "gimmick.h"
#include "gimmick_firehydrant.h"
#include "gimmick_guardrail.h"
#include "gimmick_policestation.h"
#include "bridge.h"

Clist<CGimmick*> CGimmick::m_List = {};

//==========================================================
// コンストラクタ
//==========================================================
CGimmick::CGimmick()
{
	m_Info = SInfo();
	m_List.Regist(this);
}

//==========================================================
// デストラクタ
//==========================================================
CGimmick::~CGimmick()
{
	
}

//==========================================================
// 終了処理
//==========================================================
void CGimmick::Uninit()
{
	m_List.Delete(this);
	Release();
}

//==========================================================
// 最大値取得
//==========================================================
D3DXVECTOR3 CGimmick::GetVtxMax()
{
	D3DXVECTOR3 max = m_Info.CollisionMax;

	max.x *= m_Info.scale.x;
	max.y *= m_Info.scale.y;
	max.z *= m_Info.scale.z;

	return max;
}

//==========================================================
// 最小値取得
//==========================================================
D3DXVECTOR3 CGimmick::GetVtxMin()
{
	D3DXVECTOR3 min = m_Info.CollisionMin;

	min.x *= m_Info.scale.x;
	min.y *= m_Info.scale.y;
	min.z *= m_Info.scale.z;

	return min;
}

//==========================================================
// 生成
//==========================================================
CGimmick* CGimmick::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale, const TYPE type)
{
	CGimmick* pGimmick = nullptr;

	// 種類ごとに生成
	switch (type)
	{
	case TYPE::TYPE_FIREHYDRANT:
		pGimmick = CGimmickFireHydrant::Create(pos, rot, scale);
		break;

	case TYPE::TYPE_GUARDRAIL:
		pGimmick = CGimmickGuardRail::Create(pos, rot, scale);
		break;

	case TYPE::TYPE_POLICESTATION:
		pGimmick = CGimmickPoliceStation::Create(pos, rot, scale);
		break;

	case TYPE::TYPE_BRIDGE:
		pGimmick = CBridge::Create(pos, rot, scale, 600.0f, 600.0f);
		break;
	}

	return pGimmick;
}