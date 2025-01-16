//============================================================
//
//	�V�F�[�_�[�Ǘ��֐� [Postprocess.cpp]
//	Author�F�O�엳�V��
//
//============================================================
//============================================================
//	�C���N���[�h�t�@�C��
//============================================================
#include "Postprocess.h"
#include "manager.h"
#include "renderer.h"
//============================================================
//	�萔�錾
//============================================================
namespace
{
	const char* EFFECT_BRIGHT = "code\\shader\\BrightCheck.fx";	// �V�F�[�_�[�̃G�t�F�N�g�t�@�C��
	const char* EFFECT_GAUSS = "code\\shader\\Gauss.fx";	// �V�F�[�_�[�̃G�t�F�N�g�t�@�C��
	const char* EFFECT_ABERRATION = "code\\shader\\aberration.fx";	// �V�F�[�_�[�̃G�t�F�N�g�t�@�C��
	const D3DXCOLOR LIGHT_COLOR = D3DXVECTOR4(0.6f, 0.6f, 0.7f, 1.0f);
	const D3DXVECTOR4 LIGHT_VEC = D3DXVECTOR4(-0.5f, -0.87f, 0.05f, 0.0f);
}
//************************************************************
//	�ÓI�����o�ϐ��錾
//************************************************************
CPostprocess* CPostprocess::m_pShader = nullptr;	// �V�F�[�_�[���

//============================================================
//	�R���X�g���N�^
//============================================================
CPostprocess::CPostprocess()
{

}

//============================================================
//	�f�X�g���N�^
//============================================================
CPostprocess::~CPostprocess()
{

}

