//==========================================================
//
// カメラの処理全般 [camera.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "camera.h"
#include "input.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "game.h"
#include "debugproc.h"
#include "camera.h"
#include "camera_manager.h"
#include "slow.h"

//無名名前空間
namespace
{
	const D3DXVECTOR3 GOAL_CAMERAROT = { 0.0f, 0.75f, D3DX_PI * 0.38f };   // 目標の角度
	const D3DXVECTOR3 GOAL_CAMERAPOSR = { -970.0f, 100.0f, 1100.0f };      // 目標の位置
	const D3DXVECTOR3 GOAL_CAMERAPOSV = { -660.0f, 280.0f, 1460.0f };      // 目標の位置
	const float DEFAULT_LENGTH = 700.0f;
	const float ZOOM_SPEED = 0.18f;
	const float ZOOMSTARTDOOR_COUNT = 300;          // スタートドアを見ている時間
	const float CAMERA_MOVESPEED = (1.0f);			// 移動量
	const float CAMERA_LENGTHMOVE = (0.8f);			// カメラ距離移動量
	const float ROTATE_SPEED = (0.00625f);			// カメラの回転速度
	const float PAD_ROTATE = (0.02f);		// 向き
	const float CAMERA_MAXLENGTH = (5000.0f);		// カメラ最大距離
	const float CAMERA_MINLENGTH = (120.0f);			// カメラ最小距離
	const float MOUSE_MOVESPEED = (0.9f);		// マウス移動速度
	const float MOUSE_ROTATESPEED_X = (0.001f);		// マウス回転速度x軸
	const float MOUSE_ROTATESPEED_Z = (0.005f);		// マウス回転速度z軸
	const float MOUSE_ROTATESPEEDEDIT_X = (0.01f);		// マウス回転速度x軸
	const float MOUSE_ROTATESPEEDEDIT_Z = (0.01f);		// マウス回転速度z軸
	const float MOUSE_WHEELSPEED = (0.1f);			// マウスホイール回転速度
	const float MAX_SLOWROT = (0.15f);		// 
	const float MESSAGERAND = (120);
	const float SLOW_CAMERAROT = (0.7f);
	const float TITLE_ROTATESPD = (0.0025f);	// タイトル回転量
	const float CAMERA_PADMAX = (D3DX_PI * 0.76f);
	const float CAMERA_LENGTHINER = (0.25f);
}

//==========================================================
// コンストラクタ
//==========================================================
CCamera::CCamera()
{
	m_pNext = nullptr;
	m_pPrev = nullptr;
	m_bDraw = true;
	m_nId = 0;
	m_bActive = true;
	m_fZoom = 1.0f;
	m_fDestZoom = m_fZoom;

	// リストに挿入
	CCameraManager::GetInstance()->ListIn(this);
}

//==========================================================
// デストラクタ
//==========================================================
CCamera::~CCamera()
{

}

//==========================================================
//カメラの初期化処理
//==========================================================
HRESULT CCamera::Init(void)
{
	//各変数の初期化
	m_posR = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_posV = D3DXVECTOR3(0.0f, 1900.0f, 2200.0f);
	m_OldposV = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_OldposR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fLength = sqrtf((m_posV.x - m_posR.x) * (m_posV.x - m_posR.x) + (m_posV.z - m_posR.z) * (m_posV.z - m_posR.z));
	m_rot = D3DXVECTOR3(0.0f, atan2f(m_posR.x - m_posV.x, m_posR.z - m_posV.z), 1.56f);
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_rot.z = MIN_CAMERA_ROTZ;
	m_mode = MODE_NORMAL;
	m_nZoomCount = 0;
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_GoalPos = D3DXVECTOR3(-660.0f, 100.0f, 1300.0f);

	//視点設定
	SetV();

	return S_OK;
}

//==========================================================
//カメラの終了処理
//==========================================================
void CCamera::Uninit(void)
{
	// リストから外す
	CCameraManager::GetInstance()->ListOut(this);
}

//==========================================================
//カメラの更新処理
//==========================================================
void CCamera::Update(void)
{
	if (!m_bDraw || !m_bActive) {	// 使用しない場合
		return;
	}

	if(m_mode != MODE_STARTDOOR)
	{
		//視点の移動
		MoveV();

		m_OldposR = m_posR;
		m_OldposV = m_posV;
		m_Oldrot = m_rot;
	}
	
	MouseCamera();

	//ズーム
	Zoom();

	CManager::GetInstance()->GetDebugProc()->Print("向き[%f, %f, %f]\n", m_rot.x, m_rot.y, m_rot.z);
	
}

