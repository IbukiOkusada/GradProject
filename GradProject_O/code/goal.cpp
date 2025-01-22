//==========================================================
//
// ゴール [goal.cpp]
// Author : 丹野竜之介
//
//==========================================================
#include "goal.h"
#include "goal_manager.h"
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
#include "objectX.h"
#include "game.h"

namespace
{
	const D3DXVECTOR3 SCALE = D3DXVECTOR3(3.0f, 3.0f, 3.0f);

	const int SET_COL_MAX = (50);				// 設定する色の最大値（100 = 1.0）
	const float SET_FLOAT_COL = (100.0f);		// 色のintの値をfloatに直す用
}

//==========================================================
// コンストラクタ
//==========================================================
CGoal::CGoal(int nId)
{
	m_nId = nId;
	pEffect = nullptr;
	m_bEnd = false;
	m_People = SPeople();
	m_Info = SInfo();
	m_pBaggage = nullptr;
	m_pFont = nullptr;
	m_bRecvEnd = false;
	m_RecvId = -1;
	CGoalManager::GetInstance()->ListIn(this);
}

//==========================================================
// デストラクタ
//==========================================================
CGoal::~CGoal()
{
	
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CGoal::Init(void)
{
	pEffect = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\goal_radius.efkefc", m_Info.pos, VECTOR3_ZERO, VECTOR3_ZERO, m_Info.fRange, false, false);
	
	{
		// 最も近い道を取得
		Clist<CRoad*>* List = CRoadManager::GetInstance()->GetList();
		float fDis = FLT_MAX;
		m_pRoad = nullptr;
		for (int i = 0; i < List->GetNum(); i++)
		{
			float F = GetDistance(List->Get(i)->GetPosition(), m_Info.pos);
			if (F < fDis/* && List->Get(i)->GetType() !=CRoad::TYPE_NONE*/)
			{
				fDis = F;
				m_pRoad = List->Get(i);
			}
		}
	}

	float range = atan2f(m_Info.pos.x - m_pRoad->GetPosition().x, m_Info.pos.z - m_pRoad->GetPosition().z);
	m_People.setpos;
	m_People.setpos.x += sinf(range) * 800.0f;
	m_People.setpos.z += cosf(range) * 800.0f;
	m_People.pChara = CCharacter::Create(m_People.setpos + m_Info.pos, VECTOR3_ZERO, "data\\TXT\\motion_mob.txt");
	m_People.pChara->SetScale(SCALE);
	m_People.pChara->SetMtx();
	// 車体の色をランダムに
	float fColR = (float)(rand() % SET_COL_MAX) / SET_FLOAT_COL + 0.5f;
	float fColG = (float)(rand() % SET_COL_MAX) / SET_FLOAT_COL + 0.5f;
	float fColB = (float)(rand() % SET_COL_MAX) / SET_FLOAT_COL + 0.5f;

	for (int i = 0; i < m_People.pChara->GetNumParts() - 1; i++)
	{
		m_People.pChara->GetParts(i)->SetColMulti(D3DXCOLOR(fColR, fColR, fColR, 1.0f));
	}

	m_People.pChara->SetDrawshader(false);

	D3DXVECTOR3 rot = VECTOR3_ZERO;
	rot.y = atan2f(m_People.pChara->GetPosition().x - m_Info.pos.x, m_People.pChara->GetPosition().z - m_Info.pos.z);
	m_People.pChara->SetRotation(rot);

	{
		// 最も近い道を取得
		Clist<CRoad*>* List = CRoadManager::GetInstance()->GetList();
		float fDis = FLT_MAX;
		m_pRoad = nullptr;
		for (int i = 0; i < List->GetNum(); i++)
		{
			float F = GetDistance(List->Get(i)->GetPosition(), m_Info.pos);
			if (F < fDis && List->Get(i)->GetType() !=CRoad::TYPE_NONE)
			{
				fDis = F;
				m_pRoad = List->Get(i);
			}
		}
	}

	m_pFont = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false,
		SCREEN_CENTER, 0.035f, 100.0f, 100.0f, XALIGN_CENTER, YALIGN_BOTTOM);

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CGoal::Uninit(void)
{
	CGoalManager::GetInstance()->ListOut(this);
	SAFE_UNINIT(m_pBaggage);
	SAFE_UNINIT(m_People.pChara);
	SAFE_DELETE(pEffect);
	Release();

	SAFE_UNINIT(m_pFont);
}

//==========================================================
// 更新処理
//==========================================================
void CGoal::Update(void)
{
	if (CGame::GetInstance() != nullptr) {
		if (CGame::GetInstance()->GetState() != CGame::STATE::STATE_NONE)
		{
			return;
		}
	}
	// エフェクト表示
	//ScreenEffect();

	CDebugProc::GetInstance()->Print("\nゴール座標 [ %f, %f, %f ]\n", m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);

	if (m_People.pChara != nullptr)
	{
		m_People.pChara->Update();
	}
	else
	{
		return;
	}

	if (!CBaggage::GetThrowList()->Empty())
	{
		return;
	}

	if (!m_bEnd)
	{
		auto net = CNetWork::GetInstance();
		int nId = -2;

		// オンラインとシングルで判定を変える
		//if (net->GetState() == CNetWork::STATE::STATE_ONLINE)
		//{
		//	// 全員確認
		//	for (int i = 0; i < NetWork::MAX_CONNECT; i++)
		//	{
		//		if (CheckRange(i) && CheckSpeed(i))
		//		{
		//			nId = i;
		//			break;
		//		}
		//	}
		//}
		//else
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
			auto mgr = CGoalManager::GetInstance();
			SetEnd(nId);
			// ゴールしたよーを送信
			net->SendPlGoal(m_nId);

			// 次のゴール番号を送信
			net->SendNextGoal(mgr->GetNextIdx());

			// シングルプレイなら次のゴールを生成
			if (net->GetState() == CNetWork::STATE::STATE_SINGLE)
			{
				mgr->GoalCreate(mgr->GetNextIdx());
			}
		}
		else if (m_bRecvEnd)
		{
			SetEnd(m_RecvId);
		}
	}
	else
	{
		SAFE_DELETE(pEffect);
	}

	CDebugProc::GetInstance()->Print("%dPがゴールしたよ\n", m_RecvId);

	// 到着
	if (m_pBaggage == nullptr) { return; }
	if (m_pBaggage->GetState() == CBaggage::STATE::STATE_THROW) { return; }
	m_pBaggage->GetObj()->SetParent(m_People.pChara->GetParts(8)->GetMtx());
	m_People.pChara->GetMotion()->BlendSet(2);
	m_pBaggage->SetThrowScale(m_People.pChara->GetScale());
	m_pFont->SetEnableScroll(false);
	m_pFont->DeleteStringAll();
}
//==========================================================
// 距離のチェック
//==========================================================
bool CGoal::CheckRange(int nId)
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(nId);
	if (pPlayer == nullptr) { return false; }

	if (pPlayer->GetState() == CPlayer::STATE::STATE_DEATH) { return false; }

	float fDis = GetDistance(m_Info.pos, pPlayer->GetPosition());
	return (m_Info.fRange >= fDis);
}

