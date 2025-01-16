//===================================================
//
// DirectX�̃����_���[���� [renderer.cpp]
// Author : Ibuki Okusada
//
//===================================================
#include "renderer.h"
#include "debugproc.h"
#include "manager.h"
#include "input.h"
#include "task_manager.h"
#include "object_manager.h"
#include "object.h"
#include "camera.h"
#include "input_keyboard.h"
#include "effekseerControl.h"
#include "edit_manager.h"
#include "postprocess.h"
namespace
{
	const D3DXCOLOR ALPHACOLOR = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);	// �}���`�^�[�Q�b�g�����x
	const D3DXCOLOR NONE_ALPHACOLOR = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	const D3DXVECTOR2 NORMALSIZE = D3DXVECTOR2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);
	const D3DXVECTOR2 MINISIZE = NORMALSIZE * 1.0f;
}

//===================================================
// �R���X�g���N�^
//===================================================
CRenderer::CRenderer()
{
	// �l�̃N���A
	m_pD3D = NULL;			//Direct3D�I�u�W�F�N�g�̃|�C���^
	m_pD3DDevice = NULL;	//Direct3D�f�o�C�X�ւ̃|�C���^
	pTexture = NULL;
	m_pRenderTextureSurface = NULL;
	m_pZSurface = NULL;
	m_pOrgSurface = NULL;
	m_pOrgZBuffer = NULL;

	// �}���`�^�[�Q�b�g�����_�����O�p
	m_MultitargetInfo = MultiTargetInfo();
}

//===================================================
// �f�X�g���N�^
//===================================================
CRenderer::~CRenderer()
{

}

//===================================================
// ����������
//===================================================
HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;	// �f�B�X�v���C���[�h
	D3DPRESENT_PARAMETERS d3dpp;	// �v���[���e�[�V�����p�����[�^

	// DirectX3D�I�u�W�F�N�g�̍쐬
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (m_pD3D == NULL)
	{
		return E_FAIL;
	}

	// ���݂̃f�B�X�v���C���[�h���擾
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// �f�o�C�X�̃v���[���e�[�V�����p�����[�^�̐ݒ�
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.BackBufferWidth = SCREEN_WIDTH;						// �Q�[����ʃT�C�Y(��)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;						// �Q�[����ʃT�C�Y(����)
	d3dpp.BackBufferFormat = d3ddm.Format;						// �o�b�N�o�b�t�@�[�̌`��
	d3dpp.BackBufferCount = 1;									// �o�b�N�o�b�t�@�̐�
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					// �_�u���o�b�t�@�̐؂�ւ�
	d3dpp.EnableAutoDepthStencil = TRUE;						// �f�v�X�o�b�t�@�ƃX�e���V���o�b�t�@���쐬
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;					// �f�v�X�o�b�t�@�Ƃ���16bit���g��
	d3dpp.Windowed = bWindow;									// �E�C���h�E���[�h
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	// ���t���b�V�����[�g
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// �C���^�[�o��
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;					// �f�v�X�o�b�t�@�Ƃ���16bit���g��	

	// Direct3D�f�o�C�X�̍쐬
	if (FAILED(m_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_pD3DDevice)))
	{
		if (FAILED(m_pD3D->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&m_pD3DDevice)))
		{
			if (FAILED(m_pD3D->CreateDevice(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&m_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	//�����_�[�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//�T���v���[�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	//�e�N�X�`���X�e�[�W�X�e�[�g
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	// �O���[�o�������̐ݒ�
	m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(0, 0, 0));

	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);  // Z�o�b�t�@�L��
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);  // ���C�g�L����
	m_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);  // �X�y�L�����L��

	// �}���`�^�[�Q�b�g�����_���[�̏�����
	InitMTRender();

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CRenderer::Uninit(void)
{
	// �^�X�N�̔p��
	//CTaskManager::GetInstance()->Uninit();

	// �I�u�W�F�N�g�̔p��
	//CObjectManager::GetInstance()->Uninit();

	for (int cnt = 0; cnt < 2; cnt++)
	{
		if (m_Multitarget.pTextureMT[cnt] != nullptr)
		{
			m_Multitarget.pTextureMT[cnt]->Release();
			m_Multitarget.pTextureMT[cnt] = nullptr;
		}
		if (m_Multitarget.pRenderMT[cnt] != nullptr)
		{
			m_Multitarget.pRenderMT[cnt]->Release();
			m_Multitarget.pRenderMT[cnt] = nullptr;
		}
	}

	if (m_Multitarget.pZBuffMT != nullptr)
	{
		m_Multitarget.pZBuffMT->Release();
		m_Multitarget.pZBuffMT = nullptr;
	}

	if (m_Multitarget.pVtxBuff != nullptr)
	{
		m_Multitarget.pVtxBuff->Release();
		m_Multitarget.pVtxBuff = nullptr;
	}

	// Direct3D�f�o�C�X�̔p��
	if (m_pD3DDevice != NULL)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	// Direct3D�I�u�W�F�N�g�̔p��
	if (m_pD3D != NULL)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}

	if (pTexture != NULL)
	{
		pTexture->Release();
		pTexture = NULL;
	}

	if (m_pRenderTextureSurface != NULL)
	{
		m_pRenderTextureSurface->Release();
		m_pRenderTextureSurface = NULL;
	}

	if (m_pZSurface != NULL)
	{
		m_pZSurface->Release();
		m_pZSurface = NULL;
	}

	if (m_pOrgSurface != nullptr)
	{
		m_pOrgSurface->Release();
		m_pOrgSurface = nullptr;
	}

	if (m_pOrgZBuffer != nullptr)
	{
		m_pOrgZBuffer->Release();
		m_pOrgZBuffer = nullptr;
	}

	CPostprocess::Release();
}