//==========================================================
//カメラの設定処理
//==========================================================
void CCamera::SetCamera(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//デバイスへのポインタを取得

	//プロジェクションマトリクスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	//プロジェクションマトリックスの生成[ 透視投影 ]
	/*D3DXMatrixOrthoLH(&m_mtxProjection,
		(float)SCREEN_WIDTH,
		(float)SCREEN_HEIGHT,
		0.0f,
		8000.0f);*/

	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
								D3DXToRadian(45.0f),
								(float)SCREEN_WIDTH / (float) SCREEN_HEIGHT,
								10.0f,
								40000.0f);

	//プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);

	//ビューマックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	//ビューマトリックスの作成
	D3DXMatrixLookAtLH(&m_mtxView,
		&m_posV,
		&m_posR,
		&m_vecU);

	//ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);
}

//==========================================================
//カメラの全移動処理
//==========================================================
void CCamera::MoveVR(void)
{
	CInputKeyboard *pKey = CManager::GetInstance()->GetInputKeyboard();

	//視点注視点の同時移動
	if (pKey->GetPress(DIK_A) == true)
	{//Aキー入力

		//視点の移動
		if (pKey->GetPress(DIK_W) == true)
		{
			m_posV.x += cosf(m_rot.y + (-D3DX_PI * 0.75f)) * CAMERA_MOVESPEED;
			m_posV.z += sinf(m_rot.y + (-D3DX_PI * 0.75f)) * CAMERA_MOVESPEED;
		}
		else if (pKey->GetPress(DIK_S) == true)
		{
			m_posV.x += cosf(m_rot.y + (-D3DX_PI * 0.25f)) * CAMERA_MOVESPEED;
			m_posV.z += sinf(m_rot.y + (-D3DX_PI * 0.25f)) * CAMERA_MOVESPEED;
		}
		else
		{
			m_posV.x += cosf(m_rot.y + (-D3DX_PI * 0.5f)) * CAMERA_MOVESPEED;
			m_posV.z += sinf(m_rot.y + (-D3DX_PI * 0.5f)) * CAMERA_MOVESPEED;
		}
	}
	else if (pKey->GetPress(DIK_D) == true)
	{//Dキー入力

		//視点の移動
		if (pKey->GetPress(DIK_W) == true)
		{
			m_posV.x += cosf(m_rot.y + (D3DX_PI * 0.75f)) * CAMERA_MOVESPEED;
			m_posV.z += sinf(m_rot.y + (D3DX_PI * 0.75f)) * CAMERA_MOVESPEED;
		}
		else if (pKey->GetPress(DIK_S) == true)
		{
			m_posV.x += cosf(m_rot.y + (D3DX_PI * 0.25f)) * CAMERA_MOVESPEED;
			m_posV.z += sinf(m_rot.y + (D3DX_PI * 0.25f)) * CAMERA_MOVESPEED;
		}
		else
		{
			m_posV.x += cosf(m_rot.y + (D3DX_PI * 0.5f)) * CAMERA_MOVESPEED;
			m_posV.z += sinf(m_rot.y + (D3DX_PI * 0.5f)) * CAMERA_MOVESPEED;
		}

	}
	else if (pKey->GetPress(DIK_W) == true)
	{//Wキー入力

		//視点の移動
		m_posV.x += -cosf(m_rot.y) * CAMERA_MOVESPEED;
		m_posV.z += -sinf(m_rot.y) * CAMERA_MOVESPEED;
	}
	else if (pKey->GetPress(DIK_S) == true)
	{//Sキー入力
		//視点の移動
		m_posV.x += cosf(m_rot.y) * CAMERA_MOVESPEED;
		m_posV.z += sinf(m_rot.y) * CAMERA_MOVESPEED;

	}

	//注視点設定
	SetR();
}

