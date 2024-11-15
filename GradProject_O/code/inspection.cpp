//==========================================================
//
// 検問処理 [inspection.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "inspection.h"
#include "add_police.h"
#include "objectX.h"
#include "debugproc.h"
#include "player.h"
#include "player_manager.h"
#include "deltatime.h"

// 無名名前空間を定義
namespace
{
	const float POLICE_SETROT = D3DX_PI * 0.5f;
	const float POLICE_SETLENGTH = 700.0f;
	const float TIMER = 10.0f;
}

//==========================================================
// コンストラクタ
//==========================================================
CInstpection::CInstpection()
{
	// 値のクリア
	m_Guard = SGuardInfo();
	m_Info = SInfo();
	m_bHit = false;
}

//==========================================================
// デストラクタ
//==========================================================
CInstpection::~CInstpection()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CInstpection::Init(void)
{
	// 柵の生成
	m_Guard.pObj = CObjectX::Create(m_Info.pos, m_Info.rot, "data\\MODEL\\map\\guardrail001.x");
	m_Guard.pObj->SetScale(D3DXVECTOR3(5.0f, 2.0f, 2.0f));

	// パトカーの設定
	{
		// 向き取得
		D3DXVECTOR3 rot = m_Info.rot;
		rot.y = POLICE_SETROT + m_Info.rot.y;
		Adjust(rot);

		// 座標取得
		D3DXVECTOR3 pos = m_Info.pos;
		pos.x += sinf(rot.y) * POLICE_SETLENGTH;
		pos.z += cosf(rot.y) * POLICE_SETLENGTH;

		// 生成
		m_pPolice = CAddPolice::Create(pos, rot, VECTOR3_ZERO);
		m_pPolice->SetState(CPolice::STATE::STATE_STOP);
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CInstpection::Uninit(void)
{
	// 警察廃棄
	if (m_pPolice != nullptr)
	{
		m_pPolice = nullptr;
	}

	if (m_Guard.pObj != nullptr)
	{
		m_Guard.pObj = nullptr;
	}

	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CInstpection::Update(void)
{
	if (m_Guard.pObj != nullptr)
	{
		if (m_Guard.pObj->GetHit() || m_Guard.pObj->GetHitOld())
		{
			m_bHit = true;

			// 当たり判定を無しにする
			m_Guard.pObj->SetEnableCollision(false);

			CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
			float rot = atan2f(m_Guard.pObj->GetPosition().x - pPlayer->GetPosition().x, m_Guard.pObj->GetPosition().z - pPlayer->GetPosition().z);
			float speed = CPlayerManager::GetInstance()->GetTop()->GetEngine();

			// 座標設定
			m_Guard.targetpos = {
				m_Guard.pObj->GetPosition().x + sinf(rot) * speed * 500.0f,
				m_Guard.pObj->GetPosition().y,
				m_Guard.pObj->GetPosition().z + cosf(rot) * speed * 500.0f,
			};

			m_Guard.targetrot = {
				D3DX_PI * 0.5f,
				rot,
				0.0f
			};

			m_Guard.startpos = m_Guard.pObj->GetPosition();
			m_Guard.startrot = m_Guard.pObj->GetRotation();

			// 警察を切り離す
			if (m_pPolice != nullptr)
			{
				m_pPolice->SetState(CPolice::STATE::STATE_SEARCH);
				m_pPolice = nullptr;
			}
		}
	}

	// デバッグ表示
	CDebugProc::GetInstance()->Print("検問の座標 : [ %f, %f, %f]\n", m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);

	// 衝突していない
	if (!m_bHit) { return; }

	// 移動
	Away();
}

//==========================================================
// 生成
//==========================================================
CInstpection* CInstpection::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	CInstpection* pInsp = nullptr;

	pInsp = DEBUG_NEW CInstpection;

	if (pInsp != nullptr)
	{
		// 値設定
		pInsp->SetPosition(pos);
		pInsp->SetRotation(rot);

		// 初期化処理
		pInsp->Init();
	}

	return pInsp;
}

//==========================================================
// 移動
//==========================================================
void CInstpection::Away()
{
	// 移動した
	if (m_Guard.pObj == nullptr) { return; }
	if (!m_bHit) { return; }

	// 位置更新
	D3DXVECTOR3 posdiff = m_Guard.targetpos - m_Guard.pObj->GetPosition();
	D3DXVECTOR3 setpos = m_Guard.pObj->GetPosition() + posdiff * 0.1f;
	m_Guard.pObj->SetPosition(setpos);

	// 向き更新
	D3DXVECTOR3 rotdiff = m_Guard.targetrot - m_Guard.pObj->GetRotation();
	Adjust(rotdiff);
	D3DXVECTOR3 setrot = m_Guard.pObj->GetRotation() + rotdiff * 0.1f;
	Adjust(setrot);
	m_Guard.pObj->SetRotation(setrot);

	m_Guard.fTimer += CDeltaTime::GetInstance()->GetDeltaTime();
}