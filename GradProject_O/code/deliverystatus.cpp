//==========================================================
//
// 配達状況の処理 [deliverystatus.cpp]
// Author : Ryosuke Ohara
//
//==========================================================
#include "deliverystatus.h"
#include "number.h"
#include "object2D.h"
#include "player.h"
#include "player_manager.h"
#include "texture.h"
#include "manager.h"

//==========================================================
// 静的メンバ変数
//==========================================================

//==========================================================
// 定数定義
//==========================================================
namespace FRAME
{
	const float HEIGHT = 30.0f;  // 高さ
	const float WIDTH = 100.0f;  // 横幅
	const D3DXVECTOR3 POS = D3DXVECTOR3(5.0f, 0.0f, 0.0f);  // 位置
	const char* TEX_PATH = "data\\TEXTURE\\frame.png";       // テクスチャパス
	const D3DXCOLOR COL = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f); // 色
}

namespace GAGE
{
	const float HEIGHT = 20.0f;  // 高さ
	const float WIDTH = 15.0f;   // 横幅
	const D3DXVECTOR3 POS = D3DXVECTOR3(30.0f, 0.0f, 0.0f);  // 位置
	const char* TEX_PATH = "data\\TEXTURE\\delivery_gage.png";     // テクスチャパス
	const D3DXCOLOR COL = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f); // 色
}

namespace NUM_NUMBER
{
	const float HEIGHT = 30.0f;  // 高さ
	const float WIDTH = 15.0f;   // 横幅
	const D3DXVECTOR3 POS = D3DXVECTOR3(0.0f, 7.0f, 0.0f);  // 位置
	const char* TEX_PATH = "data\\TEXTURE\\number000.png";   // テクスチャパス
}

namespace COUNT_NUMBER
{
	const float HEIGHT = 40.0f;  // 高さ
	const float WIDTH = 20.0f;   // 横幅
	const D3DXVECTOR3 POS = D3DXVECTOR3(-50.0f, 0.0f, 0.0f);  // 位置
	const char* TEX_PATH = "data\\TEXTURE\\number000.png";   // テクスチャパス
}

namespace LINE
{
	const float HEIGHT = 20.0f;  // 高さ
	const float WIDTH = 20.0f;   // 横幅
	const D3DXVECTOR3 POS = D3DXVECTOR3(-22.0f, 7.0f, 0.0f);  // 位置
	const char* TEX_PATH = "data\\TEXTURE\\line-2.png";     // テクスチャパス
}

//==========================================================
// コンストラクタ
//==========================================================
CDeliveryStatus::CDeliveryStatus()
{
	m_pGage = nullptr;
	m_pLine = nullptr;
	m_pNumDeliveryPointNumber = nullptr;
	m_pCountDeliveryPointNumber = nullptr;
	
	m_pos = {};
	m_nNumDeliveryPoint = 0;
}

//==========================================================
// コンストラクタ
//==========================================================
CDeliveryStatus::CDeliveryStatus(D3DXVECTOR3 pos, int nNumPoint)
{
	m_pGage = nullptr;
	m_pLine = nullptr;
	m_pNumDeliveryPointNumber = nullptr;
	m_pCountDeliveryPointNumber = nullptr;

	m_pos = pos;
	m_nNumDeliveryPoint = nNumPoint;
}


//==========================================================
// デストラクタ
//==========================================================
CDeliveryStatus::~CDeliveryStatus()
{

}