//==========================================================
//カメラの視点移動処理
//==========================================================
void CCamera::MoveV(void)
{
	CInputKeyboard *pKey = CManager::GetInstance()->GetInputKeyboard();
	CInputPad *pInputPad = CManager::GetInstance()->GetInputPad();	// キーボードのポインタ
	float fMultiSlow = 1.0f;
	int nId = m_nId;

	if (nId < 0 && nId >= PLAYER_MAX) {
		nId = 0;
	}

	//x軸の移動
	if (pInputPad->GetStickPress(nId, CInputPad::BUTTON_RIGHT_X, 0.0f, CInputPad::STICK_PLUS) == true)
	{//Qキー入力
		m_rot.y += -D3DX_PI * ROTATE_SPEED * pInputPad->GetStickAdd(nId, CInputPad::BUTTON_RIGHT_X, 0.0f, CInputPad::STICK_PLUS) * fMultiSlow;
		if (m_rot.y < -D3DX_PI)
		{//角度がΠを超えた場合
			m_rot.y += D3DX_PI * 2;
		}
	}
	else if (pInputPad->GetStickPress(nId, CInputPad::BUTTON_RIGHT_X, 0.0f, CInputPad::STICK_MINUS) == true)
	{//Eキー入力
		m_rot.y += -D3DX_PI * ROTATE_SPEED * pInputPad->GetStickAdd(nId, CInputPad::BUTTON_RIGHT_X, 0.0f, CInputPad::STICK_MINUS) * fMultiSlow;
	}

	////x軸の移動
	//if ((pKey->GetPress(DIK_Z) == true && pKey->GetPress(DIK_C) != true))
	//{//Qキー入力
	//	m_rot.y += -D3DX_PI * ROTATE_SPEED;
	//	if (m_rot.y < -D3DX_PI)
	//	{//角度がΠを超えた場合
	//		m_rot.y += D3DX_PI * 2;
	//	}
	//}
	//else if ((pKey->GetPress(DIK_C) == true && pKey->GetPress(DIK_Z) != true))
	//{//Eキー入力
	//	m_rot.y += D3DX_PI * ROTATE_SPEED;
	//	if (m_rot.y > D3DX_PI)
	//	{//角度がΠを超えた場合
	//		m_rot.y += -D3DX_PI * 2;
	//	}
	//}

	//z軸の移動
	if (pInputPad->GetStickPress(nId, CInputPad::BUTTON_RIGHT_Y, 0.1f, CInputPad::STICK_PLUS) == true)
	{//Yキー入力
		//角度の変更
		m_rot.z += PAD_ROTATE * pInputPad->GetStickAdd(nId, CInputPad::BUTTON_RIGHT_Y, 0.5f, CInputPad::STICK_PLUS) * fMultiSlow;
		if (m_rot.z > CAMERA_PADMAX)
		{//角度が限界を超えた場合
			m_rot.z = CAMERA_PADMAX;
		}
	}
	else if (pInputPad->GetStickPress(nId, CInputPad::BUTTON_RIGHT_Y, 0.1f, CInputPad::STICK_MINUS) == true)
	{//Nキー入力
		//角度の変更
		m_rot.z += PAD_ROTATE * 2 * pInputPad->GetStickAdd(nId, CInputPad::BUTTON_RIGHT_Y, 0.5f, CInputPad::STICK_MINUS) * fMultiSlow;

		if (m_rot.z < MIN_CAMERA_ROTZ)
		{//角度が限界を超えた場合
			m_rot.z = MIN_CAMERA_ROTZ;
		}
	}

	////z軸の移動
	//if (pKey->GetPress(DIK_Y) == true && pKey->GetPress(DIK_N) != true)
	//{//Yキー入力
	// //角度の変更
	//	m_rot.z += -D3DX_PI * ROTATE_SPEED;
	//	if (m_rot.z < MIN_CAMERA_ROTZ)
	//	{//角度が限界を超えた場合
	//		m_rot.z = MIN_CAMERA_ROTZ;
	//	}

	//}
	//else if (pKey->GetPress(DIK_N) == true && pKey->GetPress(DIK_Y) != true)
	//{//Nキー入力
	// //角度の変更
	//	m_rot.z += D3DX_PI * ROTATE_SPEED;

	//	if (m_rot.z > MAX_CAMERA_ROTZ)
	//	{//角度が限界を超えた場合
	//		m_rot.z = MAX_CAMERA_ROTZ;
	//	}
	//}

	////注視点からの距離の操作
	//if (pKey->GetPress(DIK_U) == true && pKey->GetPress(DIK_M) != true)
	//{//Uキー入力
	//	m_fLength -= CAMERA_LENGTHMOVE;

	//	if (m_fLength < CAMERA_MINLENGTH)
	//	{//距離が最小を超えた場合
	//		m_fLength = CAMERA_MINLENGTH;
	//	}
	//	
	//}
	//else if (pKey->GetPress(DIK_M) == true && pKey->GetPress(DIK_U) != true)
	//{//Mキー入力
	//	m_fLength += CAMERA_LENGTHMOVE;

	//	if (m_fLength > CAMERA_MAXLENGTH)
	//	{//距離が最大を超えた場合
	//		m_fLength = CAMERA_MAXLENGTH;
	//	}
	//}

	if (m_mode == MODE_SLOWGUN)
	{
		Slow();
	}

	//視点設定
	SetV();
}

//==========================================================
//カメラの注視点移動処理
//==========================================================
void CCamera::MoveR(void)
{
	CInputKeyboard *pKey = CManager::GetInstance()->GetInputKeyboard();

	//x軸の移動
	if (pKey->GetPress(DIK_Q) == true && pKey->GetPress(DIK_E) != true)
	{//Qキー入力
		//角度の変更
		m_rot.y += D3DX_PI * ROTATE_SPEED;
		if (m_rot.y > D3DX_PI)
		{//角度がΠを超えた場合
			m_rot.y += -D3DX_PI * 2;
		}
	}
	else if (pKey->GetPress(DIK_E) == true && pKey->GetPress(DIK_Q) != true)
	{//Eキー入力
		//角度の変更
		m_rot.y += -D3DX_PI * ROTATE_SPEED;

		if (m_rot.y < -D3DX_PI)
		{//角度がΠを超えた場合
			m_rot.y += D3DX_PI * 2;
		}
	}

	//z軸の移動
	if (pKey->GetPress(DIK_T) == true && pKey->GetPress(DIK_B) != true)
	{//Tキー入力
		//角度の変更
		m_rot.z += D3DX_PI * ROTATE_SPEED;

		if (m_rot.z > MAX_CAMERA_ROTZ)
		{//角度が限界を超えた場合
			m_rot.z = MAX_CAMERA_ROTZ;
		}
	}
	else if (pKey->GetPress(DIK_B) == true && pKey->GetPress(DIK_T) != true)
	{//Bキー入力
		//角度の変更
		m_rot.z += -D3DX_PI * ROTATE_SPEED;
		if (m_rot.z < MIN_CAMERA_ROTZ)
		{//角度が限界を超えた場合
			m_rot.z = MIN_CAMERA_ROTZ;
		}
	}

	//注視点設定
	SetR();
}

