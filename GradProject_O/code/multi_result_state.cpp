//===============================================
//
// マルチリザルト画面の状態管理.cpp [multi_result_state.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "multi_result_state.h"
#include "multi_result_manager.h"
#include "multi_result.h"
#include "input_keyboard.h"
#include "input_gamepad.h"
#include "scrollText2D.h"
#include "deltatime.h"
#include "manager.h"
#include "player.h"
#include "police.h"
#include "fade.h"
#include "camera.h"
#include "camera_action.h"
#include "character.h"
#include "motion.h"

//===============================================
// 定数定義
//===============================================
// ランク
namespace Rank
{
	const float PLAYER_INER = 0.075f;			// プレイヤー移動量慣性
	const float PLAYER_TARGET_POSX = -800.0f;	// プレイヤーランク表示時目標Z座標
	const float PLAYER_SPACE = 275.0f;
	
}

// リザルト表示
namespace Result
{
	const int NUM_POLICE = 2;					// 警察の一列生成数
	const float POLICE_SPACE = 200.0f;			// 警察の前後SPACE
	const float LENGTH = 1000.0f;				// カメラの距離
	const float ROTZ = D3DX_PI * 0.4f;
}

// 終了
namespace End
{
	const float ADD_ROTZ = D3DX_PI * 0.1f;
	const float ADD_ROTY = D3DX_PI * 0.015f;
	const float ADD_ROTX = D3DX_PI * 0.3f;
	const float ROT_INER = 0.1f;
	const float FADE_CNT = 7.0f;				// フェードまでのカウント
}

//*************************************************************************************
// 開始状態クラス コンストラクタ
//===============================================
CMultiResultStateStart::CMultiResultStateStart() : CMultiResultState(STATE::STATE_START)
{

}

//===============================================
// デストラクタ
//===============================================
CMultiResultStateStart::~CMultiResultStateStart()
{

}

//===============================================
// 操作処理関連
//===============================================
void CMultiResultStateStart::Controll(CMultiResult* pResult)
{
	// 文字無い
	if (pResult->GetTitleStr() == nullptr) { return; }

	// スクロール終了していない
	if (!pResult->GetTitleStr()->GetEnd()) { return; }

	// 終了したら状態をランク表示状態に変更する
	pResult->ChangeState(DEBUG_NEW CMultiResultStateRank);
}

//*************************************************************************************
// ランク表示状態クラス コンストラクタ
//===============================================
CMultiResultStateRank::CMultiResultStateRank() : CMultiResultState(STATE::STATE_RANK)
{

}

//===============================================
// デストラクタ
//===============================================
CMultiResultStateRank::~CMultiResultStateRank()
{

}

//===============================================
// 操作処理関連
//===============================================
void CMultiResultStateRank::Controll(CMultiResult* pResult)
{
	// マネージャー無し
	CMultiResultManager* pMgr = pResult->GetMgr();
	if (pMgr == nullptr) { return; }

	// 最後まで見た
	if (pResult->GetNowScr() >= pMgr->GetNumPlayer()) {
		// 結果表示状態に変更
		pResult->ChangeState(DEBUG_NEW CMultiResultStateResult);
		return;
	}

	CMultiResult::SPlayerInfo* pInfo = pResult->GetInfo();
	CScrollText2D* pStr = pInfo[pResult->GetNowScr()].pString;

	// 文字がない
	if (pStr == nullptr) { 
		pResult->SetNowScr(pResult->GetNowScr() + 1); 
		return; 
	}

	// 文字をスクロールしていない
	if (!pStr->IsScroll()) {
		pStr->SetEnableScroll(true);
	}

	// 文字がない
	if (pStr == nullptr) { return; }

	// 文字を最後まで表示していない
	if (!pStr->GetEnd()) {
		// プレイヤー移動
		RankPlayerMove(pResult);
		return;
	}

	// 表示していたら次の文字を表示する
	pResult->SetNowScr(pResult->GetNowScr() + 1);
}


//===============================================
// ランク表示中のプレイヤーの動き制作
//===============================================
void CMultiResultStateRank::RankPlayerMove(CMultiResult* pResult)
{
	// マネージャー無し
	CMultiResultManager* pMgr = pResult->GetMgr();
	if (pMgr == nullptr) { return; }

	// 最後まで見た
	if (pResult->GetNowScr() >= pMgr->GetNumPlayer()) { return; }

	// プレイヤー取得
	CMultiResult::SPlayerInfo* pInfo = pResult->GetInfo();
	CPlayer* pPlayer = pInfo[pResult->GetNowScr()].pPlayer;
	if (pPlayer == nullptr) { return; }

	// 座標を補正する
	D3DXVECTOR3 pos = pPlayer->GetPosition();
	D3DXVECTOR3 posdest = pos;
	posdest.x = Rank::PLAYER_TARGET_POSX;
	pos += ((posdest - pos) * Rank::PLAYER_INER);

	pPlayer->SetPosition(pos);
}

