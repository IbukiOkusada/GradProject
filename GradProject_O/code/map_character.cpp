//==========================================================
//
// �}�b�v�z�u�̃L�����N�^�[���� [map_character.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "map_character.h"
#include "character.h"
#include "motion.h"

//==========================================================
// �萔��`
//==========================================================
namespace
{
	
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CMapCharacter::CMapCharacter()
{
	// �l�̃N���A
	m_pCharacter = nullptr;
	m_Info = SInfo();
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CMapCharacter::~CMapCharacter()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CMapCharacter::Init()
{
	return S_OK;
}

//==========================================================
// ����������
//==========================================================
HRESULT CMapCharacter::Init(const D3DXVECTOR3& rot, const char* pPath, int nPlayMotion)
{
	m_pCharacter = CCharacter::Create(pPath);
	m_pCharacter->SetParent(NULL);
	m_pCharacter->GetMotion()->InitSet(nPlayMotion);
	m_pCharacter->SetScale(D3DXVECTOR3(7.0f, 7.0f, 7.0f));

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CMapCharacter::Uninit(void)
{
	SAFE_UNINIT(m_pCharacter)

	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CMapCharacter::Update(void)
{
	Set();

	if (m_pCharacter != nullptr)
	{
		m_pCharacter->Update();
	}
}

//==========================================================
// ����
//==========================================================
CMapCharacter* CMapCharacter::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale, const char* pPath, int nPlayMotion)
{
	CMapCharacter* pMapCharacter = nullptr;

	pMapCharacter = DEBUG_NEW CMapCharacter;

	if (pMapCharacter != nullptr)
	{
		// ����������
		pMapCharacter->Init(rot, pPath, nPlayMotion);

		// ���W�ݒ�
		pMapCharacter->SetPosition(pos);

		// �����ݒ�
		pMapCharacter->SetRotation(rot);

		// �傫���ݒ�
		pMapCharacter->SetScale(scale);
	}

	return pMapCharacter;
}

//==========================================================
// ���W�ݒ�
//==========================================================
void CMapCharacter::SetPosition(const D3DXVECTOR3& pos)
{
	m_Info.pos = pos;

	if (m_pCharacter != nullptr)
	{
		m_pCharacter->SetPosition(m_Info.pos);
	}
}

//==========================================================
// �����ݒ�
//==========================================================
void CMapCharacter::SetRotation(const D3DXVECTOR3& rot)
{
	m_Info.rot = rot;
	correction::Adjust(&m_Info.rot);

	if (m_pCharacter != nullptr)
	{
		m_pCharacter->SetRotation(m_Info.rot);
	}
}

//==========================================================
// �傫���ݒ�
//==========================================================
void CMapCharacter::SetScale(const D3DXVECTOR3& scale)
{
	m_Info.scale = scale;

	if (m_pCharacter != nullptr)
	{
		m_pCharacter->SetScale(m_Info.scale);
	}
}

//==========================================================
// �ݒ�
//==========================================================
void CMapCharacter::Set()
{
	if (m_pCharacter != nullptr)
	{
		m_pCharacter->SetPosition(m_Info.pos);
		m_pCharacter->SetRotation(m_Info.rot);
	}
}