//==========================================================
//
// エディターオブジェクト [edit_object.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_object.h"
#include "manager.h"
#include "debugproc.h"
#include "edit_handle.h"
#include "map_obstacle.h"
#include "objectX.h"
#include "input_mouse.h"
#include "input_keyboard.h"
#include "map_manager.h"

namespace
{
	
}

//==========================================================
// コンストラクタ
//==========================================================
CEdit_Obj::CEdit_Obj()
{
	// 値のクリア
	m_pSelect = nullptr;
	m_pHandle = nullptr;
	m_fMouseWheel = 0.0f;
	m_startScale = VECTOR3_ZERO;
	m_startRotate = VECTOR3_ZERO;
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
	Clist<CMapObstacle*>* pList = CMapObstacle::GetList();

	if (pList != nullptr)
	{
		for (int i = 0; i < pList->GetNum(); i++)
		{
			CMapObstacle* pObj = pList->Get(i);
			pObj->GetObj()->SetColMulti(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			pObj->GetObj()->SetDraw();
		}
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CEdit_Obj::Uninit(void)
{
	if (m_pHandle != nullptr)
	{
		m_pHandle->Uninit();
		m_pHandle = nullptr;
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
	if (m_pHandle != nullptr)
	{
		m_pHandle->Update();

		if (m_pHandle->GetOldHold() == nullptr && m_pHandle->GetHold() != nullptr)
		{
			m_startScale = m_pSelect->GetScale();
			m_startRotate = m_pSelect->GetRotation();
		}
	}

	// 矢印変更
	ModeChange();

	// モデル変更
	ModelChange();

	// 移動
	Move();

	// スケール
	Scale();

	// 向き
	Rotate();

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

	if (CMapObstacle::GetList() == nullptr) { return; }

	// 入力があれば確認する
	if (!pMouse->GetTrigger(CInputMouse::BUTTON_LBUTTON))
	{
		return;
	}

	// 矢印選択中
	if (m_pHandle != nullptr)
	{
		if (m_pHandle->GetHold() != nullptr)
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

			if (m_pHandle == nullptr)
			{
				m_pHandle = CEdit_Handle::Create(m_pSelect->GetPosition(), CEdit_Handle::TYPE_MOVE);
			}

			m_pHandle->SetPosition(m_pSelect->GetPosition());
		}
	}

	// 道が選ばれていない
	if (m_pSelect != nullptr) { return; }

	// 矢印使用中
	if (m_pHandle == nullptr) { return; }

	m_pHandle->Uninit();
	m_pHandle = nullptr;
}

//==========================================================
// 選択したものとの衝突判定
//==========================================================
bool CEdit_Obj::CursorCollision(CMapObstacle* pObj)
{
	// マウス情報
	CInputMouse* pMouse = CInputMouse::GetInstance();
	CInputMouse::SRayInfo info = pMouse->GetRayInfo();
	if (pObj->GetObj() == nullptr) { return false; }

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
			vec = touchpos - origin;
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
	if (m_pHandle == nullptr) { return; }

	m_pHandle->Uninit();
	m_pHandle = nullptr;
}

//==========================================================
// 移動
//==========================================================
void CEdit_Obj::Move()
{
	if (m_pSelect == nullptr) { return; }
	if (m_pHandle == nullptr) { return; }
	if (m_pHandle->GetType() != CEdit_Handle::TYPE_MOVE) { return; }

	D3DXVECTOR3 pos = m_pHandle->GetPosition();	// 座標

	// 選択した道の座標設定
	m_pSelect->SetPosition(pos);
}

//==========================================================
// スケール
//==========================================================
void CEdit_Obj::Scale()
{
	if (m_pSelect == nullptr) { return; }
	if (m_pHandle == nullptr) { return; }
	if (m_pHandle->GetType() != CEdit_Handle::TYPE_SCALE) { return; }
	if (m_pHandle->GetHold() == nullptr) { return; }

	// ハンドルの移動から変更スケール取得
	D3DXVECTOR3 handlescale = m_pHandle->GetDiffPosition();	// 座標
	handlescale *= 0.001f;
	handlescale += D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	// 調整
	D3DXVECTOR3 scale = m_startScale;
	scale.x *= handlescale.x;
	scale.y *= handlescale.y;
	scale.z *= handlescale.z;

	// 選択した道の座標設定
	m_pSelect->SetScale(scale);
}

//==========================================================
// 回転
//==========================================================
void CEdit_Obj::Rotate()
{
	if (m_pSelect == nullptr) { return; }
	if (m_pHandle == nullptr) { return; }
	if (m_pHandle->GetType() != CEdit_Handle::TYPE_ROT) { return; }
	if (m_pHandle->GetHold() == nullptr) { return; }

	// ハンドルの移動から変更スケール取得
	D3DXVECTOR3 handlerotate = m_pHandle->GetDiffRotation();	// 座標

	// 調整
	D3DXVECTOR3 rotate = m_startRotate;
	rotate.x += handlerotate.x;
	rotate.y += handlerotate.y;
	rotate.z += handlerotate.z;

	// 選択した障害物の向き設定
	m_pSelect->SetRotation(rotate);
}

//==========================================================
// 回転リセット
//==========================================================
void CEdit_Obj::RotateReset()
{
	if (m_pSelect == nullptr) { return; }
	CDebugProc::GetInstance()->Print("回転リセット : Enter, ");

	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	// 入力確認
	if (!pKey->GetTrigger(DIK_RETURN)) { return; }

	m_pSelect->SetRotation(VECTOR3_ZERO);
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
	std::ofstream File(EDITFILENAME::OBSTACLE, std::ios::binary);
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
	info.scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	CMapObstacle::Create(info);
}

//==========================================================
// 状態変更
//==========================================================
void CEdit_Obj::ModeChange()
{
	if (m_pHandle == nullptr) { return; }

	CInputMouse* pMouse = CInputMouse::GetInstance();
	CDebugProc::GetInstance()->Print("状態変更 : [ マウスホイールクリック ] : ");

	// 入力確認
	if (!pMouse->GetTrigger(CInputMouse::BUTTON_WHEEL)) { return; }

	// 種類変更
	int type = m_pHandle->GetType();
	type = (type + 1) % CEdit_Handle::TYPE_MAX;

	// 廃棄
	m_pHandle->Uninit();
	m_pHandle = nullptr;
	m_pHandle = CEdit_Handle::Create(m_pSelect->GetPosition(), static_cast<CEdit_Handle::TYPE>(type));
}