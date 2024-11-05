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
#include <math.h>
#include "meshwall.h"
#include "player_manager.h"
#include "fade.h"
#include "input_keyboard.h"
#include "input_gamepad.h"
#include "camera_manager.h"
#include "road.h"
#include "road_manager.h"
#include "collision.h"
#include "deltatime.h"

//===============================================
// マクロ定義
//===============================================

namespace 
{
	const float DAMAGE_APPEAR = (110.0f);	// 無敵時間インターバル
	const float DEATH_INTERVAL = (120.0f);	// 死亡インターバル
	const float SPAWN_INTERVAL = (60.0f);	// 生成インターバル
	const float CAMROT_INER = (0.2f);			// カメラ慣性
	const float KICK_LENGTH = (1000.0f);	// 攻撃範囲
	const float LIFE = (100.0f);
	const float CAMERA_NORMAL = (3000.0f);
	const float CAMERA_DETAH = (6000.0f);
	const float MOVE = (6.2f * 0.7f);		// 移動量
	const float BRAKE = (0.7f);		// ブレーキ
	const float DRIFT = (+0.3f);		// ドリフト時の補正力
	const float TURN = (0.006f);		// 旋回量
	const float GRAVITY = (-0.6f);		//プレイヤー重力
	const float ROT_MULTI = (1.0f);	// 向き補正倍率
	const float WIDTH = (20.0f);	// 幅
	const float HEIGHT = (80.0f);	// 高さ
	const float INER = (0.9f);		// 慣性
	const float ENGINE_INER = (0.01f);		// 慣性
	const float ENGINE_BRAKE = (0.006f);		// 慣性
	const float TUURN_INER = (0.9f);		// 慣性
	const float DRIFT_INER = (0.98f);		// ドリフト慣性
	const float BRAKE_INER = (0.05f);
	const float RES = (1.98f);		// 減速
	const float JUMP = (16.0f);
	const float REF_INER = (1.2f);	// 壁で反射する時の倍率
	const float FRAME_RATE_SCALER = 60.0f;  // フレームレートを考慮した速度の調整
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
	m_Info.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Info.posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Info.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Info.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Info.pRoad = nullptr;
	m_fRotMove = 0.0f;
	m_fRotDiff = 0.0f;
	m_fRotDest = 0.0f;
	m_fBrake = 0.0f;
	m_fEngine = 0.0f;
	m_fTurnSpeed = 0.0f;
	m_fHandle = 0.0f;
	m_fLife = LIFE;
	m_fCamera = CAMERA_NORMAL;
	m_type = TYPE_NONE;
	m_nId = -1;
	m_fNitroCool = 0.0f;
	m_Info.fSlideMove = 0.0f;
	m_pObj = nullptr;
	m_pPrev = nullptr;
	m_pNext = nullptr;
	m_pDamageEffect = nullptr;
	m_pSound = nullptr;
	m_fbrakePitch = 0.0f;
	m_fbrakeVolume = 0.0f;
	m_pAfterburner = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\afterburner.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\taillamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	m_pBackdust = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\backdust.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
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
	m_pObj = CObjectX::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), "data\\MODEL\\flyingscooter.x");
	SetMatrix();
	m_pSound = CMasterSound::CObjectSound::Create("data\\SE\\idol.wav", -1);
	m_pSoundBrake = CMasterSound::CObjectSound::Create("data\\SE\\flight.wav", -1);
	m_pSoundBrake->SetVolume(0.0f);
	pRadio = CRadio::Create();
	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CPlayer::Uninit(void)
{	
	SAFE_UNINIT(m_pObj)
	SAFE_DELETE(m_pTailLamp);
	SAFE_DELETE(m_pBackdust);
	SAFE_DELETE(m_pAfterburner);
	SAFE_DELETE(m_pDamageEffect);
	SAFE_UNINIT_DELETE(m_pSound);
	SAFE_UNINIT_DELETE(m_pSoundBrake);

	SAFE_UNINIT_DELETE(pRadio);
	CPlayerManager::GetInstance()->ListOut(this);

	// 廃棄
	Release();
}