//==========================================================
//カメラ視点設定処理
//==========================================================
void CCamera::SetV(void)
{
	m_posV.y = m_posR.y + cosf(m_rot.z) * (m_fLength * m_fZoom);
	m_posV.x = m_posR.x + (sinf(m_rot.z) * cosf(m_rot.y)) * (m_fLength * m_fZoom);
	m_posV.z = m_posR.z + (sinf(m_rot.z) * sinf(m_rot.y)) * (m_fLength * m_fZoom);
}

//==========================================================
//カメラ注視点設定処理
//==========================================================
void CCamera::SetR(void)
{
	m_posR.y = m_posV.y - cosf(m_rot.z) * (m_fLength * m_fZoom);
	m_posR.x = m_posV.x - (sinf(m_rot.z) * cosf(m_rot.y)) * (m_fLength * m_fZoom);
	m_posR.z = m_posV.z - (sinf(m_rot.z) * sinf(m_rot.y)) * (m_fLength * m_fZoom);
}

//==========================================================
//マウスでのカメラ移動処理
//==========================================================
void CCamera::MouseCamera(void)
{
	CInputMouse *pMouse = CManager::GetInstance()->GetInputMouse();

	//if (pMouse->GetPress(CInputMouse::BUTTON_LBUTTON) == true && pMouse->GetPress(CInputMouse::BUTTON_RBUTTON) == true)
	//{//左右同時に押されているとき
	//	m_move.x += cosf(m_rot.y + (-D3DX_PI * 0.5f)) * (pMouse->GetCousorMove().x * MOUSE_MOVESPEED) + -cosf(m_rot.y) * (pMouse->GetCousorMove().y * MOUSE_MOVESPEED);
	//	m_move.z += sinf(m_rot.y + (-D3DX_PI * 0.5f)) * (pMouse->GetCousorMove().x * MOUSE_MOVESPEED) + -sinf(m_rot.y) * (pMouse->GetCousorMove().y * MOUSE_MOVESPEED);

	//	CManager::GetInstance()->GetDebugProc()->Print("移動量 [ %f, %f ]", m_move.x, m_move.z);

	//	//移動量を加算
	//	m_posV.x += m_move.x;
	//	m_posV.z += m_move.z;

	//	//移動量を初期化
	//	m_move.x = 0.0f;
	//	m_move.z = 0.0f;

	//	//注視点設定
	//	SetR();
	//}
	//else if (pMouse->GetPress(CInputMouse::BUTTON_LBUTTON) == true)
	//{//左キーが押されているとき
	//	m_rot.y -= pMouse->GetCousorMove().x * MOUSE_ROTATESPEED_X;
	//	m_rot.z -= pMouse->GetCousorMove().y * 0.005f;

	//	if (m_rot.y > D3DX_PI)
	//	{//角度がΠを超えた場合
	//		m_rot.y = D3DX_PI;
	//		m_rot.y *= -1.0f;
	//	}
	//	else if (m_rot.y < -D3DX_PI)
	//	{//角度がΠを超えた場合
	//		m_rot.y = -D3DX_PI;
	//		m_rot.y *= -1.0f;
	//	}
	//	if (m_rot.z < MIN_CAMERA_ROTZ)
	//	{//角度が限界を超えた場合
	//		m_rot.z = MIN_CAMERA_ROTZ;
	//	}
	//	else if (m_rot.z > MAX_CAMERA_ROTZ)
	//	{//角度が限界を超えた場合
	//		m_rot.z = MAX_CAMERA_ROTZ;
	//	}
	//}
	//else if (pMouse->GetPress(CInputMouse::BUTTON_RBUTTON) == true)
	//{//右キーが押されているとき
	//	m_rot.y -= pMouse->GetCousorMove().x * MOUSE_ROTATESPEED_X;
	//	m_rot.z -= pMouse->GetCousorMove().y * MOUSE_ROTATESPEED_Z;

	//	if (m_rot.y > D3DX_PI)
	//	{//角度がΠを超えた場合
	//		m_rot.y = D3DX_PI;
	//		m_rot.y *= -1.0f;
	//	}
	//	else if (m_rot.y < -D3DX_PI)
	//	{//角度がΠを超えた場合
	//		m_rot.y = -D3DX_PI;
	//		m_rot.y *= -1.0f;
	//	}
	//	if (m_rot.z < MIN_CAMERA_ROTZ)
	//	{//角度が限界を超えた場合
	//		m_rot.z = MIN_CAMERA_ROTZ;
	//	}
	//	else if (m_rot.z > MAX_CAMERA_ROTZ)
	//	{//角度が限界を超えた場合
	//		m_rot.z = MAX_CAMERA_ROTZ;
	//	}

	//	//視点設定
	//	SetR();
	//}

	m_rot.y -= pMouse->GetCousorMove().x * MOUSE_ROTATESPEED_X;

	if (m_rot.y > D3DX_PI)
	{//角度がΠを超えた場合
		m_rot.y = D3DX_PI;
		m_rot.y *= -1.0f;
	}
	else if (m_rot.y < -D3DX_PI)
	{//角度がΠを超えた場合
		m_rot.y = -D3DX_PI;
		m_rot.y *= -1.0f;
	}

	//m_rot.y -= pMouse->GetCousorMove().x * MOUSE_ROTATESPEED_X;
	//m_rot.z -= pMouse->GetCousorMove().y * 0.003f;

	//if (m_rot.y < -D3DX_PI)
	//{//角度がΠを超えた場合
	//	m_rot.y += D3DX_PI * 2;
	//}
	//else if (m_rot.y > D3DX_PI)
	//{//角度がΠを超えた場合
	//	m_rot.y += -D3DX_PI * 2;
	//}
	//if (m_rot.z < MIN_CAMERA_ROTZ)
	//{//角度が限界を超えた場合
	//	m_rot.z = MIN_CAMERA_ROTZ;
	//}
	//else if (m_rot.z > MAX_CAMERA_ROTZ)
	//{//角度が限界を超えた場合
	//	m_rot.z = MAX_CAMERA_ROTZ;
	//}

	////ホイールの使用量で距離の変更
	//m_fLength += pMouse->GetCousorMove().z * MOUSE_WHEELSPEED;

	//if (m_fLength > CAMERA_MAXLENGTH)
	//{//距離が最大を超えた場合
	//	m_fLength = CAMERA_MAXLENGTH;
	//}
	//else if (m_fLength < CAMERA_MINLENGTH)
	//{//距離が最小を超えた場合
	//	m_fLength = CAMERA_MINLENGTH;
	//}

	//視点設定
	SetV();
}

