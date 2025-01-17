//==========================================================
//
// X�t�@�C�����f���̏����S�� [objectX.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "main.h"
#include "objectX.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "Xfile.h"
#include "fxmanager.h"
#include "edit_manager.h"
// �ÓI�����o�ϐ��錾
Clist<CObjectX*> CObjectX::m_List = {};

//==========================================================
// �R���X�g���N�^
//==========================================================
CObjectX::CObjectX(int nPriority) : CObject(nPriority)
{
	m_nIdxModel = -1;
	m_bEnableCollision = true;
	m_fShadowHeight = 0.0f;
	m_ColMulti = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_AddCol = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_scale = VECTOR3_ZERO;
	m_pos = VECTOR3_ZERO;
	m_rot = VECTOR3_ZERO;
	m_bHit = false;
	m_bHitOld = false;
	m_bShadow = true;
	SetDrawShader(true);
	m_List.Regist(this);
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CObjectX::~CObjectX()
{

}

//==========================================================
//���f���̏���������
//==========================================================
HRESULT CObjectX::Init(void)
{
	//�e��ϐ��̏�����
	m_Type = TYPE_NORMAL;
	m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	return S_OK;
}

//==========================================================
//���f���̏I������
//==========================================================
void CObjectX::Uninit(void)
{
	if (!GetDeath())
	{
		// ���X�g����O��
		ListOut();

		// �p��
		Release();
	}
}

//==========================================================
//���f���̍X�V����
//==========================================================
void CObjectX::Update(void)
{

}

//==========================================================
//���f���̕`�揈��
//==========================================================
void CObjectX::Draw(void)
{
	//Update();

	m_bHitOld = m_bHit;
	m_bHit = false;

	// �}�g���b�N�X�v�Z
	if (m_Type == TYPE_NORMAL)
	{
		CalWorldMtx();
	}
	else
	{
		Quaternion();
	}

	// �`��
	DrawOnry();

	// �e�̕`��
	DrawShadow();
}
//==========================================================
//���f���̕`�揈��
//==========================================================
void CObjectX::DrawOnShader(void)
{
	//Update();

	m_bHitOld = m_bHit;
	m_bHit = false;

	// �}�g���b�N�X�v�Z
	if (m_Type == TYPE_NORMAL)
	{
		CalWorldMtx();
	}
	else
	{
		Quaternion();
	}

	// �`��
	if (CEditManager::GetInstance() == nullptr)
	{
		//DrawOnry();
		DrawOnryShader();
	}
	else
	{
		DrawOnry();
	}

	// �e�̕`��
	DrawShadow();
}
//==========================================================
// ���[���h�}�g���b�N�X�v�Z
//==========================================================
void CObjectX::CalWorldMtx()
{
	D3DXMATRIX mtxRot, mtxTrans;			//�v�Z�p�}�g���b�N�X

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �X�P�[���̔��f
	D3DXMatrixScaling(&m_mtxscale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &m_mtxscale);

	//�����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);
}

//==========================================================
// �N�H�[�^�j�I�����[���h�}�g���b�N�X�v�Z
//==========================================================
void CObjectX::Quaternion()
{
	D3DXMATRIX mtxRot, mtxTrans;			//�v�Z�p�}�g���b�N�X
	D3DXQUATERNION qYaw, qPitch, qRoll;
	// ���[: Y����]
	D3DXQuaternionRotationAxis(&qYaw, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), m_rot.y);

	// �s�b�`: X����]
	D3DXQuaternionRotationAxis(&qPitch, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), m_rot.x);

	// ���[��: Z����]
	D3DXQuaternionRotationAxis(&qRoll, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), m_rot.z);
	D3DXQUATERNION qResult;
	D3DXQuaternionMultiply(&qResult, &qRoll, &qPitch);  // ���[���ƃs�b�`���|�����킹
	D3DXQuaternionMultiply(&qResult, &qResult, &qYaw);  // ����Ƀ��[���|�����킹

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �X�P�[���̔��f
	D3DXMatrixScaling(&m_mtxscale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &m_mtxscale);

	//�����𔽉f
	D3DXMatrixRotationQuaternion(&mtxRot, &qResult);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);
}
//==========================================================
// �`��̂�
//==========================================================
void CObjectX::DrawOnry()
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//�f�o�C�X�ւ̃|�C���^���擾
	CTexture* pTexture = CManager::GetInstance()->GetTexture();	// �e�N�X�`���ւ̃|�C���^
	CXFile* pModelFile = CManager::GetInstance()->GetModelFile();	// X�t�@�C�����̃|�C���^
	CXFile::SFileData* pFileData = pModelFile->SetAddress(m_nIdxModel);
	D3DMATERIAL9 matDef;					//���݂̃}�e���A���ۑ��p
	D3DXMATERIAL* pMat;						//�}�e���A���f�[�^�ւ̃|�C���^

	// ���K����L���ɂ���
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// ���f���g�p����Ă��Ȃ�
	if (pFileData == nullptr) { return; }

	//�A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 10);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//���݂̃}�e���A�����擾
	pDevice->GetMaterial(&matDef);

	//�}�e���A���f�[�^�ւ̃|�C���^���擾
	pMat = (D3DXMATERIAL*)pFileData->pBuffMat->GetBufferPointer();
	for (int nCntMat = 0; nCntMat < (int)pFileData->dwNumMat; nCntMat++)
	{
		D3DMATERIAL9 mat = pMat[nCntMat].MatD3D;
		mat.Diffuse.r = mat.Diffuse.r * m_ColMulti.r + m_AddCol.r;
		mat.Diffuse.g = mat.Diffuse.g * m_ColMulti.g + m_AddCol.g;
		mat.Diffuse.b = mat.Diffuse.b * m_ColMulti.b + m_AddCol.b;
		mat.Diffuse.a = mat.Diffuse.a * m_ColMulti.a + m_AddCol.a;
		mat.Ambient.r = mat.Ambient.r * m_ColMulti.r + m_AddCol.r;
		mat.Ambient.g = mat.Ambient.g * m_ColMulti.g + m_AddCol.g;
		mat.Ambient.b = mat.Ambient.b * m_ColMulti.b + m_AddCol.b;
		mat.Ambient.a = mat.Ambient.a * m_ColMulti.a + m_AddCol.a;

		//�}�e���A���̐ݒ�
		pDevice->SetMaterial(&mat);

		//�e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, pTexture->SetAddress(pFileData->pIdexTexture[nCntMat]));

		//���f��(�p�[�c)�̕`��
		pFileData->pMesh->DrawSubset(nCntMat);
	}

	//�ۑ����Ă����}�e���A����߂�
	pDevice->SetMaterial(&matDef);

	//�A���t�@�e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 255);

	// ���K���𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}