//===============================================
// 更新処理
//===============================================
void CPlayer::Update(void)
{	
	DEBUGKEY();
	// 前回の座標を取得
	m_Info.posOld = GetPosition();

	StateSet();	
	pRadio->Update();
	if (m_type == TYPE_ACTIVE)
	{
		
		// プレイヤー操作
		Controller();

		// 最寄りの道検索
		//SearchRoad();

		// 当たり判定
		Collision();

		// オンライン送信
		CManager::GetInstance()->GetScene()->SendPosition(m_Info.pos);
		CManager::GetInstance()->GetScene()->SendRotation(m_Info.rot);
	}
	else 
	{

	}
	
	SetMatrix();

	if (m_pObj != nullptr)
	{
		D3DXVECTOR3 rot = GetRotation();
		D3DXVECTOR3 pos = GetPosition();
		pos.y += 100.0f;
		rot.y -= D3DX_PI * 0.5f;
		rot.z += m_fTurnSpeed * 15.0f;
		m_pObj->SetPosition(pos);
		m_pObj->SetRotation(rot);
		// エフェクト
		{
			m_pTailLamp->m_pos = pos;
			m_pTailLamp->m_rot = rot;
			m_pBackdust->m_pos = GetPosition();
			m_pBackdust->m_rot = m_pObj->GetRotation();
			m_pBackdust->m_fScale = m_fEngine * 300.0f;
			
			m_pAfterburner->m_pos = GetPosition();;
		
			m_pAfterburner->m_fScale = m_fEngine * m_fBrake * 150.0f;
		
		}
		if (m_pDamageEffect != nullptr)
		{
			m_pDamageEffect->m_pos = pos;
			m_pDamageEffect->m_rot = rot;
		}
	}

	{
		D3DXVECTOR3 rot = GetRotation();
		rot.y -= D3DX_PI * 0.5f;
		CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
		pCamera->Pursue(GetPosition(), rot, m_fCamera);
	}
	if (m_fLife<=0)
	{
		m_Info.move *= 0.7f;
		m_fCamera += (CAMERA_DETAH - m_fCamera) * 0.02f;
	}

	// デバッグ表示
	CDebugProc::GetInstance()->Print("プレイヤー :");
	CDebugProc::GetInstance()->Print("座標: [ %f, %f, %f ]", m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);
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

	// 向き補正
	Adjust();
}

