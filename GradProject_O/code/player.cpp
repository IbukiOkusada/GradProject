//===============================================
//
// プレイヤーの処理 [player.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "player.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "camera.h"
#include "Xfile.h"
#include "slow.h"
#include "texture.h"
#include "meshfield.h"
#include "Xfile.h"
#include "meshorbit.h"
#include "game.h"
#include "filter.h"
#include "object2D.h"
#include "meshwall.h"
#include "objectX.h"
#include "sound.h"
#include <assert.h>
#include "billboard.h"
#include "character.h"
#include "motion.h"
#include "sound.h"
#include "model.h"
#include "particle.h"
#include "effect.h"
#include <math.h>
#include "meshwall.h"
#include "player_manager.h"
#include "fade.h"
#include "input_keyboard.h"
#include "input_gamepad.h"
#include "camera_manager.h"
#include "convenience.h"
//===============================================
// マクロ定義
//===============================================

namespace {
	const float DAMAGE_APPEAR = (110.0f);	// 無敵時間インターバル
	const float DEATH_INTERVAL = (120.0f);	// 死亡インターバル
	const float SPAWN_INTERVAL = (60.0f);	// 生成インターバル
	const float CAMROT_INER = (0.2f);			// カメラ慣性
	const float KICK_LENGTH = (1000.0f);	// 攻撃範囲
	const int LIFE = (10);
	const float MOVE = (1.2f);		// 移動量
	const float BRAKE = (0.98f);		// ブレーキ

	const float TURN = (0.025f);		// 旋回量
	const float GRAVITY = (-0.6f);		//プレイヤー重力
	const float ROT_MULTI = (1.0f);	// 向き補正倍率
	const float WIDTH = (20.0f);	// 幅
	const float HEIGHT = (80.0f);	// 高さ
	const float INER = (0.98f);		// 慣性
	const float RES = (1.98f);		// 減速
	const float JUMP = (16.0f);

	const float STIFFNESS = (11.0f); //剛性係数 一般的な設定値の範囲: 10 ～ 15（バイクのタイヤ）、15 ～ 30（レース用タイヤ）
	const float SHAPE = (1.2f); //形状係数 一般的なバイクのタイヤでは、1.2 ～ 1.5くらいに設定する
	const float PEAK_FORCE = (1000.0f);  //最大力 重量の軽いバイクのタイヤは、一般的に 1000 ～ 1500 N の範囲に設定
	const float CURVATURE = (0.9f); //曲率調整係数 0～1。通常は 0.9～1.0

	const float MAX_RPM = (6750.0f); //最大回転数
	const float IDOL_RPM = (1.0f); //最低回転数
	const float ENGINE_INR = (0.1f); //エンジンの慣性
	const float TIRERADIUS = (0.35f); // タイヤの半径（m）
	const float WHEEL_CIRCUMFERENCE = (2.0f * D3DX_PI * TIRERADIUS); // タイヤの円周（m）
}

// 前方宣言

//===============================================
// コンストラクタ
//===============================================
//CPlayer::CPlayer()
//{
//	// 値をクリアする
//	m_nCounterAnim = 0;
//	m_nPatternAnim = 0;
//}

//===============================================
// コンストラクタ(オーバーロード)
//===============================================
CPlayer::CPlayer()
{
	// 値をクリアする
	m_Info.pos = VECTOR3_ZERO;
	m_Info.posOld = VECTOR3_ZERO;
	m_Info.rot = VECTOR3_ZERO;
	m_Info.move = VECTOR3_ZERO;
	Tire_rot = VECTOR3_ZERO;
	RotSpeed = VECTOR3_ZERO;
	m_fRotMove = 0.0f;
	m_fRotDiff = 0.0f;
	m_fRotDest = 0.0f;
	m_fBrake = 0.0f;
	m_fAccel = 0.0f;
	m_fRPM = 0.0f;
	m_type = TYPE_NONE;
	m_nId = -1;
	m_Info.fSlideMove = 0.0f;
	m_pObj = nullptr;
	m_pPrev = nullptr;
	m_pNext = nullptr;

	CPlayerManager::GetInstance()->ListIn(this);
}

//===============================================
// デストラクタ
//===============================================
CPlayer::~CPlayer()
{
	
}

