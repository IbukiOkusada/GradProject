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
#define MAX_LIGHT (8)
//************************************************************
//	�N���X��`
//************************************************************
// �g�D�[���V�F�[�_�[�N���X
class CShaderLight
{
public:

	// �R���X�g���N�^
	CShaderLight();

	// �f�X�g���N�^
	~CShaderLight();

	struct SLight
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 direction;
		D3DXVECTOR3 color;
		float intensity;
		float range;
		float spotAngle;
	};

	// �I�[�o�[���C�h�֐�
	HRESULT Init(void);	// ������
	void Uninit(void);		// �I��

	

	// �ÓI�����o�֐�
	static CShaderLight* Create(void);		// ����
	static CShaderLight* GetInstance(void);	// �擾
	static void Release(void);				// �j��

private:


	// �ÓI�����o�ϐ�
	static CShaderLight* m_pShaderLight;	// �V�F�[�_�[���

	// �����o�ϐ�
	static Clist<SLight> * m_List;
};

#endif	// _ShaderLight_#pragma once
