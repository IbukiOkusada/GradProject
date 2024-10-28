//==========================================================
//
// エディターオブジェクト [edit_object.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_object.h"
#include "manager.h"
#include "debugproc.h"
#include "edit_arrow.h"
#include "map_obstacle.h"
#include "objectX.h"
#include "input_mouse.h"
#include "input_keyboard.h"
#include "map_manager.h"

namespace
{
	const std::string FILENAME = "data\\FILE\\map\\obstacle.bin";
}

//==========================================================
// コンストラクタ
//==========================================================
CEdit_Obj::CEdit_Obj()
{
	// 値のクリア
	m_pSelect = nullptr;
	m_pArrow = nullptr;
	m_fMouseWheel = 0.0f;
}

//==========================================================
// デストラクタ
//==========================================================
CEdit_Obj::~CEdit_Obj()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CEdit_Obj::Init(void)
{
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CEdit_Obj::Uninit(void)
{
	if (m_pArrow != nullptr)
	{
		m_pArrow->Uninit();
		m_pArrow = nullptr;
	}

	CEdit::Uninit();
}

//==========================================================
// 更新処理
//==========================================================
void CEdit_Obj::Update(void)
{
	CDebugProc::GetInstance()->Print(" [ 障害物配置モード ]\n");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();
	CMapObstacle* pOld = m_pSelect;

	// 選択
	ClickCheck();

	CDebugProc::GetInstance()->Print("[");

	// 保存
	Save();

	if (pKey->GetPress(DIK_LALT) || pKey->GetPress(DIK_RALT)) { CDebugProc::GetInstance()->Print("]\n"); return; }

	// 選択されていない、もしくは選択した直後
	if (m_pSelect == nullptr || pOld == nullptr) {

		// 生成
		Create();

		CDebugProc::GetInstance()->Print("]\n");
		return;
	}

	// 選択されたものを色変える
	m_pSelect->GetObj()->SetColMulti(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));

	// 矢印の更新
	if (m_pArrow != nullptr)
	{
		m_pArrow->Update();
	}

	// モデル変更
	ModelChange();

	// 移動
	Move();

	// 削除
	Delete();

	CDebugProc::GetInstance()->Print("]\n\n");

	// 障害物情報
	if (m_pSelect == nullptr) { return; }
	CDebugProc::GetInstance()->Print("[ 道情報 : ");
	D3DXVECTOR3 pos = m_pSelect->GetPosition();
	CDebugProc::GetInstance()->Print("座標 : [ %f, %f, %f] : ", pos.x, pos.y, pos.z);
	D3DXVECTOR3 rot = m_pSelect->GetRotation();
	CDebugProc::GetInstance()->Print("向き : [ %f, %f, %f] : ", rot.x, rot.y, rot.z);
	CDebugProc::GetInstance()->Print("]\n");
}

