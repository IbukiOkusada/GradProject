//==========================================================
//
// ���⏈�� [inspection.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "inspection.h"
#include "add_police.h"
#include "objectX.h"

// �������O��Ԃ��`
namespace
{
	const float POLICE_SETROT = D3DX_PI * 0.5f;
	const float POLICE_SETLENGTH = 500.0f;
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CInstpection::CInstpection()
{
	// �l�̃N���A
	m_Guard = SGuardInfo();
	m_Info = SInfo();
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CInstpection::~CInstpection()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CInstpection::Init(void)
{
	// ��̐���
	m_Guard.pObj = CObjectX::Create(m_Info.pos, m_Info.rot, "data\\MODEL\\map\\guardrail001.x");

	// �p�g�J�[�̐ݒ�
	{
		// �����擾
		D3DXVECTOR3 rot = m_Info.rot;
		rot.y = POLICE_SETROT + m_Info.rot.y;
		Adjust(rot);

		// ���W�擾
		D3DXVECTOR3 pos = m_Info.pos;
		pos.x += sinf(rot.y) * POLICE_SETLENGTH;
		pos.z += cosf(rot.y) * POLICE_SETLENGTH;

		// ����
		m_pPolice = CAddPolice::Create(pos, rot, VECTOR3_ZERO);
	}

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CInstpection::Uninit(void)
{
	// �x�@�p��
	if (m_pPolice != nullptr)
	{
		m_pPolice->Uninit();
		m_pPolice = nullptr;
	}

	if (m_Guard.pObj != nullptr)
	{
		m_Guard.pObj->Uninit();
		m_Guard.pObj = nullptr;
	}

	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CInstpection::Update(void)
{
	if (m_Guard.pObj != nullptr)
	{
		if (m_Guard.pObj->GetHit() || m_Guard.pObj->GetHitOld())
		{
			m_bHit = true;
		}
	}

	// �Փ˂��Ă��Ȃ�
	if (m_bHit) { return; }
}

//==========================================================
// ����
//==========================================================
CInstpection* CInstpection::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot)
{
	CInstpection* pInsp = nullptr;

	pInsp = DEBUG_NEW CInstpection;

	if (pInsp != nullptr)
	{
		// �l�ݒ�
		pInsp->SetPosition(pos);
		pInsp->SetRotation(rot);

		// ����������
		pInsp->Init();
	}

	return pInsp;
}