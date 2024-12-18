//==========================================================
// 
//タイトルでのプレイヤ―の動き処理 [player_result.cpp]
//Author : Kenta Hashimoto
// 
//==========================================================
#include "player_result.h"
#include "camera.h"
#include "camera_manager.h"
#include "debugproc.h"
#include "objectX.h"

//==========================================================
// 定数定義
//==========================================================
namespace POS
{
	D3DXVECTOR3 PATTERN_1 = D3DXVECTOR3(0.0f, 100.0f, 0.0f);
	D3DXVECTOR3 PATTERN_2 = D3DXVECTOR3(15250.0f, 100.0f, 14000.0f);
}

namespace ENDPOS
{
	D3DXVECTOR3 PATTERN_1 = D3DXVECTOR3(10000.0f, 100.0f, 0.0f);
	D3DXVECTOR3 PATTERN_2 = D3DXVECTOR3(15250.0f, 100.0f, -7000.0f);
}

namespace ROTATION
{
	D3DXVECTOR3 PATTERN_1 = D3DXVECTOR3(0.0f, -D3DX_PI / 2, 0.0f);
	D3DXVECTOR3 PATTERN_2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	D3DXVECTOR3 ANGLE = D3DXVECTOR3(0.0f, 0.8f + D3DX_PI / 2, 1.2f);
}

namespace MOVE
{
	D3DXVECTOR3 PATTERN_1 = D3DXVECTOR3(18.0f, 0.0f, 0.0f);
	D3DXVECTOR3 PATTERN_2 = D3DXVECTOR3(0.0f, 0.0f, -18.0f);
}

namespace CAMERAANGLE
{
	D3DXVECTOR3 PATTERN_1 = D3DXVECTOR3(800.0f, 340.0f, 800.0f);
	D3DXVECTOR3 PATTERN_2 = D3DXVECTOR3(-800.0f, 340.0f, 800.0f);
}
//==========================================================
// コンストラクタ
//==========================================================
CPlayerResult::CPlayerResult()
{
	m_bStartPtn = true;
}

//==========================================================
// デストラクタ
//==========================================================
CPlayerResult::~CPlayerResult()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CPlayerResult::Init(void)
{
	CPlayer::Init();

	return S_OK;
}

