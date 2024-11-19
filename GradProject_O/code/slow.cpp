//==========================================================
//
// スローの処理管理 [slow.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "main.h"
#include "slow.h"
#include "manager.h"
#include "deltatime.h"
#include "debugproc.h"
#include "input_keyboard.h"

//==========================================================
// 静的メンバ変数宣言
//==========================================================
CSlow *CSlow::m_pInstance = nullptr;

//==========================================================
// 定数定義
//==========================================================
namespace {

	const int MAX_SLOWTIME = 10;  // スローの限界時間
	const int BOOST_CNT = 3;      // スロー量増加タイマー
	const float SLOW_RATE = 60.0f;
}

//==========================================================
// コンストラクタ
//==========================================================
CSlow::CSlow()
{
	// 値のクリア
	m_b = false;
	m_bUse = false;
	m_nTimeCnt = 0;
	m_nTimer = 0;
	m_fAdd = 0.0f;
	m_DeltaTime = 0.0f;
	m_pInstance = this;
}

//==========================================================
// デストラクタ
//==========================================================
CSlow::~CSlow()
{

}

//==========================================================
// 初期化処理
//==========================================================
void CSlow::Init(void)
{
	// 値のクリア
	m_b = false;
	m_bUse = false;
	m_nTimeCnt = 0;
	m_nTimer = 0;

	CDeltaTime* pDeltaTime = CDeltaTime::GetInstance();
	
	if (pDeltaTime != nullptr)
		m_DeltaTime = pDeltaTime->GetDeltaTime();
	

	m_fAdd = SLOW_RATE * m_DeltaTime;
}

//==========================================================
// 終了処理
//==========================================================
void CSlow::Uninit(void)
{
	// 使用していない状態にする
	m_pInstance = nullptr;
}

//==========================================================
// 更新処理
//==========================================================
void CSlow::Update(void)
{
	CDeltaTime* pDeltaTime = CDeltaTime::GetInstance();
	
	if (pDeltaTime != nullptr)
		m_DeltaTime = pDeltaTime->GetDeltaTime();

	m_fMulOld = m_fAdd;

	//if (m_nTimer > 0)
	//{// スローにする
	//	m_nTimeCnt++;	// カウントアップ
	//	if (m_nTimeCnt % m_nTimer == 0)
	//	{// スローの規定値を超えた場合
	//		m_nTimeCnt = 0;

	//		if (m_bUse == false)
	//		{//使用しない場合
	//			m_nTimer--;
	//		}
	//		m_b = true;
	//	}
	//	else
	//	{// 規定値ではない
	//		m_b = false;
	//	}
	//}
	//else
	//{// しない場合
	//	m_b = true;
	//}

	if (m_nTimer > 0)
	{
		m_fAdd = (SLOW_RATE / (float)(m_nTimer)) * m_DeltaTime;	// 位置フレーム辺りの倍率
	}
	else
	{
		m_fAdd = SLOW_RATE * m_DeltaTime;
	}

	//if (CInputKeyboard::GetInstance()->GetTrigger(DIK_F3))
	//{
	//	m_nTimer = 10;
	//}
}

//==========================================================
// スロー状態取得
//==========================================================
float CSlow::Get(void)
{
	return m_fAdd;
}

//==========================================================
// スロー状態量設定
//==========================================================
void CSlow::SetSlow(bool bUse)
{
	// 使用状態設定
	m_bUse = bUse;

	if (m_bUse == true)
	{// 使用する場合
		m_nBoostCnt++;

		if (m_nBoostCnt % BOOST_CNT == 0)
		{// 規定値になった場合
			m_nBoostCnt = 0;

			if (m_nTimer < MAX_SLOWTIME)
			{// スローの最大値を超えていない場合
				m_nTimer++;
			}
		}
	}
}

//==========================================================
// 前回の倍率を取得
//==========================================================
float CSlow::GetOld(void)
{
	return m_fMulOld;
}