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
#include "bridge.h"
#include "meter.h"
#include "baggage.h"
#include "camera_action.h"
#include "baggage.h"
#include "goal.h"
#include "a_star.h"
#include "container.h"
#include "pred_route.h"
#include "scrollString2D.h"
#include "scrollText2D.h"
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
	const float CAMERA_NORMAL = (2600.0f);
	const float ENGINE_ADDCAMERA = (800.0f);
	const float CAMERA_DETAH = (6000.0f);
	const float MOVE = (6.2f * 0.7f);		// 移動量
	const float BRAKE = (0.7f);		// ブレーキ
	const float DRIFT = (+0.3f);		// ドリフト時の補正力
	const float TURN = (0.006f);		// 旋回量
	const float GRAVITY = (-12.0f);		//プレイヤー重力
	const float ROT_MULTI = (1.0f);	// 向き補正倍率
	const float WIDTH = (20.0f);	// 幅
	const float HEIGHT = (80.0f);	// 高さ
	const float INER = (0.9f);		// 慣性
	const float ENGINE_INER = (0.01f);		// 慣性
	const float ENGINE_BRAKE = (0.006f);		// 慣性
	const float TUURN_INER = (0.9f);		// 慣性
	const float DRIFT_INER = (0.975f);		// ドリフト慣性
	const float BRAKE_INER = (0.05f);
	const float RES = (1.98f);		// 減速
	const float JUMP = (16.0f);
	const float REF_INER = (1.2f);	// 壁で反射する時の倍率
	const float FRAME_RATE_SCALER = 60.0f;  // フレームレートを考慮した速度の調整
	const float BAGGAGE_LENGTH = 200.0f;
	const float CAMERA_ENGINEMULTI = 0.15f;
	const float NITRO_COUNTER = 6.0f;
	const float NITRO_COOL = 120.0f;
	const vector<string> START_TEXT[NUM_TXT]
	{
	{">> BOOT SEQUENCE INITIATED.......................................",
	">> CORE ENGINE ONLINE : FUSION - CYCLE MK.7",
	">> BATTERY LEVEL : 97 % | RANGE ESTIMATE : 320 KM	",
	">> NEURAL LINK : SYNCHRONIZED",
	">> COOLING SYSTEM : INTEGRATED | STATUS : OPTIMAL	",
	">> MODULE CHECK :									",
	"-DELIVERY MODULE : ACTIVE							",
	"- FREEZER UNIT : LOCKED & COLD(-20°C)			   " ,
	"- AI NAVIGATION : ONLINE							",
	"",
	"",
	"",
	"WELCOME BACK, RIDER. ALL SYSTEMS GO. READY TO DELIVER."},
	{
	">> FREEZER UNIT - STATUS CHECK :										  ",
	"-TEMPERATURE : -20°C													  ",
	"- INVENTORY CAPACITY : 50 %											  ",
	">> CURRENT STOCK :														  ",
	"-VANILLA BEAM : 12 UNITS												  ",
	"- NEON STRAWBERRY : 10 UNITS											  ",
	"- CHROME CHOCO : 15 UNITS												  ",
	"- CYBER MINT : 8 UNITS													  ",
	"- GLITCH COCONUT : 5 UNITS												  ",
	"																		  ",
	">> WARNING : COOLANT RESERVOIR BELOW 15 % .REPLENISH TO AVOID TEMP SPIKE.",
	},
	{
	">> DELIVERY MANAGEMENT SYSTEM :							   ",
	">> PENDING DELIVERIES :										   ",
	"1. CLIENT : SILVER SKY RESORT | ITEM : 6x NEON STRAWBERRY		   ",
	"STATUS : PACKED & READY | ETA : 15 MIN							   ",
	"2. CLIENT : AXON PLAZA | ITEM : 3x CYBER MINT, 2x GLITCH COCONUT  ",
	"STATUS : PENDING | ETA : 35 MIN								   ",
	"3. CLIENT : DARK CIRCUIT CLUB | ITEM : 10x CHROME CHOCO		   ",
	"STATUS : PACKED | ETA : 50 MIN									   ",
	"																   ",
	">> ROUTE OPTIMIZATION :										   ",
	"CALCULATING...													   ",
	"SUGGESTED ROUTE : 1 → 3 → 2 (LOW TRAFFIC ZONES)					",
	""
	">> ALERT : HIGHWAY 42 BLOCKED.RECOMMENDED DETOUR VIA SKYLINE LANE.",
	},
	{
	">> ALL MODULES GREEN.NAVIGATION LOCKED.",
	">> SYSTEM MESSAGE : ICE DELIVERED COLD, HEARTS WARMED. GOOD LUCK OUT THERE, RIDER.			                      ",
	">> ENGAGE ENGINE IN 3............ 2............ 1............                               ",
	">> [NEON TRAIL ACTIVATED]                                                                   ",
	}
	};
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
	m_fLifeOrigin = m_fLife;
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
	m_pBaggage = nullptr;	
	m_pPredRoute = nullptr;
	m_fbrakePitch = 0.0f;
	m_fbrakeVolume = 0.0f;
	m_nNumDeliveryStatus = 0;
	m_pAfterburner = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\afterburner.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	m_pTailLamp = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\taillamp.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	m_pBackdust = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\backdust.efkefc", VECTOR3_ZERO, VECTOR3_ZERO, VECTOR3_ZERO, 45.0f, false, false);
	m_pCollSound = CMasterSound::CObjectSound::Create("data\\SE\\collision.wav", 0);
	m_pCollSound->Stop();
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
	m_pObj->SetType(CObject::TYPE_PLAYER);
	m_pObj->SetRotateType(CObjectX::TYPE_QUATERNION);
	SetMatrix();
	m_pSound = CMasterSound::CObjectSound::Create("data\\SE\\idol.wav", -1);
	m_pSound->SetVolume(0.0f);
	m_pSoundBrake = CMasterSound::CObjectSound::Create("data\\SE\\flight.wav", -1);
	m_pSoundBrake->SetVolume(0.0f);
	pRadio = CRadio::Create();
	m_pNavi = CNavi::Create();
	CContainer::Create();
	m_pPredRoute = CPredRoute::Create(this);
	m_pFont[0] = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, D3DXVECTOR3(400.0f, 200.0f, 0.0f),0.0025f,20.0f, 20.0f, XALIGN_LEFT, YALIGN_TOP);
	m_pFont[1] = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, D3DXVECTOR3(500.0f, 150.0f, 0.0f), 0.0025f, 15.0f, 15.0f, XALIGN_LEFT, YALIGN_TOP);
	m_pFont[2] = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, D3DXVECTOR3(50.0f, 50.0f, 0.0f), 0.001f, 15.0f, 15.0f, XALIGN_LEFT, YALIGN_TOP);
	m_pFont[3] = CScrollText2D::Create("data\\FONT\\x12y16pxMaruMonica.ttf", false, D3DXVECTOR3(300.0f, 300.0f, 0.0f), 0.025f, 20.0f, 20.0f, XALIGN_LEFT, YALIGN_TOP,VECTOR3_ZERO,D3DXCOLOR(0.0f,1.0f,1.0f,1.0f));
	for (int i = 0; i < NUM_TXT; i++)
	{
		for (int j = 0; j < START_TEXT[i].size(); j++)
		{
			m_pFont[i]->PushBackString(START_TEXT[i][j]);
		}
	}
	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CPlayer::Uninit(void)
{	
	SAFE_UNINIT(m_pObj);
	SAFE_UNINIT(m_pBaggage);
	SAFE_DELETE(m_pTailLamp);
	SAFE_DELETE(m_pBackdust);
	SAFE_DELETE(m_pAfterburner);
	SAFE_DELETE(m_pDamageEffect);
	SAFE_UNINIT_DELETE(m_pSound);
	SAFE_UNINIT_DELETE(m_pSoundBrake);
	SAFE_UNINIT_DELETE(pRadio);
	SAFE_UNINIT_DELETE(m_pCollSound);
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
	for (int i = 0; i < NUM_TXT; i++)
	{
		if (CManager::GetInstance()->GetFade()->GetState() == CFade::STATE_NONE && !m_pFont[i]->GetEnd())
		{
			if (i>0)
			{
				if (!m_pFont[i - 1]->GetEnd())continue;
			}
			m_pFont[i]->SetEnableScroll(true);
		}
		else
		{
			if (m_pFont[i]->GetNumString() > 0 && m_pFont[i]->GetEnd())
			{
				m_pFont[i]->DeleteString(0);
			}
		}
	}

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
	
	// マトリックス
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
		m_pObj->SetShadowHeight(GetPosition().y);
		// エフェクト
		{
			m_pTailLamp->m_pos = pos;
			m_pTailLamp->m_rot = rot;
			m_pBackdust->m_pos = GetPosition();
			m_pBackdust->m_rot = m_pObj->GetRotation();
			m_pBackdust->m_Scale =VECTOR3_ONE* m_fEngine * 300.0f;
			
			m_pAfterburner->m_pos = GetPosition();;
		
			m_pAfterburner->m_Scale = VECTOR3_ONE * m_fEngine * m_fBrake * 150.0f;
		
		}
		if (m_pDamageEffect != nullptr)
		{
			m_pDamageEffect->m_pos = pos;
			m_pDamageEffect->m_rot = rot;
		}
	}

	// 荷物を所持
	if (m_pBaggage != nullptr)
	{
		D3DXVECTOR3 rot = GetRotation();
		rot.y -= D3DX_PI * 0.5f;
		CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
		pCamera->Pursue(GetPosition(), rot, m_fCamera);
	}

	// カメラの距離設定
	if (m_fLife<=0)
	{
		// 死亡カメラ
		m_Info.move *= 0.7f;
		m_fCamera += (CAMERA_DETAH - m_fCamera) * 0.02f;
	}
	else
	{
		// 速度によって補正カメラ
		float engine = m_fEngine;
		if (engine < CAMERA_ENGINEMULTI) { engine = 0.0f; }
		else { engine -= CAMERA_ENGINEMULTI; }
		CDebugProc::GetInstance()->Print("engine [ %f ]", engine);
		m_fCamera = CAMERA_NORMAL + ENGINE_ADDCAMERA * engine;
	}

	if (CBaggage::GetThrowList()->GetNum() == 0 && m_pBaggage == nullptr)
	{
		m_pBaggage = CBaggage::Create(GetPosition());
	}

	if (m_pBaggage != nullptr)
	{
		D3DXVECTOR3 pos = GetPosition();

		pos.y += 100.0f;
		m_pBaggage->SetPosition(pos);
		m_pBaggage->GetObj()->SetShadowHeight(GetPosition().y);
	}

	// デバッグ表示
	CDebugProc::GetInstance()->Print("プレイヤー :");
	CDebugProc::GetInstance()->Print("座標: [ %f, %f, %f ]", m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);
	CDebugProc::GetInstance()->Print(" : 向き: [ %f, %f, %f ]\n", m_pObj->GetRotation().x, m_pObj->GetRotation().y, m_pObj->GetRotation().z);
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
	float moveY = m_Info.move.y;
	m_Info.move *= fIner;//移動量の減衰
	m_Info.move.y = moveY;
	m_Info.move.y += GRAVITY * deltatime;

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
	m_pSound->SetVolume(0.5f + fAccel*100.0f + m_fEngine);

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
	float slowmulti = CManager::GetInstance()->GetDeltaTime()->GetSlow();
	CInputKeyboard* pInputKey = CInputKeyboard::GetInstance();	// キーボードのポインタ
	CInputPad* pInputPad = CInputPad::GetInstance();
	float diff = 0.0f;

	if (pInputKey->GetPress(DIK_D))
	{
		diff = 1.0f;
	}
	else if (pInputKey->GetPress(DIK_A))
	{
		diff = -1.0f;
	}
	else
	{
		diff = pInputPad->GetLStick(0, 0.1f).x;
	}

	float dest = (diff - m_fHandle) * slowmulti;
	m_fHandle += dest * slowmulti;
	
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
	auto list = pRoadManager->GetList();
	CRoad* pRoad = list->Get(0);
	CRoad* pRoadClose = list->Get(0);
	

	if (pRoad == nullptr) 
	{
		return;
	}

	float length = D3DXVec3Length(&(pRoadClose->GetPosition() - m_Info.pos));
	float lengthClose = 0.0f;

	for (int i = 0; i < list->GetNum() - 1; i++)
	{
		pRoad = list->Get(i);

		// 距離判定処理
		lengthClose = D3DXVec3Length(&(pRoad->GetPosition() - m_Info.pos));

		if (length > lengthClose)
		{
			length = lengthClose;
			pRoadClose = pRoad;
		}
	}

	m_Info.pRoad = pRoadClose;
}

