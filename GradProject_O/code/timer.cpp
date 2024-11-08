//===============================================
//
// フェードの処理全般 [fade.h]
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

//==========================================================
// 定数定義
//==========================================================
namespace NUMBER
{
	const float HEIGHT = 60.0f;		// 高さ
	const float WIDTH = 30.0f;		// 横幅
	const float MAG = 0.7f;			// 倍率
	const float INTERVAL = 60.0f;	// 倍率
	const D3DXVECTOR3 POS = D3DXVECTOR3(SCREEN_WIDTH * 0.5f, 50.0f, 0.0f);
}

namespace DECPOINT
{
	const float HEIGHT = 3.0f;  // 高さ
	const float WIDTH = 3.0f;   // 横幅
	const char* TEX_PATH = "data\\TEXTURE\\Point.png";
}

//===============================================
// コンストラクタ
//===============================================
CTimer::CTimer()
{
	// 値のクリア
	m_LimitTime = 0;
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
		m_pObject[2]->GetObject2D()->GetPosition().y + (NUMBER::HEIGHT - DECPOINT::HEIGHT) * 0.4f,
		0.0f));
	m_pDecPoint->SetSize(DECPOINT::WIDTH, DECPOINT::HEIGHT);
	m_pDecPoint->BindTexture(CManager::GetInstance()->GetTexture()->Regist(DECPOINT::TEX_PATH));
	m_pDecPoint->SetDraw();

	m_LimitTime = 200.0f;

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

	m_LimitTime = m_LimitTime - m_pDeltaTime->GetDestTime();

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
