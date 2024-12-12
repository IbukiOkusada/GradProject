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
	m_bFinish = true;
	m_bPause = false;
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
	if (m_bPause) { return; }

	// 進める(タイムも達していない)
	if (m_time.fNow < m_time.fEnd) {
		m_time.fNow += CDeltaTime::GetInstance()->GetDeltaTime();
	}

	// 基本情報取得
	D3DXVECTOR3 rot = m_startInfo.rot;
	float length = m_startInfo.fLength;

	// タイムが進んでいる
	if (m_time.fNow > 0)
	{
		// 差分を補正
		float multi = m_time.fNow / m_time.fEnd;
		if (multi > 1.0f) { multi = 1.0f; }

		// 向き
		D3DXVECTOR3 rotDiff = m_targetInfo.rot - rot;
		Adjust(rotDiff);
		rot += rotDiff * multi;
		Adjust(rot);

		// 長さ
		float lenDiff = m_targetInfo.fLength - length;
		length += lenDiff * multi;
	}

	// 種類ごとに設定
	pCamera->SetRotation(rot);
	pCamera->SetLength(length);

	// 種類と別の座標移動
	if (m_time.fNow > 0)
	{
		// 差分を補正
		float multi = m_time.fNow / m_time.fEnd;
		if (multi > 1.0f) { multi = 1.0f; }

		switch (m_move)
		{
		case MOVE_POSV:
		{
			// 座標
			D3DXVECTOR3 pos = m_startInfo.posR;
			D3DXVECTOR3 posDiff = m_targetInfo.pos - pos;
			pos += posDiff * multi;
			pCamera->SetPositionR(pos);
		}
			break;

		case MOVE_POSR:
		{
			// 座標
			D3DXVECTOR3 pos = m_startInfo.posV;
			D3DXVECTOR3 posDiff = m_targetInfo.pos - pos;
			pos += posDiff * multi;
			pCamera->SetPositionV(pos);
		}
			break;

		default:
			break;
		}
	}

	// 種類ごとに変更
	switch (m_move)
	{
	case MOVE_POSV:
		pCamera->SetV();
		break;

	case MOVE_POSR:
		pCamera->SetR();
		break;

	default:
		break;
	}

	// 時間終了
	if (m_time.fNow >= m_time.fEnd) {

		// 停止時間加算
		m_time.fStopNow += CDeltaTime::GetInstance()->GetDeltaTime();

		// 時間終了
		if (m_time.fStopNow >= m_time.fStop) {

			if (m_bNext) m_bPause = true;
			else m_bFinish = true;

		}
	}
}

//==========================================================
// アクション設定
//==========================================================
void CCameraAction::Set(CCamera* pCamera, const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const float& fLength,
	const float fTime, const float fStopTime, const MOVE& move, const bool& bNext)
{
	if (pCamera == nullptr) { return; }

	// 初期値点の設定
	m_startInfo.posV = pCamera->GetPositionV();
	m_startInfo.posR = pCamera->GetPositionR();
	m_startInfo.fLength = pCamera->GetLength();
	m_startInfo.rot = pCamera->GetRotation();
	Adjust(m_startInfo.rot);
	
	// 目標地点の値の設定
	m_targetInfo.pos = pos;
	m_targetInfo.rot = rot;
	m_targetInfo.fLength = fLength;
	Adjust(m_targetInfo.rot);

	// 時間の設定
	m_time.fEnd = fTime;
	m_time.fStop = fStopTime;
	m_time.fNow = 0.0f;
	m_time.fStopNow = 0.0f;

	// フラグの設定
	m_bFinish = false;
	m_bPause = false;
	m_bNext = bNext;
	m_move = move;
}

//==========================================================
// 向き補正
//==========================================================
void CCameraAction::Adjust(D3DXVECTOR3& rot)
{
	while (1)
	{
		if (rot.x > D3DX_PI || rot.x < -D3DX_PI)
		{//-3.14～3.14の範囲外の場合
			if (rot.x > D3DX_PI)
			{
				rot.x += (-D3DX_PI * 2);
			}
			else if (rot.x < -D3DX_PI)
			{
				rot.x += (D3DX_PI * 2);
			}
		}
		else
		{
			break;
		}
	}

	while (1)
	{
		if (rot.y > D3DX_PI || rot.y < -D3DX_PI)
		{//-3.14～3.14の範囲外の場合
			if (rot.y > D3DX_PI)
			{
				rot.y += (-D3DX_PI * 2);
			}
			else if (rot.y < -D3DX_PI)
			{
				rot.y += (D3DX_PI * 2);
			}
		}
		else
		{
			break;
		}
	}

	while (1)
	{
		if (rot.z > D3DX_PI || rot.z < -D3DX_PI)
		{//-3.14～3.14の範囲外の場合
			if (rot.z > D3DX_PI)
			{
				rot.z += (-D3DX_PI * 2);
			}
			else if (rot.z < -D3DX_PI)
			{
				rot.z += (D3DX_PI * 2);
			}
		}
		else
		{
			break;
		}
	}
}