//===============================================
// 初期化処理
//===============================================
HRESULT CPlayer::Init(void)
{
	// 腰の生成
	m_Info.state = STATE_APPEAR;
	m_type = TYPE_NONE;

	return S_OK;
}

//===============================================
// 初期化処理(オーバーロード)
//===============================================
HRESULT CPlayer::Init(const char *pBodyName, const char *pLegName)
{
	m_pObj = CObjectX::Create(VECTOR3_ZERO, VECTOR3_ZERO, "data\\MODEL\\car000.x");
	SetMatrix();

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CPlayer::Uninit(void)
{
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	CPlayerManager::GetInstance()->ListOut(this);

	// 廃棄
	Release();
}

//===============================================
// 更新処理
//===============================================
void CPlayer::Update(void)
{	
	// 前回の座標を取得
	m_Info.posOld = GetPosition();

	StateSet();	

	if (m_type == TYPE_ACTIVE)
	{
		
		// プレイヤー操作
		Controller();

		// オンライン送信
		CManager::GetInstance()->GetScene()->SendPosition(m_Info.pos);
		CManager::GetInstance()->GetScene()->SendRotation(m_Info.rot);
	}
	else {

	}
	
	SetMatrix();

	if (m_pObj != nullptr)
	{
		D3DXVECTOR3 rot = GetRotation();
		rot.y += D3DX_PI * 0.5f;
		m_pObj->SetPosition(GetPosition());
		m_pObj->SetRotation(rot);
	}

	{
		D3DXVECTOR3 rot = GetRotation();
		rot.y -= D3DX_PI * 0.5f;
		CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
		pCamera->Pursue(GetPosition(), rot, 3000.0f);
	}

	// エフェクト
	{
		D3DXVECTOR3 rot = GetRotation();
		rot.y -= D3DX_PI * 0.5f;
		D3DXVECTOR3 pos = GetPosition();
		pos.x += sinf(rot.y) * 100.0f;
		pos.z += cosf(rot.y) * 100.0f;
		CParticle::Create(pos, CEffect::TYPE_SMAKE);
	}
}

//===============================================
// 生成
//===============================================
CPlayer *CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, const char *pBodyName, const char *pLegName)
{
	CPlayer *pPlayer = nullptr;

	// オブジェクト2Dの生成
	pPlayer = DEBUG_NEW CPlayer;

	if (nullptr != pPlayer)
	{// 生成できた場合
		// 初期化処理
		pPlayer->Init(pBodyName, pLegName);

		// 座標設定
		pPlayer->SetPosition(pos);

		// 向き設定
		pPlayer->SetRotation(rot);

		pPlayer->m_fRotDest = rot.y;

		// 移動量設定
		pPlayer->SetMove(move);
	}
	else
	{// 生成に失敗した場合
		return nullptr;
	}

	return pPlayer;
}

//===============================================
// 操作処理
//===============================================
void CPlayer::Controller(void)
{
	D3DXVECTOR3 rot = GetRotation();	// 向きを取得
	m_fRotMove = rot.y;	//現在の向きを取得

	// 移動
	Move();

	// 回転
	Rotate();

	//// 向き補正
	//Adjust();
}

//===============================================
// 移動
//===============================================
void CPlayer::Move(void)
{
	CInputKeyboard *pInputKey = CInputKeyboard::GetInstance();	// キーボードのポインタ
	CInputPad *pInputPad = CInputPad::GetInstance();
	m_fBrake = 0.0f;
	if (pInputKey->GetPress(DIK_W))
	{
		m_fAccel = 1.0f;
	/*	m_Info.move.z += MOVE * sinf(-m_Info.rot.y);
		m_Info.move.x += MOVE * cosf(-m_Info.rot.y);*/
	}
	else
	{
		m_fAccel = (float)pInputPad->GetRightTriggerPress(0) / 255;
	}
	Engine(m_fAccel);
	LateralMove();
	LongitudinalMove();

	// 入力装置確認
	if (nullptr == pInputKey){
		return;
	}


	m_Info.pos += m_Info.move;
	m_Info.move *= INER;//移動量の減衰
}

