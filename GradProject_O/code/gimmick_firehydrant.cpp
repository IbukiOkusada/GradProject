//==========================================================
//
// ���ΐ��M�~�b�N���� [gimmick_firehydrant.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "manager.h"
#include "gimmick_firehydrant.h"
#include "deltatime.h"
// �}�N����`

//==========================================================
// �R���X�g���N�^
//==========================================================
CFireHydrant::CFireHydrant()
{
	// �l�̃N���A
	m_pObj = nullptr;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CFireHydrant::~CFireHydrant()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CFireHydrant::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CFireHydrant::Uninit(void)
{
	CGimmick::Uninit();
}

//==========================================================
// �X�V����
//==========================================================
void CFireHydrant::Update(void)
{
	
}

//==========================================================
// ����
//==========================================================
CFireHydrant* CFireHydrant::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale)
{
	CFireHydrant* pFireHydrant = nullptr;

	pFireHydrant = DEBUG_NEW CFireHydrant;

	if (pFireHydrant != nullptr)
	{
		pFireHydrant->SetPos(pos);
		pFireHydrant->SetRot(rot);
		pFireHydrant->SetScale(scale);

		// ����������
		pFireHydrant->Init();
	}

	return pFireHydrant;
}