//==========================================================
// �V�F�[�_�[�`��
//==========================================================
void CObjectX::DrawOnryShader()
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//�f�o�C�X�ւ̃|�C���^���擾
	CFXManager* pFx = CFXManager::GetInstance();
	CTexture* pTexture = CManager::GetInstance()->GetTexture();	// �e�N�X�`���ւ̃|�C���^
	CXFile* pModelFile = CManager::GetInstance()->GetModelFile();	// X�t�@�C�����̃|�C���^
	CXFile::SFileData* pFileData = pModelFile->SetAddress(m_nIdxModel);
	D3DMATERIAL9 matDef;					//���݂̃}�e���A���ۑ��p
	D3DXMATERIAL* pMat;						//�}�e���A���f�[�^�ւ̃|�C���^

	// ���K����L���ɂ���
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// ���f���g�p����Ă��Ȃ�
	if (pFileData == nullptr) { return; }

	//�A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 10);

	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);
	pFx->SetMatrixWorld(m_mtxWorld);
	pFx->SetScale(m_mtxscale);
	//���݂̃}�e���A�����擾
	pDevice->GetMaterial(&matDef);

	//�}�e���A���f�[�^�ւ̃|�C���^���擾
	pMat = (D3DXMATERIAL*)pFileData->pBuffMat->GetBufferPointer();
	for (int nCntMat = 0; nCntMat < (int)pFileData->dwNumMat; nCntMat++)
	{
		D3DMATERIAL9 mat = pMat[nCntMat].MatD3D;
		mat.Diffuse.r = mat.Diffuse.r * m_ColMulti.r + m_AddCol.r;
		mat.Diffuse.g = mat.Diffuse.g * m_ColMulti.g + m_AddCol.g;
		mat.Diffuse.b = mat.Diffuse.b * m_ColMulti.b + m_AddCol.b;
		mat.Diffuse.a = mat.Diffuse.a * m_ColMulti.a + m_AddCol.a;
		
		//�}�e���A���̐ݒ�
		pDevice->SetMaterial(&mat);
		pFx->SetMaterial(mat);
		//�e�N�X�`���̐ݒ�
	
		pFx->SetParamToEffect();
		pFx->Begin();
		pFx->BeginPass();
		pDevice->SetTexture(0, pTexture->SetAddress(pFileData->pIdexTexture[nCntMat]));
		//���f��(�p�[�c)�̕`��
		pFileData->pMesh->DrawSubset(nCntMat);
		pFx->EndPass();
		pFx->End();
	}

	//�ۑ����Ă����}�e���A����߂�
	pDevice->SetMaterial(&matDef);

	//�A���t�@�e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 255);

	// ���K���𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}
