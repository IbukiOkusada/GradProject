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

private:	// �����������A�N�Z�X�\�Ȓ�`
	
	// �t�@�C�����
	struct File
	{
		LPDIRECT3DTEXTURE9 pTexture;	// �e�N�X�`���̃|�C���^
		std::string filename;				// �t�@�C����

		// �R���X�g���N�^ 
		File() : pTexture(nullptr), filename("") {}
	};

public:	// �N�ł��A�N�Z�X�\

	CTexture();		// �R���X�g���N�^
	~CTexture();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Load(void);
	void Unload(void);
	int Regist(const char* pFileName);
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
