//============================================================
//
//	�V�F�[�_�[�p���C�g���[shaderlight.h]
//	Author�F�O�엳�V��
//
//============================================================
//************************************************************
//	��d�C���N���[�h�h�~
//************************************************************
#ifndef _ShaderLight_
#define _ShaderLight_

//************************************************************
//	�C���N���[�h�t�@�C��
//************************************************************
#include "list.h"
#include "task.h"
#define MAX_LIGHT (20)
//************************************************************
//	�N���X��`
//************************************************************
// ���C�g�V�F�[�_�[�N���X
class CShaderLight
{
public:
	// �R���X�g���N�^
	CShaderLight();
	// �f�X�g���N�^
	~CShaderLight();
	// �I�[�o�[���C�h�֐�
	HRESULT Init(void);	// ������
	void Uninit(void);		// �I��
	void Update(void);
	
	struct SLight 
	{
		D3DXVECTOR3 position;    // ���C�g�̈ʒu
		float intensity;         // ���̋��x

		D3DXVECTOR3 direction;   // �X�|�b�g���C�g�̕���
		float range;             // ���C�g�̉e���͈�

		D3DXVECTOR3 color;       // ���C�g�̐F
		float spotAngle;         // �X�|�b�g���C�g�̊p�x
	};
	// �ÓI�����o�֐�
	
	static CShaderLight::SLight* Create(D3DXVECTOR3 pos, D3DXVECTOR3 col,float intens,float range,D3DXVECTOR3 vec = VECTOR3_ZERO,float Angle = 0.0f);		// ����
	static void Delete(CShaderLight::SLight* plight) { if (m_List != nullptr) { m_List->Delete(plight); } }
	static void Release() {SAFE_DELETE(m_List)}

	static Clist<CShaderLight::SLight*>* GetList() { if (m_List != nullptr) { return m_List; } return nullptr; }
private:
	

	// �ÓI�����o�ϐ�
	static Clist<CShaderLight::SLight*>* m_List;

};

#endif	// _ShaderLight_#pragma once
