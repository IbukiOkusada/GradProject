//============================================================
//
//	�g�D�[���V���h�E�w�b�_�[ [FXManager.h]
//	Author�F�O�엳�V��
//
//============================================================
//************************************************************
//	��d�C���N���[�h�h�~
//************************************************************
#ifndef _TOON_SHADOW_H_
#define _TOON_SHADOW_H_

//************************************************************
//	�C���N���[�h�t�@�C��
//************************************************************
#include"texture.h"

//************************************************************
//	�N���X��`
//************************************************************
// �g�D�[���V�F�[�_�[�N���X
class CFXManager 
{
public:

	// �R���X�g���N�^
	CFXManager();

	// �f�X�g���N�^
	~CFXManager();

	// �I�[�o�[���C�h�֐�
	HRESULT Init(void);	// ������
	void Uninit(void);		// �I��

	// �����o�֐�
	void SetMaterial(const D3DMATERIAL9& rMaterial);		// �}�e���A���ݒ�
	void SetDiffuse(const D3DXCOLOR& rDiffuse);				// �g�U���ݒ�
	void SetMatrix(const D3DXMATRIX& rmtxWorld);
	void SetTechnique(const D3DXHANDLE pTechnique);
	// �ÓI�����o�֐�
	static CFXManager* Create(void);		// ����
	static CFXManager* GetInstance(void);	// �擾
	static void Release(void);				// �j��

private:
	
	
	// �ÓI�����o�ϐ�
	static CFXManager* m_pShader;	// �V�F�[�_�[���

	// �����o�ϐ�
	LPD3DXEFFECT m_pEffect;						// �G�t�F�N�g

	D3DXHANDLE m_pTechnique;					// �e�N�j�b�N

	D3DXHANDLE m_hDirectLight;					// ���s�����̕����x�N�g��
	D3DXHANDLE m_hLightDiffuse;					// �g�U���F
	D3DXHANDLE m_hMatDiffuse;					// �}�e���A���g�U���F
	D3DXHANDLE m_hUseTex;						// �e�N�X�`���̎g�p��

	D3DXHANDLE m_hWorldMat;						// ���[���h�ϊ��s��n���h��
	D3DXHANDLE m_hViewMat;						// �r���[�ϊ��s��n���h��
	D3DXHANDLE m_hProjMat;						// �ˉe�ϊ��s��n���h��

	D3DXMATRIX m_matWorld;						// ���[���h�ϊ��s��
	D3DXMATRIX m_matView;						// �r���[�ϊ��s��
	D3DXMATRIX m_matProj;						// �ˉe�ϊ��s��
};

#endif	// _TOON_SHADOW_H_
