//==========================================================
//
// カメラアクションの処理 [camera_action.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "camera_action.h"
#include "camera.h"
#include "deltatime.h"

//==========================================================
// コンストラクタ
//==========================================================
CCameraAction::CCameraAction()
{
	// 値のクリア
	m_bNext = false;
	m_bFinish = false;
	m_startInfo = SStartInfo();
	m_targetInfo = STargetInfo();
	m_time = STime();
}

//==========================================================
// デストラクタ
//==========================================================
CCameraAction::~CCameraAction()
{

}


//==========================================================
// 初期化処理
//==========================================================
HRESULT CCameraAction::Init(void)
{
	return S_OK;
}



//==========================================================
// 終了処理
//==========================================================
void CCameraAction::Uninit(void)
{
	delete this;
}

//==========================================================
// 更新処理
//==========================================================
void CCameraAction::Update(CCamera* pCamera)
{
	if (pCamera == nullptr) { return; }
	if (m_bFinish) { return; }

	// 進める
	if (!m_bPause && m_time.fNow < m_time.fEnd)
	{
		m_time.fNow += CDeltaTime::GetInstance()->GetDeltaTime();
	}

	D3DXVECTOR3 rot = m_startInfo.rot;
	float length = m_startInfo.fLength;

	// タイムが進んでいる
	if (m_time.fNow > 0)
	{
		D3DXVECTOR3 rotDiff = m_targetInfo.rot - rot;
		float lenDiff = m_targetInfo.fLength - length;
		float multi = m_time.fNow / m_time.fEnd;
		rot += rotDiff * multi;
		length += lenDiff * multi;
	}

	// 種類ごとに設定
	pCamera->SetRotation(rot);
	pCamera->SetLength(length);

	switch (m_move)
	{
	case MOVE_POSV:
		pCamera->SetV();
		break;

	case MOVE_POSR:
		pCamera->SetV();
		break;
	default:
		break;
	}

	// 時間終了
	if (m_time.fNow >= m_time.fEnd)
	{
		m_time.fStopNow += CDeltaTime::GetInstance()->GetDeltaTime();

		if (m_time.fStopNow >= m_time.fStop)
		{
			m_bFinish = true;
		}
	}
}

//==========================================================
// アクション設定
//==========================================================
void CCameraAction::Set(CCamera* pCamera, const D3DXVECTOR3& rot, const float& fLength,
	const float fTime, const float fStopTime, const MOVE& move, const bool& bNext)
{
	if (pCamera == nullptr) { return; }

	// 初期値点の設定
	m_startInfo.posV = pCamera->GetPositionV();
	m_startInfo.posR = pCamera->GetPositionR();
	m_startInfo.fLength = pCamera->GetLength();
	m_startInfo.rot = pCamera->GetRotation();
	
	// 目標地点の値の設定
	m_targetInfo.rot = rot;
	m_targetInfo.fLength = fLength;

	// 時間の設定
	m_time.fEnd = fTime;
	m_time.fStop = fStopTime;
	m_time.fNow = 0.0f;
	m_time.fStopNow = 0.0f;

	// フラグの設定
	m_bFinish = false;
	m_bPause = false;
	m_bNext = bNext;
}