//==========================================================
// 追従処理
//==========================================================
void CCamera::Pursue(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const float fLength)
{
	D3DXVECTOR3 posRDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 目標の注視点
	D3DXVECTOR3 posVDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 目標の視点
	D3DXVECTOR3 RDiff = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 注視点の差分
	D3DXVECTOR3 VDiff = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 視点の差分
	float fDiff = fLength - m_fLength;
	m_fLength += fDiff * CAMERA_LENGTHINER;

	//目的の注視点の座標を取得
	posRDest = D3DXVECTOR3(pos.x - sinf(rot.y) * 50.0f, pos.y + 20.0f + (40.0f * (1.0f - m_fZoom)), pos.z - cosf(rot.y) * 50.0f);

	//目的の視点の座標を獲得
	posVDest = D3DXVECTOR3(
		posRDest.x + (sinf(m_rot.z) * cosf(m_rot.y)) * (m_fLength * m_fZoom),
		posRDest.y + cosf(m_rot.z) * (m_fLength * m_fZoom),
		posRDest.z + (sinf(m_rot.z) * sinf(m_rot.y)) * (m_fLength * m_fZoom));

	//注視点の補正
	RDiff = D3DXVECTOR3(posRDest.x - m_posR.x, posRDest.y - m_posR.y, posRDest.z - m_posR.z);
	m_posR.x += RDiff.x * 0.15f;
	m_posR.y += RDiff.y * 0.1f;
	m_posR.z += RDiff.z * 0.15f;

	//視点の補正
	VDiff = D3DXVECTOR3(posVDest.x - m_posV.x, posVDest.y - m_posV.y, posVDest.z - m_posV.z);
	m_posV.x += VDiff.x * 0.12f;
	m_posV.y += VDiff.y * 0.075f;
	m_posV.z += VDiff.z * 0.12f;

	if (CManager::GetInstance()->GetSlow()->Get() == 1.0f)
	{
		float fRotDiff;
		float fRotDest;

		fRotDest = 1.4f;	//目的の向きを取得

		fRotDiff = fRotDest - m_rot.z;

		if (fRotDiff > D3DX_PI || fRotDiff < -D3DX_PI)
		{//-3.14～3.14の範囲外の場合
			if (fRotDiff > D3DX_PI)
			{
				fRotDiff += (-D3DX_PI * 2);
			}
			else if (fRotDiff < -D3DX_PI)
			{
				fRotDiff += (D3DX_PI * 2);
			}
		}

		m_rot.z += fRotDiff * 0.06f;

		if (m_rot.z > D3DX_PI || m_rot.z < -D3DX_PI)
		{//-3.14～3.14の範囲外の場合
			if (m_rot.z > D3DX_PI)
			{
				m_rot.z += (-D3DX_PI * 2);
			}
			else if (m_rot.z < -D3DX_PI)
			{
				m_rot.z += (D3DX_PI * 2);
			}
		}
	}
}

