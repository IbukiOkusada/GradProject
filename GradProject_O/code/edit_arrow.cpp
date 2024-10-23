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
		{130.0f, 0.0f, 0.0f},
		{0.0f, 130.0f, 0.0f},
		{0.0f, 0.0f, -130.0f},
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

	// マウスの移動量取得
	D3DXVECTOR2 nowpoint= D3DXVECTOR2(pMouse->GetPoint().x, pMouse->GetPoint().y);
	D3DXVECTOR3 vec = pMouse->GetRayInfo().vec;
	D3DXVECTOR2 diff = nowpoint - m_Info.touchscrpos;

	// 3. 初期レイの方向と現在のレイの方向を比較して、ズレを計算
	float dotProduct = D3DXVec3Dot(&m_Info.touchvec, &pMouse->GetRayInfo().vec);

	// 4. クリックした位置から進む新しい座標を計算
	float distance = dotProduct;  // ドット積で方向を補正
	D3DXVECTOR3 newpos = m_Info.touchpos;

	CDebugProc::GetInstance()->Print("初回タッチ座標 [ %f, %f ]\n", m_Info.touchscrpos.x, m_Info.touchscrpos.y);
	CDebugProc::GetInstance()->Print("タッチ座標 [ %f, %f ]\n", nowpoint.x, nowpoint.y);
	CDebugProc::GetInstance()->Print("ズレ [ %f ]\n", nowpoint.x, nowpoint.y);

	// 種類指定
	switch (m_pHold->type)
	{
	case TYPE_ALL:
	{
		newpos = m_Info.touchpos + vec * distance;
	}
	break;

	case TYPE_X:
	{
		newpos.x = m_Info.touchpos.x + vec.x * distance;
	}
	break;

	case TYPE_Y:
	{
		newpos.y = m_Info.touchpos.y + vec.y * distance;
	}
	break;

	case TYPE_Z:
	{
		newpos.z = m_Info.touchpos.z + vec.z * distance;
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
		if (!trigger) { return; }

		// 記憶 触れた座標取得
		m_pHold = &m_aObj[i];
		m_Info.startpos = m_pos;
		m_Info.touchpos = touchpos;
		m_Info.touchscrpos.x = pMouse->GetPoint().x;
		m_Info.touchscrpos.y = pMouse->GetPoint().y;
		m_Info.touchvec = pMouse->GetRayInfo().vec;
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