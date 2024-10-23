//==========================================================
//
// エディター矢印 [edit_arrow.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_arrow.h"
#include "objectX.h"
#include "input_mouse.h"
#include "debugproc.h"

// 無名名前空間
namespace
{
	const char* FILENAME[CEdit_Arrow::TYPE_MAX] = {	// モデルファイル名
		"data\\MODEL\\edit\\cube.x",
		"data\\MODEL\\edit\\x_arrow.x",
		"data\\MODEL\\edit\\y_arrow.x",
		"data\\MODEL\\edit\\z_arrow.x",
	};

	const D3DXVECTOR3 SETPOS[CEdit_Arrow::TYPE_MAX] = {	// 設置座標
		{0.0f, 0.0f, 0.0f},
		{180.0f, 0.0f, 0.0f},
		{0.0f, 180.0f, 0.0f},
		{0.0f, 0.0f, -180.0f},
	};
}

//==========================================================
// コンストラクタ
//==========================================================
CEdit_Arrow::CEdit_Arrow()
{
	// 値のクリア
	for (int i = 0; i < TYPE_MAX; i++)
	{
		m_aObj[i].pObj = nullptr;
		m_aObj[i].type = static_cast<TYPE>(i);
	}

	m_pos = VECTOR3_ZERO;
	m_Info = SInfo();
	m_pHold = nullptr;
}

//==========================================================
// デストラクタ
//==========================================================
CEdit_Arrow::~CEdit_Arrow()
{

}

//==========================================================
// 生成
//==========================================================
CEdit_Arrow* CEdit_Arrow::Create(const D3DXVECTOR3& pos)
{
	CEdit_Arrow* pArrow = DEBUG_NEW CEdit_Arrow;

	if (pArrow != nullptr)
	{
		pArrow->Init();
		pArrow->SetPosition(pos);
		pArrow->m_Info.startpos = pos;
		pArrow->m_Info.touchpos = pos;
	}

	return pArrow;
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CEdit_Arrow::Init(void)
{
	m_pos = VECTOR3_ZERO;

	// モデル生成
	for (int i = 0; i < TYPE_MAX; i++)
	{
		m_aObj[i].pObj = CObjectX::Create(m_pos + SETPOS[i], VECTOR3_ZERO, FILENAME[i], 7);
		m_aObj[i].pObj->SetType(CObject::TYPE_EDIT);
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CEdit_Arrow::Uninit(void)
{
	// 値のクリア
	for (int i = 0; i < TYPE_MAX; i++)
	{
		if (m_aObj[i].pObj == nullptr) { continue; }

		m_aObj[i].pObj->Uninit();
		m_aObj[i].pObj = nullptr;
	}

	delete this;
}

//==========================================================
// 更新処理
//==========================================================
void CEdit_Arrow::Update(void)
{
	// 選択
	Select();

	// 移動
	Move();

	// 離す
	Release();

	// 座標の更新
	for (int i = 0; i < TYPE_MAX; i++)
	{
		if (m_aObj[i].pObj == nullptr) { continue; }

		m_aObj[i].pObj->SetPosition(m_pos + SETPOS[i]);
	}
}

//==========================================================
// 座標設定
//==========================================================
void CEdit_Arrow::SetPosition(const D3DXVECTOR3& pos)
{
	m_pos = pos;

	// 座標の更新
	for (int i = 0; i < TYPE_MAX; i++)
	{
		if (m_aObj[i].pObj == nullptr) { continue; }

		m_aObj[i].pObj->SetPosition(m_pos + SETPOS[i]);
	}
}

//==========================================================
// 座標設定
//==========================================================
void CEdit_Arrow::Move()
{
	// 操作できるときのみ
	if (m_pHold == nullptr) { return; }

	// 入力確認
	CInputMouse* pMouse = CInputMouse::GetInstance();
	bool press = pMouse->GetPress(CInputMouse::BUTTON_LBUTTON);
	if (!press) { return; }

	D3DXVECTOR3 worldpos = pMouse->GetWorldInfo().pos;
	D3DXVECTOR3 move = worldpos - m_Info.touchworldpos;
	
	// 4. 新しい位置を計算
	D3DXVECTOR3 newpos = m_Info.touchpos;

	// 種類指定
	switch (m_pHold->type)
	{
	case TYPE_ALL:
	{
		newpos = m_Info.touchpos + move;
		newpos.y = m_Info.touchpos.y;
		CDebugProc::GetInstance()->Print("全部！\n");
	}
	break;

	case TYPE_X:
	{
		newpos.x = m_Info.touchpos.x + move.x;
		CDebugProc::GetInstance()->Print("X！\n");
	}
	break;

	case TYPE_Y:
	{
		newpos.y = m_Info.touchpos.y + move.y;
		CDebugProc::GetInstance()->Print("Y！\n");
	}
	break;

	case TYPE_Z:
	{
		newpos.z = m_Info.touchpos.z + move.z;
		CDebugProc::GetInstance()->Print("Z！\n");
	}
	break;

	default:
		break;
	}

	m_pos = newpos - (m_Info.touchpos - m_Info.startpos);
}

//==========================================================
// 選択
//==========================================================
void CEdit_Arrow::Select()
{
	// 入力確認
	CInputMouse* pMouse = CInputMouse::GetInstance();
	bool trigger = pMouse->GetTrigger(CInputMouse::BUTTON_LBUTTON);

	// マウスレイ情報取得
	D3DXVECTOR3 origin = pMouse->GetRayInfo().origin;
	D3DXVECTOR3 vec = pMouse->GetRayInfo().vec;
	D3DXVECTOR3 touchpos = m_Info.startpos;
	float length = 1000000.0f;

	if (!trigger) { return; }

	for (int i = 0; i < TYPE_MAX; i++)
	{
		if (m_aObj[i].pObj == nullptr) { continue; }

		D3DXVECTOR3 pos = m_aObj[i].pObj->GetPosition();
		D3DXVECTOR3 rot = VECTOR3_ZERO;
		D3DXVECTOR3 vtxmax = m_aObj[i].pObj->GetVtxMax();
		D3DXVECTOR3 vtxmin = m_aObj[i].pObj->GetVtxMin();

		// 当たり判定
		bool hit = collision::CollideRayToOBB(&touchpos, origin, vec, pos, rot, vtxmax, vtxmin);

		// 当たった時のみ
		if (!hit) { continue; }

		D3DXVECTOR3 diff = (touchpos - origin);
		float nowlength = D3DXVec3Length(&diff);

		// 距離
		if (nowlength > length) { continue; }

		// 記憶 触れた座標取得
		m_pHold = &m_aObj[i];
		m_Info.startpos = m_pos;
		m_Info.touchpos = touchpos;
		m_Info.touchworldpos = pMouse->GetWorldInfo().pos;
		length = nowlength;
	}
}

//==========================================================
// 離す
//==========================================================
void CEdit_Arrow::Release()
{
	CInputMouse* pMouse = CInputMouse::GetInstance();
	bool press = pMouse->GetPress(CInputMouse::BUTTON_LBUTTON);
	if (press) { return; }

	m_pHold = nullptr;
	m_Info.startpos = m_pos;
}