//==========================================================
//
// マップ内障害物の処理 [map_obstacle.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "map_obstacle.h"
#include "map_manager.h"
#include "objectX.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"
#include "camera_manager.h"
#include "game.h"

namespace
{
	const float RANGE_WIDTH = SCREEN_WIDTH * 0.1f;
	const float RANGE_HEIGHT = SCREEN_HEIGHT * 0.1f;
}

// 静的メンバ変数宣言
Clist<CMapObstacle*>* CMapObstacle::m_pList = nullptr;

//==========================================================
// コンストラクタ
//==========================================================
CMapObstacle::CMapObstacle(const SInfo& info)
{
	//自身をリストに登録
	GetInstance()->Regist(this);
	m_Info = info;
	m_pObj = nullptr;
}

//==========================================================
// デストラクタ
//==========================================================
CMapObstacle::~CMapObstacle()
{
	//自身をリストから削除
	GetInstance()->Delete(this);
	if (GetInstance()->Empty())
	{//リストの中身が空ならリストを破棄
		ListRelease();
	}
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CMapObstacle::Init(void)
{
	std::vector<std::string> str = CMapManager::GetInstance()->GetFileNameList();
	m_pObj = CObjectX::Create(m_Info.pos, m_Info.rot, str[m_Info.fileidx].c_str());
	m_pObj->SetScale(m_Info.scale);

	return S_OK;
}

//==========================================================
// 初期化処理(オーバーロード)
//==========================================================
HRESULT CMapObstacle::Init(const std::string& filename)
{
	m_pObj = CObjectX::Create(m_Info.pos, m_Info.rot, filename.c_str());

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CMapObstacle::Uninit(void)
{
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	Release();
}

//==========================================================
// 更新処理
//==========================================================
void CMapObstacle::Update(void)
{
	DrawCheck();

	// オブジェクトの更新
	if (m_pObj != nullptr)
	{
		m_pObj->SetPosition(m_Info.pos);
		m_pObj->SetRotation(m_Info.rot);
		m_pObj->SetScale(m_Info.scale);
	}
}

//==========================================================
// 生成
//==========================================================
CMapObstacle* CMapObstacle::Create(const SInfo& info)
{
	CMapObstacle* pGoal = nullptr;

	pGoal = DEBUG_NEW CMapObstacle(info);

	if (pGoal != nullptr)
	{
		pGoal->m_Info = info;
		// 初期化処理
		pGoal->Init();
	}

	return pGoal;
}

//==========================================================
// 座標設定
//==========================================================
void CMapObstacle::SetPosition(const D3DXVECTOR3& pos)
{
	m_Info.pos = pos;

	// オブジェクトの更新
	if (m_pObj != nullptr)
	{
		m_pObj->SetPosition(m_Info.pos);
	}
}

//==========================================================
// 向き設定
//==========================================================
void CMapObstacle::SetRotation(const D3DXVECTOR3& rot)
{
	m_Info.rot = rot;

	// オブジェクトの更新
	if (m_pObj != nullptr)
	{
		m_pObj->SetRotation(m_Info.rot);
	}
}

//==========================================================
// スケール設定
//==========================================================
void CMapObstacle::SetScale(const D3DXVECTOR3& scale)
{
	m_Info.scale = scale;

	// オブジェクトの更新
	if (m_pObj != nullptr)
	{
		m_pObj->SetScale(m_Info.scale);
	}
}

//==========================================================
// モデル設定
//==========================================================
void CMapObstacle::BindModel(const int& idx)
{
	if (idx < 0 || idx >= CMapManager::GetInstance()->GetFileNameList().size()) { m_Info.fileidx = 0;}

	m_Info.fileidx = idx;

	if (m_pObj == nullptr) { return; }

	std::vector<std::string> str = CMapManager::GetInstance()->GetFileNameList();

	m_pObj->BindFile(str[idx].c_str());
}

//==========================================================
// 描画確認
//==========================================================
void CMapObstacle::DrawCheck()
{
	// 消火栓が移動した
	if (m_pObj == nullptr) { return; }

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();
	D3DXMATRIX mtxProjection, mtxView, mtxWorld;
	D3DVIEWPORT9 Viewport;
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	D3DXVECTOR3 mypos = m_pObj->GetPosition();

	// 必要な情報取得
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetViewport(&Viewport);

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// スクリーン座標取得
	D3DXVec3Project(&pos, &mypos, &Viewport, &mtxProjection, &mtxView, &mtxWorld);

	float f = 0.5f * (m_Info.scale.x + m_Info.scale.z);

	// 画面外なら出さない
	if (pos.x < 0.0f - (SCREEN_WIDTH * 0.3f * f) || pos.x > SCREEN_WIDTH + (SCREEN_WIDTH * 0.3f * f) ||
		pos.y < 0.0f - (SCREEN_HEIGHT * 0.3f * m_Info.scale.y) || pos.y > SCREEN_HEIGHT + (SCREEN_HEIGHT * 0.3f * m_Info.scale.y) ||
		pos.z >= 1.0f) {

		// 色を透明に近づける
		D3DXCOLOR col = m_pObj->GetColMulti();
		if (col.a > 0.0f)
		{
			col.a -= 0.1f;
			
			if (col.a <= 0.0f)
			{
				col.a = 0.0f;
				m_pObj->SetDraw(false);
			}
		}

		m_pObj->SetColMulti(col);

		return;
	}

	// 距離を取る
	{
		CCamera* pCamera = CCameraManager::GetInstance()->GetTop();
		D3DXVECTOR3 lenpos = pCamera->GetPositionR() - m_Info.pos;
		if (D3DXVec3Length(&lenpos) >= Game::DOME_LENGTH)
		{
			m_pObj->SetDraw(false);
			return;
		}
	}

	// 色を元に近づける
	D3DXCOLOR col = m_pObj->GetColMulti();
	if (col.a < 1.0f)
	{
		col.a += 0.1f;

		if (col.a >= 1.0f)
		{
			col.a = 1.0f;
		}
	}

	m_pObj->SetColMulti(col);
	m_pObj->SetDraw();
}
