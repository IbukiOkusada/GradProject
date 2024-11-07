//==========================================================
//
// ゴール [goal.cpp]
// Author : 丹野竜之介
//
//==========================================================
#include "goal.h"
#include "player_manager.h"
#include "player.h"
#include "convenience.h"
#include "meshcylinder.h"
#include "particle2D.h"
#include "camera_manager.h"
#include "camera.h"
#include "renderer.h"
#include "manager.h"
#include "debugproc.h"
#include "camera_action.h"

// マクロ定義
Clist<CGole*> * CGole::pList = nullptr;
//==========================================================
// コンストラクタ
//==========================================================
CGole::CGole()
{
	pEffect = nullptr;
	m_fRange = 0.0f;
	m_fLimit = 0.0f;
	//自身をリストに登録
	GetInstance()->Regist(this);
}

//==========================================================
// デストラクタ
//==========================================================
CGole::~CGole()
{
	//自身をリストから削除
	GetInstance()->Delete(this);
	if (GetInstance()->Empty())
	{//リストの中身が空ならリストを破棄
		ListRelease();
	}
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CGole::Init(void)
{
	pEffect = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\goal_radius.efkefc", m_pos, VECTOR3_ZERO, VECTOR3_ZERO, m_fRange, false, false);
	
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CGole::Uninit(void)
{
	SAFE_DELETE(pEffect);
	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CGole::Update(void)
{
	// エフェクト表示
	ScreenEffect();

	if (CheckRange() && CheckSpeed())
	{
		// カメラアクション入れる
		CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
		CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
		pCamera->GetAction()->Set(pCamera, D3DXVECTOR3(0.0f, pPlayer->GetRotation().y + D3DX_PI, D3DX_PI* 0.4f), 500.0f, 2.0f, 2.0f, CCameraAction::MOVE_POSV);
		pPlayer->AddDeliveryCount();
		Uninit();
	}
}
//==========================================================
// 距離のチェック
//==========================================================
bool CGole::CheckRange()
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	if (pPlayer != NULL)
	{
		float fDis = GetDistance(m_pos, pPlayer->GetPosition());
		return (m_fRange >= fDis);
	}
	return false;
}
//==========================================================
// 速度のチェック
//==========================================================
bool CGole::CheckSpeed()
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	if (pPlayer != NULL)
	{
		float fDis = GetDistance(VECTOR3_ZERO, pPlayer->GetMove());
		return (m_fLimit >= fDis);
	}
	return false;
}
//==========================================================
// 生成
//==========================================================
CGole* CGole::Create(D3DXVECTOR3 pos, float fRange, float fLimit)
{
	CGole* pGoal = nullptr;

	pGoal = DEBUG_NEW CGole;

	if (pGoal != nullptr)
	{
		pGoal->m_pos = pos;
		pGoal->m_fRange = fRange;
		pGoal->m_fLimit = fLimit;
		// 初期化処理
		pGoal->Init();
	}

	return pGoal;
}

//==========================================================
// 画面上にエフェクトの表示
//==========================================================
void CGole::ScreenEffect()
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();
	CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
	D3DXMATRIX mtxProjection, mtxView, mtxWorld;
	D3DVIEWPORT9 Viewport;
	D3DXVECTOR3 pos = VECTOR3_ZERO;

	// 必要な情報取得
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetViewport(&Viewport);

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// スクリーン座標取得
	D3DXVec3Project(&pos, &m_pos, &Viewport, &mtxProjection, &mtxView, &mtxWorld);

	// 画面内なら出さない
	if(pos.x > 0.0f && pos.x < SCREEN_WIDTH &&
		pos.y > 0.0f && pos.y < SCREEN_HEIGHT) {
		return;
	}

	if (pos.z > 1.0f) { pos *= -1.0f; }

	// 補正
	if (pos.x > SCREEN_WIDTH) { pos.x = SCREEN_WIDTH; }
	else if (pos.x < 0.0f) { pos.x = 0.0f; }
	if (pos.y > SCREEN_HEIGHT) { pos.y = SCREEN_HEIGHT; }
	else if (pos.y < 0.0f) { pos.y = 0.0f; }

	CDebugProc::GetInstance()->Print("ゴールの画面座標 : [ %f, %f, %f ] \n", pos.x, pos.y, pos.z);

	CParticle2D::Create(pos, CEffect2D::TYPE_TARGET);
}
