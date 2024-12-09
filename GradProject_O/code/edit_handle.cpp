//==========================================================
//
// エディターハンドル基底 [edit_handle.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_handle.h"
#include "objectX.h"
#include "input_mouse.h"
#include "debugproc.h"
#include "camera.h"
#include "camera_manager.h"
#include "edit_arrow.h"
#include "edit_scale.h"
#include "edit_rotate.h"

// 無名名前空間
namespace
{
	const char* TEXT[CEdit_Handle::TYPE::TYPE_MAX] = {
		"移動",
		"回転",
		"拡縮",
	};

	const D3DXVECTOR3 SETPOS[CEdit_Handle::VEC_MAX] = {	// 設置座標
		{0.0f, 0.0f, 0.0f},
		{180.0f, 0.0f, 0.0f},
		{0.0f, 180.0f, 0.0f},
		{0.0f, 0.0f, -180.0f},
	};

	const float DEF_LENGTH = 500.0f;
}

//==========================================================
// コンストラクタ
//==========================================================
CEdit_Handle::CEdit_Handle()
{
	// 値のクリア
	for (int i = 0; i < VEC_MAX; i++)
	{
		m_aObj[i].pObj = nullptr;
		m_aObj[i].type = static_cast<VEC>(i);
	}

	m_pos = VECTOR3_ZERO;
	m_rot = VECTOR3_ZERO;
	m_posDiff = VECTOR3_ZERO;
	m_rotDiff = VECTOR3_ZERO;
	m_Info = SInfo();
	m_pHold = nullptr;
	m_pOldHold = nullptr;
}

//==========================================================
// デストラクタ
//==========================================================
CEdit_Handle::~CEdit_Handle()
{

}

//==========================================================
// 生成
//==========================================================
CEdit_Handle* CEdit_Handle::Create(const D3DXVECTOR3& pos, const TYPE type)
{
	CEdit_Handle* pHandle = nullptr;

	switch (type)
	{
	case TYPE::TYPE_MOVE:

		pHandle = DEBUG_NEW CEdit_Arrow;

		break;
	case TYPE::TYPE_ROT:

		pHandle = DEBUG_NEW CEdit_Rotate;

		break;
	case TYPE::TYPE_SCALE:

		pHandle = DEBUG_NEW CEdit_Scale;

		break;
	default:
		break;
	}

	if (pHandle != nullptr)
	{
		pHandle->Init();
		pHandle->SetPosition(pos);
		pHandle->m_Info.startpos = pos;
		pHandle->m_Info.touchpos = pos;
		pHandle->m_type = type;
	}

	return pHandle;
}

#if 0
//==========================================================
// 初期化処理
//==========================================================
HRESULT CEdit_Handle::Init(void)
{
	m_pos = VECTOR3_ZERO;

	// モデル生成
	for (int i = 0; i < TYPE_MAX; i++)
	{
		m_aObj[i].pObj = CObjectX::Create(m_pos + SETPOS[i], VECTOR3_ZERO, FILENAME[i], 2);
		m_aObj[i].pObj->SetType(CObject::TYPE_EDIT);
	}

	return S_OK;
}
#endif

//==========================================================
// 終了処理
//==========================================================
void CEdit_Handle::Uninit(void)
{
	// 値のクリア
	for (int i = 0; i < VEC_MAX; i++)
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
void CEdit_Handle::Update(void)
{
	CDebugProc::GetInstance()->Print("現在の変更状態 [ %s ] :", TEXT[m_type]);
	m_pOldHold = m_pHold;

	// 選択
	Select();

	// 移動
	Move();

	// 拡大縮小
	Scale();

	// 回転
	Rotation();

	// 離す
	Release();

	// 座標と向きの差分保存
	m_posDiff = (m_pos - m_Info.startpos);
	m_rotDiff = (m_rot - m_Info.startrot);

	// 座標の更新
	for (int i = 0; i < VEC_MAX; i++)
	{
		if (m_aObj[i].pObj == nullptr) { continue; }

		if (m_type == TYPE::TYPE_MOVE)
		{
			m_aObj[i].pObj->SetPosition(m_pos + SETPOS[i]);
			m_aObj[i].pObj->SetRotation(m_rot);
		}
		else if (m_type == TYPE::TYPE_ROT)
		{
			m_aObj[i].pObj->SetPosition(m_pos);
			m_aObj[i].pObj->SetRotation(m_rot);
		}

		if (m_pHold != &m_aObj[i]) {
			m_aObj[i].pObj->SetColMulti(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			m_aObj[i].pObj->SetColAdd(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));
		}
		else {
			m_aObj[i].pObj->SetColMulti(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));
			m_aObj[i].pObj->SetColAdd(D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f));
		}

		m_aObj[i].pObj->SetEnableShadow(false);
	}
}

//==========================================================
// 座標設定
//==========================================================
void CEdit_Handle::SetPosition(const D3DXVECTOR3& pos)
{
	m_pos = pos;

	// 座標の更新
	for (int i = 0; i < VEC_MAX; i++)
	{
		if (m_aObj[i].pObj == nullptr) { continue; }

		m_aObj[i].pObj->SetPosition(m_pos + SETPOS[i]);
	}
}

//==========================================================
// 選択
//==========================================================
void CEdit_Handle::Select()
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

	for (int i = 0; i < VEC_MAX; i++)
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
		m_Info.startrot = m_rot;
		m_Info.touchpos = touchpos;
		m_Info.touchworldpos = pMouse->GetWorldInfo().pos;
		length = nowlength;
	}
}

//==========================================================
// 離す
//==========================================================
void CEdit_Handle::Release()
{
	CInputMouse* pMouse = CInputMouse::GetInstance();
	bool press = pMouse->GetPress(CInputMouse::BUTTON_LBUTTON);
	if (press) { return; }

	m_pHold = nullptr;
	m_Info.startpos = m_pos;
	m_Info.startrot = VECTOR3_ZERO;
	m_rot = VECTOR3_ZERO;
}