//==========================================================
// �V���h�E�}�g���b�N�X�ł̉e�̕`��
//==========================================================
void CObjectX::DrawShadow()
{
	// �e�g�p���Ȃ�
	if (!m_bShadow) { return; }

	// ���f���g�p����Ă��Ȃ�
	CXFile* pModelFile = CManager::GetInstance()->GetModelFile();	// X�t�@�C�����̃|�C���^
	CXFile::SFileData* pFileData = pModelFile->SetAddress(m_nIdxModel);
	if (pFileData == nullptr) { return; }

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//�f�o�C�X�ւ̃|�C���^���擾
	D3DMATERIAL9 matDef;					//���݂̃}�e���A���ۑ��p
	D3DXMATERIAL* pMat;						//�}�e���A���f�[�^�ւ̃|�C���^
	D3DXMATRIX mtxShadow;
	D3DLIGHT9 light;
	D3DXVECTOR4 posLight;
	D3DXVECTOR3 pos, normal;
	D3DXPLANE plane;

	// ���C�g�̈ʒu��ݒ�
	pDevice->GetLight(0, &light);
	posLight = D3DXVECTOR4(-light.Direction.x, -light.Direction.y, -light.Direction.z, 0.0f);

	// ���ʏ���ݒ�
	D3DXVECTOR3 DefPos = D3DXVECTOR3(m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43);
	pos = D3DXVECTOR3(m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43);

	if (pos.y > DefPos.y) {	// ���݂̘r�̈ʒu��������
		pos = DefPos;
	}

	pos.y = m_fShadowHeight + 0.1f;
	normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXPlaneFromPointNormal(&plane, &pos, &normal);

	// �V���h�E�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxShadow);

	// �V���h�E�}�g���b�N�X�̍쐬
	D3DXMatrixShadow(&mtxShadow, &posLight, &plane);
	D3DXMatrixMultiply(&mtxShadow, &m_mtxWorld, &mtxShadow);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &mtxShadow);

	//���݂̃}�e���A�����擾
	pDevice->GetMaterial(&matDef);

	// ���f�������擾
	pFileData = pModelFile->SetAddress(m_nIdxModel);

	if (pFileData != NULL)
	{// �g�p����Ă���ꍇ
		//�}�e���A���f�[�^�ւ̃|�C���^���擾
		pMat = (D3DXMATERIAL*)pFileData->pBuffMat->GetBufferPointer();
		for (int nCntMat = 0; nCntMat < (int)pFileData->dwNumMat; nCntMat++)
		{
			D3DMATERIAL9 mat = pMat[nCntMat].MatD3D;
			mat.Ambient = { 0.0f, 0.0f, 0.0f, 0.4f };
			mat.Diffuse = { 0.0f, 0.0f, 0.0f, 0.4f };
			mat.Emissive = { 0.0f, 0.0f, 0.0f, 0.7f };
			mat.Specular = { 0.0f, 0.0f, 0.0f, 0.7f };
			pDevice->SetMaterial(&mat);

			//�e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, nullptr);

			//���f��(�p�[�c)�̕`��
			pFileData->pMesh->DrawSubset(nCntMat);
		}
	}

	//�ۑ����Ă����}�e���A����߂�
	pDevice->SetMaterial(&matDef);

	m_fShadowHeight = GetPosition().y;
}
void CObjectX::DrawShadowonShader()
{
	// �e�g�p���Ȃ�
	if (!m_bShadow) { return; }

	// ���f���g�p����Ă��Ȃ�
	CXFile* pModelFile = CManager::GetInstance()->GetModelFile();	// X�t�@�C�����̃|�C���^
	CXFile::SFileData* pFileData = pModelFile->SetAddress(m_nIdxModel);
	if (pFileData == nullptr) { return; }

	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();		//�f�o�C�X�ւ̃|�C���^���擾
	CFXManager* pFx = CFXManager::GetInstance();
	D3DMATERIAL9 matDef;					//���݂̃}�e���A���ۑ��p
	D3DXMATERIAL* pMat;						//�}�e���A���f�[�^�ւ̃|�C���^
	D3DXMATRIX mtxShadow;
	D3DLIGHT9 light;
	D3DXVECTOR4 posLight;
	D3DXVECTOR3 pos, normal;
	D3DXPLANE plane;

	// ���C�g�̈ʒu��ݒ�
	pDevice->GetLight(0, &light);
	posLight = D3DXVECTOR4(-light.Direction.x, -light.Direction.y, -light.Direction.z, 0.0f);

	// ���ʏ���ݒ�
	D3DXVECTOR3 DefPos = D3DXVECTOR3(m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43);
	pos = D3DXVECTOR3(m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43);

	if (pos.y > DefPos.y) {	// ���݂̘r�̈ʒu��������
		pos = DefPos;
	}

	pos.y = m_fShadowHeight + 0.1f;
	normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXPlaneFromPointNormal(&plane, &pos, &normal);

	// �V���h�E�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxShadow);

	// �V���h�E�}�g���b�N�X�̍쐬
	D3DXMatrixShadow(&mtxShadow, &posLight, &plane);
	D3DXMatrixMultiply(&mtxShadow, &m_mtxWorld, &mtxShadow);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &mtxShadow);
	pFx->SetMatrixWorld(mtxShadow);
	//���݂̃}�e���A�����擾
	pDevice->GetMaterial(&matDef);

	// ���f�������擾
	pFileData = pModelFile->SetAddress(m_nIdxModel);

	if (pFileData != NULL)
	{// �g�p����Ă���ꍇ
		//�}�e���A���f�[�^�ւ̃|�C���^���擾
		pMat = (D3DXMATERIAL*)pFileData->pBuffMat->GetBufferPointer();
		for (int nCntMat = 0; nCntMat < (int)pFileData->dwNumMat; nCntMat++)
		{
			D3DMATERIAL9 mat = pMat[nCntMat].MatD3D;
			mat.Ambient = { 0.0f, 0.0f, 0.0f, 0.7f };
			mat.Diffuse = { 0.0f, 0.0f, 0.0f, 0.7f };
			mat.Emissive = { 0.0f, 0.0f, 0.0f, 0.7f };
			mat.Specular = { 0.0f, 0.0f, 0.0f, 0.7f };
			pDevice->SetMaterial(&mat);
			pFx->SetMaterial(mat);
			pFx->SetParamToEffect();
			pFx->Begin();
			pFx->BeginPass();
			//�e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, nullptr);
		
			//���f��(�p�[�c)�̕`��
			pFileData->pMesh->DrawSubset(nCntMat);
			pFx->EndPass();
			pFx->End();
		}
	}

	//�ۑ����Ă����}�e���A����߂�
	pDevice->SetMaterial(&matDef);

	m_fShadowHeight = GetPosition().y;
}
//==========================================================
// ����
//==========================================================
CObjectX *CObjectX::Create(D3DXVECTOR3 pos,D3DXVECTOR3 rot, const char *pFileName, const int nPriority)
{
	CObjectX *pObjectX = NULL;
	CXFile *pModelFile = CManager::GetInstance()->GetModelFile();

	// �I�u�W�F�N�gX�̐���
	pObjectX = DEBUG_NEW CObjectX(nPriority);

	if (pObjectX != NULL)
	{// �����ł����ꍇ
		// ����������
		pObjectX->Init();

		// ���W
		pObjectX->SetPosition(pos);

		// ����
		pObjectX->SetRotation(rot);

		// ��ސݒ�
		pObjectX->SetType(TYPE_MODEL);

		// �ǂݍ��݊m�F
		pObjectX->BindFile(pModelFile->Regist(pFileName));
	}
	else
	{// �����Ɏ��s�����ꍇ
		return NULL;
	}

	return pObjectX;
}