//*************************************************************************************
// 結果表示状態クラス コンストラクタ
//===============================================
CMultiResultStateResult::CMultiResultStateResult() : CMultiResultState(STATE::STATE_MULTIRANK),
m_ppPolice(nullptr),
m_nPassingPolice(0),
m_nNumCreatePolice(0)
{
	
}

//===============================================
// デストラクタ
//===============================================
CMultiResultStateResult::~CMultiResultStateResult()
{

}

//===============================================
// 操作処理関連
//===============================================
void CMultiResultStateResult::Controll(CMultiResult* pResult)
{
	// 警察更新
	PoliceUpdate(pResult);

	// 文字がない
	if (pResult->GetEndStr() == nullptr) { return; }

	// マネージャー無し
	CMultiResultManager* pMgr = pResult->GetMgr();
	if (pMgr == nullptr) { return; }

	// 文字がスクロールされている
	if (pResult->GetEndStr()->GetEnd() && m_nPassingPolice >= (pMgr->GetNumPlayer() -1) * Result::NUM_POLICE) 
	{ 
		Uninit(pResult);
		return; 
	}

	// 文字がスクロールされている
	if (pResult->GetEndStr()->IsScroll()) { return; }

	pResult->GetEndStr()->SetEnableScroll(true);
}

//===============================================
// 生成処理
//===============================================
void CMultiResultStateResult::Create(CMultiResult* pResult)
{
	// マネージャー無し
	CMultiResultManager* pMgr = pResult->GetMgr();
	if (pMgr == nullptr) { return; }

	// プレイヤー取得
	CMultiResult::SPlayerInfo* pInfo = pResult->GetInfo();

	// ポインタ生成
	m_nNumCreatePolice = Result::NUM_POLICE * pMgr->GetNumPlayer();
	m_ppPolice = DEBUG_NEW CPolice*[m_nNumCreatePolice];

	for (int i = 0; i < pMgr->GetNumPlayer(); i++)
	{
		// 配列カウント
		int cnt = i * Result::NUM_POLICE;

		// プレイヤー取得
		CPlayer* pPlayer = pInfo[i].pPlayer;

		// 自分自身の場合
		if (pInfo[i].nId == pResult->GetTopId()) { continue; }

		// 最初の一台目だけ生成する
		for (int j = 0; j < Result::NUM_POLICE; j++)
		{			
			// 情報設定
			D3DXVECTOR3 pos = pPlayer->GetPosition();
			if (j == 0)
			{
				pos.x = -Rank::PLAYER_TARGET_POSX * 3.0f - ((rand() % 6) * 50);
			}
			else
			{
				pos.x = m_ppPolice[j - 1]->GetPosition().x + 600.0f + ((rand() % 6) * 50);
			}

			pos.y = 10.0f;

			D3DXVECTOR3 rot = VECTOR3_ZERO;
			rot.y = -D3DX_PI * 0.5f;

			D3DXVECTOR3 move = VECTOR3_ZERO;

			// 初回のみ
			{
				move.x = -(rand() % 10 + 30.0f);
			}

			// 警察生成
			m_ppPolice[cnt + j] = CPolice::Create(pos, rot, move, cnt + j);
			m_ppPolice[cnt + j]->SetType(CCar::TYPE::TYPE_NONE);
		}
	}

	// 一位のプレイヤーの座標を取得する
	D3DXVECTOR3 toppos = pInfo[pMgr->GetNumPlayer() - 1].pPlayer->GetPosition();

	// カメラのアクション設定
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	CCameraAction* pCamAc = pCamera->GetAction();

	// 向き
	D3DXVECTOR3 rot = pCamera->GetRotation();
	rot.z = Result::ROTZ;

	pCamAc->Set(pCamera, toppos, rot, Result::LENGTH, 2.5f, 5.0f, CCameraAction::MOVE::MOVE_POSV);
}

//===============================================
// 警察更新
//===============================================
void CMultiResultStateResult::PoliceUpdate(CMultiResult* pResult)
{
	m_nPassingPolice = 0;

	// マネージャー無し
	CMultiResultManager* pMgr = pResult->GetMgr();
	if (pMgr == nullptr) { return; }

	// プレイヤー取得
	CMultiResult::SPlayerInfo* pInfo = pResult->GetInfo();

	for (int i = 0; i < pMgr->GetNumPlayer(); i++)
	{
		// 配列カウント
		int cnt = i * Result::NUM_POLICE;

		// プレイヤー取得
		CPlayer* pPlayer = pInfo[i].pPlayer;

		// 自分自身の場合
		if (pInfo[i].nId == pResult->GetTopId()) { continue; }

		for (int j = 0; j < Result::NUM_POLICE; j++)
		{
			if (pPlayer == nullptr) { continue; }

			if (m_ppPolice[cnt + j] == nullptr) { continue; }

			// 情報設定
			CPolice* pPolice = m_ppPolice[cnt + j];
			D3DXVECTOR3 pos = pPolice->GetPosition();
			pos += pPolice->GetMove();
			pPolice->SetPosition(pos);

			// 次を生成する地点に達していない
			if (pos.x >= -Rank::PLAYER_TARGET_POSX) { continue; }

			if (pos.x <= Rank::PLAYER_TARGET_POSX * 2.0f)
			{
				m_nPassingPolice++;
			}

			// 次がない
			if ((j + 1) % Result::NUM_POLICE == 0) { continue; }

			CPolice* pNext = m_ppPolice[cnt + (j + 1)];

			// 先頭車両の場合
			if (j == 0)
			{
				// 座標を補正して移動させる
				D3DXVECTOR3 playpos = pPlayer->GetPosition();
				D3DXVECTOR3 posdest = playpos;
				posdest.x = Rank::PLAYER_TARGET_POSX * 2;
				playpos += ((posdest - playpos) * Rank::PLAYER_INER);

				pPlayer->SetPosition(playpos);
			}
		}
	}
}

