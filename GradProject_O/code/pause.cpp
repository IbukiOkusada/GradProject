//==========================================================
//
// ポーズ [pause.cpp]
// Author : Riku Nakamura
//
//==========================================================
#include "pause.h"
#include "manager.h"
#include "texture.h"
#include "fade.h"
#include "input_gamepad.h"
#include "input_keyboard.h"

// マクロ定義

// 無名名前空間を定義
namespace
{
	const float COLORTIMER_INI = (-1.0f);
	const float COLORTIMER_ADD = (0.1f);

	const float WIDTH_FRAME = (1080.0f*0.5);		// フレームの横幅
	const float HEIGHT_FRAME = (600.0f*0.5);	// フレームの縦幅
	const D3DXVECTOR3 POS_FRAME = D3DXVECTOR3(1050.0f, 200.0f, 0.0f);	// フレームの位置

	const float WIDTH_BUTTON = (220.0f*0.5);	// ボタンの横幅
	const float HEIGHT_BUTTON = (70.0f*0.5);	// ボタンの縦幅
	const D3DXVECTOR3 POS_BUTTON[CPause::TYPE_MAX] = {	// ボタンの位置
		D3DXVECTOR3(985.0f, 160.0f, 0.0f),
		D3DXVECTOR3(985.0f, 215.0f, 0.0f),
		D3DXVECTOR3(985.0f, 270.0f, 0.0f),
	};
	const D3DXVECTOR3 OFFSET_OUT = D3DXVECTOR3(+600.0f, 0.0f, 0.0f);//アニメーションの待機位置
	const char* TEXTURENAMEFRAME = "data\\TEXTURE\\pause\\pause_0.png";
	const char* TEXTURENAMEBUTTON[CPause::TYPE_MAX] = {	// テクスチャ名
			"data\\TEXTURE\\pause\\pause_1.png",
			"data\\TEXTURE\\pause\\pause_2.png",
			"data\\TEXTURE\\pause\\pause_3.png",
	};
}

//==========================================================
// コンストラクタ
//==========================================================
CPause::CPause()
{
	// 値のクリア
	m_nNumSelect = 0;
}

//==========================================================
// デストラクタ
//==========================================================
CPause::~CPause()
{

}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CPause::Init(void)
{
	CTexture* pTexture = CManager::GetInstance()->GetTexture();
	m_Offset = VECTOR3_ZERO;
	m_pPauseFrame = CObject2D::Create(POS_FRAME, D3DXVECTOR3(0.0f, 0.0f, 0.0f), 7);
	m_pPauseFrame->BindTexture(pTexture->Regist(TEXTURENAMEFRAME));
	m_pPauseFrame->SetSize(WIDTH_FRAME, HEIGHT_FRAME);
	m_pPauseFrame->SetVtx();
	m_pSE = CMasterSound::CObjectSound::Create("data\\SE\\pi.wav", 0);
	m_pSE->Stop();
	for (int i = 0; i < CPause::TYPE_MAX; i++)
	{
		m_pPauseButton[i] = CObject2D::Create(POS_BUTTON[i], D3DXVECTOR3(0.0f, 0.0f, 0.0f), 7);
		m_pPauseButton[i]->BindTexture(pTexture->Regist(TEXTURENAMEBUTTON[i]));
		m_pPauseButton[i]->SetSize(WIDTH_BUTTON, HEIGHT_BUTTON);
		m_pPauseButton[i]->SetVtx();
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CPause::Uninit(void)
{
	SAFE_UNINIT_DELETE(m_pSE);
	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CPause::Update(void)
{
	CInputPad* pInputPad = CInputPad::GetInstance();
	CInputKeyboard* pInputKey = CInputKeyboard::GetInstance();

	//m_pPauseFrame->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	m_pPauseFrame->SetPosition(POS_FRAME + m_Offset);
	m_pPauseFrame->SetVtx();
	for (int i = 0; i < CPause::TYPE_MAX; i++)
	{
		//m_pPauseButton[i]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
		m_pPauseButton[i]->SetPosition(POS_BUTTON[i] + m_Offset);
		m_pPauseButton[i]->SetVtx();
	}
	if (m_bPause)
	{
		m_Offset *= 0.85f;
	}
	else
	{
		m_Offset += (OFFSET_OUT-m_Offset)*0.15f;
	}
	if (!m_bPause) { return; }

	if (pInputKey->GetTrigger(DIK_UPARROW) == true || pInputKey->GetTrigger(DIK_W) == true || pInputPad->GetTrigger(CInputPad::BUTTON_UP, 0))
	{//上キー(↑キー)が押された
		m_nNumSelect += TYPE_MAX - 1;
		m_fTimerColor = COLORTIMER_INI;
		m_pSE->Play();
	}

	if (pInputKey->GetTrigger(DIK_DOWNARROW) == true || pInputKey->GetTrigger(DIK_S) == true || pInputPad->GetTrigger(CInputPad::BUTTON_DOWN, 0))
	{//下キー(↓キー)が押された
		m_nNumSelect++;
		m_fTimerColor = COLORTIMER_INI;
		m_pSE->Play();
	}

	m_pPauseFrame->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	for (int i = 0; i < CPause::TYPE_MAX; i++)
	{
		float alpha;
		if (i == m_nNumSelect % TYPE_MAX)
		{
			alpha = cos(m_fTimerColor) * 0.5f + 0.5f;
		}
		else
		{
			alpha = 0.2f;
		}

		m_pPauseButton[i]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha));
	}

	m_fTimerColor += COLORTIMER_ADD;

	if (pInputKey->GetTrigger(DIK_RETURN) == true || pInputPad->GetTrigger(CInputPad::BUTTON_A, 0))
	{//決定キー(エンターキー)が押された
		switch (m_nNumSelect % TYPE_MAX)
		{
		case CPause::TYPE_RETRY:
			CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);
			break;

		case CPause::TYPE_QUIT:
			CManager::GetInstance()->GetFade()->Set(CScene::MODE_TITLE);
			break;
		case CPause::TYPE_CANCEL:
			m_bPause = m_bPause ? false : true;
			break;
		default:
			break;
		}
	}
}

//==========================================================
// 生成
//==========================================================
CPause *CPause::Create(void)
{
	CPause *pSampleTask = nullptr;

	pSampleTask = DEBUG_NEW CPause;

	if (pSampleTask != nullptr)
	{
		// 初期化処理
		pSampleTask->Init();
	}

	return pSampleTask;
}

//==========================================================
// ポーズ状態変更
//==========================================================
void CPause::ChangePause(void)
{
	CInputPad* pInputPad = CInputPad::GetInstance();
	CInputKeyboard* pInputKey = CInputKeyboard::GetInstance();

	if (pInputKey->GetTrigger(DIK_P) == true || pInputPad->GetTrigger(CInputPad::BUTTON_START, 0))
	{//ポーズキー(Pキー)が押された
		m_bPause = m_bPause ? false : true;
		m_nNumSelect = 0;
		m_fTimerColor = COLORTIMER_INI;
	}
}