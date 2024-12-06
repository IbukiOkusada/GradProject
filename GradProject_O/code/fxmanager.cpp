//============================================================
//
//	�V�F�[�_�[�Ǘ��֐� [fxmanager.cpp]
//	Author�F�O�엳�V��
//
//============================================================
//============================================================
//	�C���N���[�h�t�@�C��
//============================================================
#include "fxmanager.h"
#include "manager.h"
#include "renderer.h"
//============================================================
//	�萔�錾
//============================================================
namespace
{
	const char* EFFECT_FX = "code\shader\draw.fx";	// �V�F�[�_�[�̃G�t�F�N�g�t�@�C��
}
//************************************************************
//	�ÓI�����o�ϐ��錾
//************************************************************
CFXManager* CFXManager::m_pShader = nullptr;	// �V�F�[�_�[���

//============================================================
//	�R���X�g���N�^
//============================================================
CFXManager::CFXManager() :

	m_hDirectLight(nullptr),	
	m_hLightDiffuse(nullptr),	
	m_hUseTex(nullptr),
	m_hWorldMat(nullptr),
	m_hViewMat(nullptr),
	m_hProjMat(nullptr),
	m_pEffect(nullptr)
{

}

//============================================================
//	�f�X�g���N�^
//============================================================
CFXManager::~CFXManager()
{

}

//============================================================
//	����������
//============================================================
HRESULT CFXManager::Init(void)
{
	// �ϐ���錾
	D3DCAPS9 caps;	// �n�[�h�E�F�A�@�\
	HRESULT hr;		// �ُ�I���̊m�F�p

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// �f�o�C�X���
	LPD3DXBUFFER pError = nullptr;	// �R���p�C���G���[���
	LPD3DXEFFECT pEffect = nullptr;	// �G�t�F�N�g�ݒ�p

	// �����o�ϐ���������
	m_hDirectLight=nullptr;
	m_hLightDiffuse=nullptr;
	m_hUseTex=nullptr;
	m_hWorldMat=nullptr;
	m_hViewMat=nullptr;
	m_hProjMat=nullptr;
	m_pEffect = nullptr;
	// �n�[�h�E�F�A�@�\���擾
	pDevice->GetDeviceCaps(&caps);

	if (caps.VertexShaderVersion >= D3DVS_VERSION(1, 1)
		&& caps.PixelShaderVersion >= D3DPS_VERSION(2, 0))
	{ // ���_�E�s�N�Z���V�F�[�_�̃o�[�W�������g�p�\�ȏꍇ

		// �G�t�F�N�g�t�@�C���̓Ǎ�
		hr = D3DXCreateEffectFromFile
		( // ����
			pDevice,	// �f�o�C�X�ւ̃|�C���^
			EFFECT_FX,	// �G�t�F�N�g�t�@�C��
			nullptr,	// �v���v���Z�b�T��`
			nullptr,	// �C���N���[�h����
			0,			// �Ǎ��I�v�V�����t���O
			nullptr,	// �n���h���C���^�[�t�F�[�X
			&pEffect,	// �G�t�F�N�g�C���^�[�t�F�[�X
			&pError		// �R���p�C���G���[���
		);
		if (SUCCEEDED(hr))
		{ // �Ǎ��ɐ��������ꍇ

			// �G�t�F�N�g����ݒ�
			m_pEffect = pEffect;

			// �e�N�j�b�N�֐����擾�E�ݒ�
			SetTechnique(pEffect->GetTechniqueByName("TShader"));

			// �n���h�����擾
			m_hWorldMat = pEffect->GetParameterByName(nullptr, "s_mtxWorld");
			m_hViewMat	= pEffect->GetParameterByName(nullptr, "g_mtxView");
			m_hProjMat	= pEffect->GetParameterByName(nullptr, "g_mtxProj");

			m_hDirectLight	 = pEffect->GetParameterByName(nullptr, "m_LightDir");
			m_hLightDiffuse	 = pEffect->GetParameterByName(nullptr, "m_LightCol");
			m_hMatDiffuse	 = pEffect->GetParameterByName(nullptr, "m_diffus");
			m_hUseTex		 = pEffect->GetParameterByName(nullptr, "bUseTex");
		}
		else
		{ // �Ǎ��Ɏ��s�����ꍇ
			if (pError)
			{
				OutputDebugStringA((LPCSTR)pError->GetBufferPointer());
				//�f�o�b�O�R���\�[���ɕ\������
				MessageBoxA(NULL, (LPCSTR)pError->GetBufferPointer(), "Shader Error", MB_OK);
			}
			// �ǂݍ��ݎ��s��Ԃ�
			assert(false);
			return E_FAIL;
		}
	}
	else
	{ // �o�[�W�������g�p�s�ȏꍇ

		// �Â��o�[�W�����ɂ�鎸�s��Ԃ�
		assert(false);
		return E_FAIL;
	}

	// ������Ԃ�
	return S_OK;
}
//============================================================
//	�I������
//============================================================
void CFXManager::Uninit(void)
{
	SAFE_RELEASE(m_pEffect);
}
//============================================================
//	��������
//============================================================
CFXManager* CFXManager::Create(void)
{
	// �C���X�^���X�g�p��
	assert(m_pShader == nullptr);

	// �g�D�[���V�F�[�_�[�̐���
	m_pShader = DEBUG_NEW CFXManager;
	if (m_pShader == nullptr)
	{ // �����Ɏ��s�����ꍇ

		assert(false);
		return nullptr;
	}
	else
	{ // �����ɐ��������ꍇ

		// �g�D�[���V�F�[�_�[�̏�����
		if (FAILED(m_pShader->Init()))
		{ // �������Ɏ��s�����ꍇ

			// �g�D�[���V�F�[�_�[�̔j��
			SAFE_DELETE(m_pShader);
			return nullptr;
		}

		// �m�ۂ����A�h���X��Ԃ�
		return m_pShader;
	}
}
//============================================================
//	�擾����
//============================================================
CFXManager* CFXManager::GetInstance(void)
{
	if (m_pShader == nullptr)
	{
		m_pShader = CFXManager::Create();
	}

	// �|�C���^��Ԃ�
	return m_pShader;
}

//============================================================
//	�j������
//============================================================
void CFXManager::Release(void)
{
	// �g�D�[���V�F�[�_�[�̏I��
	assert(m_pShader != nullptr);
	m_pShader->Uninit();

	// �������J��
	SAFE_DELETE(m_pShader);
}

//============================================================
//	�e�N�j�b�N�֐��̐ݒ菈��
//============================================================
void CFXManager::SetTechnique(const D3DXHANDLE pTechnique)
{
	// �e�N�j�b�N�֐���ݒ�
	m_pTechnique = pTechnique;

	if (m_pEffect != nullptr)
	{ // �G�t�F�N�g���g�p����Ă���ꍇ

		// �G�t�F�N�g�Ƀe�N�j�b�N�֐���ݒ�
		m_pEffect->SetTechnique(m_pTechnique);
	}
	else { assert(false); }	// �G�t�F�N�g�g�p�s��
}