//==========================================================
// 初期化処理(オーバーロード)
//==========================================================
HRESULT CPlayerResult::Init(const char* pBodyName, const char* pLegName)
{
	m_pObj = CObjectX::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), "data\\MODEL\\bike.x");
	m_pObj->SetType(CObject::TYPE_PLAYER);
	m_pObj->SetRotateType(CObjectX::TYPE_QUATERNION);
	SetMatrix();
	SetCol();

	//エフェクト生成
	m_pAfterburner = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\afterburner.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);

	m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\trail.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 10.0f, false, false);
	m_pBackdust = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\backdust.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);

	//カメラ初期化
	{
		CManager::GetInstance()->GetCamera()->SetLength(300.0f);
		CManager::GetInstance()->GetCamera()->SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		CManager::GetInstance()->GetCamera()->SetPositionR(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

		D3DVIEWPORT9 viewport;

		//プレイヤー追従カメラの画面位置設定
		viewport.X = 0;
		viewport.Y = 0;
		viewport.Width = (DWORD)(SCREEN_WIDTH * 1.0f);
		viewport.Height = (DWORD)(SCREEN_HEIGHT * 1.0f);
		viewport.MinZ = 0.0f;
		viewport.MaxZ = 1.0f;
		CManager::GetInstance()->GetCamera()->SetViewPort(viewport);
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CPlayerResult::Uninit(void)
{
	CPlayer::Uninit();
}

//==========================================================
// 更新処理
//==========================================================
void CPlayerResult::Update(void)
{
	MovePtnSelect();
	Move();
	MoveEnd();

	//====================================
	//エフェクトを出す
	//====================================
	{
		m_pTailLamp->m_pos = GetPosition();
		m_pTailLamp->m_rot = GetRotation();
		m_pBackdust->m_pos = GetPosition();
		m_pBackdust->m_rot = GetRotation();
		m_pBackdust->m_Scale = VECTOR3_ONE * 50.0f;
		m_pAfterburner->m_pos = GetPosition();
		m_pAfterburner->m_Scale = VECTOR3_ONE * m_fBrake * 150.0f;
	}

	// デバッグ表示
	CDebugProc::GetInstance()->Print("座標: [ %f, %f, %f ]", m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);
}

//==========================================================
// 動きに関する処理
//==========================================================
void CPlayerResult::Moving()
{

}

//==========================================================
// 生成処理
//==========================================================
CPlayerResult* CPlayerResult::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move
	, const char* pBodyName, const char* pLegName)
{
	CPlayerResult* pPlayerResult = DEBUG_NEW CPlayerResult;

	if (pPlayerResult)
	{// 生成できた場合
		// 初期化処理
		pPlayerResult->Init(pBodyName, pLegName);

		// 座標設定
		pPlayerResult->SetPosition(pos);

		// 向き設定
		pPlayerResult->SetRotation(rot);

		pPlayerResult->m_fRotDest = rot.y;

		// 移動量設定
		pPlayerResult->SetMove(move);
	}
	else
	{
		return nullptr;
	}

	return pPlayerResult;
}

//==========================================================
// 動きに関する処理
//==========================================================
void CPlayerResult::Move()
{
	D3DXVECTOR3 PosCamera = CManager::GetInstance()->GetCamera()->GetPositionR();

	m_Info.posOld = m_Info.pos;

	m_Info.pos += m_Info.move;
	PosCamera += m_Info.move;

	m_pObj->SetPosition(GetPosition());
	m_pObj->SetRotation(GetRotation());

	CManager::GetInstance()->GetCamera()->SetPositionR(PosCamera);
}

//==========================================================
// 動きのパターンを選ぶ処理
//==========================================================
void CPlayerResult::MovePtnSelect()
{
	if (m_bStartPtn == true)
	{
		D3DXVECTOR3 CameraAngle = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		int nRand = rand() % 2;

		switch (nRand)
		{
		case 0:
			SetPosition(POS::PATTERN_1);
			SetMove(MOVE::PATTERN_1);
			SetRotation(ROTATION::PATTERN_1);
			CameraAngle = CAMERAANGLE::PATTERN_1;
			m_EndPos = ENDPOS::PATTERN_1;

			m_bStartPtn = false;
			break;

		case 1:
			SetPosition(POS::PATTERN_2);
			SetMove(MOVE::PATTERN_2);
			SetRotation(ROTATION::PATTERN_2);
			CameraAngle = CAMERAANGLE::PATTERN_2;
			m_EndPos = ENDPOS::PATTERN_2;

			m_bStartPtn = false;
			break;

		default:
			break;
		}
		D3DXVECTOR3 PosCameraR = CManager::GetInstance()->GetCamera()->GetPositionR();
		D3DXVECTOR3 RotCamera = CManager::GetInstance()->GetCamera()->GetRotation();

		PosCameraR = m_Info.pos + CameraAngle;
		RotCamera = m_Info.rot + ROTATION::ANGLE;

		CManager::GetInstance()->GetCamera()->SetPositionR(PosCameraR);
		CManager::GetInstance()->GetCamera()->SetRotation(RotCamera);
		m_pObj->SetPosition(GetPosition());
		m_pObj->SetRotation(GetRotation());
	}
}

//==========================================================
// 目的地に着いたか判定
//==========================================================
void CPlayerResult::MoveEnd()
{
	if (GetPosition().x >= m_EndPos.x && GetOldPosition().x < m_EndPos.x ||
		GetPosition().x <= m_EndPos.x && GetOldPosition().x > m_EndPos.x)
	{
		m_bStartPtn = true;
	}

	else if (GetPosition().y >= m_EndPos.y && GetOldPosition().y < m_EndPos.y ||
		GetPosition().y <= m_EndPos.y && GetOldPosition().y > m_EndPos.y)
	{
		m_bStartPtn = true;
	}

	else if (GetPosition().z >= m_EndPos.z && GetOldPosition().z < m_EndPos.z ||
		GetPosition().z <= m_EndPos.z && GetOldPosition().z > m_EndPos.z)
	{
		m_bStartPtn = true;
	}

}