//===================================================
// �X�V����
//===================================================
void CRenderer::Update(void)
{
#if _DEBUG	// �f�o�b�O��
	CInputKeyboard *pKey = CInputKeyboard::GetInstance();

	if (pKey->GetTrigger(DIK_F2) == true)
	{
		// ���C���[�t���[���ݒ�
		SetWire(m_bWire ? false : true);
	}
#endif
}

//===================================================
// �`�揈��
//===================================================
void CRenderer::Draw(void)
{
	CDebugProc *pDebugProc = CManager::GetInstance()->GetDebugProc();

	// ��ʃN���A
	m_pD3DDevice->Clear(
		0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0),
		1.0f,
		0);

	// �J�����̐ݒ�
	CCamera* pCamera = CManager::GetInstance()->GetCamera();

	// �ؑ֗p
	LPDIRECT3DTEXTURE9 pTextureWk;	// �ؑ֗p�p�e�N�X�`��
	LPDIRECT3DSURFACE9 pRenderWk;	// �e�N�X�`�������_�����O�p�C���^�[�t�F�[�X

	// �`��J�n
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{// �`�悪���������ꍇ

		D3DVIEWPORT9 viewportDef;	//�r���[�|�[�g�ۑ�

		// ���݂̃r���[�|�[�g���擾
		m_pD3DDevice->GetViewport(&viewportDef);

		// �ۑ��p�o�b�t�@
		LPDIRECT3DSURFACE9 pRenderDef = nullptr, pZBuffDef = nullptr;
		D3DXMATRIX mtxView, mtxProjection;

		// �f�t�H���g�̃����_���[�^�[�Q�b�g���擾
		GetDefaultRenderTarget(&pRenderDef, &pZBuffDef, &mtxView, &mtxProjection);

		// �^�[�Q�b�g�ؑ�
		CManager::GetInstance()->GetRenderer()->ChangeTarget(pCamera->GetPositionV(), pCamera->GetPositionR(), D3DXVECTOR3(0.0f, 1.0f, 0.0f));

		// �e�N�X�`��[0]�̃N���A
		m_pD3DDevice->Clear(
			0, nullptr,
			(D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
			D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f),
			1.0f,
			0);

		// �I�u�W�F�N�g�̕`��
		CObjectManager::GetInstance()->Draw();

		//if (CEditManager::GetInstance() == nullptr)
		{
			pCamera->SetCamera();
			CEffekseer::GetInstance()->Draw();
		}

		// �t�B�[�h�o�b�N�G�t�F�N�g�Ƀe�N�X�`��[1]��\��t���ĕ`��
		float multidef = m_MultitargetInfo.fMulti - m_MultitargetInfo.fStartMulti;
		float multi = m_MultitargetInfo.fStartMulti + multidef * m_MultitargetInfo.fTimer;
		float alphadef = m_MultitargetInfo.fColAlpha - m_MultitargetInfo.fStartColAlpha;
		float alpha = m_MultitargetInfo.fStartColAlpha + alphadef * m_MultitargetInfo.fTimer;
		DrawMultiTargetScreen(1, D3DXCOLOR(ALPHACOLOR.r, ALPHACOLOR.g, ALPHACOLOR.b, alpha), MINISIZE * multi);

		// �J�����̐ݒ�
		CManager::GetInstance()->GetCamera()->SetCamera();
		// �����_�[�^�[�Q�b�g�����Ƃɖ߂�
		CManager::GetInstance()->GetRenderer()->ChangeRendertarget(pRenderDef, pZBuffDef, mtxView, mtxProjection);

		DrawMultiTargetScreen(0, NONE_ALPHACOLOR, NORMALSIZE);

		// �e�N�X�`��0��1�̐ؑ�
		pTextureWk = m_Multitarget.pTextureMT[0];
		m_Multitarget.pTextureMT[0] = m_Multitarget.pTextureMT[1];
		m_Multitarget.pTextureMT[1] = pTextureWk;

		pRenderWk = m_Multitarget.pRenderMT[0];
		m_Multitarget.pRenderMT[0] = m_Multitarget.pRenderMT[1];
		m_Multitarget.pRenderMT[1] = pRenderWk;

		CManager::GetInstance()->GetCamera()->SetCamera();

		CPostprocess::GetInstance()->Draw();
		CObjectManager::GetInstance()->DrawAllnotShader();
		// �f�o�b�O�\��
		if (pDebugProc != NULL)
		{
			pDebugProc->Draw();
		}

		// �r���[�|�[�g�����ɖ߂�
		m_pD3DDevice->SetViewport(&viewportDef);

		// �`��I��
		m_pD3DDevice->EndScene();
	}

	// �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);

	// �}���`�^�[�Q�b�g����
	if (m_MultitargetInfo.bActive) {
		SetMultiTarget();
	}
}