//==========================================================
// 選択
//==========================================================
void CEdit_Obj::ClickCheck()
{
	CInputMouse* pMouse = CInputMouse::GetInstance();
	Clist<CMapObstacle*>* pList = CMapObstacle::GetList();
	float length = 10000000.0f;

	if (CMapObstacle::GetList() == nullptr) { return; }

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

	if (m_pSelect != nullptr)
	{
		m_pSelect->GetObj()->SetColMulti(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}

	m_pSelect = nullptr;

	for (int i = 0; i < pList->GetNum(); i++)
	{
		CMapObstacle* pObj = pList->Get(i);

		// 衝突した
		if (CursorCollision(pObj))
		{
			m_pSelect = pObj;

			if (m_pArrow == nullptr)
			{
				m_pArrow = CEdit_Arrow::Create(m_pSelect->GetPosition());
			}

			m_pArrow->SetPosition(m_pSelect->GetPosition());
		}
	}

	// 道が選ばれていない
	if (m_pSelect != nullptr) { return; }

	// 矢印使用中
	if (m_pArrow == nullptr) { return; }

	m_pArrow->Uninit();
	m_pArrow = nullptr;
}

//==========================================================
// 選択したものとの衝突判定
//==========================================================
bool CEdit_Obj::CursorCollision(CMapObstacle* pObj)
{
	// マウス情報
	CInputMouse* pMouse = CInputMouse::GetInstance();
	CInputMouse::SRayInfo info = pMouse->GetRayInfo();

	// 床情報
	D3DXVECTOR3 pos = pObj->GetPosition();
	D3DXVECTOR3 rot = pObj->GetRotation();
	D3DXVECTOR3 vtxmax = pObj->GetObj()->GetVtxMax();
	D3DXVECTOR3 vtxmin = pObj->GetObj()->GetVtxMin();
	D3DXVECTOR3 touchpos = VECTOR3_ZERO;
	D3DXVECTOR3 origin = pMouse->GetRayInfo().origin;
	D3DXVECTOR3 vec = pMouse->GetRayInfo().vec;

	// 使用されていなかったら返す
	if (collision::CollideRayToOBB(&touchpos, origin, vec, pos, rot, vtxmax, vtxmin))
	{
		if (m_pSelect != nullptr)
		{
			D3DXVECTOR3 vec = touchpos - origin;
			float nowlength = D3DXVec3Length(&vec);

			vec = m_pSelect->GetPosition() - origin;
			float length = D3DXVec3Length(&vec);

			if (nowlength > length)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

//==========================================================
// 削除
//==========================================================
void CEdit_Obj::Delete()
{
	CDebugProc::GetInstance()->Print(" 道削除 : Delete or BackSpace, ");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	// 入力確認
	if (!pKey->GetTrigger(DIK_DELETE) && !pKey->GetTrigger(DIK_BACKSPACE)) { return; }

	// 再接続
	m_pSelect->Uninit();
	m_pSelect = nullptr;

	// 矢印使用中
	if (m_pArrow == nullptr) { return; }

	m_pArrow->Uninit();
	m_pArrow = nullptr;
}

//==========================================================
// 移動
//==========================================================
void CEdit_Obj::Move()
{
	if (m_pSelect == nullptr) { return; }
	if (m_pArrow == nullptr) { return; }

	D3DXVECTOR3 pos = m_pArrow->GetPosition();	// 座標

	// 選択した道の座標設定
	m_pSelect->SetPosition(pos);
}

//==========================================================
// モデル変更
//==========================================================
void CEdit_Obj::ModelChange()
{
	if (m_pSelect == nullptr) { return; }
	CInputMouse* pMouse = CInputMouse::GetInstance();

	float old = m_fMouseWheel;
	m_fMouseWheel += pMouse->GetCousorMove().z;

	// マウスホイール
	if (m_fMouseWheel == old) { return; }

	if (static_cast<int>(m_fMouseWheel) % 20 != 0) { return; }

	int idx = m_pSelect->GetInfo().fileidx;

	if (m_fMouseWheel >= old)
	{
		idx = (idx + 1) % CMapManager::GetInstance()->GetFileNameList().size();
	}
	else
	{
		idx = (idx + CMapManager::GetInstance()->GetFileNameList().size() - 1) % CMapManager::GetInstance()->GetFileNameList().size();
	}

	m_pSelect->BindModel(idx);
}

//==========================================================
// 保存
//==========================================================
void CEdit_Obj::Save()
{
	CDebugProc::GetInstance()->Print(" 保存 : F7, ");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	Clist<CMapObstacle*>* pList = CMapObstacle::GetList();
	if (pList == nullptr) { return; }

	// 入力確認
	if (!pKey->GetTrigger(DIK_F7)) { return; }

	// ファイルを開く
	std::ofstream File(FILENAME, std::ios::binary);
	if (!File.is_open()) {
		return;
	}

	std::vector<CMapObstacle::SInfo> savedata;

	for (int i = 0; i < pList->GetNum(); i++)
	{
		savedata.push_back(pList->Get(i)->GetInfo());
	}

	int size = savedata.size();

	// ベクトルのサイズをセーブ
	File.write(reinterpret_cast<const char*>(&size), sizeof(size));

	// データをバイナリファイルに書き出す
	File.write(reinterpret_cast<char*>(savedata.data()), size * sizeof(CMapObstacle::SInfo));

	// ファイルを閉じる
	File.close();
}

//==========================================================
// 保存
//==========================================================
void CEdit_Obj::Create()
{
	CDebugProc::GetInstance()->Print(" 生成 : マウスホイールクリック, ");
	CInputMouse* pMouse = CInputMouse::GetInstance();

	// 入力確認
	if (!pMouse->GetTrigger(CInputMouse::BUTTON_WHEEL)) { return; }

	D3DXVECTOR3 rayDir = pMouse->GetRayInfo().vec;
	D3DXVECTOR3 rayOrigin = pMouse->GetRayInfo().origin;

	// レイが Y = 0 の平面と平行な場合（方向ベクトルの Y 成分が 0）
	if (fabs(rayDir.y) < 1e-6f) {
		return;  // 交差しない
	}

	// 交差までの距離 t を計算
	float t = -rayOrigin.y / rayDir.y;

	// t が負の場合、レイの逆方向にあるので交差しない
	if (t < 0.0f) {
		return;
	}

	CMapObstacle::SInfo info = CMapObstacle::SInfo();

	// 交点の座標を計算（Y = 0 平面上）
	info.pos.x = rayOrigin.x + t * rayDir.x;
	info.pos.y = 0.0f;  // Y = 0 なので固定
	info.pos.z = rayOrigin.z + t * rayDir.z;
	info.fileidx = 0;
	info.rot = VECTOR3_ZERO;

	CMapObstacle::Create(info);
}
