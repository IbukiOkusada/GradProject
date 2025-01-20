//==========================================================
//
// ���C�g�̏����S�� [light.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "light.h"
#include "input.h"
#include "manager.h"
#include "renderer.h"

// �ÓI�����o�ϐ��錾
int CLight::m_nNumLightCnt = 0;

//==========================================================
// �R���X�g���N�^
//==========================================================
CLight::CLight()
{
	
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CLight::~CLight()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CLight::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		// �f�o�C�X�ւ̃|�C���^���擾
	D3DXVECTOR3 vecDir;		// �ݒ�ύX�p�x�N�g��

	for (int nCntLight = 0; nCntLight < NUM_LIGHT; nCntLight++)
	{
		// ���C�g�̏����N���A����
		ZeroMemory(&m_aLight[nCntLight], sizeof(D3DLIGHT9));

		// ���C�g�̎�ނ�ݒ�
		m_aLight[nCntLight].Type = D3DLIGHT_DIRECTIONAL;

		// ���C�g�̊g�U����ݒ�
		switch (nCntLight)
		{
		case 0:
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.6f, 0.6f, 0.7f, 1.0f);
			break;
		case 1:
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.52f, 0.52f, 0.52f, 1.0f);
			break;
		case 2:
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.52f, 0.52f, 0.52f, 1.0f);
			break;
		case 3:
			m_aLight[nCntLight].Diffuse = D3DXCOLOR(0.6f, 0.6f, 0.7f, 1.0f);
			break;
		}
		// ���C�g�̕�����ݒ�
		switch (nCntLight)
		{
		case 0:
			//vecDir = D3DXVECTOR3(0.1f, -0.11f, 0.44f);
			//vecDir = D3DXVECTOR3(0.8f, 0.23f, -0.55f);
			vecDir = D3DXVECTOR3(-0.5f, -0.87f, 0.05f);

			break;
		case 1:
			vecDir = D3DXVECTOR3(0.46f, 0.88f, -0.44f);
			break;
		case 2:
			vecDir = D3DXVECTOR3(-0.60f, -0.82f, 0.42f);
			break;
		case 3:
			vecDir = D3DXVECTOR3(0.22f, -0.87f, -0.44f);
			break;
		}

		D3DXVec3Normalize(&vecDir, &vecDir);	// �x�N�g���𐳋K������
		m_aLight[nCntLight].Direction = vecDir;

		// ���C�g��ݒ肷��
		pDevice->SetLight(m_nNumLightCnt, &m_aLight[nCntLight]);

		// ���C�g��L��������
		pDevice->LightEnable(m_nNumLightCnt, TRUE);
		AddNumLight();
	}

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CLight::Uninit(void)
{
	for (int nCntLight = 0; nCntLight < NUM_LIGHT; nCntLight++)
	{
		MinusNumLight();
	}
}

//==========================================================
// �X�V����
//==========================================================
void CLight::Update(void)
{

}