//==========================================================
// ���f���̃t�@�C���ݒ�
//==========================================================
void CObjectX::BindFile(int nIdx)
{
	m_nIdxModel = nIdx;	//�g�p���郂�f���̐ݒ�
}

//==========================================================
// ���f���t�@�C���ǂݍ���
//==========================================================
void CObjectX::BindFile(const char* file)
{
	CXFile* pModelFile = CManager::GetInstance()->GetModelFile();
	m_nIdxModel = pModelFile->Regist(file);	//�g�p���郂�f���̐ݒ�
}

//==========================================================
// ���W�̐ݒ�
//==========================================================
void CObjectX::SetPosition(const D3DXVECTOR3& pos)
{
	m_pos = pos; 
}

//==========================================================
// �����̐ݒ�
//==========================================================
void CObjectX::SetRotation(const D3DXVECTOR3& rot)
{ 
	m_rot = rot;

	Adjust(&m_rot);
}

//==========================================================
// �X�P�[���̐ݒ�
//==========================================================
void CObjectX::SetScale(const D3DXVECTOR3& scale)
{
	m_scale = scale;
}

//==========================================================
// �����蔻��
//==========================================================
void CObjectX::SetRotSize(D3DXVECTOR3 &SetMax, D3DXVECTOR3 &SetMin, D3DXVECTOR3 vtxMax, D3DXVECTOR3 vtxMin, float fRot)
{
	//���ׂē����l���ɓ����
	SetMax.y = vtxMax.y;	//Max��y���W
	SetMin.y = vtxMin.y;	//Min��y���W

	//�����ɂ���ĕύX����
	if (fRot == D3DX_PI * 0.5f)
	{//������
		//�ő�l���f
		SetMax.x = vtxMax.z;	//x���W
		SetMax.z = -vtxMin.x;	//z���W

		//�ŏ��l���f
		SetMin.x = vtxMin.z;	//x���W
		SetMin.z = -vtxMax.x;	//z���W
	}
	else if (fRot == -D3DX_PI * 0.5f)
	{//�E����
		//�ő�l���f
		SetMax.x = -vtxMin.z;	//x���W
		SetMax.z = vtxMax.x;	//z���W

		//�ŏ��l���f
		SetMin.x = -vtxMax.z;	//x���W
		SetMin.z = vtxMin.x;	//z���W
	}
	else if(fRot == D3DX_PI || fRot == -D3DX_PI)
	{//���]���Ă���ꍇ
		//�����ȊO�̍ő�l���ŏ��l�ɂ���
		SetMax.x = -vtxMin.x;	//x���W
		SetMax.z = -vtxMin.z;	//z���W

		//�����ȊO�̍ŏ��l���ő�l�ɂ���
		SetMin.x = -vtxMax.x;	//x���W
		SetMin.z = -vtxMax.z;	//z���W
	}
	else
	{//����ȊO�̏ꍇ
		//�ő�l���f
		SetMax.x = vtxMax.x;	//x���W
		SetMax.z = vtxMax.z;	//z���W

		//�ŏ��l���f
		SetMin.x = vtxMin.x;	//x���W
		SetMin.z = vtxMin.z;	//z���W
	}
}

