//============================================================
//
//	�V�F�[�_�[�Ǘ�[FXManager.h]
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
#include "shaderlight.h"
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
	void SetLightVec(const D3DXVECTOR4& rVec);
	void SetCameraVec(const D3DXVECTOR3& rVec);
	void SetMatrixWorld(const D3DXMATRIX& rmtxWorld);
	void SetView(const D3DXMATRIX& rmtxView);
	void SetViewpos(const D3DXVECTOR3& rposView);
	void SetProj(const D3DXMATRIX& rmtxPorj);
	void SetScale(const D3DXMATRIX& rmtxScale);
	void SetTechnique(const D3DXHANDLE pTechnique);
	void SetLight();
	void SetParamToEffect();
	// �`��̊J�n��錾����
	HRESULT Begin();
	// �`��̏I����錾����
	HRESULT End();
	// �p�X�̊J�n��錾����
	HRESULT BeginPass();
	// �p�X�̏I����錾����
	HRESULT EndPass();
	LPD3DXEFFECT GetEffect() { return m_pEffect; }
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
	D3DXHANDLE m_hMatAmbient;					// �}�e���A�����F
	D3DXHANDLE m_hMatEmissive;					// �}�e���A�������F
	D3DXHANDLE m_hMatSpecula;					// �}�e���A�����ːF
	D3DXHANDLE m_hMatPower;					// �}�e���A�����x
	D3DXHANDLE m_hUseTex;						// �e�N�X�`���̎g�p��
	D3DXHANDLE m_hViewvec;					//���_�x�N�g��
	D3DXHANDLE m_hWorldMat;						// ���[���h�ϊ��s��n���h��
	D3DXHANDLE m_hViewMat;						// �r���[�ϊ��s��n���h��
	D3DXHANDLE m_hProjMat;						// �ˉe�ϊ��s��n���h��
	D3DXHANDLE m_hMatScaleReverse;				// �X�P�[�����O�t�s��
	D3DXHANDLE m_hnumLights;				// ���C�g�̐�
	D3DXHANDLE m_hLights;				// ���C�g�\����
	D3DXHANDLE m_hviewPos;				// ���_

	
	D3DXMATRIX m_matWorld;						// ���[���h�ϊ��s��
	D3DXMATRIX m_matView;						// �r���[�ϊ��s��
	D3DXMATRIX m_matProj;						// �ˉe�ϊ��s��
	D3DXMATRIX m_matScaleReverse;				// �X�P�[�����O�t�s��
	D3DXVECTOR4 m_DirectLight;					// ���s�����̕����x�N�g��
	D3DXVECTOR4 m_viewPos;				// ���_
	D3DXVECTOR4 m_LightDiffuse;					// �g�U���F
	D3DXVECTOR4 m_MatDiffuse;					// �}�e���A���g�U���F
	D3DXVECTOR4 m_MatAmbient;					// �}�e���A�����F
	D3DXVECTOR4 m_MatEmissive;					// �}�e���A�������F
	D3DXVECTOR4 m_Viewvec;					//���_�x�N�g��
	D3DXVECTOR4 m_MatSpecula;					// �}�e���A�����ːF
	float m_MatPower;					// �}�e���A�����x
	int m_numLights;
	CShaderLight::SLight m_lightArray[MAX_LIGHT];
	bool m_bBegin;
};

#endif	// _TOON_SHADOW_H_
