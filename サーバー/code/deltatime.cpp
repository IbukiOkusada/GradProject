//==========================================================
//
// デルタタイム [deltatime.cpp]
// Author : Ryosuke Ohara
//
//==========================================================
#include "deltatime.h"

//==========================================================
// 静的メンバ変数宣言
//==========================================================
CDeltaTime *CDeltaTime::m_pInstance = nullptr;

//==========================================================
// 定数定義
//==========================================================
namespace
{
	const int MAX_SLOW_TIME = 10;    // スローの限界時間
	const int BOOST_CNT = 3;        // スロー量増加タイマー
	const float SLOW_RATE = 60.0f;  // スローの倍率
}

//==========================================================
// コンストラクタ
//==========================================================
CDeltaTime::CDeltaTime()
{
	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();
	m_Timer.LastTime = std::chrono::high_resolution_clock::now();
	m_fAdd = 0.0f;
	m_pInstance = this;
}

//==========================================================
// デストラクタ
//==========================================================
CDeltaTime::~CDeltaTime()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CDeltaTime::Init(void)
{
	m_fAdd = 1.0f;

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CDeltaTime::Uninit(void)
{
	// 使用していない状態にする
	m_pInstance = nullptr;
}

//==========================================================
// 更新処理
//==========================================================
void CDeltaTime::Update(void)
{
	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // 現在の時間を取得
	m_Timer.DeltaTime = m_Timer.CurrentTime - m_Timer.LastTime;       // 現在の時間と前回の時間の差分を計算
	m_Timer.LastTime = m_Timer.CurrentTime;                           // 前回の時間に現在の時間を代入

	// スロー
	Slow();
}

//==========================================================
// 生成処理
//==========================================================
CDeltaTime* CDeltaTime::Create(void)
{
	CDeltaTime* pDeltaTime = nullptr;

	if (pDeltaTime == nullptr)
	{
		pDeltaTime = DEBUG_NEW CDeltaTime;
		pDeltaTime->Init();
	}

	return pDeltaTime;
}

//==========================================================
// デルタタイム取得処理
//==========================================================
float CDeltaTime::GetDeltaTime(void)
{
	return m_Timer.DeltaTime.count() * m_fAdd;
}

//==========================================================
// スロー処理
//==========================================================
void CDeltaTime::Slow(void)
{
	//// フラグが立っていないとき処理を抜ける
	//if (!m_bSlow)
	//	return;

	//if (m_nSlowTime > 0)
	//{// スローにする

	//	m_SlowTimeCount++;	// カウントアップ

	//	if (m_SlowTimeCount % m_nSlowTime == 0)
	//	{// スローの規定値を超えた場合

	//		m_SlowTimeCount = 0;

	//		m_nSlowTime--;
	//	}
	//}

	//if (m_nSlowTime > 0)
	//{
	//	m_fAdd = (SLOW_RATE / (float)(m_nSlowTime)) * m_Timer.DeltaTime.count();	// 位置フレーム辺りの倍率
	//}
	//else
	//{
	//	m_fAdd = SLOW_RATE * m_Timer.DeltaTime.count();

	//	m_bSlow = false;
	//}
}