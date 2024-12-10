//===============================================
//
// マルチリザルト画面の状態管理.cpp [multi_result_state.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "multi_result_state.h"
#include "multi_result_manager.h"
#include "multi_result.h"
#include "scrollText2D.h"
#include "player.h"

//===============================================
// 定数定義
//===============================================
namespace
{
	const float PLAYER_INER = 0.075f;			// プレイヤー移動量慣性
	const float PLAYER_TARGET_POSZ = -800.0f;	// プレイヤーランク表示時目標Z座標
	const float PLAYER_SPACE = 275.0f;
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
	posdest.x = PLAYER_TARGET_POSZ;
	pos += ((posdest - pos) * PLAYER_INER);

	pPlayer->SetPosition(pos);
}

//*************************************************************************************
// 結果表示状態クラス コンストラクタ
//===============================================
CMultiResultStateResult::CMultiResultStateResult() : CMultiResultState(STATE::STATE_MULTIRANK)
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
	// 文字がない
	if (pResult->GetEndStr() == nullptr) { return; }

	// 文字がスクロールされている
	if (pResult->GetEndStr()->IsScroll()) { return; }

	pResult->GetEndStr()->SetEnableScroll(true);
}

//*************************************************************************************
// 終了可能状態クラス コンストラクタ
//===============================================
CMultiResultStateEnd::CMultiResultStateEnd() : CMultiResultState(STATE::STATE_END)
{

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

}