//===================================================
// �f�o�C�X�̎擾
//===================================================
LPDIRECT3DDEVICE9 CRenderer::GetDevice(void)
{
	return m_pD3DDevice;
}

//===================================================
// ���C���[�t���[���ݒ�
//===================================================
void CRenderer::SetWire(bool bWire)
{
	m_bWire = bWire;
	//�`�惂�[�h�̕ύX
	switch (m_bWire)
	{
	case true:
		// ���C���[�t���[���`��ɕύX
		m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		break;
	case false:
		// �\���b�h�`��ɕύX
		m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		break;
	}
}

//==========================================================================
// �}���`�^�[�Q�b�g�����_���[�̏�����
//==========================================================================
void CRenderer::InitMTRender()
{
	// �ۑ��p�o�b�t�@
	LPDIRECT3DSURFACE9 pRenderDef, pZBuffDef;

	// ���݂̉�ʕ��擾
	float screen_width = SCREEN_WIDTH;
	float screen_height = SCREEN_HEIGHT;

	// �����_�����O�^�[�Q�b�g�p�e�N�X�`���̐���
	for (int i = 0; i < 2; i++)
	{
		m_pD3DDevice->CreateTexture(screen_width, screen_height,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_X8R8G8B8,
			D3DPOOL_DEFAULT,
			&m_Multitarget.pTextureMT[i],
			NULL);

		// �e�N�X�`�������_�����O�p�C���^�[�t�F�[�X�̐���
		m_Multitarget.pTextureMT[i]->GetSurfaceLevel(0, &m_Multitarget.pRenderMT[i]);
	}

	// �e�N�X�`�������_�����O�pZ�o�b�t�@�̐���
	m_pD3DDevice->CreateDepthStencilSurface(
		screen_width, screen_height,
		D3DFMT_D24S8,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&m_Multitarget.pZBuffMT,
		NULL);

	// ���݂̃����_�����O�^�[�Q�b�g���擾(�ۑ�)
	m_pD3DDevice->GetRenderTarget(0, &pRenderDef);

	// ���݂�Z�o�b�t�@���擾(�ۑ�)
	m_pD3DDevice->GetDepthStencilSurface(&pZBuffDef);

	// �����_�����O�^�[�Q�b�g�𐶐������e�N�X�`���ɐݒ�
	m_pD3DDevice->SetRenderTarget(0, m_Multitarget.pRenderMT[0]);

	// Z�o�b�t�@�𐶐�����Z�o�b�t�@�ɐݒ�
	m_pD3DDevice->SetDepthStencilSurface(m_Multitarget.pZBuffMT);

	// �����_�����O�^�[�Q�b�g�p�̃e�N�X�`���̃N���A
	m_pD3DDevice->Clear(
		0, nullptr,
		(D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f),
		1.0f,
		0);


	// �����_�����O�^�[�Q�b�g�����ɖ߂�
	m_pD3DDevice->SetRenderTarget(0, pRenderDef);

	// Z�o�b�t�@�����ɖ߂�
	m_pD3DDevice->SetDepthStencilSurface(pZBuffDef);

	// ���u�������_�����O�^�[�Q�b�g�EZ�o�b�t�@�̊J���i��������Ȃ��ƃ��������[�N���N����j
	if (pRenderDef != nullptr)
	{
		pRenderDef->Release();
		pRenderDef = nullptr;
	}
	if (pZBuffDef != nullptr)
	{
		pZBuffDef->Release();
		pZBuffDef = nullptr;
	}

	// �e�N�X�`���p�r���[�|�[�g�̐ݒ�
	m_Multitarget.viewportMT.X = 0;						// �`�悷���ʂ̍���X���W
	m_Multitarget.viewportMT.Y = 0;						// �`�悷���ʂ̍���Y���W
	m_Multitarget.viewportMT.Width = screen_width;		// �`�悷���ʂ̕�
	m_Multitarget.viewportMT.Height = screen_height;	// �`�悷���ʂ̍���
	m_Multitarget.viewportMT.MinZ = 0.0f;
	m_Multitarget.viewportMT.MaxZ = 1.0f;


	m_pD3DDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_Multitarget.pVtxBuff,
		nullptr);



	// ���_���ւ̃|�C���^
	VERTEX_2D* pVtx;

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_Multitarget.pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// �X�N���[���\����Ԃɉ����Ēʏ�̃T�C�Y�ύX
	D3DXVECTOR2 size = D3DXVECTOR2(SCREEN_WIDTH* 0.5f, SCREEN_HEIGHT * 0.5f);


	// ���_���W�̐ݒ�
	pVtx[0].pos.x = (screen_width * 0.5f) - size.x;
	pVtx[0].pos.y = (screen_height * 0.5f) - size.y;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = (screen_width * 0.5f) + size.x;
	pVtx[1].pos.y = (screen_height * 0.5f) - size.y;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = (screen_width * 0.5f) - size.x;
	pVtx[2].pos.y = (screen_height * 0.5f) + size.y;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = (screen_width * 0.5f) + size.x;
	pVtx[3].pos.y = (screen_height * 0.5f) + size.y;
	pVtx[3].pos.z = 0.0f;

	// rhw�̐ݒ�
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// ���_�J���[�̐ݒ�
	pVtx[0].col = ALPHACOLOR;
	pVtx[1].col = ALPHACOLOR;
	pVtx[2].col = ALPHACOLOR;
	pVtx[3].col = ALPHACOLOR;

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// ���_�o�b�t�@���A�����b�N���b�N
	m_Multitarget.pVtxBuff->Unlock();

}

