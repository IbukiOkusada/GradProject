//==========================================================
//
// エディター矢印 [edit_arrow.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_arrow.h"
#include "objectX.h"

// 無名名前空間
namespace
{
	const char* FILENAME[CEdit_Arrow::TYPE_MAX] = {	// モデルファイル名
		"data\\MODEL\\edit\\cube.x",
		"data\\MODEL\\edit\\x_arrow.x",
		"data\\MODEL\\edit\\y_arrow.x",
		"data\\MODEL\\edit\\z_arrow.x",
	};

	const D3DXVECTOR3 SETPOS[CEdit_Arrow::TYPE_MAX] = {	// 設置座標
		{0.0f, 0.0f, 0.0f},
		{30.0f, 0.0f, 0.0f},
		{0.0f, 30.0f, 0.0f},
		{0.0f, 0.0f, -30.0f},
	};
}

//==========================================================
// コンストラクタ
//==========================================================
CEdit_Arrow::CEdit_Arrow()
{
	// 値のクリア
	for (int i = 0; i < TYPE_MAX; i++)
	{
		m_apObj[i] = nullptr;
	}

	m_pos = VECTOR3_ZERO;
}

//==========================================================
// デストラクタ
//==========================================================
CEdit_Arrow::~CEdit_Arrow()
{

}

//==========================================================
// 生成
//==========================================================
CEdit_Arrow* CEdit_Arrow::Create(const D3DXVECTOR3& pos)
{
	CEdit_Arrow* pArrow = DEBUG_NEW CEdit_Arrow;

	if (pArrow != nullptr)
	{
		pArrow->Init();
		pArrow->SetPosition(pos);
	}

	return pArrow;
}

//==========================================================
// 初期化処理
//==========================================================
HRESULT CEdit_Arrow::Init(void)
{
	m_pos = VECTOR3_ZERO;

	// モデル生成
	for (int i = 0; i < TYPE_MAX; i++)
	{
		m_apObj[i] = CObjectX::Create(m_pos + SETPOS[i], VECTOR3_ZERO, FILENAME[i]);
	}

	return S_OK;
}

//==========================================================
// 終了処理
//==========================================================
void CEdit_Arrow::Uninit(void)
{
	// 値のクリア
	for (int i = 0; i < TYPE_MAX; i++)
	{
		if (m_apObj[i] == nullptr) { continue; }

		m_apObj[i]->Uninit();
		m_apObj[i] = nullptr;
	}

	delete this;
}

//==========================================================
// 更新処理
//==========================================================
void CEdit_Arrow::Update(void)
{

	// 座標の更新
	for (int i = 0; i < TYPE_MAX; i++)
	{
		if (m_apObj[i] == nullptr) { continue; }

		m_apObj[i]->SetPosition(m_pos + SETPOS[i]);
	}
}

//==========================================================
// 座標設定
//==========================================================
void CEdit_Arrow::SetPosition(const D3DXVECTOR3& pos)
{
	m_pos = pos;

	// 座標の更新
	for (int i = 0; i < TYPE_MAX; i++)
	{
		if (m_apObj[i] == nullptr) { continue; }

		m_apObj[i]->SetPosition(m_pos + SETPOS[i]);
	}
}