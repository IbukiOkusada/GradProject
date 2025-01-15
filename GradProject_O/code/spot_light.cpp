//==========================================================
//
// �X�|�b�g���C�g�̏��� [spot_light.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "spot_light.h"
#include "renderer.h"
#include "manager.h"
#include "light.h"
#include "debugproc.h"

//==========================================================
// �R���X�g���N�^
//==========================================================
CSpotLight::CSpotLight()
{
	m_nId = -1;
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CSpotLight::~CSpotLight()
{

}

//==========================================================
// ��������
//==========================================================
CSpotLight* CSpotLight::Create()
{
	CSpotLight* pLight = DEBUG_NEW CSpotLight;

	if (pLight != nullptr)
	{
		pLight->Init();
	}

	return pLight;
}

//==========================================================
// ����������
//==========================================================
HRESULT CSpotLight::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		// �f�o�C�X�ւ̃|�C���^���擾

	D3DXVECTOR3 vecDir;		// �ݒ�ύX�p�x�N�g��

	// ���C�g�̏����N���A����
	ZeroMemory(&m_Light, sizeof(D3DLIGHT9));

	// ���C�g�̎�ނ�ݒ�
	m_Light.Type = D3DLIGHT_SPOT;
	m_Light.Position = D3DXVECTOR3(0.0f, 500.0f, 0.0f);

	// ���C�g�̊g�U����ݒ�
	m_Light.Diffuse = D3DXCOLOR(7.0f, 7.0f, 7.0f, 1.0f);

	// �Ƃ炷�����𐳂����l�ɐݒ�
	D3DXVECTOR3 target = D3DXVECTOR3(0.0f, 0.0f, 0.0f);  // �Ƃ炵�����ꏊ
	D3DXVECTOR3 position = m_Light.Position;            // ���C�g�̈ʒu
	D3DXVECTOR3 direction = target - position;          // ���C�g����^�[�Q�b�g�ւ̕���
	D3DXVec3Normalize(&direction, &direction);          // ���K��
	m_Light.Direction = direction;

	//���C�g�͈̔͐ݒ�
	m_Light.Range = 2000.0f;

	m_Light.Phi = D3DXToRadian(45.0f);	// �O���̃R�[���̊p�x
	m_Light.Theta = D3DXToRadian(00.0f);	// �����̃R�[���̊p�x
	m_Light.Falloff = 1.0f;				// �����ƊO���̃R�[���̌�����

	//���C�g�̌���0�̐ݒ�
	m_Light.Attenuation0 = 0.94f;

	//���C�g�̌���1�̐ݒ�
	m_Light.Attenuation1 = 0.0f;

	//���C�g�̌���2�̐ݒ�
	m_Light.Attenuation2 = 0.0f;

	// ���C�g��ݒ肷��
	m_nId = CLight::GetNumLight();
	CLight::AddNumLight();
	pDevice->SetLight(m_nId, &m_Light);

	// ���C�g��L��������
	pDevice->LightEnable(m_nId, TRUE);
	CLight::AddNumLight();

	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CSpotLight::Uninit()
{
	// ���C�g�𖳌�������
	CRenderer* pRenderer = CManager::GetInstance()->GetRenderer();
	if (pRenderer != nullptr)
	{
		LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// �f�o�C�X�ւ̃|�C���^���擾
		if (pDevice != nullptr)
		{
			pDevice->LightEnable(m_nId, false);
		}
	}

	CLight::MinusNumLight();
	delete this;
}

//==========================================================
// �X�V����
//==========================================================
void CSpotLight::Update()
{
	// ���C�g��ݒ肷��
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		// �f�o�C�X�ւ̃|�C���^���擾
	pDevice->SetLight(m_nId, &m_Light);

	CDebugProc* pProc = CDebugProc::GetInstance();
	pProc->Print("���C�g�̍��W : [ %f, %f, %f ]\n", m_Light.Position.x, m_Light.Position.y, m_Light.Position.z);
	pProc->Print("���C�g�̕��� : [ %f, %f, %f ]\n", m_Light.Direction.x, m_Light.Direction.y, m_Light.Direction.z);
	pProc->Print("���C�g�̐F : [ %f, %f, %f, %f ]\n", m_Light.Diffuse.r, m_Light.Diffuse.g, m_Light.Diffuse.b, m_Light.Diffuse.a);
}

//==========================================================
// ���W�ݒ�
//==========================================================
void CSpotLight::SetPositon(const D3DXVECTOR3& pos)
{
	// ���C�g�̈ʒu
	m_Light.Position = pos;
}

//==========================================================
// �����ݒ�
//==========================================================
void CSpotLight::SetDirection(const D3DXVECTOR3& dic)
{
	// ���C�g�̕���
	m_Light.Direction = dic;
}

//==========================================================
// �g�U���ݒ�
//==========================================================
void CSpotLight::SetDiffuse(const D3DXCOLOR& dif)
{
	// ���C�g��
	m_Light.Diffuse = dif;
}

//==========================================================
// �͈͐ݒ�
//==========================================================
void CSpotLight::SetRange(const float fRange)
{
	// ���C�g�̈ʒu (�Ԃ̃��C�g�̈ʒu�ɑ���)
	m_Light.Range = fRange;
}

//==========================================================
// �p�����[�^�ݒ�
//==========================================================
void CSpotLight::SetParameter(float fOutRadian, float fInRadian, float fFallOff)
{
	m_Light.Phi = fOutRadian;	// �O���̃R�[���̊p�x
	m_Light.Theta = fInRadian;	// �����̃R�[���̊p�x
	m_Light.Falloff = fFallOff; // �����ƊO���̃R�[���̌�����
}

//==========================================================
// �����ݒ�
//==========================================================
void CSpotLight::SetAttenuation(float fConstant, float fLinear, float fSecondary)
{
	// �����ݒ�
	m_Light.Attenuation0 = fConstant;  // �萔����
	m_Light.Attenuation1 = fLinear;		// ���`����
	m_Light.Attenuation2 = fSecondary; // �񎟌���
}