//==========================================================
// 生成処理
//==========================================================
CDeliveryStatus* CDeliveryStatus::Create(D3DXVECTOR3 pos, int nNumPoint)
{
	CDeliveryStatus* pStatus = DEBUG_NEW CDeliveryStatus(pos, nNumPoint);

	if (pStatus != nullptr)
	{
		pStatus->Init();
	}

	return pStatus;
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CDeliveryStatus::Init(void)
{
	CObject2D* pFrame = CObject2D::Create(6);

	if (pFrame != nullptr)
	{
		pFrame->SetPosition(D3DXVECTOR3(m_pos.x + FRAME::POS.x, m_pos.y + FRAME::POS.y, m_pos.z));
		pFrame->SetSize(FRAME::WIDTH, FRAME::HEIGHT);
		pFrame->BindTexture(CManager::GetInstance()->GetTexture()->Regist(FRAME::TEX_PATH));
		pFrame->SetCol(FRAME::COL);
	}

	// ゲージ生成
	if (m_pGage == nullptr)
	{
		m_pGage = DEBUG_NEW CObject2D*[m_nNumDeliveryPoint];

		for (int i = 0; i < m_nNumDeliveryPoint; i++)
		{
			m_pGage[i] = CObject2D::Create(7);
			m_pGage[i]->SetPosition(D3DXVECTOR3(m_pos.x + GAGE::POS.x + (i * 20.0f), m_pos.y + GAGE::POS.y, m_pos.z));
			m_pGage[i]->SetSize(GAGE::WIDTH, GAGE::HEIGHT);
			m_pGage[i]->BindTexture(CManager::GetInstance()->GetTexture()->Regist(GAGE::TEX_PATH));
			m_pGage[i]->SetCol(GAGE::COL);
		}
	}

	// ライン生成
	if (m_pLine == nullptr)
	{
		m_pLine = CObject2D::Create(7);
		m_pLine->SetPosition(D3DXVECTOR3(m_pos.x + LINE::POS.x, m_pos.y + LINE::POS.y, m_pos.z));
		m_pLine->SetSize(LINE::WIDTH, LINE::HEIGHT);
		m_pLine->BindTexture(CManager::GetInstance()->GetTexture()->Regist(LINE::TEX_PATH));
	}

	// 配達先の総数
	if (m_pNumDeliveryPointNumber == nullptr)
	{
		m_pNumDeliveryPointNumber = CNumber::Create(D3DXVECTOR3(m_pos.x + NUM_NUMBER::POS.x, m_pos.y + NUM_NUMBER::POS.y, m_pos.z), NUM_NUMBER::WIDTH, NUM_NUMBER::HEIGHT);
		m_pNumDeliveryPointNumber->SetIdx(m_nNumDeliveryPoint);
	}

	// 現在の配達数
	if (m_pCountDeliveryPointNumber == nullptr)
	{
		m_pCountDeliveryPointNumber = CNumber::Create(D3DXVECTOR3(m_pos.x + COUNT_NUMBER::POS.x, m_pos.y, m_pos.z), COUNT_NUMBER::WIDTH, COUNT_NUMBER::HEIGHT);
		m_pCountDeliveryPointNumber->SetIdx(0);
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CDeliveryStatus::Uninit(void)
{
	// ゲージの破棄
	if (m_pGage != nullptr)
	{
		for (int i = 0; i < m_nNumDeliveryPoint; i++)
		{
			if (m_pGage[i] == nullptr) { continue; }
				
			m_pGage[i]->Uninit();
		}
		
		delete[] m_pGage;
		m_pGage = nullptr;
	}

	// ラインの破棄
	if (m_pLine != nullptr)
	{
		m_pLine->Uninit();
		m_pLine = nullptr;
	}

	// 配達する総数の数字の破棄
	if (m_pNumDeliveryPointNumber != nullptr)
	{
		m_pNumDeliveryPointNumber->Uninit();
		m_pNumDeliveryPointNumber = nullptr;
	}

	// 現在の配達した数の数字の破棄
	if (m_pCountDeliveryPointNumber != nullptr)
	{
		m_pCountDeliveryPointNumber->Uninit();
		m_pCountDeliveryPointNumber = nullptr;
	}

	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CDeliveryStatus::Update(void)
{
	Number();

	Gage();
}

//==========================================================
// 描画処理
//==========================================================
void CDeliveryStatus::Draw(void)
{

}

//==========================================================
// 現在の配達数を変更する処理
//==========================================================
void CDeliveryStatus::Number(void)
{
	if (m_pCountDeliveryPointNumber == nullptr)
		return;

	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
	if (pPlayer == nullptr) { return; }
	int nNum = pPlayer->GetNumDeliverStatus();

	m_pCountDeliveryPointNumber->SetIdx(nNum);
}

//==========================================================
// ゲージの変更処理
//==========================================================
void CDeliveryStatus::Gage(void)
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetPlayer();
	int nNum = pPlayer->GetNumDeliverStatus();

	if (pPlayer == nullptr) { return; }

	if (nNum <= 0 && nNum > m_nNumDeliveryPoint)
		return;

	if (m_pGage[nNum - 1] == nullptr)
		return;

	m_pGage[nNum - 1]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
}