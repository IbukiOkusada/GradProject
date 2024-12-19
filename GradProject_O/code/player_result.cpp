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
#include "fade.h"
#include "character.h"

//==========================================================
// 定数定義
//==========================================================
namespace POS
{
	
	D3DXVECTOR3 PATTERN_1 = D3DXVECTOR3(-8000.0f, 100.0f, 0.0f);
	D3DXVECTOR3 PATTERN_2 = D3DXVECTOR3(15250.0f, 100.0f, 11000.0f);
}

namespace ENDPOS
{
	D3DXVECTOR3 PATTERN_1 = D3DXVECTOR3(2950.0f, 100.0f, 0.0f);
	D3DXVECTOR3 PATTERN_2 = D3DXVECTOR3(15250.0f, 100.0f, 300.0f);
}

namespace ROTATION
{
	D3DXVECTOR3 PATTERN_1 = D3DXVECTOR3(0.0f, -D3DX_PI / 2, 0.0f);
	D3DXVECTOR3 PATTERN_2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	D3DXVECTOR3 ANGLE_1 = D3DXVECTOR3(0.0f, 0.8f + D3DX_PI / 2, 1.2f);
	D3DXVECTOR3 ANGLE_2 = D3DXVECTOR3(0.0f, -0.8f + D3DX_PI / 2, 1.2f);
	float MOVE = 0.06f;
}

namespace ENDROTATION
{
	D3DXVECTOR3 PATTERN_1 = D3DXVECTOR3(0.0f, -D3DX_PI, 0.0f);
	D3DXVECTOR3 PATTERN_2 = D3DXVECTOR3(0.0f, -D3DX_PI / 2, 0.0f);
}

namespace MOVE
{
	float CONST_VAULE = 18.0f;

	D3DXVECTOR3 PATTERN_1 = D3DXVECTOR3(CONST_VAULE, 0.0f, 0.0f);
	D3DXVECTOR3 PATTERN_2 = D3DXVECTOR3(0.0f, 0.0f, -CONST_VAULE);
}

namespace CAMERAANGLE
{
	D3DXVECTOR3 PATTERN_1 = D3DXVECTOR3(800.0f, 340.0f, 800.0f);
	D3DXVECTOR3 PATTERN_2 = D3DXVECTOR3(-800.0f, 340.0f, 800.0f);
	D3DXVECTOR3 PATTERN_3 = D3DXVECTOR3(800.0f, 340.0f, -800.0f);
	D3DXVECTOR3 PATTERN_4 = D3DXVECTOR3(-800.0f, 340.0f, -800.0f);
}
//==========================================================
// コンストラクタ
//==========================================================
CPlayerResult::CPlayerResult()
{
	m_bStartPtn = true;
	m_bEndPtn = false;
	m_FadeStartCnt = 0;
	m_CameraAngle = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
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
	CPlayer::Init(pBodyName, pLegName);

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
	if (m_bEndPtn == false)
	{
		MoveEnd();
	}
	else if (m_bEndPtn == true)
	{
		MoveRot();
		m_FadeStartCnt++;

		if (m_FadeStartCnt >= 60 && CManager::GetInstance()->GetFade()->GetState() == CFade::STATE_NONE)
		{
			CManager::GetInstance()->GetFade()->Set(CScene::MODE_TITLE);
		}
	}

	// キャラクター更新
	if (m_pCharacter != nullptr)
	{
		m_pCharacter->Update();
	}

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

	if (m_bEndPtn == false)
	{
		CManager::GetInstance()->GetCamera()->SetPositionR(PosCamera);
	}
}

//==========================================================
// 動きのパターンを選ぶ処理
//==========================================================
void CPlayerResult::MovePtnSelect()
{
	if (m_bStartPtn == true)
	{
		D3DXVECTOR3 CameraAngle;

		int nRand = rand() % 2;
		nRand = 1;
		switch (nRand)
		{
		case 0:
			
			MovePtnSet(POS::PATTERN_1, MOVE::PATTERN_1, ROTATION::PATTERN_1,
				CAMERAANGLE::PATTERN_3, ENDPOS::PATTERN_1, ENDROTATION::PATTERN_1, TYPE_LEFT);

			CameraAngle = ROTATION::ANGLE_2;
			break;

		case 1:
			
			MovePtnSet(POS::PATTERN_2, MOVE::PATTERN_2, ROTATION::PATTERN_2,
				CAMERAANGLE::PATTERN_2, ENDPOS::PATTERN_2, ENDROTATION::PATTERN_2, TYPE_LEFT);
			
			CameraAngle = ROTATION::ANGLE_1;
			break;

		default:
			break;
		}
		D3DXVECTOR3 PosCameraR = CManager::GetInstance()->GetCamera()->GetPositionR();
		D3DXVECTOR3 RotCamera = CManager::GetInstance()->GetCamera()->GetRotation();

		PosCameraR = m_Info.pos + m_CameraAngle;
		RotCamera = m_Info.rot + CameraAngle;

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
		m_bEndPtn = true;
	}

	else if (GetPosition().y >= m_EndPos.y && GetOldPosition().y < m_EndPos.y ||
		GetPosition().y <= m_EndPos.y && GetOldPosition().y > m_EndPos.y)
	{
		m_bEndPtn = true;
	}

	else if (GetPosition().z >= m_EndPos.z && GetOldPosition().z < m_EndPos.z ||
		GetPosition().z <= m_EndPos.z && GetOldPosition().z > m_EndPos.z)
	{
		m_bEndPtn = true;
	}

}

//==========================================================
// 目的地に着いたか判定
//==========================================================
void CPlayerResult::MoveRot()
{
	D3DXVECTOR3 rot = GetRotation();
	D3DXVECTOR3 Oldrot = GetRotation();
	D3DXVECTOR3 move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	switch (m_type)
	{
	case CPlayerResult::TYPE_RIGHT:

		rot.y += ROTATION::MOVE;

		if (rot.y >= m_EndRot.y && Oldrot.y <= m_EndRot.y)
		{
			rot = m_EndRot;
		}
		move.x = -sinf(rot.y) * MOVE::CONST_VAULE;
		move.z = -cosf(rot.y) * MOVE::CONST_VAULE;

		SetMove(move);
		SetRotation(rot);
		m_pObj->SetRotation(GetRotation());

		break;

	case CPlayerResult::TYPE_LEFT:
		rot.y -= ROTATION::MOVE;

		if (rot.y <= m_EndRot.y && Oldrot.y >= m_EndRot.y)
		{
			rot = m_EndRot;
		}
		move.x = -sinf(rot.y) * MOVE::CONST_VAULE;
		move.z = -cosf(rot.y) * MOVE::CONST_VAULE;

		SetMove(move);
		SetRotation(rot);
		m_pObj->SetRotation(GetRotation());

		break;

	default:
		break;
	}
}

//==========================================================
// 動きのパターンを設定
//==========================================================
void CPlayerResult::MovePtnSet(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const D3DXVECTOR3 rot,
	const D3DXVECTOR3 angle, const D3DXVECTOR3 Endpos, const D3DXVECTOR3 Endrot, const ROTATE type)
{
	SetPosition(pos);
	SetMove(move); 
	SetRotation(rot);
	m_CameraAngle = angle;
	m_EndPos = Endpos;
	m_EndRot = Endrot;
	m_type = type;

	m_bStartPtn = false;
}