//===============================================
//
// リザルト画面の管理処理 [result.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "result.h"
#include "object2D.h"
#include "texture.h"
#include "manager.h"
#include "main.h"
#include "input.h"
#include "fade.h"
#include "ranking.h"
#include "sound.h"
#include "result.h"
#include "camera.h"
#include "ranking.h"
#include "object_manager.h"
#include "time.h"

// 静的メンバ変数
int CResult::m_nScore = 0;

//===============================================
// コンストラクタ
//===============================================
CResult::CResult()
{
	m_pMeshSky = NULL;
	m_pTime = NULL;
	m_pObjClear = nullptr;
}

//===============================================
// デストラクタ
//===============================================
CResult::~CResult()
{

}

//===============================================
// 初期化処理
//===============================================
HRESULT CResult::Init(void)
{

	//カメラ初期化
	{
		CManager::GetInstance()->GetCamera()->SetLength(300.0f);
		CManager::GetInstance()->GetCamera()->SetRotation(D3DXVECTOR3(1.0f, -D3DX_PI * 0.5f, 2.63f));
		CManager::GetInstance()->GetCamera()->SetPositionR(D3DXVECTOR3(0.0f, 137.77f, -301.94f));
		D3DVIEWPORT9 viewport;

		//プレイヤー追従カメラの画面位置設定
		viewport.X = 0;
		viewport.Y = 0;
		viewport.Width = (DWORD)(SCREEN_WIDTH * 1.0f);
		viewport.Height = (DWORD)(SCREEN_HEIGHT * 1.0f);
		viewport.MinZ = 0.0f;
		viewport.MaxZ = 1.0f;
		CManager::GetInstance()->GetCamera()->SetViewPort(viewport);
	}

	//CManager::GetInstance()->GetSound()->Play(CSound::LABEL_BGM_RESULT);

	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CResult::Uninit(void)
{

	CRanking::SetScore(m_nScore);
	m_nScore = 0;
	CManager::GetInstance()->GetCamera()->SetActive(true);
	CManager::GetInstance()->GetSound()->Stop();
}

//===============================================
// 更新処理
//===============================================
void CResult::Update(void)
{

	if (CManager::GetInstance()->GetInputPad()->GetTrigger(CInputPad::BUTTON_A, 0) ||
		CManager::GetInstance()->GetInputPad()->GetTrigger(CInputPad::BUTTON_START, 0))
	{
		CManager::GetInstance()->GetFade()->Set(CScene::MODE_RANKING);
	}

	CScene::Update();
}

//===============================================
// 描画処理
//===============================================
void CResult::Draw(void)
{
	CScene::Draw();
}