//===============================================
// 移動
//===============================================
void CPlayer::Move(void)
{
	CInputKeyboard *pInputKey = CInputKeyboard::GetInstance();	// キーボードのポインタ
	CInputPad *pInputPad = CInputPad::GetInstance();
	float fThrottle = 0.0f;
	m_fBrake = 0.0f;
	if (pInputKey->GetPress(DIK_W))
	{
		fThrottle = 0.8f;
	}
	else
	{
		fThrottle = (float)pInputPad->GetRightTriggerPress(0) / 255;
	}

	if (pInputKey->GetPress(DIK_S))
	{
		m_fBrake = 1.0f;
	}
	else
	{
		m_fBrake = (float)pInputPad->GetLeftTriggerPress(0) / 255;
	
	}
	Nitro(); 
	Engine(fThrottle);
	// 入力装置確認
	if (nullptr == pInputKey)
	{
		return;
	}

	// デルタタイム取得
	float deltatime = CManager::GetInstance()->GetDeltaTime()->GetDeltaTime();
	
	m_Info.pos += m_Info.move * FRAME_RATE_SCALER * deltatime;
	float fHandle = m_fHandle;
	if (fHandle < 0.0f)
	{
		fHandle *= -1;
	}
	float fEngine = m_fEngine;
	if (fEngine > 1.0f)
	{
		fEngine = 1.0f;
	}
	float fIner = INER + (fEngine * m_fBrake * fHandle) * (DRIFT_INER - INER);
	m_fbrakeVolume += (1.0f - m_fbrakeVolume) * (m_fEngine * m_fBrake) * BRAKE_INER;
	m_fbrakePitch += (1.0f - m_fbrakePitch) * (m_fEngine * m_fBrake * fHandle) * BRAKE_INER;
	m_fbrakeVolume -= m_fbrakeVolume* BRAKE_INER;
	m_fbrakePitch -= m_fbrakePitch *BRAKE_INER;
	m_pSoundBrake->SetVolume(m_fbrakeVolume * 0.8f);
	m_pSoundBrake->SetPitch(0.5f + m_fbrakePitch);
	m_Info.move *= fIner;//移動量の減衰
}
//===============================================
//簡易エンジンシミュ
//===============================================
void  CPlayer::Engine(float fThrottle)
{
	float fAccel = (1.0f - m_fEngine) * fThrottle * ENGINE_INER;
	m_fEngine += fAccel;
	if (fThrottle <= 0)
	{
		m_fEngine -= ENGINE_BRAKE;
		if (m_fEngine <= 0)
		{
			m_fEngine = 0;
		}
	}
	//回転数から音量とピッチを操作
	m_pSound->SetPitch(0.5f + m_fEngine*1.5f);
	m_pSound->SetVolume(0.5 + fAccel*100.0f + m_fEngine);

	m_pSoundBrake->SetVolume(m_fBrake * m_fEngine * 0.3f);
	//加速
	m_Info.move.z += MOVE * sinf(-m_Info.rot.y) * m_fEngine;
	m_Info.move.x += MOVE * cosf(-m_Info.rot.y) * m_fEngine;
	//ブレーキ
	m_Info.move.z -= MOVE * sinf(-m_Info.rot.y) * m_fEngine * m_fBrake * BRAKE;
	m_Info.move.x -= MOVE * cosf(-m_Info.rot.y) * m_fEngine * m_fBrake * BRAKE;
	CManager::GetInstance()->GetDebugProc()->Print("fThrottle:%f\nm_fEngine%f\n", fThrottle, m_fEngine);
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
		m_fHandle = 1.0f;
	}
	else if (pInputKey->GetPress(DIK_A))
	{
		m_fHandle = -1.0f;
	}
	else
	{
		m_fHandle = pInputPad->GetLStick(0, 0.1f).x;
	}
	
	m_fTurnSpeed += TURN * m_fHandle * (1.0f + m_fBrake * m_fEngine * DRIFT);
	m_fTurnSpeed*= TUURN_INER;
	// 入力装置確認
	if (nullptr == pInputKey) 
	{
		return;
	}

	// m_fRotDiffの値変えてくれれば補正するよん
}

