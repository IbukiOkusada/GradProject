//==========================================================
//
// �T���v���^�X�N(���X�g�Ǘ�) [Bridge.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "manager.h"
#include "Bridge.h"
#include "deltatime.h"
// �}�N����`

//==========================================================
// �R���X�g���N�^
//==========================================================
CBridge::CBridge()
{
	// �l�̃N���A
	m_bPass = false;
	m_fDownTime = 0.0f;
	m_fUpTime = 300.0f;
	m_fTimeCount = 300.0f;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CBridge::~CBridge()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CBridge::Init(void)
{

	for (int i = 0; i < BRIDGE_NUM; i++)
	{
		D3DXMATRIX mtxRot;
		D3DXVECTOR3 pos = GetPos();
		D3DXVECTOR3 offset = VECTOR3_ZERO;
		D3DXVECTOR3 rot = GetRot();
		D3DXVECTOR3 scale = GetScale();
		offset.z += (GetScale().z * 0.5f) ;
		rot.y += D3DX_PI * i;
		D3DXMatrixIdentity(&mtxRot);
		D3DXMatrixRotationY(&mtxRot, rot.y);
		D3DXVec3TransformCoord(&offset, &offset, &mtxRot);
		m_pBridge[i] = CObjectX::Create(pos+offset, rot, "data\\MODEL\\map\\cube_bridge.x");
		scale.z *= 0.5f;
		m_pBridge[i]->SetScale(scale);
	}

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CBridge::Uninit(void)
{
//	SAFE_DELETEARRAY(m_pBridge[0]);
	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CBridge::Update(void)
{
	m_fTimeCount += 1.0f;
	if (m_bPass)
	{
		if (m_fTimeCount > m_fDownTime)
		{
			m_fTimeCount = 0.0f;
			m_bPass = false;
		}
	}
	else
	{
		if (m_fTimeCount > m_fUpTime)
		{
			m_fTimeCount = 0.0f;
			m_bPass = true;
		}
	}
	MoveBridge();
	//CManager::GetInstance()->GetDeltaTime()->GetDeltaTime();
}
//==========================================================
// ���̉�
//==========================================================
void CBridge::MoveBridge(void)
{
	float fAngle;
	if (m_bPass)
	{
		fAngle = 0.0f;
	}
	else
	{
		fAngle = D3DX_PI * 0.4f;
		
	}
	for (int i = 0; i < BRIDGE_NUM; i++)
	{
		D3DXVECTOR3 rot = m_pBridge[i]->GetRotation();
		D3DXVECTOR3 targetrot = rot;
		targetrot.x = fAngle;
		m_pBridge[i]->SetRotation(rot + ((targetrot - rot) * 0.03f));
	}
}
//==========================================================
// ����
//==========================================================
CBridge* CBridge::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fdown, float fup)
{
	CBridge* pBridge = nullptr;

	pBridge = DEBUG_NEW CBridge;

	if (pBridge != nullptr)
	{
		pBridge->SetPos(pos);
		pBridge->SetRot(rot);
		pBridge->SetScale(scale);
		pBridge->m_fDownTime = fdown;
		pBridge->m_fUpTime = fup;
		// ����������
		pBridge->Init();
	}

	return pBridge;
}
