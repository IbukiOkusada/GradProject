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
	const char* EFFECT_FX = "code\\shader\\draw.fx";	// �V�F�[�_�[�̃G�t�F�N�g�t�@�C��
	const D3DXCOLOR LIGHT_COLOR = D3DXVECTOR4(0.5f, 0.5f, 0.6f, 1.0f);
	const D3DXVECTOR4 LIGHT_VEC = D3DXVECTOR4(-0.5f, -0.87f, 0.05f, 0.0f);
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
	m_pEffect(nullptr),
	m_lightArray(),
	m_numLights(0)
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
			m_hWorldMat = pEffect->GetParameterByName(nullptr, "g_mtxWorld");
			m_hViewMat	= pEffect->GetParameterByName(nullptr, "g_mtxView");
			m_hProjMat	= pEffect->GetParameterByName(nullptr, "g_mtxProj");

			m_hViewvec = pEffect->GetParameterByName(nullptr, "viewDir");
			m_hviewPos = pEffect->GetParameterByName(nullptr, "viewPos");
			m_hDirectLight	 = pEffect->GetParameterByName(nullptr, "m_LightDir");
			m_hLightDiffuse	 = pEffect->GetParameterByName(nullptr, "m_LightCol");
			m_hMatDiffuse	 = pEffect->GetParameterByName(nullptr, "m_diffus");
			m_hMatAmbient = pEffect->GetParameterByName(nullptr, "m_ambient");
			m_hMatEmissive = pEffect->GetParameterByName(nullptr, "m_Emissive");
			m_hMatSpecula = pEffect->GetParameterByName(nullptr, "m_specula");
			m_hMatPower = pEffect->GetParameterByName(nullptr, "m_power");
			m_hUseTex = pEffect->GetParameterByName(nullptr, "bUseTex");
			m_hMatScaleReverse = pEffect->GetParameterByName(nullptr, "g_mMatScaleReverse");
			m_hnumLights = pEffect->GetParameterByName(nullptr, "numLights");
			m_hLights = pEffect->GetParameterByName(nullptr, "lights");
			m_hCubeMap = pEffect->GetParameterByName(nullptr, "CubeMapSampler");
			D3DXCreateCubeTextureFromFile(pDevice, "data\\TEXTURE\\cube.dds", &m_pCubeTexture);
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
	
	SetLightVec(LIGHT_VEC);
	SetDiffuse(LIGHT_COLOR*0.75f);
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
//============================================================
//	���C�g�ݒ菈��
//============================================================
void CFXManager::SetLight()
{
	Clist<CShaderLight::SLight*>* List = CShaderLight::GetList();
	if (List != nullptr)
	{
		m_numLights = List->GetNum();
		for (int i = 0; i < m_numLights; i++)
		{
			m_lightArray[i] = *List->Get(i);
		}
	}
	else
	{
		m_numLights = 0;
	}
}
//============================================================
// �}�e���A���ݒ�
//============================================================
void CFXManager::SetMaterial(const D3DMATERIAL9& rMaterial)
{
	D3DXVECTOR4 Diff;
	Diff.x = rMaterial.Diffuse.r;
	Diff.y = rMaterial.Diffuse.g;
	Diff.z = rMaterial.Diffuse.b;
	Diff.w = rMaterial.Diffuse.a;
	m_MatDiffuse = Diff;
	D3DXVECTOR4 ambient;
	ambient.x = rMaterial.Ambient.r;
	ambient.y = rMaterial.Ambient.g;
	ambient.z = rMaterial.Ambient.b;
	ambient.w = rMaterial.Ambient.a;
	m_MatAmbient = ambient;
	D3DXVECTOR4 Emissive;
	Emissive.x = rMaterial.Emissive.r;
	Emissive.y = rMaterial.Emissive.g;
	Emissive.z = rMaterial.Emissive.b;
	Emissive.w = rMaterial.Emissive.a;
	m_MatEmissive = Emissive;
	D3DXVECTOR4 specula;
	specula.x = rMaterial.Specular.r;
	specula.y = rMaterial.Specular.g;
	specula.z = rMaterial.Specular.b;
	specula.w = rMaterial.Specular.a;
	m_MatSpecula = specula;
	m_MatPower = rMaterial.Power;
}
//============================================================
// �g�U���ݒ�
//============================================================
void CFXManager::SetDiffuse(const D3DXCOLOR& rDiffuse)
{
	D3DXVECTOR4 Diff;
	Diff.x = rDiffuse.r;
	Diff.y = rDiffuse.g;
	Diff.z = rDiffuse.b;
	Diff.w = rDiffuse.a;
	m_LightDiffuse = Diff;

}
//============================================================
// ���������ݒ�
//============================================================
void CFXManager::SetLightVec(const D3DXVECTOR4& rVec)
{
	D3DXVECTOR4 vec = rVec;
	D3DXVec4Normalize(&vec, &vec);
	m_DirectLight = vec;

}
//============================================================
// ���_�����ݒ�
//============================================================
void  CFXManager::SetCameraVec(const D3DXVECTOR3& rVec)
{
	D3DXVECTOR4 vec = rVec;
	D3DXVec4Normalize(&vec, &vec);
	m_Viewvec = vec;
}
//============================================================
//	�}�g���b�N�X�̐ݒ�
//============================================================
void  CFXManager::SetMatrixWorld(const D3DXMATRIX& rmtxWorld)
{
	m_matWorld = rmtxWorld;

}
void  CFXManager::SetView(const D3DXMATRIX& rmtxView)
{
	m_matView = rmtxView;

}
void CFXManager::SetViewpos(const D3DXVECTOR3& rposView)
{
	D3DXVECTOR4 pos = rposView;
	m_viewPos = pos;
}
void  CFXManager::SetProj(const D3DXMATRIX& rmtxProj)
{
	m_matProj = rmtxProj;

}
void CFXManager::SetScale(const D3DXMATRIX& rmtxScale)
{
	m_matScaleReverse = rmtxScale;
	D3DXMatrixInverse(&m_matScaleReverse, NULL, &m_matWorld);
	D3DXMatrixTranspose(&m_matScaleReverse, &m_matScaleReverse);
}
void CFXManager::SetParamToEffect()
{
	
	m_pEffect->SetMatrix(m_hWorldMat, &m_matWorld);
	m_pEffect->SetMatrix(m_hViewMat, &m_matView);
	m_pEffect->SetMatrix(m_hProjMat, &m_matProj);
	m_pEffect->SetMatrix(m_hMatScaleReverse, &m_matScaleReverse);
	m_pEffect->SetVector(m_hLightDiffuse, &m_LightDiffuse);
	m_pEffect->SetVector(m_hDirectLight, &m_DirectLight);
	m_pEffect->SetVector(m_hMatDiffuse, &m_MatDiffuse);
	m_pEffect->SetVector(m_hMatAmbient, &m_MatAmbient);
	m_pEffect->SetVector(m_hMatEmissive, &m_MatEmissive);
	m_pEffect->SetVector(m_hMatSpecula, &m_MatSpecula);
	m_pEffect->SetFloat(m_hMatPower, m_MatPower);
	m_pEffect->SetVector(m_hViewvec, &m_Viewvec);
	m_pEffect->SetVector(m_hviewPos, &m_viewPos);
	m_pEffect->SetInt(m_hnumLights, m_numLights);
	m_pEffect->SetTexture(m_hCubeMap, m_pCubeTexture);
	m_pEffect->SetValue(m_hLights, m_lightArray, sizeof(m_lightArray));
}	
// �`��̊J�n��錾����
HRESULT CFXManager::Begin()
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// �f�o�C�X���
	
	// �v���O���}�u���V�F�[�_�̃e�N�j�b�N��ݒ�
	m_pEffect->SetTechnique(m_pTechnique);

	// �V�F�[�_�̊J�n��錾
	UINT Tmp;
	m_pEffect->Begin(&Tmp, 0);
	m_bBegin = true;
	return S_OK;
}


// �p�X�̊J�n��錾����
HRESULT CFXManager::BeginPass()
{
	m_pEffect->BeginPass(0);	// Z�l�v�Z��1�p�X
	return S_OK;
}
// �p�X�̏I����錾����
HRESULT CFXManager::EndPass()
{
	m_pEffect->EndPass();
	return S_OK;
}
// �`��̏I����錾����
HRESULT CFXManager::End()
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// �f�o�C�X���
	m_pEffect->End();

	// �Œ�@�\�ɖ߂�
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
	m_bBegin = false;
	return S_OK;
}