//===============================================
// 当たり判定処理
//===============================================
bool CPlayer::Collision(void)
{
	auto mgr = CObjectX::GetList();
	for (int i = 0; i < mgr->GetNum(); i++)
	{// 使用されていない状態まで

		CObjectX* pObjectX = mgr->Get(i);	// 先頭を取得

		// 衝突判定を取らない
		if(!pObjectX->GetEnableCollision()){
			continue;
		}

		D3DXVECTOR3 posObjectX = pObjectX->GetPosition();
		D3DXVECTOR3 rotObjectX = pObjectX->GetRotation();
		D3DXVECTOR3 sizeMax = pObjectX->GetVtxMax();
		D3DXVECTOR3 sizeMin = pObjectX->GetVtxMin();
		D3DXVECTOR3 pVecCollision;

		bool bCollision = collision::ReflectPointToOBB(&pVecCollision, &m_Info.pos, &m_Info.move, m_Info.posOld, posObjectX, rotObjectX, (sizeMax - sizeMin) * 0.5f, REF_INER);

		if (bCollision)
		{
			CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\spark.efkefc", (m_Info.pos + m_Info.move), VECTOR3_ZERO, VECTOR3_ZERO, 300.0f);
			m_pCollSound->SetVolume(m_fEngine*2.0f);
			m_pCollSound->Play();
			pObjectX->SetHit(true);
			D3DXVECTOR3 vecMoveNor = m_Info.move;
			D3DXVec3Normalize(&vecMoveNor, &m_Info.move);
			D3DXVec3Normalize(&pVecCollision, &pVecCollision);

			float engineLow = fabsf(D3DXVec3Dot(&pVecCollision, &vecMoveNor) * 0.1f);
			m_fEngine *= 0.9f + engineLow;

			return true;
		}
	}

	m_Info.pRoad = nullptr;

	// 道との判定
	auto listRoad = CRoadManager::GetInstance()->GetList();
	for (int i = 0; i < listRoad->GetNum(); i++)
	{// 使用されていない状態まで

		// 道確認
		CRoad* pRoad = listRoad->Get(i);	// 先頭を取得
		if (pRoad == nullptr) { continue; }

		D3DXVECTOR3* pVtx = pRoad->GetVtxPos();
		D3DXVECTOR3 pos = pRoad->GetPosition();
		float height = m_Info.pos.y - 0.1f;;
		D3DXVECTOR3 vec1 = pVtx[1] - pVtx[0], vec2 = pVtx[2] - pVtx[0];
		D3DXVECTOR3 nor0, nor1;

		D3DXVec3Cross(&nor0, &vec1, &vec2);
		D3DXVec3Normalize(&nor0, &nor0);	// ベクトルを正規化する

		vec1 = pVtx[2] - pVtx[3];
		vec2 = pVtx[1] - pVtx[3];
		D3DXVec3Cross(&nor1, &vec1, &vec2);
		D3DXVec3Normalize(&nor1, &nor1);	// ベクトルを正規化する

		// 判定
		collision::IsOnSquarePolygon(pos + pVtx[0], pos + pVtx[1], pos + pVtx[2], pos + pVtx[3],
			nor0, nor1, m_Info.pos, m_Info.posOld, height);

		if (height >= m_Info.pos.y)
		{
			m_Info.pos.y = height;
			m_Info.move.y = 0.0f;
			m_Info.pRoad = pRoad;
		}
	}

	auto listGimmick = CGimmick::GetList();
	for (int i = 0; i < listGimmick->GetNum(); i++)
	{// 使用されていない状態まで

		// 道確認
		CGimmick* pGimmick = listGimmick->Get(i);	// 先頭を取得
		if (pGimmick == nullptr) { continue; }

		if (pGimmick->GetType() == CGimmick::TYPE_BRIDGE)
		{
			pGimmick->GetPos();
		}
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
		if (m_fLife > 0.0f)
		{
			CManager::GetInstance()->GetRenderer()->SetEnableDrawMultiScreen(0.3f, 1.035f, 2.0f);
		}
		m_Info.state = STATE::STATE_NITRO;
		m_Info.fStateCounter = NITRO_COUNTER;
		m_fNitroCool = NITRO_COOL;
	}
	if (m_Info.state == STATE::STATE_NITRO)
	{
		m_fEngine = 1.5f;
	}
	else
	{
		m_fNitroCool--;
		if (m_fNitroCool == NITRO_COOL - 1.0f)
		{
			CManager::GetInstance()->GetRenderer()->SetEnableDrawMultiScreen(0.0f, 1.0f, 320.0f);
		}
		if (m_fNitroCool < 0.0f)
		{
			m_fNitroCool = 0.0f;
		}
	}
	
}
void CPlayer::GetBestPath()
{
	Clist<CRoad*>* List = CRoadManager::GetInstance()->GetList();
	float fDis = FLT_MAX;
	CRoad* pStart = nullptr;
	for (int i = 0; i < List->GetNum(); i++)
	{
		float F = GetDistance(List->Get(i)->GetPosition(), GetPosition());
		if (F<fDis)
		{
			fDis = F;
			pStart = List->Get(i);
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
	if (pInputKey->GetTrigger(DIK_K))
	{
		Damage(40.0f);
	}
	if (pInputKey->GetTrigger(DIK_B))
	{
		GetBestPath();
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
	float fSlawMul = CDeltaTime::GetInstance()->GetSlow();
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
		/*if (m_Info.fStateCounter == NITRO_COUNTER - fSlawMul)
		{
			CManager::GetInstance()->GetRenderer()->SetEnableDrawMultiScreen(0.0f, 1.0f, 180.0f);
		}*/

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

//===============================================
// 荷物を投げる
//===============================================
CBaggage* CPlayer::ThrowBaggage(D3DXVECTOR3* pTarget)
{
	if (pTarget == nullptr) { return nullptr; }

	/*CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
	float rotY = atan2f(m_Info.pos.z - pTarget->z, m_Info.pos.x - pTarget->x);
	pCamera->GetAction()->Set(pCamera, D3DXVECTOR3(0.0f, rotY, D3DX_PI * 0.35f), 600.0f, 0.5f, 1.0f, CCameraAction::MOVE_POSV, true);*/

	D3DXVECTOR3 pos = GetPosition();

	pos.y += 100.0f;

	// 使用されていない
	if (CBaggage::GetList()->GetNum() == 0 || m_pBaggage == nullptr)
	{
		m_pBaggage = CBaggage::Create(pos);
	}

	CManager::GetInstance()->GetRenderer()->SetEnableDrawMultiScreen(0.65f, 0.95f, 1.0f);

	// 荷物を投げる
	CBaggage* pBag = m_pBaggage;
	m_pBaggage->Set(pos, pTarget, 0.75f);
	m_pBaggage = nullptr;

	return pBag;
}