//==========================================================
// カメラ角度慣性変更
//==========================================================
void CCamera::InerRot(const D3DXVECTOR3& rot, const float fMul)
{
	D3DXVECTOR3 RotDiff = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 注視点の差分

	// 向きの補正
	RotDiff = rot - m_rot;

	if (RotDiff.y > D3DX_PI)
	{//角度がΠを超えた場合
		RotDiff.y = D3DX_PI;
		RotDiff.y *= -1.0f;
	}
	else if (RotDiff.y < -D3DX_PI)
	{//角度がΠを超えた場合
		RotDiff.y = -D3DX_PI;
		RotDiff.y *= -1.0f;
	}
	if (RotDiff.z > D3DX_PI)
	{//角度がΠを超えた場合
		RotDiff.z = D3DX_PI;
		RotDiff.z *= -1.0f;
	}
	else if (RotDiff.z < -D3DX_PI)
	{//角度がΠを超えた場合
		RotDiff.z = -D3DX_PI;
		RotDiff.z *= -1.0f;
	}

	m_rot += RotDiff * fMul;

	if (m_rot.y > D3DX_PI)
	{//角度がΠを超えた場合
		m_rot.y = D3DX_PI;
		m_rot.y *= -1.0f;
	}
	else if (m_rot.y < -D3DX_PI)
	{//角度がΠを超えた場合
		m_rot.y = -D3DX_PI;
		m_rot.y *= -1.0f;
	}
	if (m_rot.z < MIN_CAMERA_ROTZ)
	{//角度が限界を超えた場合
		m_rot.z = MIN_CAMERA_ROTZ;
	}
	else if (m_rot.z > MAX_CAMERA_ROTZ)
	{//角度が限界を超えた場合
		m_rot.z = MAX_CAMERA_ROTZ;
	}
}

//==========================================================
// 投影場所設定
//==========================================================
void CCamera::Setting(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	m_rot.y = -rot.y + (D3DX_PI * 0.5f);

	//目的の注視点の座標を取得
	m_posR = D3DXVECTOR3(pos.x - sinf(rot.y) * 30.0f, pos.y + 50.0f, pos.z - cosf(rot.y) * 30.0f);

	//目的の視点の座標を獲得
	m_posV = D3DXVECTOR3(
		m_posR.x + (sinf(m_rot.z) * cosf(m_rot.y)) * m_fLength,
		m_posR.y + cosf(m_rot.z) * m_fLength,
		m_posR.z + (sinf(m_rot.z) * sinf(m_rot.y)) * m_fLength);
}

//==========================================================
// エディット用
//==========================================================
void CCamera::Edit(void)
{
	CInputMouse *pMouse = CManager::GetInstance()->GetInputMouse();
	if (pMouse->GetPress(CInputMouse::BUTTON_LBUTTON) == true && pMouse->GetPress(CInputMouse::BUTTON_RBUTTON) == true)
	{//左右同時に押されているとき
		m_posV.x += cosf(m_rot.y + (-D3DX_PI * 0.5f)) * (pMouse->GetCousorMove().x * MOUSE_MOVESPEED) + -cosf(m_rot.y) * (pMouse->GetCousorMove().y * MOUSE_MOVESPEED);
		m_posV.z += sinf(m_rot.y + (-D3DX_PI * 0.5f)) * (pMouse->GetCousorMove().x * MOUSE_MOVESPEED) + -sinf(m_rot.y) * (pMouse->GetCousorMove().y * MOUSE_MOVESPEED);

		//移動量を加算
		m_posV.x += m_posV.x;
		m_posV.z += m_posV.z;

		//移動量を初期化
		m_posV.x = 0.0f;
		m_posV.z = 0.0f;

		//注視点設定
		SetR();
	}
	else if (pMouse->GetPress(CInputMouse::BUTTON_LBUTTON) == true)
	{//左キーが押されているとき
		m_rot.y -= pMouse->GetCousorMove().x * MOUSE_ROTATESPEEDEDIT_X;
		m_rot.z -= pMouse->GetCousorMove().y * 0.003f;
	}
	else if (pMouse->GetPress(CInputMouse::BUTTON_RBUTTON) == true)
	{//右キーが押されているとき
		m_rot.y -= pMouse->GetCousorMove().x * MOUSE_ROTATESPEEDEDIT_X;
		m_rot.z -= pMouse->GetCousorMove().y * MOUSE_ROTATESPEEDEDIT_Z;

		if (m_rot.y > D3DX_PI)
		{//角度がΠを超えた場合
			m_rot.y = D3DX_PI;
			m_rot.y *= -1.0f;
		}
		else if (m_rot.y < -D3DX_PI)
		{//角度がΠを超えた場合
			m_rot.y = -D3DX_PI;
			m_rot.y *= -1.0f;
		}
		if (m_rot.z < MIN_CAMERA_ROTZ)
		{//角度が限界を超えた場合
			m_rot.z = MIN_CAMERA_ROTZ;
		}
		else if (m_rot.z > MAX_CAMERA_ROTZ)
		{//角度が限界を超えた場合
			m_rot.z = MAX_CAMERA_ROTZ;
		}

		//視点設定
		SetR();
	}

	if (m_rot.y > D3DX_PI)
	{//角度がΠを超えた場合
		m_rot.y = D3DX_PI;
		m_rot.y *= -1.0f;
	}
	else if (m_rot.y < -D3DX_PI)
	{//角度がΠを超えた場合
		m_rot.y = -D3DX_PI;
		m_rot.y *= -1.0f;
	}
	if (m_rot.z < MIN_CAMERA_ROTZ)
	{//角度が限界を超えた場合
		m_rot.z = MIN_CAMERA_ROTZ;
	}
	else if (m_rot.z > MAX_CAMERA_ROTZ)
	{//角度が限界を超えた場合
		m_rot.z = MAX_CAMERA_ROTZ;
	}

	//ホイールの使用量で距離の変更
	m_fLength += pMouse->GetCousorMove().z * MOUSE_WHEELSPEED;

	if (m_fLength > CAMERA_MAXLENGTH)
	{//距離が最大を超えた場合
		m_fLength = CAMERA_MAXLENGTH;
	}
	else if (m_fLength < CAMERA_MINLENGTH)
	{//距離が最小を超えた場合
		m_fLength = CAMERA_MINLENGTH;
	}

	//視点設定
	SetV();
}

