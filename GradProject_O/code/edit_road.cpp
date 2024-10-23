//==========================================================
//
// エディター道 [edit_road.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_road.h"
#include "input_mouse.h"
#include "input_keyboard.h"
#include "debugproc.h"
#include "road_manager.h"
#include "road.h"
#include "object3D.h"
#include "edit_arrow.h"

namespace
{
	const float MIN_LENGTH = 100.0f;	// 最小移動量
}

//==========================================================
// コンストラクタ
//==========================================================
CEdit_Road::CEdit_Road()
{
	// 値のクリア
	m_pSelectRoad = nullptr;
	m_pArrow = nullptr;
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
	m_fMoveLength = MIN_LENGTH;
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CEdit_Road::Uninit(void)
{
	// 矢印終了
	if (m_pArrow != nullptr)
	{
		m_pArrow->Uninit();
		m_pArrow = nullptr;
	}

	m_pSelectRoad = nullptr;

	CEdit::Uninit();
}


//==========================================================
// 更新処理
//==========================================================
void CEdit_Road::Update(void)
{
	CDebugProc::GetInstance()->Print(" [ 道配置モード ]\n");
	CRoad* pOld = m_pSelectRoad;

	// 選択
	ClickCheck();

	CDebugProc::GetInstance()->Print("[");

	// 再連結
	ReConnect();

	// 選択されていない、もしくは選択した直後
	if (m_pSelectRoad == nullptr || pOld == nullptr) { CDebugProc::GetInstance()->Print("]\n"); return; }

	// 選択されたものを色変える
	m_pSelectRoad->GetObj()->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));

	// 矢印の更新
	if (m_pArrow != nullptr)
	{
		m_pArrow->Update();
	}

	// 移動
	Move();

	// 削除
	Delete();

	CDebugProc::GetInstance()->Print("]\n");
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

	// 矢印選択中
	if (m_pArrow != nullptr)
	{
		if (m_pArrow->GetHold() != nullptr)
		{
			return;
		}
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

			if (m_pArrow == nullptr)
			{
				m_pArrow = CEdit_Arrow::Create(m_pSelectRoad->GetPosition());
			}

			m_pArrow->SetPosition(m_pSelectRoad->GetPosition());

			return;
		}

		pRoad = pNext;
	}

	// 道が選ばれていない
	if (m_pSelectRoad != nullptr) { return; }

	// 矢印使用中
	if (m_pArrow == nullptr) { return; }

	m_pArrow->Uninit();
	m_pArrow = nullptr;
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

//==========================================================
// 繋ぎなおし
//==========================================================
void CEdit_Road::ReConnect()
{
	CDebugProc::GetInstance()->Print(" 道再連結 : F5, ");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	// 入力確認
	if (!pKey->GetTrigger(DIK_F5)) { return; }

	// 再接続
	CRoadManager::GetInstance()->AllConnect();
}

//==========================================================
// 削除
//==========================================================
void CEdit_Road::Delete()
{
	CDebugProc::GetInstance()->Print(" 道削除 : Delete or BackSpace, ");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	// 入力確認
	if (!pKey->GetTrigger(DIK_DELETE) && !pKey->GetTrigger(DIK_BACKSPACE)) { return; }

	// 再接続
	m_pSelectRoad->Uninit();
	m_pSelectRoad = nullptr;

	// 再連結
	ReConnect();

	// 矢印使用中
	if (m_pArrow == nullptr) { return; }

	m_pArrow->Uninit();
	m_pArrow = nullptr;
}

//==========================================================
// 移動
//==========================================================
void CEdit_Road::Move()
{
	if (m_pSelectRoad == nullptr) { return; }
	if (m_pArrow == nullptr) { return; }

	D3DXVECTOR3 pos = m_pSelectRoad->GetPosition();	// 座標
	D3DXVECTOR3 Arrowpos = m_pArrow->GetPosition();	// 矢印座標

	// X座標
	float length = fabsf(Arrowpos.x - pos.x);
	int setpos = static_cast<int>(Arrowpos.x);
	int movelength = static_cast<int>(m_fMoveLength);

	// 大きい
	if (length >= m_fMoveLength)
	{
		pos.x = static_cast<float>(setpos - (setpos % movelength));
	}

	// Y座標
	length = fabsf(Arrowpos.y - pos.y);
	setpos = static_cast<int>(Arrowpos.y);
	movelength = static_cast<int>(m_fMoveLength);

	// 大きい
	if (length >= m_fMoveLength)
	{
		pos.y = static_cast<float>(setpos - (setpos % movelength));
	}

	// Z座標
	length = fabsf(Arrowpos.z - pos.z);
	setpos = static_cast<int>(Arrowpos.z);
	movelength = static_cast<int>(m_fMoveLength);

	// 大きい
	if (length >= m_fMoveLength)
	{
		pos.z = static_cast<float>(setpos - (setpos % movelength));
	}

	// 選択した道の座標設定
	m_pSelectRoad->SetPosition(pos);
}