//==========================================================
// ���X�g����O��
//==========================================================
void CObjectX::ListOut(void)
{	
	m_List.Delete(this);
}

//==========================================================
// �ő�l�擾
//==========================================================
D3DXVECTOR3& CObjectX::GetVtxMax(void)
{
	CXFile* pModelFile = CManager::GetInstance()->GetModelFile();	// X�t�@�C�����̃|�C���^
	CXFile::SFileData* pFileData = pModelFile->SetAddress(m_nIdxModel);
	D3DXVECTOR3 max = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if (pFileData == nullptr) { return max; }

	max.x = pFileData->vtxMax.x * m_scale.x;
	max.y = pFileData->vtxMax.y * m_scale.y;
	max.z = pFileData->vtxMax.z * m_scale.z;

	return max;
}

//==========================================================
// �ŏ��l�擾
//==========================================================
D3DXVECTOR3& CObjectX::GetVtxMin(void)
{
	CXFile* pModelFile = CManager::GetInstance()->GetModelFile();	// X�t�@�C�����̃|�C���^
	CXFile::SFileData* pFileData = pModelFile->SetAddress(m_nIdxModel);

	D3DXVECTOR3 min = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if (pFileData == nullptr) { return min; }

	min.x = pFileData->vtxMin.x * m_scale.x;
	min.y = pFileData->vtxMin.y * m_scale.y;
	min.z = pFileData->vtxMin.z * m_scale.z;

	return min;
}