//==========================================================
// 向き
//==========================================================
void CCamera::SetRot(const D3DXVECTOR3 rot)
{
	float fRotDiff;
	float fRotDest;

	fRotDest = -rot.y + (D3DX_PI * 0.5f);	//目的の向きを取得

	fRotDiff = fRotDest - m_rot.y;

	if (fRotDiff > D3DX_PI || fRotDiff < -D3DX_PI)
	{//-3.14～3.14の範囲外の場合
		if (fRotDiff > D3DX_PI)
		{
			fRotDiff += (-D3DX_PI * 2);
		}
		else if (fRotDiff < -D3DX_PI)
		{
			fRotDiff += (D3DX_PI * 2);
		}
	}

	m_rot.y += fRotDiff * 0.04f;

	if (m_rot.y > D3DX_PI || m_rot.y < -D3DX_PI)
	{//-3.14～3.14の範囲外の場合
		if (m_rot.y > D3DX_PI)
		{
			m_rot.y += (-D3DX_PI * 2);
		}
		else if (m_rot.y  < -D3DX_PI)
		{
			m_rot.y += (D3DX_PI * 2);
		}
	}

	//視点の設定
	SetV();
}

//==========================================================
// スロー時の角度矯正
//==========================================================
void CCamera::Slow(void)
{
	float fLeftRot = m_rot.y - (m_SlowOldRot.y + D3DX_PI * MAX_SLOWROT);
	float fRightRot = m_rot.y - (m_SlowOldRot.y + -D3DX_PI * MAX_SLOWROT);

	if (fLeftRot < -D3DX_PI)
	{//角度がΠを超えた場合
		fLeftRot += D3DX_PI * 2;
	}
	if (fLeftRot > D3DX_PI)
	{//角度がΠを超えた場合
		fLeftRot += -D3DX_PI * 2;
	}

	if (fRightRot < -D3DX_PI)
	{//角度がΠを超えた場合
		fRightRot += D3DX_PI * 2;
	}
	if (fRightRot > D3DX_PI)
	{//角度がΠを超えた場合
		fRightRot += -D3DX_PI * 2;
	}

	if (fLeftRot > 0.0f)
	{
		m_rot.y = (m_SlowOldRot.y + D3DX_PI * MAX_SLOWROT);
	}

	if (fRightRot < 0.0f)
	{
		m_rot.y = (m_SlowOldRot.y + -D3DX_PI * MAX_SLOWROT);
	}
}

//==========================================================
// ズーム
//==========================================================
void CCamera::Zoom(void)
{
	m_fZoom += (m_fDestZoom - m_fZoom) * ZOOM_SPEED;
}

//==========================================================
// 向きを設定
//==========================================================
void CCamera::SetRotation(D3DXVECTOR3 rot)
{
	m_rot = rot;

	if (m_rot.y > D3DX_PI)
	{//角度がΠを超えた場合
		m_rot.y = D3DX_PI;
		m_rot.y *= -1.0f;
	}
	else if (m_rot.y < -D3DX_PI)
	{//角度がΠを超えた場合
		m_rot.y = -D3DX_PI;
		m_rot.y *= -1.0f;
	}

	if (m_rot.z < MIN_CAMERA_ROTZ)
	{//角度が限界を超えた場合
		m_rot.z = MIN_CAMERA_ROTZ;
	}
	else if (m_rot.z > MAX_CAMERA_ROTZ)
	{//角度が限界を超えた場合
		m_rot.z = MAX_CAMERA_ROTZ;
	}

	SetV();
}

