//==========================================================
//
// ���ΐ��M�~�b�N���� [gimmick_firehydrant.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "manager.h"
#include "gimmick_firehydrant.h"
#include "deltatime.h"
#include "objectX.h"
#include "camera.h"
#include "camera_manager.h"
#include "renderer.h"
#include "particle3D.h"

// �萔��`
namespace
{
	// �t�@�C����
	const char* FILENAME = "data\\MODEL\\map\\fire_hydrant_00.x";
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CGimmickFireHydrant::CGimmickFireHydrant()
{
	// �l�̃N���A
	m_pObj = nullptr;
	m_bHit = false;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CGimmickFireHydrant::~CGimmickFireHydrant()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CGimmickFireHydrant::Init(void)
{
	m_pObj = CObjectX::Create(GetPos(), GetRot(), FILENAME);
	m_pObj->SetScale(GetScale());

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CGimmickFireHydrant::Uninit(void)
{
	// �I�u�W�F�N�g�p��
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	CGimmick::Uninit();
}

//==========================================================
// �X�V����
//==========================================================
void CGimmickFireHydrant::Update(void)
{
	if (m_pObj == nullptr) { return; }

	// �G�t�F�N�g�ݒ�
	SetEffect();
}

//==========================================================
// ����
//==========================================================
CGimmickFireHydrant* CGimmickFireHydrant::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale)
{
	CGimmickFireHydrant* pFireHydrant = nullptr;

	pFireHydrant = DEBUG_NEW CGimmickFireHydrant;

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

//==========================================================
// �G�t�F�N�g����
//==========================================================
void CGimmickFireHydrant::SetEffect()
{
	// ���ΐ����ړ�����
	if (m_pObj == nullptr) { return; }
	if (m_bHit) { return; }

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();
	D3DXMATRIX mtxProjection, mtxView, mtxWorld;
	D3DVIEWPORT9 Viewport;
	D3DXVECTOR3 pos = VECTOR3_ZERO;
	D3DXVECTOR3 mypos = GetPos();

	// �K�v�ȏ��擾
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetViewport(&Viewport);

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxWorld);

	// �X�N���[�����W�擾
	D3DXVec3Project(&pos, &mypos, &Viewport, &mtxProjection, &mtxView, &mtxWorld);

	// ��ʊO�Ȃ�o���Ȃ�
	if (pos.x < 0.0f || pos.x > SCREEN_WIDTH ||
		pos.y < 0.0f || pos.y > SCREEN_HEIGHT) {
		return;
	}

	CParticle3D::Create(GetPos(), CEffect3D::TYPE_SMAKE);
}
