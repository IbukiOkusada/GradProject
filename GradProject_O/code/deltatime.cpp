//==========================================================
//
// デルタタイム [deltatime.cpp]
// Author : Ryosuke Ohara
//
//==========================================================
#include "deltatime.h"
#include "debugproc.h"

//==========================================================
// コンストラクタ
//==========================================================
CDeltaTime::CDeltaTime()
{
	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();
	m_Timer.LastTime = std::chrono::high_resolution_clock::now();
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
	

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CDeltaTime::Uninit(void)
{

}

//==========================================================
// 更新処理
//==========================================================
void CDeltaTime::Update(void)
{
	m_Timer.CurrentTime = std::chrono::high_resolution_clock::now();  // 現在の時間を取得
	m_Timer.DeltaTime = m_Timer.CurrentTime - m_Timer.LastTime;       // 現在の時間と前回の時間の差分を計算
	m_Timer.LastTime = m_Timer.CurrentTime;      
	
	CManager::GetInstance()->GetDebugProc()->Print("[ でるた～たいむ : %f ]\n", m_Timer.DeltaTime.count());
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
	return m_Timer.DeltaTime.count();
}