//===============================================
// 調整
//===============================================
void CPlayer::Adjust(void)
{

	m_Info.rot.y += m_fTurnSpeed;

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
// 最寄りの道判定
//===============================================
void CPlayer::SearchRoad()
{
	CRoadManager* pRoadManager = CRoadManager::GetInstance();

	CRoad* pRoad = pRoadManager->GetTop();
	CRoad* pRoadClose = pRoadManager->GetTop();
	

	if (pRoad == nullptr) 
	{
		return;
	}

	float length = D3DXVec3Length(&(pRoadClose->GetPosition() - m_Info.pos));
	float lengthClose = 0.0f;

	while (pRoad != nullptr)
	{// 使用されていない状態まで

		CRoad* pRoadNext = pRoad->GetNext();	// 次のオブジェクトへのポインタを取得

		// 距離判定処理
		lengthClose = D3DXVec3Length(&(pRoad->GetPosition() - m_Info.pos));

		if (length > lengthClose)
		{
			length = lengthClose;
			pRoadClose = pRoad;
		}

		pRoad = pRoadNext;	// 次のオブジェクトに移動
	}

	m_Info.pRoad = pRoadClose;
}

//===============================================
// 当たり判定処理
//===============================================
bool CPlayer::Collision(void)
{
	CObjectX* pObjectX = CObjectX::GetTop();	// 先頭を取得

	while (pObjectX != nullptr)
	{// 使用されていない状態まで

		CObjectX* pObjectXNext = pObjectX->GetNext();	// 次のオブジェクトへのポインタを取得

		D3DXVECTOR3 posObjectX = pObjectX->GetPosition();
		D3DXVECTOR3 rotObjectX = pObjectX->GetRotation();
		D3DXVECTOR3 sizeMax = pObjectX->GetVtxMax();
		D3DXVECTOR3 sizeMin = pObjectX->GetVtxMin();
		D3DXVECTOR3 pVecCollision;

		bool bCollision = collision::ReflectPointToOBB(&pVecCollision, &m_Info.pos, &m_Info.move, m_Info.posOld, posObjectX, rotObjectX, (sizeMax - sizeMin) * 0.5f, REF_INER);

		if (bCollision)
		{
			D3DXVECTOR3 vecMoveNor = m_Info.move;
			D3DXVec3Normalize(&vecMoveNor, &m_Info.move);
			D3DXVec3Normalize(&pVecCollision, &pVecCollision);

			float engineLow = fabsf(D3DXVec3Dot(&pVecCollision, &vecMoveNor) * 0.1f);
			m_fEngine *= 0.9f + engineLow;

			return true;
		}

		pObjectX = pObjectXNext;	// 次のオブジェクトに移動
	}

	return false;
}

//===============================================
// ダメージ
//===============================================
void CPlayer::Damage(float fDamage)
{
	m_fLife -= fDamage;
	if (m_fLife <= 0.0f)
	{
		SAFE_DELETE(m_pDamageEffect);
		m_pDamageEffect = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\explosion.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 120.0f, false, false);
	}
	else if (m_fLife <= LIFE * 0.5f)
	{
		SAFE_DELETE(m_pDamageEffect);
		m_pDamageEffect = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\moderately_damage.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 60.0f, false, false);
	}
	else if (m_fLife <= LIFE * 0.8f)
	{
		SAFE_DELETE(m_pDamageEffect);
		m_pDamageEffect = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\minor_damage.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 60.0f, false, false);
	}
}
//===============================================
// 加速
//===============================================
void CPlayer::Nitro()
{
	CInputKeyboard* pInputKey = CInputKeyboard::GetInstance();	// キーボードのポインタ
	CInputPad* pInputPad = CInputPad::GetInstance();
	if (m_fNitroCool==0.0f&& (pInputKey->GetTrigger(DIK_SPACE)|| pInputPad->GetTrigger(CInputPad::BUTTON_B,0)))
	{
 		Damage(LIFE * 0.1f);
		m_Info.state = STATE::STATE_NITRO;
		m_Info.fStateCounter = 60.0f;
		m_fNitroCool = 120.0f;
	}
	if (m_Info.state == STATE::STATE_NITRO)
	{
		m_fEngine = 2.0f;
	}
	else
	{
		m_fNitroCool--;;
		if (m_fNitroCool < 0.0f)
		{
			m_fNitroCool = 0.0f;
		}
	}
	
}
//===============================================
// デバッグキー
//===============================================
#if _DEBUG
void CPlayer::DEBUGKEY(void)
{
	CInputKeyboard* pInputKey = CInputKeyboard::GetInstance();	// キーボードのポインタ
	CInputPad* pInputPad = CInputPad::GetInstance();
	if (pInputKey->GetTrigger(DIK_K))
	{
		Damage(40.0f);
	}
}
#else
void CPlayer::DEBUGKEY(void){}
#endif
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
	case STATE_NITRO:
	{
		m_Info.fStateCounter -= fSlawMul;

		if (m_Info.fStateCounter <= 0.0f)
		{
			m_Info.fStateCounter = 0.0f;
			m_Info.state = STATE_NORMAL;
		}
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
void CPlayer::SetMotion(int nMotion)
{
	
	
}

//===============================================
// プレイヤーの描画設定
//===============================================
void CPlayer::SetDraw(bool bDraw)
{
	
}
