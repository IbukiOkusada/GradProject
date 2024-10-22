//==========================================================
//
// エディター道 [edit_road.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_road.h"
#include "input_mouse.h"
#include "debugproc.h"
#include "road_manager.h"
#include "road.h"
#include "object3D.h"

//==========================================================
// コンストラクタ
//==========================================================
CEdit_Road::CEdit_Road()
{
	// 値のクリア
	m_pSelectRoad = nullptr;
}

//==========================================================
// デストラクタ
//==========================================================
CEdit_Road::~CEdit_Road()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CEdit_Road::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CEdit_Road::Uninit(void)
{
	CEdit::Uninit();
}


//==========================================================
// 更新処理
//==========================================================
void CEdit_Road::Update(void)
{
	CDebugProc::GetInstance()->Print(" [ 道配置モード ]\n");

	// 選択
	ClickCheck();

	if (m_pSelectRoad != nullptr)
	{
		m_pSelectRoad->GetObj()->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));
	}
}

//==========================================================
// 選択
//==========================================================
void CEdit_Road::ClickCheck()
{
	CInputMouse* pMouse = CInputMouse::GetInstance();
	CRoad* pRoad = CRoadManager::GetInstance()->GetTop();

	// 入力があれば確認する
	if (!pMouse->GetTrigger(CInputMouse::BUTTON_LBUTTON)) 
	{ 
		return; 
	}

	if (m_pSelectRoad != nullptr)
	{
		m_pSelectRoad->GetObj()->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}

	m_pSelectRoad = nullptr;

	// 道を全て確認
	while (pRoad != nullptr)
	{
		CRoad* pNext = pRoad->GetNext();

		// 衝突した
		if (CursorCollision(pRoad))
		{
			m_pSelectRoad = pRoad;
		}

		pRoad = pNext;
	}
}

//==========================================================
// 選択したものとの衝突判定
//==========================================================
bool CEdit_Road::CursorCollision(CRoad* pRoad)
{
	// マウス情報
	CInputMouse* pMouse = CInputMouse::GetInstance();
	CInputMouse::SRayInfo info = pMouse->GetRayInfo();

	// 床情報
	D3DXVECTOR3 pos = pRoad->GetPosition();
	VERTEX_3D* pVtx = pRoad->GetObj()->GetVertex();

	// 使用されていなかったら返す
	if (pVtx == nullptr) { return false; }

	D3DXVECTOR3 pos0 = pos + pVtx[0].pos, pos1 = pos + pVtx[1].pos, pos2 = pos + pVtx[2].pos;

	// 三角形と線分の衝突確認
	if (TriangleCollision(info.origin, info.vec, pos0, pos1, pos2))
	{
		return true;
	}

	// 反対の三角へ
	pos0 = pos + pVtx[3].pos;

	// 同じく衝突確認
	if (TriangleCollision(info.origin, info.vec, pos0, pos1, pos2))
	{
		return true;
	}

	return false;
}

//==========================================================
// 選択したものとの衝突判定
//==========================================================
bool CEdit_Road::TriangleCollision(const D3DXVECTOR3& rayOrigin, const D3DXVECTOR3& rayDir,
	const D3DXVECTOR3& pos0, const D3DXVECTOR3& pos1, const D3DXVECTOR3& pos2)
{
	// 三角形のエッジベクトルを計算
	D3DXVECTOR3 edge1 = pos1 - pos0;
	D3DXVECTOR3 edge2 = pos2 - pos0;

	// レイ方向と edge2 の外積を計算
	D3DXVECTOR3 pvec;
	D3DXVec3Cross(&pvec, &rayDir, &edge2);

	float det = D3DXVec3Dot(&edge1, &pvec);  // 行列式

	// レイが三角形に平行な場合は交差しない
	if (fabs(det) < 1e-8f) return false;

	float invDet = 1.0f / det;

	// レイの始点から三角形の1つ目の頂点へのベクトル
	D3DXVECTOR3 tvec = rayOrigin - pos0;

	// uパラメータの計算（三角形の内部判定）
	float u = D3DXVec3Dot(&tvec, &pvec) * invDet;
	if (u < 0.0f || u > 1.0f) return false;

	// qvecを計算してvパラメータを求める
	D3DXVECTOR3 qvec;
	D3DXVec3Cross(&qvec, &tvec, &edge1);
	float v = D3DXVec3Dot(&rayDir, &qvec) * invDet;
	if (v < 0.0f || u + v > 1.0f) return false;

	return true;  // 衝突が発生した場合
}