//==========================================================
// タイトル自動回転
//==========================================================
void CCamera::TitleRotateCamera(void)
{
	m_rot.y += TITLE_ROTATESPD;

	while (1)
	{
		if (m_rot.y >= -D3DX_PI && m_rot.y <= D3DX_PI)
		{
			break;
		}
		else if (m_rot.y > D3DX_PI)
		{
			m_rot.y += -D3DX_PI * 2;
		}
		else if (m_rot.y < -D3DX_PI)
		{
			m_rot.y += D3DX_PI * 2;
		}
	}

	SetV();
}

//==========================================================
// すべてのスタートドアが開いたときの演出カメラ
//==========================================================
void CCamera::AllOpenCamera(int nCount)
{
	if (nCount >= ZOOMSTARTDOOR_COUNT * CGame::GetNumPlayer())
	{
		// カメラを目標の向きまで回転させる
		D3DXVECTOR3 rotDest = m_Oldrot - m_rot;
		SetRotation(m_rot + rotDest * 0.1f);

		D3DXVECTOR3 posDestR = m_OldposR - m_posR;
		SetPositionR(m_posR + posDestR * 0.1f);

		D3DXVECTOR3 posDestV = m_OldposV - m_posV;
		m_posV = m_posV + posDestV * 0.1f;
	}
	else
	{
		// カメラを目標の向きまで回転させる
		D3DXVECTOR3 rotDest = GOAL_CAMERAROT - m_rot;
		SetRotation(m_rot + rotDest * 0.03f);

		D3DXVECTOR3 posDestR = GOAL_CAMERAPOSR - m_posR;
		SetPositionR(m_posR + posDestR * 0.02f);

		D3DXVECTOR3 posDestV = GOAL_CAMERAPOSV - m_posV;
		m_posV = m_posV + posDestV * 0.02f;
	}

	CManager::GetInstance()->GetDebugProc()->Print("位置R[%f, %f, %f]\n", m_posR.x, m_posR.y, m_posR.z);
	CManager::GetInstance()->GetDebugProc()->Print("位置V[%f, %f, %f]\n", m_posV.x, m_posV.y, m_posV.z);
}

//==========================================================
// V座標設定
//==========================================================
void CCamera::SetPositionV(D3DXVECTOR3 pos)
{
	m_posV = pos;
	SetR();
}

//==========================================================
// オブジェクトとの当たり判定
//==========================================================
void CCamera::CollisionObj(void)
{
	
}

//==========================================================
// R座標設定
//==========================================================
void CCamera::SetPositionR(D3DXVECTOR3 pos)
{ 
	m_posR = pos; 
	SetV();
}

//==========================================================
// コンストラクタ
//==========================================================
CMultiCamera::CMultiCamera()
{

}

//==========================================================
// デストラクタ
//==========================================================
CMultiCamera::~CMultiCamera()
{

}

//==========================================================
//カメラの初期化処理
//==========================================================
HRESULT CMultiCamera::Init(void)
{
	CCamera::Init();

	//プレイヤー追従カメラの画面位置設定
	m_viewport.X = 0;
	m_viewport.Y = 0;
	m_viewport.Width = (DWORD)(SCREEN_WIDTH * 1.0f);
	m_viewport.Height = (DWORD)(SCREEN_HEIGHT * 1.0f);
	m_viewport.MinZ = 0.0f;
	m_viewport.MaxZ = 1.0f;

	return S_OK;
}

//==========================================================
//カメラの終了処理
//==========================================================
void CMultiCamera::Uninit(void)
{
	CCamera::Uninit();
}

//==========================================================
//カメラの更新処理
//==========================================================
void CMultiCamera::Update(void)
{
	CCamera::Update();
}

//==========================================================
//カメラの設定処理
//==========================================================
void CMultiCamera::SetCamera(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//デバイスへのポインタを取得
	D3DXMATRIX mtxView = GetMtxView(), mtxProjection = GetMtxProjection();
	D3DXVECTOR3 posV = GetPositionV();
	D3DXVECTOR3 posR = GetPositionR();
	D3DXVECTOR3 vecU = GetVectorU();

	//ビューポートの設定
	pDevice->SetViewport(&m_viewport);

	//プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&mtxProjection);

	//プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH(&mtxProjection,
		D3DXToRadian(45.0f),
		(float)m_viewport.Width / (float)m_viewport.Height,
		10.0f,
		40000.0f);

	//プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &mtxProjection);

	//ビューマトリックスの初期化
	D3DXMatrixIdentity(&mtxView);

	//ビューマトリックスの作成
	D3DXMatrixLookAtLH(&mtxView,
		&posV,
		&posR,
		&vecU);

	//ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &mtxView);
}
