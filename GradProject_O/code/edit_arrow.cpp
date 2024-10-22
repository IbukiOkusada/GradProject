//==========================================================
//
// �G�f�B�^�[��� [edit_arrow.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "edit_arrow.h"
#include "objectX.h"

// �������O���
namespace
{
	const char* FILENAME[CEdit_Arrow::TYPE_MAX] = {	// ���f���t�@�C����
		"data\\MODEL\\edit\\cube.x",
		"data\\MODEL\\edit\\x_arrow.x",
		"data\\MODEL\\edit\\y_arrow.x",
		"data\\MODEL\\edit\\z_arrow.x",
	};

	const D3DXVECTOR3 SETPOS[CEdit_Arrow::TYPE_MAX] = {	// �ݒu���W
		{0.0f, 0.0f, 0.0f},
		{30.0f, 0.0f, 0.0f},
		{0.0f, 30.0f, 0.0f},
		{0.0f, 0.0f, -30.0f},
	};
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CEdit_Arrow::CEdit_Arrow()
{
	// �l�̃N���A
	for (int i = 0; i < TYPE_MAX; i++)
	{
		m_apObj[i] = nullptr;
	}

	m_pos = VECTOR3_ZERO;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CEdit_Arrow::~CEdit_Arrow()
{

}

//==========================================================
// ����
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
// ����������
//==========================================================
HRESULT CEdit_Arrow::Init(void)
{
	m_pos = VECTOR3_ZERO;

	// ���f������
	for (int i = 0; i < TYPE_MAX; i++)
	{
		m_apObj[i] = CObjectX::Create(m_pos + SETPOS[i], VECTOR3_ZERO, FILENAME[i]);
	}

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CEdit_Arrow::Uninit(void)
{
	// �l�̃N���A
	for (int i = 0; i < TYPE_MAX; i++)
	{
		if (m_apObj[i] == nullptr) { continue; }

		m_apObj[i]->Uninit();
		m_apObj[i] = nullptr;
	}

	delete this;
}

//==========================================================
// �X�V����
//==========================================================
void CEdit_Arrow::Update(void)
{

	// ���W�̍X�V
	for (int i = 0; i < TYPE_MAX; i++)
	{
		if (m_apObj[i] == nullptr) { continue; }

		m_apObj[i]->SetPosition(m_pos + SETPOS[i]);
	}
}

//==========================================================
// ���W�ݒ�
//==========================================================
void CEdit_Arrow::SetPosition(const D3DXVECTOR3& pos)
{
	m_pos = pos;

	// ���W�̍X�V
	for (int i = 0; i < TYPE_MAX; i++)
	{
		if (m_apObj[i] == nullptr) { continue; }

		m_apObj[i]->SetPosition(m_pos + SETPOS[i]);
	}
}