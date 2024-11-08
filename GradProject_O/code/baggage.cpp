//==========================================================
//
// 荷物の処理 [baggage.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "baggage.h"
#include "objectX.h"
#include "deltatime.h"
#include "camera.h"
#include "camera_manager.h"
#include "camera_action.h"
#include "player.h"
#include "manager.h"
#include "renderer.h"

namespace
{
	const float ROTATE = (D3DX_PI * 2.0f) * 5.0f;	// 回転量
	const float HEIGHT = 600.0f;
}

// 静的メンバ変数
Clist<CBaggage*> CBaggage::m_List = {};

//==========================================================
// コンストラクタ
//==========================================================
CBaggage::CBaggage()
{
	// 値のクリア
	m_time = STime();
	m_pos = VECTOR3_ZERO;
	m_rot = VECTOR3_ZERO;
	m_pObj = nullptr;
	m_bFinish = false;
	m_bFirst = false;
	m_List.Regist(this);
	m_state = STATE_NONE;
}

//==========================================================
// デストラクタ
//==========================================================
CBaggage::~CBaggage()
{

}

//==========================================================
// 生成
//==========================================================
CBaggage* CBaggage::Create(const D3DXVECTOR3& pos)
{
	CBaggage* pBaggage = DEBUG_NEW CBaggage;

	if (pBaggage != nullptr)
	{
		pBaggage->Init();
		pBaggage->m_pos = pos;
	}

	return pBaggage;
}

//==========================================================
// 値設定
//==========================================================
void CBaggage::Set(const D3DXVECTOR3& pos, D3DXVECTOR3* pTarget, float fTime)
{
	// 値設定
	m_pos = pos;
	m_pTarget = pTarget;
	m_time.fEnd = fTime;
	m_rot.y = atan2f(m_pTarget->x - pos.x, m_pTarget->z - pos.z);
	m_state = STATE_THROW;

	if (m_pObj == nullptr) { return; }

	m_pObj->SetPosition(pos);
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CBaggage::Init(void)
{
	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, "data\\MODEL\\ice\\softcream.x");
	m_pObj->SetScale(D3DXVECTOR3(10.0f, 10.0f, 10.0f));
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CBaggage::Uninit(void)
{
	// 廃棄
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	m_List.Delete(this);

	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CBaggage::Update(void)
{
	if (m_pObj == nullptr) { return; }

	// 投げていない
	if (m_state != STATE_THROW) { 
		m_pObj->SetPosition(m_pos); 
		return; 
	}

	// 投げ中挙動
	Throw();
}

//==========================================================
// 投げ処理
//==========================================================
void CBaggage::Throw()
{
	if (m_pObj == nullptr) { return; }
	if (m_bFinish) { return; }

	// 進める(タイムも達していない)
	if (m_time.fNow < m_time.fEnd) {
		m_time.fNow += CDeltaTime::GetInstance()->GetDeltaTime();
	}

	// 基本情報取得
	D3DXVECTOR3 pos = m_pos;
	D3DXVECTOR3 rot = m_rot;

	// タイムが進んでいる
	if (m_time.fNow > 0)
	{
		// 差分を補正
		float multi = m_time.fNow / m_time.fEnd;

		if (multi > 1.0f) { multi = 1.0f; }

		rot.x += ROTATE * multi;

		// 位置
		if (m_pTarget != nullptr)
		{
			D3DXVECTOR3 posDiff = *m_pTarget - pos;
			pos += posDiff * multi;

			// y座標は弧を描く
			float ymulti = multi;

			if (ymulti > 0.5f)
			{
				ymulti = 0.5f - (ymulti - 0.5f);
			}

			pos.y += sinf(ymulti) * HEIGHT;
		}

		if (multi >= 0.4f)
		{
			if(!m_bFirst){ CManager::GetInstance()->GetRenderer()->SetEnableDrawMultiScreen(0.0f, 1.0f, 120.0f); }
			m_bFirst = true;
			CDeltaTime::GetInstance()->SetSlow(0.2f);
		}
	}

	// カメラ設定
	SetCamera();

	// 設定
	m_pObj->SetPosition(pos);
	m_pObj->SetRotation(rot);

	// 時間終了
	if (m_time.fNow >= m_time.fEnd) {
		CDeltaTime::GetInstance()->SetSlow(1.0f);
		CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
		pCamera->GetAction()->SetFinish(true);
		m_bFinish = true;
		Uninit();
	}
}

//==========================================================
// カメラ設定処理
//==========================================================
void CBaggage::SetCamera()
{
	if (m_pTarget == nullptr) { return; }

	CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
	float rotY = atan2f(m_pos.z - m_pTarget->z, m_pos.x - m_pTarget->x);
	pCamera->GetAction()->Set(pCamera, D3DXVECTOR3(0.0f, rotY, D3DX_PI * 0.35f), 600.0f, 0.5f, 1.0f, CCameraAction::MOVE_POSV, true);
	D3DXVECTOR3 pos = *m_pTarget;
	pos.y += 200.0f;
	pCamera->Pursue(pos, VECTOR3_ZERO, 0.0f);
}
