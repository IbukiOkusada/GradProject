//===============================================
//
// フェードの処理全般 [fade.h]
// Author : Ibuki Okusada
//
//===============================================
#include "fade.h"
#include "object_manager.h"

//グローバル宣言
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffFade = NULL;	//頂点バッファへのポインタ

//===============================================
// コンストラクタ
//===============================================
CFade::CFade()
{
	// 値のクリア
	m_modeNext = CScene::MODE_TITLE;
	m_state = STATE_NONE;
	m_Col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
}

//===============================================
// デストラクタ
//===============================================
CFade::~CFade()
{

}

//===============================================
// 初期化処理
//===============================================
HRESULT CFade::Init(void)
{
	m_pObject = CObject2D::Create(NUM_PRIORITY - 1);

	// ポリゴンの生成
	m_pObject->Init();

	// サイズ設定
	m_pObject->SetPosition(D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));
	m_pObject->SetSize(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);

	return S_OK;
}

//===============================================
// 初期化処理
//===============================================
HRESULT CFade::Init(CScene::MODE modeNext)
{
	m_pObject = CObject2D::Create(NUM_PRIORITY - 1);

	// ポリゴンの生成
	m_pObject->Init();

	// サイズ設定
	m_pObject->SetPosition(D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));
	m_pObject->SetSize(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);

	//モードの設定
	Set(modeNext);

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CFade::Uninit(void)
{
	if (m_pObject != NULL)
	{
		m_pObject->Uninit();
		m_pObject = NULL;
	}
}

//===============================================
// 更新処理
//===============================================
void CFade::Update(void)
{
	if (m_state != STATE_NONE)
	{//何もしていない状態以外
		if (m_state == STATE_IN)
		{//フェードイン状態
			m_Col.a -= 1.0f / 30;	//透明に近づけていく

			if (m_Col.a <= 0.0f)
			{//完全に透明になった場合
				m_Col.a = 0.0f;	//透明度を透明に
				m_state = STATE_NONE;	//何もしていない状態に

				if (m_pObject != NULL)
				{
					m_pObject->SetDraw(false);
				}
			}
		}
		else if (m_state == STATE_OUT)
		{//フェードアウト状態
			m_Col.a += 1.0f / 30;	//不透明に近づけていく

			if (m_Col.a >= 1.0f)
			{//完全に不透明になった場合

				m_Col.a = 1.0f;	//透明度を不透明に

				//モードの設定(次の画面に移行)
				CManager::GetInstance()->SetMode(m_modeNext);
				return;
			}
		}
	}

	if (m_pObject != NULL)
	{
		m_pObject->SetCol(m_Col);
	}
}

//===============================================
// 次の画面設定
//===============================================
void CFade::Set(CScene::MODE modeNext)
{
	if (m_state != STATE_OUT)
	{
		m_state = STATE_OUT;	//フェードアウト状態に
		m_modeNext = modeNext;	//次の画面(モード)に
		m_Col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);	//透明に設定

		if (m_pObject != NULL)
		{
			m_pObject->SetDraw();
		}
	}
}

//===============================================
// 状態設定
//===============================================
void CFade::SetState(STATE state)
{
	if (m_state == state)
	{// 現在と指定された状態が同じ
		return;
	}

	m_state = state;

	switch (m_state)
	{
	case STATE_NONE:
		m_Col.a = 0.0f;
		break;

	case STATE_IN:
		m_Col.a = 1.0f;
		break;

	case STATE_OUT:
		m_Col.a = 0.0f;
		break;

	default:

		break;
	}

	if (m_pObject != NULL)
	{
		m_pObject->SetCol(m_Col);
	}
}

//===============================================
// 生成
//===============================================
CFade *CFade::Create(CScene::MODE modeNext)
{
	CFade *pFade = NULL;

	pFade = new CFade;

	if (pFade != NULL)
	{
		pFade->Init(modeNext);
		pFade->SetState(STATE_IN);
	}

	return pFade;
}