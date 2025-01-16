//==========================================================
//
// ロボット配置エディタ [edit_robot.cpp]
// Author : Ryosuke Oohara
//
//==========================================================
#include "edit_robot.h"
#include "input_mouse.h"
#include "input_keyboard.h"
#include "debugproc.h"
#include "goal.h"
#include "goal_manager.h"
#include "edit_handle.h"
#include "robot.h"
#include "robot_manager.h"

namespace
{
	const float MIN_LENGTH = 50.0f;	// 最小移動量
	const D3DXVECTOR2 SET_SIZE = D3DXVECTOR2(500.0f, 500.0f);
	const float CHANGESIZE = 50.0f;
	const float MAX_SIZE = 1500.0f;
	const float MIN_SIZE = 250.0f;
	const float DEFAULT_DISTANCE = 1000;  // デフォルトの長さ
}

//==========================================================
// コンストラクタ
//==========================================================
CEdit_Robot::CEdit_Robot()
{
	// 値のクリア
	m_pSelect = nullptr;
	m_pHandle = nullptr;
	m_fMouseWheel = 0.0f;
}

//==========================================================
// デストラクタ
//==========================================================
CEdit_Robot::~CEdit_Robot()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CEdit_Robot::Init(void)
{
	m_fMoveLength = MIN_LENGTH;

	auto mgr = CGoalManager::GetInstance();
	auto list = mgr->GetList();
	auto infolist = mgr->GetInfoList();
	int num = list.GetNum();

	std::vector<int> id;
	id.clear();

	for (const auto& ite : *list.GetList())
	{
		id.push_back(ite.first);
	}

	for (int i = 0; i < num; i++)
	{
		CGoal* pGoal = list.Get(id[i]);
		if (pGoal == nullptr) { continue; }
		pGoal->Uninit();
	}

	// ゴールをすべて削除して改めて生成する
	for (int i = 0; i < infolist->size(); i++)
	{
		auto it = (*infolist)[i];
		CGoal::Create(it.pos, it.fRange, it.fLimit, i);
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CEdit_Robot::Uninit(void)
{
	// 矢印終了
	if (m_pHandle != nullptr)
	{
		m_pHandle->Uninit();
		m_pHandle = nullptr;
	}

	m_pSelect = nullptr;

	CEdit::Uninit();
}

//==========================================================
// 更新処理
//==========================================================
void CEdit_Robot::Update(void)
{
	CDebugProc::GetInstance()->Print(" [ ロボット配置モード ]\n");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();
	auto* pOld = m_pSelect;

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

	// 矢印の更新
	if (m_pHandle != nullptr)
	{
		m_pHandle->Update();
	}

	// 移動
	Move();

	// 削除
	Delete();

	CDebugProc::GetInstance()->Print("]\n\n");

	// ゴール情報
	if (m_pSelect == nullptr) { return; }
	CDebugProc::GetInstance()->Print("[ ロボット情報 : ");
	D3DXVECTOR3 pos = m_pSelect->GetPosition();
	CDebugProc::GetInstance()->Print("座標 : [ %f, %f, %f ] : ", pos.x, pos.y, pos.z);
	CDebugProc::GetInstance()->Print("]\n");
}

//==========================================================
// 選択
//==========================================================
void CEdit_Robot::ClickCheck()
{
	CInputMouse* pMouse = CInputMouse::GetInstance();
	auto list = CGoalManager::GetInstance()->GetList();

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

	//m_pSelect = nullptr;
	auto mgr = CGoalManager::GetInstance()->GetList();

	CRobot* pTop = CRobotManager::GetInstance()->GetTop();

	while (pTop != nullptr)
	{
		CRobot* pNext = pTop->GetNext();

		// 衝突判定
		if (CursorCollision(pTop))
		{
			m_pSelect = pTop;

			if (m_pHandle == nullptr)
			{
				m_pHandle = CEdit_Handle::Create(m_pSelect->GetPosition(), CEdit_Handle::TYPE_MOVE);
			}

			// 位置設定
			m_pHandle->SetPosition(m_pSelect->GetPosition());

			return;
		}

		pTop = pNext;
	}

	// ゴールが選ばれていない
	if (m_pSelect != nullptr) { return; }

	// 矢印使用中
	if (m_pHandle == nullptr) { return; }

	m_pHandle->Uninit();
	m_pHandle = nullptr;
}

//==========================================================
// 選択したものとの衝突判定
//==========================================================
bool CEdit_Robot::CursorCollision(CRobot* pRobot)
{
	// マウス情報
	CInputMouse* pMouse = CInputMouse::GetInstance();
	CInputMouse::SRayInfo info = pMouse->GetRayInfo();

	// ロボットの情報
	D3DXVECTOR3 pos = pRobot->GetPosition();
	float range = 50.0f;

	D3DXVECTOR3 pos0 = pos + D3DXVECTOR3(-range, 0.0f, range), pos1 = pos + D3DXVECTOR3(range, 0.0f, range),
		pos2 = pos + D3DXVECTOR3(-range, 0.0f, -range);

	// 三角形と線分の衝突確認
	if (TriangleCollision(info.origin, info.vec, pos0, pos1, pos2))
	{
		return true;
	}

	// 反対の三角へ
	pos0 = pos + D3DXVECTOR3(range, 0.0f, -range);

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
bool CEdit_Robot::TriangleCollision(const D3DXVECTOR3& rayOrigin, const D3DXVECTOR3& rayDir,
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
// 削除
//==========================================================
void CEdit_Robot::Delete()
{
	CDebugProc::GetInstance()->Print(" ロボット削除 : Delete or BackSpace, ");
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
void CEdit_Robot::Move()
{
	if (m_pSelect == nullptr) { return; }
	if (m_pHandle == nullptr) { return; }

	D3DXVECTOR3 pos = m_pSelect->GetPosition();	// 座標
	D3DXVECTOR3 Handlepos = m_pHandle->GetPosition();	// 矢印座標

	// X座標
	float length = fabsf(Handlepos.x - pos.x);
	int setpos = static_cast<int>(Handlepos.x);
	int movelength = static_cast<int>(m_fMoveLength);

	// 大きい
	if (length >= m_fMoveLength)
	{
		pos.x = static_cast<float>(setpos - (setpos % movelength));
	}

	// Y座標
	length = fabsf(Handlepos.y - pos.y);
	setpos = static_cast<int>(Handlepos.y);
	movelength = static_cast<int>(m_fMoveLength);

	// 大きい
	if (length >= m_fMoveLength)
	{
		pos.y = static_cast<float>(setpos - (setpos % movelength));
	}

	// Z座標
	length = fabsf(Handlepos.z - pos.z);
	setpos = static_cast<int>(Handlepos.z);
	movelength = static_cast<int>(m_fMoveLength);

	// 大きい
	if (length >= m_fMoveLength)
	{
		pos.z = static_cast<float>(setpos - (setpos % movelength));
	}

	// 選択したゴールの座標設定
	m_pSelect->SetPosition(pos);
}

//==========================================================
// 保存
//==========================================================
void CEdit_Robot::Save()
{
	CDebugProc::GetInstance()->Print(" 保存 : F7, ");
	CInputKeyboard* pKey = CInputKeyboard::GetInstance();

	// 入力確認
	if (!pKey->GetTrigger(DIK_F7)) { return; }

	// ファイルを開く
	std::ofstream File(EDITFILENAME::GOAL, std::ios::binary);
	if (!File.is_open()) {
		return;
	}

	std::vector<CRobot::SInfo> savedata;

	CRobot* pTop = CRobotManager::GetInstance()->GetTop();

	while (pTop != nullptr)
	{
		CRobot* pNext = pTop->GetNext();

		savedata.push_back(pTop->GetInfo());

		pTop = pNext;
	}

	//auto mgr = CGoalManager::GetInstance()->GetList();

	//// ゴールを全て確認
	//for (auto ite = mgr.GetBegin(); ite != mgr.GetEnd(); ite++)
	//{
	//	savedata.push_back(*ite->second->GetInfo());
	//}

	int size = savedata.size();

	// ベクトルのサイズをセーブ
	File.write(reinterpret_cast<const char*>(&size), sizeof(size));

	// データをバイナリファイルに書き出す
	File.write(reinterpret_cast<char*>(savedata.data()), size * sizeof(CGoal::SInfo));

	// ファイルを閉じる
	File.close();
}

//==========================================================
// 保存
//==========================================================
void CEdit_Robot::Create()
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

	D3DXVECTOR3 pos, rot;

	// 交点の座標を計算（Y = 0 平面上）
	pos.x = rayOrigin.x + t * rayDir.x;
	pos.y = 0.0f;  // Y = 0 なので固定
	pos.z = rayOrigin.z + t * rayDir.z;

	// 端数切捨て
	int setpos = static_cast<int>(pos.x);
	int movelength = static_cast<int>(MIN_LENGTH);
	pos.x -= setpos % movelength;
	setpos = static_cast<int>(pos.z);
	pos.z -= setpos % movelength;
	rot = VECTOR3_ZERO;

	CRobot* pRobot = CRobot::Create(pos, rot, DEFAULT_DISTANCE);
	pRobot->Update();
}

//==========================================================
// 状態変更
//==========================================================
void CEdit_Robot::ModeChange()
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