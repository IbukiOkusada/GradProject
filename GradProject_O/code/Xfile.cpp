//==========================================================
//
// X�t�@�C���Ǘ� [modelfile.h]
// Author : Ibuki Okusada
//
//==========================================================
#include "Xfile.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include <string.h>

//==========================================================
// �R���X�g���N�^
//==========================================================
CXFile::CXFile()
{
	m_List.clear();
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CXFile::~CXFile()
{

}

//==========================================================
// �ǂݍ��݊m�F
//==========================================================
int CXFile::Regist(const char* pFileName)
{
	int nIdx = -1;

	// �ǂݍ��܂�Ă��邩�m�F
	for (int i = 0; i < m_List.size(); i++)
	{
		// �����t�@�C�����m�F
		if (strstr(pFileName, m_List[i]->name.c_str()) != nullptr)
		{// �������̂����݂��Ă���ꍇ
			nIdx = i;
			return nIdx;	// �t�@�C������Ԃ�
		}
	}

	// ���݂��Ă��Ȃ��ꍇ���������m�ۂ���
	SFileInfo* pInfo = DEBUG_NEW SFileInfo;	// �������̊m��
	*pInfo = SFileInfo();

	// �t�@�C�����̎擾
	pInfo->name = pFileName;

	nIdx = m_List.size();
	m_List.push_back(pInfo);

	// X�t�@�C���̓ǂݍ���
	if (SUCCEEDED(FileLoad(nIdx)))
	{// ���������ꍇ
		return nIdx;	// �t�@�C������Ԃ�
	}
	else {	// ���s
		m_List.pop_back();
		delete pInfo;
		pInfo = nullptr;
	}

	return -1;
}

//==========================================================
// X�t�@�C���ǂݍ���
//==========================================================
HRESULT CXFile::FileLoad(int nIdx)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();;		//�f�o�C�X�ւ̃|�C���^
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// �e�N�X�`���ւ̃|�C���^
	D3DXMATERIAL *pMat;	//�}�e���A���f�[�^�ւ̃|�C���^
	int nNumVtx;		//���_��
	DWORD dwSizeFVF;	//���_�t�H�[�}�b�g�̃T�C�Y
	BYTE *pVtxBuff;		//���_�o�b�t�@�̃|�C���^

	//X�t�@�C���̓ǂݍ���
	if (FAILED(D3DXLoadMeshFromX(m_List[nIdx]->name.c_str(),
		D3DXMESH_SYSTEMMEM,
		pDevice,
		nullptr,
		&m_List[nIdx]->data.pBuffMat,
		nullptr,
		&m_List[nIdx]->data.dwNumMat,
		&m_List[nIdx]->data.pMesh)))
	{// �ǂݍ��݂Ɏ��s�����ꍇ
		return E_FAIL;	// ���s��Ԃ�
	}

	// �e�N�X�`���|�C���^�̊m��
	if ((int)m_List[nIdx]->data.dwNumMat > 0)
	{// �}�e���A�����g�p���Ă���ꍇ
		if (m_List[nIdx]->data.pIdexTexture == nullptr)
		{// �e�N�X�`�����g�p����Ă��Ȃ��ꍇ
			// �}�e���A�������m��
			m_List[nIdx]->data.pIdexTexture = DEBUG_NEW int[(int)m_List[nIdx]->data.dwNumMat];
		}
	}

	//�}�e���A���f�[�^�ւ̃|�C���^���擾
	pMat = (D3DXMATERIAL*)m_List[nIdx]->data.pBuffMat->GetBufferPointer();

	for (int iMat = 0; iMat < (int)m_List[nIdx]->data.dwNumMat; iMat++)
	{
		if (pMat[iMat].pTextureFilename != nullptr)
		{//�e�N�X�`���t�@�C�������݂��Ă���
			//�e�N�X�`���̓ǂݍ���
			m_List[nIdx]->data.pIdexTexture[iMat] = pTexture->Regist(pMat[iMat].pTextureFilename);
		}
		else
		{
			m_List[nIdx]->data.pIdexTexture[iMat] = -1;
		}
	}

	//���_�����擾
	nNumVtx = m_List[nIdx]->data.pMesh->GetNumVertices();

	//���_�t�H�[�}�b�g�̃T�C�Y���擾
	dwSizeFVF = D3DXGetFVFVertexSize(m_List[nIdx]->data.pMesh->GetFVF());

	//���_�o�b�t�@�����b�N
	m_List[nIdx]->data.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	for (int iVtx = 0; iVtx < nNumVtx; iVtx++)
	{
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;	//���_���W�̑��

		// X���W
		if (vtx.x < m_List[nIdx]->data.vtxMin.x)
		{//�ŏ��l�����l���������ꍇ
			m_List[nIdx]->data.vtxMin.x = vtx.x;
		}
		else if (vtx.x > m_List[nIdx]->data.vtxMax.x)
		{//�ő�l�����l���傫���ꍇ
			m_List[nIdx]->data.vtxMax.x = vtx.x;
		}

		// Z���W
		if (vtx.z < m_List[nIdx]->data.vtxMin.z)
		{//�ŏ��l�����l���������ꍇ
			m_List[nIdx]->data.vtxMin.z = vtx.z;
		}
		else if (vtx.z > m_List[nIdx]->data.vtxMax.z)
		{//�ő�l�����l���傫���ꍇ
			m_List[nIdx]->data.vtxMax.z = vtx.z;
		}

		// Y���W
		if (vtx.y < m_List[nIdx]->data.vtxMin.y)
		{//�ŏ��l�����l���������ꍇ
			m_List[nIdx]->data.vtxMin.y = vtx.y;
		}
		else if (vtx.y > m_List[nIdx]->data.vtxMax.y)
		{//�ő�l�����l���傫���ꍇ
			m_List[nIdx]->data.vtxMax.y = vtx.y;
		}

		pVtxBuff += dwSizeFVF;	//���_�t�H�[�}�b�g�̃T�C�Y���|�C���^��i�߂�
	}

	//���_�o�b�t�@���A�����b�N
	m_List[nIdx]->data.pMesh->UnlockVertexBuffer();

	return S_OK;	// ������Ԃ�
}

//==========================================================
// X�t�@�C�����p��
//==========================================================
void CXFile::Unload(void)
{
	for (int i = 0; i < m_List.size(); i++)
	{
		auto texture = m_List[i];

		//���b�V���̔p��
		if (texture->data.pMesh != nullptr)
		{
			texture->data.pMesh->Release();
			texture->data.pMesh = nullptr;
		}

		//�}�e���A���̔p��
		if (texture->data.pBuffMat != nullptr)
		{
			texture->data.pBuffMat->Release();
			texture->data.pBuffMat = nullptr;
		}

		// �e�N�X�`���ԍ��̔p��
		delete[] texture->data.pIdexTexture;	// �e�N�X�`���|�C���^�̊J��

		// �O��
		m_List.erase(m_List.begin() + i);
		delete texture;	// �������̊J��
		i--;

	}
}

//==========================================================
// �w�肳�ꂽ�ԍ��̏��擾
//==========================================================
CXFile::SFileData *CXFile::SetAddress(int nIdx)
{
	if (nIdx > m_List.size() || nIdx < 0)
	{// �ǂݍ��ݔ͈͊O�̏ꍇ
		return nullptr;
	}

	return &m_List[nIdx]->data;
}