//==========================================================================
// �f�t�H���g�̃����_�[�^�[�Q�b�g�擾
//==========================================================================
void CRenderer::GetDefaultRenderTarget(LPDIRECT3DSURFACE9* pRender, LPDIRECT3DSURFACE9* pZBuff, D3DXMATRIX* viewport, D3DXMATRIX* projection)
{
	// ���݂̃����_�����O�^�[�Q�b�g���擾(�ۑ�)
	m_pD3DDevice->GetRenderTarget(0, &(*pRender));

	// ���݂�Z�o�b�t�@���擾(�ۑ�)
	m_pD3DDevice->GetDepthStencilSurface(&(*pZBuff));

	m_pD3DDevice->GetTransform(D3DTS_VIEW, &(*viewport));
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &(*projection));
}

//==========================================================================
// �����_�����O�^�[�Q�b�g�̐ؑ�
//==========================================================================
void CRenderer::ChangeRendertarget(LPDIRECT3DSURFACE9 pRender, LPDIRECT3DSURFACE9 pZBuff, D3DXMATRIX viewport, D3DXMATRIX projection)
{
	// �����_�����O�^�[�Q�b�g�𐶐������e�N�X�`���ɐݒ�
	m_pD3DDevice->SetRenderTarget(0, pRender);

	// Z�o�b�t�@�𐶐�����Z�o�b�t�@�ɐݒ�
	m_pD3DDevice->SetDepthStencilSurface(pZBuff);

	// �e�N�X�`�������_�����O�p�̃r���[�|�[�g��ݒ�
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &viewport);

	// �e�N�X�`�������_�����O�p�̃v���W�F�N�V�����}�g���b�N�X��ݒ�
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &projection);
}

