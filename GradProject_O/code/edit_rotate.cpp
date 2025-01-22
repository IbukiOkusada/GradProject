//==========================================================
//
// エディター回転 [edit_rotate.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_rotate.h"
#include "objectX.h"
#include "input_mouse.h"
#include "debugproc.h"
#include "camera.h"
#include "camera_manager.h"

// 無名名前空間
namespace
{
	const float DEF_LENGTH = 20000.0f;
	const char* FILENAME[CEdit_Rotate::VEC_MAX] = {	// モデルファイル名
		"data\\MODEL\\edit\\cube.x",
		"data\\MODEL\\edit\\x_rotate.x",
		"data\\MODEL\\edit\\y_rotate.x",
		"data\\MODEL\\edit\\z_rotate.x",
	};
}

//==========================================================
// コンストラクタ
//==========================================================
CEdit_Rotate::CEdit_Rotate()
{

}

//==========================================================
// デストラクタ
//==========================================================
CEdit_Rotate::~CEdit_Rotate()
{

}

//==========================================================
// 生成
//==========================================================
CEdit_Rotate* CEdit_Rotate::Create(const D3DXVECTOR3& pos)
{
	CEdit_Rotate* pRotate = DEBUG_NEW CEdit_Rotate;

	if (pRotate != nullptr)
	{
		pRotate->Init();
		pRotate->SetPosition(pos);
		pRotate->m_Info.startpos = pos;
		pRotate->m_Info.touchpos = pos;
	}

	return pRotate;
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CEdit_Rotate::Init(void)
{
	m_pos = VECTOR3_ZERO;

	// モデル生成
	for (int i = 0; i < VEC_MAX; i++)
	{
		m_aObj[i].pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, FILENAME[i], 2);
		m_aObj[i].pObj->SetScale(D3DXVECTOR3(0.5f, 0.5f, 0.5f));
		m_aObj[i].pObj->SetType(CObject::TYPE_EDIT);
	}

	return S_OK;
}

#if 0
//==========================================================
// 終了処理
//==========================================================
void CEdit_Rotate::Uninit(void)
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
#endif

#if 0
//==========================================================
// 更新処理
//==========================================================
void CEdit_Rotate::Update(void)
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

		if (m_pHold != &m_aObj[i]) {
			m_aObj[i].pObj->SetColMulti(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			m_aObj[i].pObj->SetColAdd(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));
		}
		else {
			m_aObj[i].pObj->SetColMulti(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));
			m_aObj[i].pObj->SetColAdd(D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f));
		}
	}
}
#endif

//==========================================================
// 座標設定
//==========================================================
void CEdit_Rotate::Rotation()
{
	// 操作できるときのみ
	if (GetHold() == nullptr) { return; }

	// 入力確認
	CInputMouse* pMouse = CInputMouse::GetInstance();
	bool press = pMouse->GetPress(CInputMouse::BUTTON_LBUTTON);
	if (!press) { return; }

	D3DXVECTOR3 worldpos = pMouse->GetWorldInfo().pos;
	D3DXVECTOR3 move = worldpos - m_Info.touchworldpos;
	move *= (CCameraManager::GetInstance()->GetTop()->GetLength() / DEF_LENGTH);

	// 4. 新しい位置を計算
	D3DXVECTOR3 newpos = m_Info.startpos;

	// 種類指定
	switch (GetHold()->type)
	{
	case VEC_ALL:
	{
		newpos = m_Info.startpos + move;
		CDebugProc::GetInstance()->Print("全部！");
	}
	break;

	case VEC_X:
	{
		newpos.x = m_Info.startpos.x + move.x;
		CDebugProc::GetInstance()->Print("X！");
	}
	break;

	case VEC_Y:
	{
		newpos.y = m_Info.startpos.y + move.y;
		CDebugProc::GetInstance()->Print("Y！");
	}
	break;

	case VEC_Z:
	{
		newpos.z = m_Info.startpos.z + move.z;
		CDebugProc::GetInstance()->Print("Z！");
	}
	break;

	default:
		break;
	}

	m_rot = (newpos - m_Info.startpos);
	correction::Adjust(m_rot);

	CDebugProc::GetInstance()->Print("向き [ %f, %f, %f ]\n", m_rot.x, m_rot.y, m_rot.z);
}