//============================================================
//	����������
//============================================================
HRESULT CPostprocess::Init(void)
{
	
	// �ϐ���錾
	D3DCAPS9 caps;	// �n�[�h�E�F�A�@�\
	HRESULT hr;		// �ُ�I���̊m�F�p

	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// �f�o�C�X���
	LPD3DXBUFFER pError = nullptr;	// �R���p�C���G���[���
	

	// �����o�ϐ���������

	// �n�[�h�E�F�A�@�\���擾
	pDevice->GetDeviceCaps(&caps);

	if (caps.VertexShaderVersion >= D3DVS_VERSION(1, 1)
		&& caps.PixelShaderVersion >= D3DPS_VERSION(2, 0))
	{ // ���_�E�s�N�Z���V�F�[�_�̃o�[�W�������g�p�\�ȏꍇ

		Loadshader((char*)EFFECT_BRIGHT, &m_pBright);
		Loadshader((char*)EFFECT_GAUSS, &m_pGauss);
		Loadshader((char*)EFFECT_ABERRATION, &m_pAberration);
	}
	else
	{ // �o�[�W�������g�p�s�ȏꍇ

		// �Â��o�[�W�����ɂ�鎸�s��Ԃ�
		assert(false);
		return E_FAIL;
	}

	//�e�N�X�`������
	pDevice->CreateTexture(
		(SCREEN_WIDTH),          // �e�N�X�`���̕�
		(SCREEN_HEIGHT),          // �e�N�X�`���̍���
		0,              // �~�b�v�}�b�v ���x��
		D3DUSAGE_RENDERTARGET,
		D3DFMT_X8R8G8B8, // �t�H�[�}�b�g
		D3DPOOL_DEFAULT, // �v�[��
		&m_pGaussTex,  // �e�N�X�`�� �I�u�W�F�N�g
		NULL);
	//�T�[�t�F�C�X�̐ݒ�
	m_pGaussTex->GetSurfaceLevel(0, &m_pGaussSurface);
	D3DXCreateSprite(pDevice, &m_pSprite);// �X�v���C�g�쐬



	pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&pVtxBuff,
		nullptr);



	// ���_���ւ̃|�C���^
	VERTEX_2D* pVtx;

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// �X�N���[���\����Ԃɉ����Ēʏ�̃T�C�Y�ύX
	D3DXVECTOR2 size = D3DXVECTOR2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);


	// ���_���W�̐ݒ�
	pVtx[0].pos.x = (SCREEN_WIDTH * 0.5f) - size.x;
	pVtx[0].pos.y = (SCREEN_HEIGHT * 0.5f) - size.y;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = (SCREEN_WIDTH * 0.5f) + size.x;
	pVtx[1].pos.y = (SCREEN_HEIGHT * 0.5f) - size.y;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = (SCREEN_WIDTH * 0.5f) - size.x;
	pVtx[2].pos.y = (SCREEN_HEIGHT * 0.5f) + size.y;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = (SCREEN_WIDTH * 0.5f) + size.x;
	pVtx[3].pos.y = (SCREEN_HEIGHT * 0.5f) + size.y;
	pVtx[3].pos.z = 0.0f;

	// rhw�̐ݒ�
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// ���_�J���[�̐ݒ�
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// ���_�o�b�t�@���A�����b�N���b�N
	pVtxBuff->Unlock();
	// ������Ԃ�
	return S_OK;
}
//============================================================
//	�I������
//============================================================
void CPostprocess::Uninit(void)
{
	SAFE_RELEASE(m_pBright);
	SAFE_RELEASE(m_pGauss);
}
//============================================================
//	�`�揈��
//============================================================
void CPostprocess::Draw(void)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// �f�o�C�X���
	//�o�b�N�o�b�t�@�̊���l���L�^
	LPDIRECT3DSURFACE9 pBackBuffer;
	pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	//����P�x�̔�������

	m_pAberration->Begin(NULL, 0);
	m_pAberration->BeginPass(0);
	m_pAberration->SetTechnique("ChromaticAberration");
	Rendering(CManager::GetInstance()->GetRenderer()->GetTextureMT(0), 0xffffffff);
	m_pAberration->EndPass();
	m_pAberration->End();
	pDevice->SetRenderTarget(0, m_pGaussSurface);//�����_�[�^�[�Q�b�g�؂�ւ�

	m_pBright->Begin(NULL, 0);
	m_pBright->BeginPass(0);
	m_pBright->SetTechnique("brightcheck");
	Rendering(CManager::GetInstance()->GetRenderer()->GetTextureMT(0), 0xffffffff);
	m_pBright->EndPass();
	m_pBright->End();

	// �K�E�X�ڂ�����K�p
	m_pGauss->Begin(NULL, 0);
	m_pGauss->BeginPass(0);
	m_pGauss->SetTechnique("GaussianBlur");
	m_pGauss->SetTexture("InputTexture", m_pGaussTex);
	Rendering(m_pGaussTex, 0xffffffff);
	m_pGauss->EndPass();
	m_pGauss->End();
	// �Œ�@�\�ɖ߂�
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
	pDevice->SetRenderTarget(0, pBackBuffer);
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	Rendering(m_pGaussTex, 0xffffffff);
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
void CPostprocess::Rendering(LPDIRECT3DTEXTURE9 pTex, D3DXCOLOR col)
{
	// �|�C���^��錾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();	// �f�o�C�X���
	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, pVtxBuff, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, pTex);

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
//============================================================
//	��������
//============================================================
CPostprocess* CPostprocess::Create(void)
{
	// �C���X�^���X�g�p��
	assert(m_pShader == nullptr);

	// �g�D�[���V�F�[�_�[�̐���
	m_pShader = DEBUG_NEW CPostprocess;
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
CPostprocess* CPostprocess::GetInstance(void)
{
	if (m_pShader == nullptr)
	{
		m_pShader = CPostprocess::Create();
	}

	// �|�C���^��Ԃ�
	return m_pShader;
}

//============================================================
//	�j������
//============================================================
void CPostprocess::Release(void)
{
	// �g�D�[���V�F�[�_�[�̏I��
	if (m_pShader != nullptr)
	{
		m_pShader->Uninit();
	}

	// �������J��
	SAFE_DELETE(m_pShader);
}


//�V�F�[�_�[�t�@�C���ǂݍ���
HRESULT  CPostprocess::Loadshader(char* path, LPD3DXEFFECT* pEffect)
{

	ID3DXBuffer* pError = NULL;
	CRenderer* pRenderer = CManager::GetInstance()->GetRenderer();
	LPDIRECT3DDEVICE9 pDevice; //�f�o�C�X�̃|�C���^
	pDevice = pRenderer->GetDevice();
	HRESULT hr = D3DXCreateEffectFromFile(
		pDevice,				// �f�o�C�X�ւ̃|�C���^
		path,			// fx�t�@�C���̃p�X
		NULL,                  // �v���v���Z�b�T��`
		NULL,                  // �v���v���Z�b�T��`�t�@�C��
		D3DXSHADER_DEBUG,                     // �t���O
		NULL,                  // �J�X�^���G�t�F�N�g�v�[��
		pEffect,              // �ǂݍ��܂ꂽ�G�t�F�N�g
		&pError                // �G���[���b�Z�[�W
	);
	if (pError)
	{
		OutputDebugStringA((LPCSTR)pError->GetBufferPointer());
		//�f�o�b�O�R���\�[���ɕ\������
		MessageBoxA(NULL, (LPCSTR)pError->GetBufferPointer(), "Shader Error", MB_OK);
	}
	return hr;
}