//==========================================================================
// �����_�����O�^�[�Q�b�g�̐ؑ�
//==========================================================================
void CRenderer::ChangeTarget(const D3DXVECTOR3& posV, const D3DXVECTOR3& posR, const D3DXVECTOR3& vecU)
{
	D3DXMATRIX mtxview, mtxProjection;

	// �����_�����O�^�[�Q�b�g�𐶐������e�N�X�`���ɐݒ�
	m_pD3DDevice->SetRenderTarget(0, m_Multitarget.pRenderMT[0]);

	// Z�o�b�t�@�𐶐�����Z�o�b�t�@�ɐݒ�
	m_pD3DDevice->SetDepthStencilSurface(m_Multitarget.pZBuffMT);

	// �e�N�X�`�������_�����O�p�̃r���[�|�[�g��ݒ�
	m_pD3DDevice->SetViewport(&m_Multitarget.viewportMT);


	// �v���W�F�N�V�����}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxProjection);

	// �v���W�F�N�V�����}�g���b�N�X�̍쐬
	D3DXMatrixPerspectiveFovLH(
		&mtxProjection,
		D3DXToRadian(45.0f),	// ����p
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,	// �A�X�y�N�g��
		10.0f,		// ��O�̐���
		100000.0f);	// ���s���̐���

	// �v���W�F�N�V�����}�g���b�N�X�̐ݒ�
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &mtxProjection);

	// �r���[�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxview);

	// �r���[�}�g���b�N�X�̍쐬
	D3DXMatrixLookAtLH(
		&mtxview,
		&posV,	// ���_
		&posR,	// �����_
		&vecU);	// ������x�N�g��

	// �r���[�}�g���b�N�X�̐ݒ�
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &mtxview);
}

//==========================================================================
// �}���`�^�[�Q�b�g��ʂ̕`�攻��
//==========================================================================
void CRenderer::SetEnableDrawMultiScreen(float fGoalAlpha, float fGoalMulti, float fTimer)
{
	// �p�����[�^�[�̐ݒ�
	m_MultitargetInfo.fTimer = 0.0f;
	if (fTimer != 0.0f) { m_MultitargetInfo.fAddTimer = 1.0f / fTimer; }	// 1�t���[�����Ƃ̉��Z�ʐݒ�
	else { m_MultitargetInfo.fAddTimer = 1.0f; }	// 0�Ȃ瑦
	m_MultitargetInfo.fStartColAlpha = m_MultitargetInfo.fColAlpha;
	m_MultitargetInfo.fColAlpha = fGoalAlpha;
	m_MultitargetInfo.fStartMulti = m_MultitargetInfo.fMulti;
	m_MultitargetInfo.fMulti = fGoalMulti;
	m_MultitargetInfo.bActive = true;

	return;
}

//==========================================================================
// �}���`�^�[�Q�b�g��ʂ̕`�撲��
//==========================================================================
void CRenderer::SetMultiTarget()
{
	m_MultitargetInfo.fTimer += m_MultitargetInfo.fAddTimer;

	if (m_MultitargetInfo.fTimer >= 1.0f) {
		m_MultitargetInfo.bActive = false;
	}
}

//==========================================================================
// �}���`�^�[�Q�b�g��ʂ̕`��
//==========================================================================
void CRenderer::DrawMultiTargetScreen(int texIdx, const D3DXCOLOR& col, const D3DXVECTOR2& size)
{
	// ���_���ւ̃|�C���^
	VERTEX_2D* pVtx;

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_Multitarget.pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos.x = SCREEN_WIDTH*0.5f - size.x;
	pVtx[0].pos.y = SCREEN_HEIGHT * 0.5f - size.y;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = SCREEN_WIDTH * 0.5f + size.x;
	pVtx[1].pos.y = SCREEN_HEIGHT * 0.5f - size.y;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = SCREEN_WIDTH * 0.5f - size.x;
	pVtx[2].pos.y = SCREEN_HEIGHT * 0.5f + size.y;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = SCREEN_WIDTH * 0.5f + size.x;
	pVtx[3].pos.y = SCREEN_HEIGHT * 0.5f + size.y;
	pVtx[3].pos.z = 0.0f;

	// ���_�J���[�̐ݒ�
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// ���_�o�b�t�@���A�����b�N���b�N
	m_Multitarget.pVtxBuff->Unlock();

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	m_pD3DDevice->SetStreamSource(0, m_Multitarget.pVtxBuff, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	m_pD3DDevice->SetFVF(FVF_VERTEX_2D);

	// �e�N�X�`���̐ݒ�
	m_pD3DDevice->SetTexture(0, m_Multitarget.pTextureMT[texIdx]);

	// �|���S���̕`��
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}