//===============================================
// 終了処理
//===============================================
void CMultiResultStateResult::Uninit(CMultiResult* pResult)
{
	// 警察廃棄
	if (m_ppPolice != nullptr)
	{
		for (int i = 0; i < m_nNumCreatePolice; i++)
		{
			if (m_ppPolice[i] == nullptr) { continue; }
			m_ppPolice[i] = nullptr;
		}

		// ポインタも廃棄
		delete[] m_ppPolice;
		m_ppPolice = nullptr;
	}

	// 状態を終了に変更
	pResult->ChangeState(DEBUG_NEW CMultiResultStateEnd);
}

//*************************************************************************************
// 終了可能状態クラス コンストラクタ
//===============================================
CMultiResultStateEnd::CMultiResultStateEnd() : CMultiResultState(STATE::STATE_END)
{
	m_pExplosion = nullptr;
	m_fFadeCnt = 0.0f;
}

//===============================================
// デストラクタ
//===============================================
CMultiResultStateEnd::~CMultiResultStateEnd()
{

}

//===============================================
// 操作処理関連
//===============================================
void CMultiResultStateEnd::Controll(CMultiResult* pResult)
{
	CInputKeyboard* pkey = CInputKeyboard::GetInstance();
	CInputPad* ppad = CInputPad::GetInstance();
	CInputPad::BUTTON button1 = CInputPad::BUTTON::BUTTON_A, 
		button2 = CInputPad::BUTTON::BUTTON_START;

	CDeltaTime* pdel = CDeltaTime::GetInstance();

	// 自動遷移時間加算
	if (pdel != nullptr)
	{
		m_fFadeCnt += pdel->GetDeltaTime();
	}

	if (pResult->GetEndStr())
	{
		pResult->GetEndStr()->SetAlpha(fabsf(sinf(m_fFadeCnt)));
	}

	// 入力または時間経過でタイトルに遷移
	if (m_fFadeCnt >= End::FADE_CNT || ppad->GetTrigger(button1, 0) || ppad->GetTrigger(button2, 0) ||
		pkey->GetTrigger(DIK_RETURN) || pkey->GetTrigger(DIK_SPACE))
	{
		if (m_pExplosion != nullptr)
		{
			SAFE_DELETE(m_pExplosion);
		}
		m_fFadeCnt = End::FADE_CNT;
		CManager::GetInstance()->GetFade()->Set(CScene::MODE::MODE_TITLE);
	}
	else
	{
		// マネージャー無し
		CMultiResultManager* pMgr = pResult->GetMgr();
		if (pMgr == nullptr) { return; }

		// プレイヤー取得
		CMultiResult::SPlayerInfo* pInfo = pResult->GetInfo();
		D3DXVECTOR3 rot = pInfo[pMgr->GetNumPlayer() - 1].pPlayer->GetRotation();

		// 向き補正
		D3DXVECTOR3 rotdiff;
		rotdiff.x = End::ADD_ROTX - rot.x;
		rotdiff.z = End::ADD_ROTZ - rot.z;
		rotdiff.y = 0.0f;
		Adjust(rotdiff);

		rot += rotdiff * End::ROT_INER;

		rot.y += End::ADD_ROTY;
		Adjust(rot);
		pInfo[pMgr->GetNumPlayer() - 1].pPlayer->SetRotation(rot);
	}
}


//===============================================
// 生成
//===============================================
void CMultiResultStateEnd::Create(CMultiResult* pResult)
{
	// マネージャー無し
	CMultiResultManager* pMgr = pResult->GetMgr();
	if (pMgr == nullptr) { return; }

	// プレイヤー取得
	CMultiResult::SPlayerInfo* pInfo = pResult->GetInfo();
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	pos.x = (-Rank::PLAYER_TARGET_POSX) * 10;
	m_pExplosion = CEffekseer::GetInstance()->Create("data\\EFFEKSEER\\explosion.efkefc", pos, VECTOR3_ZERO, VECTOR3_ZERO, 120.0f, false, false);

	// プレイヤー取得
	pInfo[pMgr->GetNumPlayer() - 1].pPlayer->GetCharacteter()->GetMotion()->BlendSet(3);
}
