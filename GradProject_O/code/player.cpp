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

//===============================================
// マクロ定義
//===============================================
#define MOVE	(3.5f)		// 移動量
#define GRAVITY	(-0.6f)		//プレイヤー重力
#define ROT_MULTI	(0.1f)	// 向き補正倍率
#define WIDTH	(20.0f)		// 幅
#define HEIGHT	(80.0f)		// 高さ
#define INER	(0.3f)		// 慣性
#define JUMP	(16.0f)

namespace {
	const int HEADPARTS_IDX = (1);	// 頭のパーツインデックス
	const float DAMAGE_INTERVAL = (10.0f);	// ダメージインターバル
	const float DAMAGE_APPEAR = (110.0f);	// 無敵時間インターバル
	const float DEATH_INTERVAL = (120.0f);	// 死亡インターバル
	const float SPAWN_INTERVAL = (60.0f);	// 生成インターバル
	const float SLIDING_INER = (0.015f);	// スライディング慣性
	const float SLIDING_MINMOVE = (1.0f);	// スライディング可能最低移動量
	const float SLIDING_STARTMOVE = (6.0f);	// スライディング開始可能移動量
	const float SLIDING_SPEED = (0.5f);
	const float WALLKICK_MOVE = (45.0f);	// 壁キック移動量
	const float WALLKICK_INER = (0.1f);		// 壁キック中慣性
	const float WALLKICK_SPEED = (1.0f);	// 壁キック中移動速度
	const float WALLSLIDE_GRAVITY = (-1.5f);	// 壁ずり中落下速度
	const float WALLDUSH_GRAVITY = (-0.75f);
	const float SLIDEJUMP_INER = (0.02f);		// スライディング慣性
	const float SLIDEJUMP_GRAVITY = (-0.25f);	// スライディングジャンプ重力
	const float SLIDEJUMP = (7.0f);				// スライディングジャンプジャンプ力
	const float SLIDEJUMP_SPEED = (1.75f);		// スライディングジャンプ移動量
	const float WALLSLIDE_MOVE = (0.05f);		// 壁ずりいどうりょう
	const float WALLDUSH_MOVE = (5.0f);			// 壁走り移動量
	const float CAMROT_INER = (0.2f);			// カメラ慣性
	const float SLIDINNG_ROTZ = (D3DX_PI * 0.51f);	// スライディングカメラ角度
	const float SLIDING_LENGTH = (200.0f);			//スライディングカメラ距離
	const float KICKUP_SPEED = (1.5f);			// 蹴りあがり移動速度
	const float KICKUP_JUMP = (18.0f);			// 蹴りあがりジャンプ力
	const float KICKUP_QUICKJUMP = (13.0f);		// ライダーキックからのジャンプ力
	const float AXEKICK_ROTZ = (D3DX_PI * 0.21f);	// かかと落としカメラ
	const float RIDERKICK_ROTZ = (D3DX_PI * 0.31f);	// ライダーキックカメラ向き
	const float AXEKICK_CAMERALENGTH = (400.0f);	// かかと落としカメラ距離
	const float SLOW_KICKCHARGE = (15.0f);			// スローまでのチャージ時間
	const float KICK_LENGTH = (1000.0f);	// 攻撃範囲
	const float RIDERKICK_SPEED = (24.0f);	// ライダーキック速度
	const float RIDERKICK_HIGHSPEED = (60.0f);	// ライダーキック最速
	const float RIDERKICK_CAMERALENGTH = (600.0f);	// ライダーキックカメラ距離
	const float WALLKICK_GRAVITY = (-0.4f);
	const float WALLKICK_JUMP = (11.0f);
	const float AXEKICK_GRAVITY = (-2.5f);
	const float AXEKICK_MOVE = (0.75f);
	const int LIFE = (10);
	const float KICK_STEPMOVE = (45.0f);
	const float ATK_INTERVAL = (5.0f);
	const float CEILING_ROTZ = (D3DX_PI * 0.65f);
	const float CEILING_MOVE = (MOVE * 0.5f);
	const float CEILING_CAMLENGTH = (300.0f);
	const D3DXVECTOR3 LIFEUI_POS = { SCREEN_WIDTH * 0.175f, SCREEN_HEIGHT * 0.9f, 0.0f };
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
	m_fRotMove = 0.0f;
	m_fRotDiff = 0.0f;
	m_fRotDest = 0.0f;
	m_type = TYPE_NONE;
	m_nId = -1;
	m_Info.fSlideMove = 0.0f;

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
	SetMatrix();

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CPlayer::Uninit(void)
{

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
}

//===============================================
// 生成
//===============================================
CPlayer *CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, const char *pBodyName, const char *pLegName)
{
	CPlayer *pPlayer = nullptr;

	// オブジェクト2Dの生成
	pPlayer = new CPlayer();

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
	D3DXVECTOR3 pos = GetPosition();	// 座標を取得
	D3DXVECTOR3 rot = GetRotation();	// 向きを取得
	float fIner = INER;
	m_fRotMove = rot.y;	//現在の向きを取得
}

//===============================================
// 移動
//===============================================
void CPlayer::Move(void)
{
	CInputKeyboard *pInputKey = CManager::GetInstance()->GetInputKeyboard();	// キーボードのポインタ
	CInputPad *pInputPad = CManager::GetInstance()->GetInputPad();

	// 入力装置確認
	if (nullptr == pInputKey){
		return;
	}
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