//==========================================================
// 速度のチェック
//==========================================================
bool CGoal::CheckSpeed(int nId)
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer(nId);
	if (pPlayer == nullptr) { return false; }

	if (pPlayer->GetState() == CPlayer::STATE::STATE_DEATH) { return false; }

	float fDis = GetDistance(VECTOR3_ZERO, pPlayer->GetMove());
	return (m_Info.fLimit >= fDis);
}
//==========================================================
// 生成
//==========================================================
CGoal* CGoal::Create(const D3DXVECTOR3& pos, float fRange, float fLimit, int nId)
{
	CGoal* pGoal = nullptr;

	pGoal = DEBUG_NEW CGoal(nId);

	if (pGoal != nullptr)
	{
		pGoal->m_Info.pos = pos;
		pGoal->m_Info.fRange = fRange;
		pGoal->m_Info.fLimit = fLimit;
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
	D3DXVec3Project(&pos, &m_Info.pos, &Viewport, &mtxProjection, &mtxView, &mtxWorld);

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
	m_pBaggage = pPlayer->ThrowBaggage(m_People.pChara->GetParts(8)->GetMtxPos());
	pPlayer->AddDeliveryCount();
	m_People.pChara->GetMotion()->BlendSet(1);

	if (pPlayer->GetType() == CPlayer::TYPE::TYPE_RECV)
	{
		// 文字設定
		std::string str;
		str.clear();

		// ID
		std::ostringstream id;
		id << nId + 1;
		str += id.str() + "P が配達地点に到着しました";

		// 文字挿入
		m_pFont->PushBackString(str);
		m_pFont->SetEnableScroll(true);
		m_pFont->SetColor(CPlayerManager::GetInstance()->GetPlayer(nId)->GetObj()->GetColMulti());
	}
}

//==========================================================
// 座標設定
//==========================================================
void CGoal::SetPos(const D3DXVECTOR3& pos)
{
	m_Info.pos = pos;

	if (m_People.pChara == nullptr) { return; }

	m_People.pChara->SetPosition(m_Info.pos + m_People.setpos);
	m_People.pChara->SetMtx();

	pEffect->m_pos = m_Info.pos;
}