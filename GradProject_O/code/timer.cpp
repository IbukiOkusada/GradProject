//===============================================
//
// タイマーの処理 [timer.h]
// Author : Kenta Hashimoto
//
//===============================================
#include "Timer.h"
#include "object_manager.h"
#include "deltatime.h"
#include "texture.h"
#include "manager.h"
#include "object2D.h"
#include "edit_manager.h"
#include "fade.h"

//==========================================================
// 定数定義
//==========================================================
namespace NUMBER
{
	const float HEIGHT = 60.0f;		// 高さ
	const float WIDTH = 30.0f;		// 横幅
	const float MAG = 0.7f;			// 倍率
	const float INTERVAL = 60.0f;	// 間隔
	const D3DXVECTOR3 POS = D3DXVECTOR3(SCREEN_WIDTH * 0.5f, 50.0f, 0.0f);
}

namespace DECPOINT
{
	const float HEIGHT = 3.0f;  // 高さ
	const float WIDTH = 3.0f;   // 横幅
	const char* TEX_PATH = "data\\TEXTURE\\Point.png";
}

namespace BLINKING
{
	const float STERT = 190.0f;	// 点滅になる時間
	const float SPEED = 1.8f;	// 点滅の速さ
}

//===============================================
// 静的メンバ変数
//===============================================
float CTimer::m_LimitTime = 0.0f;

//===============================================
// コンストラクタ
//===============================================
CTimer::CTimer()
{
	// 値のクリア
	m_Ratio = 0.0f;
}

//===============================================
// デストラクタ
//===============================================
CTimer::~CTimer()
{

}

//===============================================
// 初期化処理
//===============================================
HRESULT CTimer::Init(void)
{
	for (int Cnt = 0; Cnt < 5; Cnt++)
	{
		// ポリゴンの生成
		switch (Cnt)
		{
		case 3:
			m_pObject[Cnt] = CNumber::Create(D3DXVECTOR3(NUMBER::POS.x + (Cnt - 2) * NUMBER::INTERVAL, NUMBER::POS.y + NUMBER::HEIGHT * (1 - NUMBER::MAG) * 0.5f, 0.0f),
				NUMBER::WIDTH * NUMBER::MAG, NUMBER::HEIGHT * NUMBER::MAG);
			break;

		case 4:
			m_pObject[Cnt] = CNumber::Create(D3DXVECTOR3(NUMBER::POS.x + (Cnt - 2) * NUMBER::INTERVAL * 0.8f, NUMBER::POS.y + NUMBER::HEIGHT * (1 - NUMBER::MAG) * 0.5f, 0.0f),
				NUMBER::WIDTH * NUMBER::MAG, NUMBER::HEIGHT * NUMBER::MAG);
			break;

		default:
			m_pObject[Cnt] = CNumber::Create(D3DXVECTOR3(NUMBER::POS.x + (Cnt - 2) * NUMBER::INTERVAL, NUMBER::POS.y, 0.0f),
				NUMBER::WIDTH, NUMBER::HEIGHT);
			break;
		}

		m_Time[Cnt] = 0;
	}

	m_pDecPoint = CObject2D::Create(7);
	m_pDecPoint->SetPosition(D3DXVECTOR3(m_pObject[2]->GetObject2D()->GetPosition().x + (m_pObject[3]->GetObject2D()->GetPosition().x - m_pObject[2]->GetObject2D()->GetPosition().x) * 0.55f,
		m_pObject[2]->GetObject2D()->GetPosition().y + (NUMBER::HEIGHT - DECPOINT::HEIGHT) * 0.45f,
		0.0f));
	m_pDecPoint->SetSize(DECPOINT::WIDTH, DECPOINT::HEIGHT);
	m_pDecPoint->BindTexture(CManager::GetInstance()->GetTexture()->Regist(DECPOINT::TEX_PATH));
	m_pDecPoint->SetDraw();

	m_LimitTime = 200.0f;
	m_Ratio = 0.0f;

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CTimer::Uninit(void)
{
	for (int Cnt = 0; Cnt < 5; Cnt++)
	{
		if (m_pObject[Cnt] != NULL)
		{
			m_pObject[Cnt]->Uninit();
			m_pObject[Cnt] = NULL;
		}
	}

	if (m_pDecPoint != NULL)
	{
		m_pDecPoint->Uninit();
		m_pDecPoint = NULL;
	}

	delete this;
}

//===============================================
// 更新処理
//===============================================
void CTimer::Update(void)
{
#if _DEBUG
	if (CEditManager::GetInstance() != nullptr) { return; }
#endif // _DEBUG

	CalTime();

	if (m_LimitTime < BLINKING::STERT)
	{
		BlinkingTime();
	}
}

//===============================================
// 生成
//===============================================
CTimer* CTimer::Create()
{
	CTimer* pTimer = NULL;

	pTimer = DEBUG_NEW CTimer;

	if (pTimer != NULL)
	{
		pTimer->Init();
	}

	return pTimer;
}

//===============================================
// タイマーの計算
//===============================================
void CTimer::CalTime(void)
{
	CDeltaTime* m_pDeltaTime = CDeltaTime::GetInstance();

	if (CManager::GetInstance()->GetFade()->GetState() == CFade::STATE_NONE)
	{
		m_LimitTime -= m_pDeltaTime->GetDestTime();

		if (m_LimitTime <= 0.0f)
		{
			m_LimitTime = 0.0f;
		}
	}

	m_LimitTime = m_LimitTime * 100.0f;

	m_Time[0] = ((int)m_LimitTime) / 10000;
	m_Time[1] = ((int)m_LimitTime) % 10000 / 1000;
	m_Time[2] = ((int)m_LimitTime) % 1000 / 100;
	m_Time[3] = ((int)m_LimitTime) % 100 / 10;
	m_Time[4] = ((int)m_LimitTime) % 10;

	m_LimitTime = m_LimitTime / 100.0f;

	for (int Cnt = 0; Cnt < 5; Cnt++)
	{	
		m_pObject[Cnt]->SetIdx(m_Time[Cnt]);
	}
}
//===============================================
// 点滅処理
//===============================================
void CTimer::BlinkingTime(void)
{
	CDeltaTime* m_pDeltaTime = CDeltaTime::GetInstance();

	D3DXCOLOR col = m_pDecPoint->GetObject2D()->GetCol();

	if (col.b >= 1.0f && col.g >= 1.0f)
	{
		m_Ratio = -m_pDeltaTime->GetDestTime() * BLINKING::SPEED;
	}

	else if (col.b <= 0.0f && col.g <= 0.0f)
	{
		m_Ratio = m_pDeltaTime->GetDestTime() * BLINKING::SPEED;
	}

	col.b += m_Ratio;
	col.g += m_Ratio;

	m_pDecPoint->GetObject2D()->SetCol(col);

	for (int Cnt = 0; Cnt < 5; Cnt++)
	{
		m_pObject[Cnt]->GetObject2D()->SetCol(col);
	}
}
