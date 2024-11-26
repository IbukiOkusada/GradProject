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
#include "deltatime.h"
#include "baggage.h"
#include "road_manager.h"
#include "character.h"
#include "motion.h"
#include "model.h"
#include "network.h"

namespace
{
	const D3DXVECTOR3 SCALE = D3DXVECTOR3(2.0f, 2.0f, 2.0f);
}

// 静的メンバ変数
Clist<CGoal*> * CGoal::pList = nullptr;
//==========================================================
// コンストラクタ
//==========================================================
CGoal::CGoal()
{
	pEffect = nullptr;
	m_fRange = 0.0f;
	m_fLimit = 0.0f;
	m_bEnd = false;
	m_pPeople = nullptr;
	m_pBaggage = nullptr;
	m_nId = GetInstance()->GetNum();
	//自身をリストに登録
	GetInstance()->Regist(this);
}

//==========================================================
// デストラクタ
//==========================================================
CGoal::~CGoal()
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
HRESULT CGoal::Init(void)
{
	pEffect = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\goal_radius.efkefc", m_pos, VECTOR3_ZERO, VECTOR3_ZERO, m_fRange, false, false);
	float range = static_cast<float>(rand() % 629 - 314) * 0.01f;
	D3DXVECTOR3 pos = m_pos;
	pos.x += sinf(range) * 800.0f;
	pos.z += cosf(range) * 800.0f;
	m_pPeople = CCharacter::Create(pos, VECTOR3_ZERO, "data\\TXT\\motion_kidsboy.txt");
	m_pPeople->SetScale(SCALE);
	D3DXVECTOR3 rot = VECTOR3_ZERO;
	rot.y = atan2f(pos.x - m_pos.x, pos.z - m_pos.z);
	m_pPeople->SetRotation(rot);

	Clist<CRoad*>* List = CRoadManager::GetInstance()->GetList();
	float fDis = FLT_MAX;
	m_pRoad = nullptr;
	for (int i = 0; i < List->GetNum(); i++)
	{
		float F = GetDistance(List->Get(i)->GetPosition(), m_pos);
		if (F < fDis && List->Get(i)->GetType() !=CRoad::TYPE_NONE)
		{
			fDis = F;
			m_pRoad = List->Get(i);
		}
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CGoal::Uninit(void)
{
	SAFE_UNINIT(m_pBaggage);
	SAFE_UNINIT(m_pPeople);
	SAFE_DELETE(pEffect);
	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CGoal::Update(void)
{
	// エフェクト表示
	//ScreenEffect();

	if (m_pPeople != nullptr)
	{
		m_pPeople->Update();
	}

	if (!m_bEnd)
	{
		auto net = CNetWork::GetInstance();
		int nId = -2;

		// オンラインとシングルで判定を変える
		if (net->GetState() == CNetWork::STATE::STATE_ONLINE)
		{
			// 全員確認
			for (int i = 0; i < NetWork::MAX_CONNECT; i++)
			{
				if (CheckRange(i) && CheckSpeed(i))
				{
					nId = i;
					break;
				}
			}
		}
		else
		{
			// 本人確認
			if (CheckRange(net->GetIdx()) && CheckSpeed(net->GetIdx()))
			{
				nId = net->GetIdx();
			}
		}

		// 誰かがゴール
		if (nId >= -1)
		{
			SetEnd(nId);
		}
	}
	else
	{
		SAFE_DELETE(pEffect);
	}

	// 到着
	if (m_pBaggage == nullptr) { return; }
	if (m_pBaggage->GetState() == CBaggage::STATE::STATE_THROW) { return; }
	m_pBaggage->GetObj()->SetParent(m_pPeople->GetParts(6)->GetMtx());
	m_pPeople->GetMotion()->BlendSet(4);
	m_pBaggage->SetThrowScale(m_pPeople->GetScale());
}
//==========================================================
// 距離のチェック
//==========================================================
bool CGoal::CheckRange(int nId)
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(nId);
	if (pPlayer != nullptr)
	{
		float fDis = GetDistance(m_pos, pPlayer->GetPosition());
		return (m_fRange >= fDis);
	}
	return false;
}
//==========================================================
// 速度のチェック
//==========================================================
bool CGoal::CheckSpeed(int nId)
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(nId);
	if (pPlayer != nullptr)
	{
		float fDis = GetDistance(VECTOR3_ZERO, pPlayer->GetMove());
		return (m_fLimit >= fDis);
	}
	return false;
}
//==========================================================
// 生成
//==========================================================
CGoal* CGoal::Create(D3DXVECTOR3 pos, float fRange, float fLimit)
{
	CGoal* pGoal = nullptr;

	pGoal = DEBUG_NEW CGoal;

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
void CGoal::ScreenEffect()
{
	if (m_bEnd) { return; }
	if (CDeltaTime::GetInstance()->GetSlow() < 1.0f) { return; }
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

	CParticle2D::Create(pos, CEffect2D::TYPE_TARGET);
}

//==========================================================
// ゴールアクション
//==========================================================
void CGoal::SetEnd(int nId)
{
	if (m_bEnd) { return; }

	// カメラアクション入れる
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(nId);
	m_bEnd = true;
	m_pBaggage = pPlayer->ThrowBaggage(m_pPeople->GetParts(6)->GetMtxPos());
	pPlayer->AddDeliveryCount();
	m_pPeople->GetMotion()->BlendSet(3);
}