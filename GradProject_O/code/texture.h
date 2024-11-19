//===============================================
//
// �e�N�X�`���̊Ǘ��S�� [texture.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _TEXTURE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _TEXTURE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "main.h"
#include "list.h"

//**********************************************************
// �e�N�X�`���N���X�̒�`
//**********************************************************
class CTexture
{
public:	// �N�ł��A�N�Z�X�\�Ȓ�`
	
	// �����ǂݍ��ݎ��
	enum TYPE
	{
		TYPE_EFFECT = 0,		// �G�t�F�N�g
		TYPE_WATER,				// ������
		TYPE_MAX				   
	};
	// �e�N�X�`���������\����
	struct SInfo
	{
	public:
		// �f�t�H���g�R���X�g���N�^
		SInfo() :
			Width(0),	// �e�N�X�`������
			Height(0),	// �e�N�X�`���c��
			MipLevels(0),	// �~�b�v�}�b�v���x��
			Usage(0),	// �����E�m�ۃI�v�V����
			Format(D3DFMT_UNKNOWN),	// �s�N�Z���t�H�[�}�b�g
			Pool(D3DPOOL_DEFAULT)	// �i�[������
		{}

		// �����t���R���X�g���N�^
		SInfo(const UINT in_nWidth, const UINT in_nHeight, const UINT in_nMip, const DWORD in_dwUsage, const D3DFORMAT in_format, const D3DPOOL in_pool) :
			Width(in_nWidth),	// �e�N�X�`������
			Height(in_nHeight),	// �e�N�X�`���c��
			MipLevels(in_nMip),		// �~�b�v�}�b�v���x��
			Usage(in_dwUsage),	// �����E�m�ۃI�v�V����
			Format(in_format),	// �s�N�Z���t�H�[�}�b�g
			Pool(in_pool)		// �i�[������
		{}

		// �f�X�g���N�^
		~SInfo() {}

		// �����o�ϐ�
		UINT		Width;		// �e�N�X�`������
		UINT		Height;		// �e�N�X�`���c��
		UINT		MipLevels;	// �~�b�v�}�b�v���x��
		DWORD		Usage;		// �����E�m�ۃI�v�V����
		D3DFORMAT	Format;		// �s�N�Z���t�H�[�}�b�g
		D3DPOOL		Pool;		// �i�[������
	};
private:	// �����������A�N�Z�X�\�Ȓ�`
	
	// �t�@�C�����
	struct File
	{
		LPDIRECT3DTEXTURE9 pTexture;	// �e�N�X�`���̃|�C���^
		std::string filename;				// �t�@�C����
		D3DXIMAGE_INFO status;			// �e�N�X�`���X�e�[�^�X�ւ̃|�C���^
		D3DXVECTOR2 aspect;
		// �R���X�g���N�^ 
		File() : pTexture(nullptr), filename(""), status({}), aspect(VECTOR2_ZERO) {}
	};

public:	// �N�ł��A�N�Z�X�\

	CTexture();		// �R���X�g���N�^
	~CTexture();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Load(void);
	void Unload(void);
	int Regist(const char* pFileName);
	int Regist(const SInfo info);				// �e�N�X�`���o�^ (����)
	LPDIRECT3DTEXTURE9 SetAddress(int nIdx);

	// �����o�֐�(�擾)
	int GetNumAll(void) { return m_List.GetNum(); }

	const static char *GetFileName(int nIdx);

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	Clist<File*> m_List;
	const static char *m_apDefFileName[TYPE_MAX];	// �����ǂݍ��݃t�@�C����
};

#endif
