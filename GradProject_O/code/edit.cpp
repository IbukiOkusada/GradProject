//==========================================================
//
// エディター基底 [edit.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit.h"
#include "edit_object.h"
#include "edit_road.h"
#include "edit_gimmick.h"
#include "edit_goal.h"
#include "edit_effect.h"

//==========================================================
// コンストラクタ
//==========================================================
CEdit::CEdit()
{
	// 値のクリア
}

//==========================================================
// デストラクタ
//==========================================================
CEdit::~CEdit()
{

}

#if 0
//==========================================================
// 初期化処理
//==========================================================
HRESULT CEdit::Init(void)
{
	return S_OK;
}
#endif

//==========================================================
// 終了処理
//==========================================================
void CEdit::Uninit(void)
{
	delete this;
}

#if 0
//==========================================================
// 更新処理
//==========================================================
void CEdit::Update(void)
{

}
#endif

//==========================================================
// 生成
//==========================================================
CEdit* CEdit::Create(const TYPE& type)
{
	CEdit* pEdit = nullptr;

	switch (type)
	{
	case TYPE_OBJ:
		pEdit = DEBUG_NEW CEdit_Obj;
	break;

	case TYPE_ROAD:
		pEdit = DEBUG_NEW CEdit_Road;
		break;

	case TYPE_GIMMICK:
		pEdit = DEBUG_NEW CEdit_Gimmick;
		break;

	case TYPE_GOAL:
		pEdit = DEBUG_NEW CEdit_Goal;
		break;

	case TYPE_EFFECT:
		pEdit = DEBUG_NEW CEdit_Effect;
		break;

	default:

		break;
	}

	if (pEdit != nullptr)
	{
		// 初期化処理
		pEdit->Init();
		pEdit->SetType(type);
	}

	return pEdit;
}
