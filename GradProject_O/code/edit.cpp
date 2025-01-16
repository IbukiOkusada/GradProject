//==========================================================
//
// �G�f�B�^�[��� [edit.cpp]
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
// �R���X�g���N�^
//==========================================================
CEdit::CEdit()
{
	// �l�̃N���A
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CEdit::~CEdit()
{

}

#if 0
//==========================================================
// ����������
//==========================================================
HRESULT CEdit::Init(void)
{
	return S_OK;
}
#endif

//==========================================================
// �I������
//==========================================================
void CEdit::Uninit(void)
{
	delete this;
}

#if 0
//==========================================================
// �X�V����
//==========================================================
void CEdit::Update(void)
{

}
#endif

//==========================================================
// ����
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
		// ����������
		pEdit->Init();
		pEdit->SetType(type);
	}

	return pEdit;
}
