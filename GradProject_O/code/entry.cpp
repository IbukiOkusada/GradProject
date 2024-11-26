//===============================================
//
// エントリー画面の管理処理 [entry.cpp]
// Author : Ryosuke Oohara
//
//===============================================
#include "entry.h"
#include "camera.h"

//===============================================
// 定数定義
//===============================================
namespace
{
    const int WIDTH_NUM = 2;   // 横の分割数
    const int HEIGHT_NUM = 2;  // 縦の分割数
}

//===============================================
// コンストラクタ
//===============================================
CEntry::CEntry()
{
	m_ppCamera = nullptr;
}

//===============================================
// デストラクタ
//===============================================
CEntry::~CEntry()
{

}

//===============================================
// 初期化処理
//===============================================
HRESULT CEntry::Init(void)
{
	//m_ppCamera = DEBUG_NEW CCamera * [4];

 //   for (int i = 0; i < 4; i++)
 //   {
 //       m_ppCamera[i] = new CMultiCamera;
 //       m_ppCamera[i]->Init();
 //       m_ppCamera[i]->SetPositionV(D3DXVECTOR3(-874.3f, 1124.15f, 1717.2f));
 //       m_ppCamera[i]->SetPositionR(D3DXVECTOR3(-320.3f, 1.0f, -91.6f));
 //       m_ppCamera[i]->SetLength(400.0f);
 //       m_ppCamera[i]->SetRotation(D3DXVECTOR3(0.0f, D3DX_PI * 1.0f, D3DX_PI * 0.41f));

 //       D3DVIEWPORT9 viewport;
 //       //プレイヤー追従カメラの画面位置設定
 //       viewport.X = (DWORD)((SCREEN_WIDTH * 0.5f) * (i % WIDTH_NUM));
 //       viewport.Y = (DWORD)((SCREEN_HEIGHT * 0.5f) * (i / WIDTH_NUM));

 //       if (i < WIDTH_NUM) {
 //           viewport.Width = (DWORD)(SCREEN_WIDTH * 1.0f);
 //       }
 //       else
 //       {
 //           viewport.Width = (DWORD)(SCREEN_WIDTH * 0.5f);
 //       }

 //       if (i <= WIDTH_NUM) {
 //           viewport.Height = (DWORD)(SCREEN_HEIGHT * 1.0f);
 //       }
 //       else
 //       {
 //           viewport.Height = (DWORD)(SCREEN_HEIGHT * 0.5f);
 //       }

 //       viewport.MinZ = 0.0f;
 //       viewport.MaxZ = 1.0f;
 //       m_ppCamera[i]->SetViewPort(viewport);
 //   }
  
	return S_OK;
}

//===============================================
// 終了処理
//===============================================
void CEntry::Uninit(void)
{

}

//===============================================
// 更新処理
//===============================================
void CEntry::Update(void)
{

}

//===============================================
// 描画処理
//===============================================
void CEntry::Draw(void)
{

}