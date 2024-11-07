//==========================================================
//
// メーター [meter.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "meter.h"
#include "manager.h"
#include "debugproc.h"
#include "deltatime.h"
#include "player_manager.h"
#include "texture.h"
// マクロ定義

//==========================================================
// コンストラクタ
//==========================================================
CMeter::CMeter()
{

	// 値のクリア
}

//==========================================================
// デストラクタ
//==========================================================
CMeter::~CMeter()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CMeter::Init(void)
{
	m_pos = D3DXVECTOR3(200.0f, 550.0f, 0.0f);
	D3DXVECTOR3 offset = VECTOR3_ZERO;
	CTexture* pTexture = CManager::GetInstance()->GetTexture();
	for (int i = 0; i < NUMBER_NUM; i++)
	{
		float Scale = 1.0f;
		if (i>2)
		{
			Scale = 0.5f;
			offset.y = 57.6f * 0.25;
		}
		m_pNumber[i] = CNumber::Create(m_pos + OFFSET_NUMBER + offset, 25.6f* Scale, 57.6f* Scale);
		offset += INTERVAL_NUMBER* Scale;
	}

	for (int i = 0; i < METER_NUM; i++)
	{
		D3DXMATRIX mtxRot;
		D3DXVECTOR3 offset = OFFSET_METER;
		float Rot = 0.157f * i - (D3DX_PI*0.975f);
		D3DXMatrixIdentity(&mtxRot);
		D3DXMatrixRotationZ(&mtxRot, Rot);
		D3DXVec3TransformCoord(&offset, &offset, &mtxRot);
		m_pMeter[i] = CObject2D::Create(m_pos + offset, D3DXVECTOR3(0.0f, 0.0f, -Rot), 7);
		m_pMeter[i]->BindTexture(pTexture->Regist("data\\TEXTURE\\UI\\meter.png"));
		m_pMeter[i]->SetSize(24.0f, 37.0f);
		m_pMeter[i]->SetLength(25.0f);
		m_pMeter[i]->SetVtx();
	}


	m_pFrame = CObject2D::Create(m_pos, VECTOR3_ZERO,7);
	m_pFrame->SetSize(100.0f, 100.0f);
	m_pFrame->BindTexture(pTexture->Regist("data\\TEXTURE\\UI\\frame.png"));
	m_pCircle = CObject2D::Create(m_pos, VECTOR3_ZERO, 7);
	m_pCircle->SetLength(210.0f);
	m_pCircle->BindTexture(pTexture->Regist("data\\TEXTURE\\UI\\circle.png"));
	m_pInnerCircle = CObject2D::Create(m_pos, VECTOR3_ZERO, 7);
	m_pInnerCircle->SetLength(150.0f);
	m_pInnerCircle->BindTexture(pTexture->Regist("data\\TEXTURE\\UI\\inner_circle.png"));
	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CMeter::Uninit(void)
{
	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CMeter::Update(void)
{
	m_pCircle->SetRotation(D3DXVECTOR3(0.0f,0.0f,m_pCircle->GetRotation().z-0.005f));
	m_pCircle->SetVtx();
	m_pInnerCircle->SetRotation(D3DXVECTOR3(0.0f, 0.0f, m_pInnerCircle->GetRotation().z + 0.005f));
	m_pInnerCircle->SetVtx();
	Measure();
}
//==========================================================
// 計測処理
//==========================================================
void CMeter::Measure(void)
{
	CPlayer* pPlayer = CPlayerManager::GetInstance()->GetTop();
	float fDelta = CManager::GetInstance()->GetDeltaTime()->GetDeltaTime();
	float fSpeed = GetDistance(pPlayer->GetMove(), VECTOR3_ZERO);
	fSpeed = fSpeed * 60.0f * 60.0f * fDelta / ONE_METRE * 3;
	int nSpeed = (int)(fSpeed * 100.0f);
	CManager::GetInstance()->GetDebugProc()->Print("Speed:%f", fSpeed);
	m_pNumber[0]->SetIdx(nSpeed % 100000 / 10000);
	m_pNumber[1]->SetIdx(nSpeed % 10000 / 1000);
	m_pNumber[2]->SetIdx(nSpeed % 1000 / 100);
	m_pNumber[3]->SetIdx(nSpeed % 100 / 10);
	m_pNumber[4]->SetIdx(nSpeed % 10 / 1);

	for (int i = 0; i < METER_NUM; i++)
	{
		float SpeedScale = 120.0f / METER_NUM;
		if (fSpeed > SpeedScale * (i +1))
		{
			if (i>24)
			{
				m_pMeter[i]->SetCol(D3DXCOLOR(0.91f,0.11f,0.3f, 1.0f));
			}
			else
			{
				m_pMeter[i]->SetCol(D3DXCOLOR(0.1f, 0.91f, 0.81f, 1.0f));
			}
		}
		else
		{
			m_pMeter[i]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f));
		}
	}
}

//==========================================================
// 生成
//==========================================================
CMeter* CMeter::Create(void)
{
	CMeter* pMeter = nullptr;

	pMeter = DEBUG_NEW CMeter;

	if (pMeter != nullptr)
	{
		// 初期化処理
		pMeter->Init();
	}

	return pMeter;
}