//===============================================
// 向き
//===============================================
void CPlayer::Rotate(void)
{
	CInputKeyboard* pInputKey = CInputKeyboard::GetInstance();	// キーボードのポインタ
	CInputPad* pInputPad = CInputPad::GetInstance();
	if (pInputKey->GetPress(DIK_D))
	{
		Tire_rot.y += TURN * (1.0f+m_fBrake);
	}
	else if (pInputKey->GetPress(DIK_A))
	{
		Tire_rot.y -= TURN * (1.0f + m_fBrake);
	}
	else
	{
		Tire_rot.y += TURN *pInputPad->GetLStick(0, 0.1f).x * (1.0f + m_fBrake);
	}
	// 入力装置確認
	if (nullptr == pInputKey) {
		return;
	}

	// m_fRotDiffの値変えてくれれば補正するよん
}

//===============================================
// 調整
//===============================================
void CPlayer::Adjust(void)
{
	while (1)
	{
		if (m_fRotDiff > D3DX_PI || m_fRotDiff < -D3DX_PI)
		{//-3.14～3.14の範囲外の場合
			if (m_fRotDiff > D3DX_PI)
			{
				m_fRotDiff += (-D3DX_PI * 2);
			}
			else if (m_fRotDiff < -D3DX_PI)
			{
				m_fRotDiff += (D3DX_PI * 2);
			}
		}
		else
		{
			break;
		}
	}

	m_fRotDest = m_fRotDiff - m_fRotMove;	//目標までの移動方向の差分

	while (1)
	{
		if (m_fRotDest > D3DX_PI || m_fRotDest < -D3DX_PI)
		{//-3.14～3.14の範囲外の場合
			if (m_fRotDest > D3DX_PI)
			{
				m_fRotDest += (-D3DX_PI * 2);
			}
			else if (m_fRotDest < -D3DX_PI)
			{
				m_fRotDest += (D3DX_PI * 2);
			}
		}
		else
		{
			break;
		}
	}

	m_Info.rot.y += m_fRotDest * ROT_MULTI;

	while (1)
	{
		if (m_Info.rot.y > D3DX_PI || m_Info.rot.y < -D3DX_PI)
		{//-3.14～3.14の範囲外の場合
			if (m_Info.rot.y > D3DX_PI)
			{
				m_Info.rot.y += (-D3DX_PI * 2);
			}
			else if (m_Info.rot.y < -D3DX_PI)
			{
				m_Info.rot.y += (D3DX_PI * 2);
			}
		}
		else
		{
			break;
		}
	}
}
//===============================================
// エンジンのシミュレーション
//===============================================
float CPlayer::Engine(float fAccel)
{
	float targetRPM = fAccel * MAX_RPM;
	m_fRPM = m_fRPM + (targetRPM - m_fRPM) * ENGINE_INR / 60;
	if (fAccel == 0.0f) {
		m_fRPM -= 1000.0f / 60;
		if (m_fRPM < IDOL_RPM) {
			m_fRPM = IDOL_RPM;  // アイドル回転数以下には下がらない
		}
	}
	return m_fRPM;
}
//===============================================
// 横力計算
//===============================================
float CPlayer::LateralForce(float fSlipAngle)
{
	float fY = PEAK_FORCE * sinf(SHAPE * atanf(STIFFNESS * fSlipAngle - CURVATURE * (STIFFNESS * fSlipAngle - atanf(STIFFNESS * fSlipAngle))));
	return fY;
}
//===============================================
// 縦力計算
//===============================================
float CPlayer::LongitudinalForce(float fSlipRatio)
{
	float fX = PEAK_FORCE * sinf(SHAPE * atanf(STIFFNESS * fSlipRatio - CURVATURE * (STIFFNESS * fSlipRatio - atanf(STIFFNESS * fSlipRatio))));
	return fX;
}
//===============================================
// 横力移動
//===============================================
void CPlayer::LateralMove()
{
	
	// Pacejkaモデルで横力を計算
	float lateralForce = LateralForce(CalculateSlipAngle());

	// 車両のヨー加速度を計算 (慣性モーメントと横力を使用)
	float yawAcceleration = lateralForce * 3 / 12;

	// ヨー角速度を更新
	RotSpeed.y += yawAcceleration / 60;

	// ヨー角を更新（バイクの向きを変える）
	m_Info.rot.y += RotSpeed.y / 60;
}
//===============================================
// 縦力移動
//===============================================
void CPlayer::LongitudinalMove()
{
	// Pacejkaモデルで縦力を計算
	float longitudinalForce = LongitudinalForce(CalculateSlipRatio());

	// 車両の加速度を計算
	float acceleration = longitudinalForce / 140.0f;

	// 速度を更新
	m_Info.move.z += acceleration /60;

	// 位置を更新
	m_Info.pos += m_Info.move;
}
//===============================================
// スリップ率の計算
//===============================================
float CPlayer::CalculateSlipRatio()
{
	// タイヤの回転速度を計算
	float wheelSpeed = (m_fRPM * WHEEL_CIRCUMFERENCE) / 60.0f; // m/s
	float vehicleSpeed = GetDistance(VECTOR3_ZERO, GetMove());
	// スリップ率を計算
	float slipRatio = (wheelSpeed - vehicleSpeed) / wheelSpeed;

	return slipRatio;
}
//===============================================
// スリップ角の計算
//===============================================
float CPlayer::CalculateSlipAngle()
{

	// タイヤの進行方向速度
	D3DXVECTOR3 Move = GetMove();
	D3DXMATRIX RotMtx;
	D3DXMatrixIdentity((&RotMtx));
	D3DXMatrixRotationY(&RotMtx, Tire_rot.y);
	D3DXVec3TransformCoord(&Move, &Move, &RotMtx);
	
	// スリップ角を計算
	float slipAngle = atan2(Move.x, fabs(Move.z));  // atan2を使用して正確に角度を取得
	return slipAngle;
}
//===============================================
// 状態管理
//===============================================
void CPlayer::StateSet(void)
{
	float fSlawMul = CManager::GetInstance()->GetSlow()->Get();
	switch (m_Info.state)
	{
	case STATE_APPEAR:
	{
		m_Info.fStateCounter -= fSlawMul;

		if (m_Info.fStateCounter <= 0.0f)
		{
			m_Info.fStateCounter = 0.0f;
			m_Info.state = STATE_NORMAL;
		}
	}
		break;

	case STATE_NORMAL:
	{

	}
		break;

	case STATE_DAMAGE:
	{
		m_Info.fStateCounter -= fSlawMul;

		if (m_Info.fStateCounter <= 0.0f)
		{
			m_Info.fStateCounter = DAMAGE_APPEAR;
			m_Info.state = STATE_APPEAR;
		}
	}
		break;

	case STATE_DEATH:
	{
		m_Info.fStateCounter -= fSlawMul;

		if (m_Info.fStateCounter <= 0.0f)
		{
			m_Info.fStateCounter = DAMAGE_APPEAR;
			m_Info.state = STATE_SPAWN;
		}
	}
		break;

	case STATE_SPAWN:
	{
		m_Info.fStateCounter -= fSlawMul;

		if (m_Info.fStateCounter <= 0.0f)
		{
			m_Info.fStateCounter = SPAWN_INTERVAL;
			m_Info.state = STATE_APPEAR;

			// 煙のパーティクル生成
			CParticle::Create(m_Info.pos, CEffect::TYPE_SMAKE);
		}
	}
		break;

	}
}

//===============================================
// 体力設定
//===============================================
void CPlayer::SetType(TYPE type)
{
	m_type = type;
}

//===============================================
// マトリックス設定
//===============================================
void CPlayer::SetMatrix(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	//デバイスへのポインタを取得
	D3DXMATRIX mtxRot, mtxTrans;	//計算用マトリックス

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_Info.mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_Info.rot.y, m_Info.rot.x, m_Info.rot.z);
	D3DXMatrixMultiply(&m_Info.mtxWorld, &m_Info.mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);
	D3DXMatrixMultiply(&m_Info.mtxWorld, &m_Info.mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_Info.mtxWorld);
}

//===============================================
// 指定モーションに設定
//===============================================
void CPlayer::SetMotion(int nMotion) {
	
	
}

//===============================================
// プレイヤーの描画設定
//===============================================
void CPlayer::SetDraw(bool bDraw)
{
	
}

//===============================================
// 
//===============================================
void CPlayer::Collision(void)
{

}