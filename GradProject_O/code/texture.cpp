//===============================================
//
// �e�N�X�`���̊Ǘ��S�� [texture.cpp]
// Author : Ibuki Okusada
//
//===============================================
#include "texture.h"
#include "manager.h"
#include "renderer.h"
#include <string.h>

// �t�@�C����
const char *CTexture::m_apDefFileName[TYPE_MAX] =
{
	"data\\TEXTURE\\effect\\effect000.jpg",
	"data\\TEXTURE\\effect\\water.png",
};

//===============================================
// �R���X�g���N�^
//===============================================
CTexture::CTexture()
{
	m_List.Clear();
}

//===============================================
// �f�X�g���N�^
//===============================================
CTexture::~CTexture()
{
	
}

//===============================================
// �ǂݍ���
//===============================================
HRESULT CTexture::Load(void)
{
	LPDIRECT3DDEVICE9 pDevice;		//�f�o�C�X�ւ̃|�C���^

	//�f�o�C�X�̎擾
	pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// �������ǂݍ���
	for (int i = 0; i < TYPE_MAX; i++)
	{
		File* pFile = DEBUG_NEW File;

		if (FAILED(D3DXCreateTextureFromFile(pDevice,
			m_apDefFileName[i],
			&pFile->pTexture)))
		{
			delete pFile;
			return -1;
		}

		m_List.Regist(pFile);
		pFile->filename = m_apDefFileName[i];

	}

	return S_OK;
}

//===============================================
// �p��
//===============================================
void CTexture::Unload(void)
{
	for (int i = 0; i < m_List.GetNum(); i++)
	{
		File* pFile = m_List.Get(i);

		// �e�N�X�`���̔p��
		if (pFile->pTexture != nullptr)
		{// �g�p����Ă���ꍇ
			pFile->pTexture->Release();
			pFile->pTexture = nullptr;
		}

		m_List.Delete(pFile);
		delete pFile;
		i--;
	}
}

//===============================================
// �w��t�@�C���ǂݍ���
//===============================================
int CTexture::Regist(const char* pFileName)
{
	CRenderer* pRenderer = CManager::GetInstance()->GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		//�f�o�C�X�ւ̃|�C���^���擾
	int nIdx = -1;	// �e�N�X�`���ԍ�

	for (int i = 0; i < m_List.GetNum(); i++)
	{
		if (strstr(&m_List.Get(i)->filename[0], pFileName) != nullptr)
		{// �t�@�C��������v���Ă���ꍇ
			nIdx = i;	// �e�N�X�`���ԍ���ݒ�
			return nIdx;
		}
	}

	nIdx = m_List.GetNum();
	File* pFile = DEBUG_NEW File;
	pFile->pTexture = nullptr;

	if(FAILED(D3DXCreateTextureFromFile(pDevice,
		pFileName,
		&pFile->pTexture)))
	{
		delete pFile;
		return -1;
	}

	m_List.Regist(pFile);

	pFile->filename = pFileName;

	return nIdx;	// �e�N�X�`���ԍ���Ԃ�
}
//============================================================
//	�e�N�X�`���o�^���� (����)
//============================================================
int CTexture::Regist(const SInfo info)
{
	int nIdx = m_List.GetNum();	// �e�N�X�`���Ǎ��ԍ�
	HRESULT hr;				// �ُ�I���̊m�F�p
	CTexture::File *tempMapInfo = DEBUG_NEW CTexture::File;	// �}�b�v���

	// �}�b�v���̃|�C���^��������
	tempMapInfo->pTexture = nullptr;	// �e�N�X�`���ւ̃|�C���^

	// ��̃e�N�X�`���𐶐�
	hr = D3DXCreateTexture
	( // ����
		CManager::GetInstance()->GetRenderer()->GetDevice(),		// Direct3D�f�o�C�X
		info.Width,		// �e�N�X�`������
		info.Height,	// �e�N�X�`���c��
		info.MipLevels,	// �~�b�v�}�b�v���x��
		info.Usage,		// �����E�m�ۃI�v�V����
		info.Format,	// �s�N�Z���t�H�[�}�b�g
		info.Pool,		// �i�[������
		&tempMapInfo->pTexture	// �e�N�X�`���ւ̃|�C���^
	);
	if (FAILED(hr))
	{ // �e�N�X�`���̐����Ɏ��s�����ꍇ

		assert(false);
		return -1;
	}

	// �e�N�X�`���X�e�[�^�X��ݒ�
	D3DXIMAGE_INFO* pStatus = &tempMapInfo->status;	// �X�e�[�^�X���
	pStatus->Width = info.Width;						// �e�N�X�`������
	pStatus->Height = info.Height;						// �e�N�X�`���c��
	pStatus->Depth = 1;								// �e�N�X�`���[�x
	pStatus->MipLevels = info.MipLevels;					// �~�b�v�}�b�v���x��
	pStatus->Format = info.Format;						// �s�N�Z���t�H�[�}�b�g
	pStatus->ResourceType = D3DRTYPE_TEXTURE;				// ���\�[�X���
	pStatus->ImageFileFormat = (D3DXIMAGE_FILEFORMAT)-1;	// �t�@�C���`�� (�쐬�̂��ߖ���)

	// �t�@�C���p�X����ۑ�
	tempMapInfo->filename = -1;	// �Ǎ��ł͂Ȃ��̂Ńp�X����

	// �A�X�y�N�g����v�Z
	tempMapInfo->aspect.x = (float)info.Width / (float)info.Height;
	tempMapInfo->aspect.y = (float)info.Height / (float)info.Width;

	// �e�N�X�`������ۑ�
	m_List.Regist(tempMapInfo);

	// �ǂݍ��񂾃e�N�X�`���̔z��ԍ���Ԃ�
	return nIdx;
}
//===============================================
// �w��A�h���X�̃e�N�X�`�����擾
//===============================================
LPDIRECT3DTEXTURE9 CTexture::SetAddress(int nIdx)
{
	if (nIdx > m_List.GetNum() || nIdx < 0)
	{// �ǂݍ��ݔ͈͊O�̏ꍇ
		return nullptr;
	}

	return m_List.Get(nIdx)->pTexture;
}

//===============================================
// �e�N�X�`�������擾
//===============================================
const char *CTexture::GetFileName(int nIdx)
{
	if (nIdx >= 0 && nIdx < TYPE_MAX)
	{// �͈͓�
		return m_apDefFileName[nIdx];
	}

	return nullptr;
}