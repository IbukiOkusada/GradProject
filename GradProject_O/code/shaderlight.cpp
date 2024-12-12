//============================================================
//
//	�V�F�[�_�[�Ǘ��֐� [ShaderLight.cpp]
//	Author�F�O�엳�V��
//
//============================================================
//============================================================
//	�C���N���[�h�t�@�C��
//============================================================
#include "shaderlight.h"
#include "manager.h"
#include "renderer.h"
//============================================================
//	�萔�錾
//============================================================
namespace
{
	const char* EFFECT_FX = "code\\shader\\draw.fx";	// �V�F�[�_�[�̃G�t�F�N�g�t�@�C��
	const D3DXCOLOR LIGHT_COLOR = D3DXVECTOR4(0.6f, 0.6f, 0.7f, 1.0f);
	const D3DXVECTOR4 LIGHT_VEC = D3DXVECTOR4(-0.5f, -0.87f, 0.05f, 0.0f);
}
//************************************************************
//	�ÓI�����o�ϐ��錾
//************************************************************
Clist<CShaderLight::SLight*>* CShaderLight::m_List = nullptr;	// �V�F�[�_�[���

//============================================================
//	�R���X�g���N�^
//============================================================
CShaderLight::CShaderLight()
{
	
}

//============================================================
//	�f�X�g���N�^
//============================================================
CShaderLight::~CShaderLight()
{

}

//============================================================
//	����������
//============================================================
HRESULT CShaderLight::Init(void)
{
	
	// ������Ԃ�
	return S_OK;
}
//============================================================
//	�I������
//============================================================
void CShaderLight::Uninit(void)
{
	
}
//============================================================
//	�X�V����
//============================================================
void CShaderLight::Update(void)
{

}
//============================================================
//	��������
//============================================================
CShaderLight::SLight* CShaderLight::Create(D3DXVECTOR3 pos, D3DXVECTOR3 col, float intens, float range, D3DXVECTOR3 vec, float Angle)
{
	if (m_List == nullptr)
	{
		m_List = DEBUG_NEW Clist<CShaderLight::SLight*>;
		m_List->Clear();
	}
	CShaderLight::SLight* plight;
	// �g�D�[���V�F�[�_�[�̐���
		plight = DEBUG_NEW CShaderLight::SLight;
	if (plight == nullptr)
	{ // �����Ɏ��s�����ꍇ

		assert(false);
		return nullptr;
	}
	else
	{ // �����ɐ��������ꍇ
	
		plight->position = pos;
		plight->color = col;
		plight->intensity = intens;
		plight->range = range;
		D3DXVec3Normalize(&vec, &vec);
		plight->direction = vec;
		plight->spotAngle = Angle;
		m_List->Regist(plight);
		// �m�ۂ����A�h���X��Ԃ�
		return plight;
	}
}
