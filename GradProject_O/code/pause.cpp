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
	const char* TEXTURENAMEFRAME = "data\\TEXTURE\\totan000.jpg";
	const char* TEXTURENAMEBUTTON[CPause::TYPE_MAX] = {	// テクスチャ名
			"data\\TEXTURE\\GameOver-Yes.png",
			"data\\TEXTURE\\T_PressEnter003.png",
			"data\\TEXTURE\\title.png",
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
	
	m_pPauseFrame = CObject2D::Create(D3DXVECTOR3(640.0f, 360.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 0);
	m_pPauseFrame->BindTexture(pTexture->Regist(TEXTURENAMEFRAME));
	m_pPauseFrame->SetSize(400.0f, 600.0f);
	m_pPauseFrame->SetVtx();

	for (int i = 0; i < CPause::TYPE_MAX; i++)
	{
		m_pPauseButton[i] = CObject2D::Create(D3DXVECTOR3(640.0f, 210.0f + (150.0f * i), 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 0);
		m_pPauseButton[i]->BindTexture(pTexture->Regist(TEXTURENAMEBUTTON[i]));
		m_pPauseButton[i]->SetSize(300.0f, 100.0f);
		m_pPauseButton[i]->SetVtx();
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CPause::Uninit(void)
{
	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CPause::Update(void)
{
	CInputPad* pInputPad = CInputPad::GetInstance();
	CInputKeyboard* pInputKey = CInputKeyboard::GetInstance();

	m_pPauseFrame->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	for (int i = 0; i < CPause::TYPE_MAX; i++)
	{
		m_pPauseButton[i]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	}

	if (!m_bPause) { return; }

	if (pInputKey->GetTrigger(DIK_UPARROW) == true || pInputPad->GetTrigger(CInputPad::BUTTON_UP, 0))
	{//上キー(↑キー)が押された
		m_nNumSelect += TYPE_MAX - 1;
		m_fTimerColor = -1.0f;
	}

	if (pInputKey->GetTrigger(DIK_DOWNARROW) == true || pInputPad->GetTrigger(CInputPad::BUTTON_DOWN, 0))
	{//下キー(↓キー)が押された
		m_nNumSelect++;
		m_fTimerColor = -1.0f;
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
			alpha = 1.0f;
		}

		m_pPauseButton[i]->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha));
	}

	m_fTimerColor += 0.1f;

	if (pInputKey->GetTrigger(DIK_RETURN) == true || pInputPad->GetTrigger(CInputPad::BUTTON_A, 0))
	{//決定キー(エンターキー)が押された
		switch (m_nNumSelect % TYPE_MAX)
		{
		case CPause::TYPE_CONTINUE:
			m_bPause = m_bPause ? false : true;
			break;

		case CPause::TYPE_RETRY:
			CManager::GetInstance()->GetFade()->Set(CScene::MODE_GAME);
			break;

		case CPause::TYPE_QUIT:
			CManager::GetInstance()->GetFade()->Set(CScene::MODE_TITLE);
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
		m_fTimerColor = -1.0f;
	}
}