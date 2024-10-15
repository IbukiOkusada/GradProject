//==========================================================
//
// ������ [road.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "road.h"
#include "object3D.h"
#include "road_manager.h"

//==========================================================
// �R���X�g���N�^
//==========================================================
CRoad::CRoad(const SInfo& info)
{
	// �l�̃N���A
	m_Info = info;
	m_pObj = nullptr;

	for (int i = 0; i < DIRECTION::DIC_MAX; i++)
	{
		m_apConnectRoad[i] = nullptr;
	}

	CRoadManager::GetInstance()->ListIn(this);
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CRoad::~CRoad()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CRoad::Init(void)
{
	// �I�u�W�F�N�g�̐���
	m_pObj = CObject3D::Create(m_Info.pos, m_Info.rot);
	m_pObj->SetpVtx(m_Info.size.x, m_Info.size.y);

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CRoad::Uninit(void)
{
	// �A�����H�̋L����p��
	for (int i = 0; i < DIRECTION::DIC_MAX; i++)
	{
		m_apConnectRoad[i] = nullptr;
	}

	// �`��I�u�W�F�N�g��p��
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	CRoadManager::GetInstance()->ListOut(this);

	// �J��
	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CRoad::Update(void)
{

}

//==========================================================
// ����
//==========================================================
CRoad* CRoad::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR2& size)
{
	// ����
	CRoad* pRoad = nullptr;
	pRoad = new CRoad(SInfo(pos, rot, size));

	// ��������
	if (pRoad != nullptr)
	{
		// ����������
		pRoad->Init();
	}

	return pRoad;
}

//==========================================================
// �A��
//==========================================================
void CRoad::Connect(CRoad* pRoad, const DIRECTION dic)
{
	// �͈͊O�m�F
	if (dic < DIRECTION::DIC_UP || dic >= DIRECTION::DIC_MAX) { return; }

	m_apConnectRoad[dic] = pRoad;
}